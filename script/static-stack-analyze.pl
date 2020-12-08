#!/usr/bin/env perl
#
# Copyright 2018 Daniel Friesel
#
# SPDX-License-Identifier: BSD-2-Clause

use strict;
use warnings;
use 5.010;

use File::Slurp qw(read_file);
use Getopt::Long;
use List::Util qw(uniq);

our $VERSION = '0.00';
my $machine_readable = 0;

GetOptions(
	'm|machine-readable!' => \$machine_readable,
);

if (@ARGV < 3) {
	die("Usage: $0 <objdump binary> <architecture> <oject files ...>\n");
}

my %arch_data = (
	x64 => {
		# For each function call, the 8-Byte return address is pushed onto the stack.
		call_cost => 8,
		call_relocation => 'R_X86_64_PLT32',
	},
	avr => {
		# On each function call, the 2-Byte return address is pushed onto the stack.
		# (unhandled exception: ATTiny2313 and other devices with <= 256 Bytes of RAM)
		call_cost => 2,
		call_relocation => 'R_AVR_CALL',
	},
	msp430 => {
		# For each function call, the 4-Byte (20 bits at 2-Byte alignment) return
		# address is pushed onto the stack.
		call_cost => 4,
		call_relocation => 'R_MSP430X_ABS16',
	},
	msp430large => {
		# For each function call, the 4-Byte (20 bits at 2-Byte alignment) return
		# address is pushed onto the stack.
		call_cost => 4,
		call_relocation => 'R_MSP430X_ABS20_ADR_DST',
	},
);

my %addresses;
my %call_graph;
my %frame_size;

my ($objdump_path, $arch, @object_files) = @ARGV;

sub add_call {
	my ($parent, $child) = @_;

	push(@{$call_graph{$parent}{children}}, $child);
	push(@{$call_graph{$child}{parents}}, $parent);

	#say "$parent -> $child;";

	if ($parent =~ m{ ^ __vector_ }x) {
		push(@{$call_graph{INTERRUPT}{children}}, $parent);
		push(@{$call_graph{$parent}{parents}}, 'INTERRUPT');
	}
}

sub trace {
	my ($function) = @_;

	my $node = $call_graph{$function};

	if ($node->{visited}) {
		$node->{recursive} = 1;
		return;
	}

	$node->{visited} = 1;
	$node->{max_depth} //= 1;
	$node->{max_cost} //= $frame_size{$function}{size} // 0;

	for my $child (@{$node->{children} // []}) {
		trace($child);
		my $child_node = $call_graph{$child};
		if ($child_node->{max_depth} + 1 > $node->{max_depth}) {
			$node->{max_depth} = $child_node->{max_depth} + 1;
		}
		if (defined $frame_size{$function}{size}) {
			my $call_cost = $child_node->{max_cost} + $arch_data{$arch}{call_cost} + $frame_size{$function}{size};
			if ($call_cost > $node->{max_cost}) {
				$node->{max_cost} = $call_cost;
			}
		}
		else {
			say "Frame size of $function is unknown!";
			$node->{unknown_child} = 1;
			my $call_cost = $child_node->{max_cost} + $arch_data{$arch}{call_cost};
			if ($call_cost > $node->{max_cost}) {
				$node->{max_cost} = $call_cost;
			}
		}
		if ($child_node->{unknown_child}) {
			$node->{unknown_child} = 1;
		}
	}

	$node->{visited} = 0;
}

sub check_recursive {
	my ($function) = @_;
	my $node = $call_graph{$function};

	if ($node->{visited}) {
		return;
	}

	$node->{visited} = 1;

	for my $child (@{$node->{children} // []}) {
		if ($call_graph{$child}{recursive} or $call_graph{$child}{calls_recursive}) {
			$node->{calls_recursive} = 1;
		}
	}

	if ($node->{recursive} or $node->{calls_recursive}) {
		for my $parent (@{$node->{parents} // []}) {
			check_recursive($parent);
		}
	}

	$node->{visited} = 0;
}

for my $file (@object_files) {
	my $dump = qx{$objdump_path -Cdr $file};
	my $current_function = 'NONE';
	my $call_reloc = $arch_data{$arch}{call_relocation};

	my %address;

	for my $line (split(qr{\n}, $dump)) {
		if ($line =~ m{ ^ (?<address> [0-9a-fA-Z]+ ) \s+ < (?<function> .* ) > : $}x) {
			$current_function = $+{function};
			my $addr = $+{address};

			$current_function =~ s{ \s \[ clone \s \S+ \] $ }{}x;
			$addr =~ s{ ^ 0* }{}x;

			$address{$addr} = $current_function;
		}
		elsif ($line =~ m{ : \s+ ${call_reloc} \s+ (?<function> .+ ) }x) {
			my $function = $+{function};

			if ($function =~ m{ ^ [.] L \d+ $ }x) {
				# MSP430, doesn't seem related to function calls
				next;
			}

			# x64 fixup. TODO what's the meaning of this? Edge cases to consider?
			$function =~ s{ -0x4 $ }{}x;

			if ($function =~ m{ ^ [.] text [+] 0x (?<addr> [0-9a-fA-F]+ ) }x) {
				$function = $address{$+{addr}} // 'UNRESOLVED';
			}
			add_call($current_function, $function);
		}
	}

	my $su_file = $file;
	$su_file =~ s{[.]o$}{.su};

	for my $line (read_file($su_file)) {
		if ($line =~ m{ ^ (?<file> [^:]+ ) : (?<line> [^:]+ ) : (?<column> [^:]+ )
				: (?: const \s+ )? (?: virtual \s+ )?
				(?<return_type> \S+ ) (?: \s (?<function> [^\t]+ ))?
				\t (?<size> [^\t]+ ) \t (?<estimate> .+ ) $ }x) {

			my $function = $+{function} // $+{return_type};
			my $size = $+{size};
			my $estimate = $+{estimate};

			$frame_size{$function} = {
				size => $size,
				estimate => $estimate,
			};

			# For C(-ish) functions, objdump does not give the function signature,
			# whereas .su does. Therefore, for each demangled function with
			# signature, we create a duplicate entry without it.
			if ($function =~ s{ \( .* \) $ }{}x) {
				$frame_size{$function} = {
					size => $size,
					estimate => $estimate,
					duplicate => 1,
				};
			}

			# There's also some disagreement about "short" vs. "short int" and
			# "long" vs. "long int"...
		}
		else {
			say "$su_file: Cannot parse $line";
		}
	}
}

for my $function (keys %call_graph) {
	@{$call_graph{$function}{children}} = uniq @{$call_graph{$function}{children}};
	@{$call_graph{$function}{parents}} = uniq @{$call_graph{$function}{parents}};
}

for my $function (keys %call_graph) {
	trace($function);
}

for my $function (keys %call_graph) {
	check_recursive($function);
}

if (not $machine_readable) {
	printf("%2s %40s %10s%1s %10s%1s %10s\n", q{}, 'Function', 'Inclusive', '', 'Exclusive', '', 'Height');
}

for my $function (reverse sort { ($call_graph{$a}{max_cost} <=> $call_graph{$b}{max_cost}) || ($a cmp $b) } keys %call_graph) {

	my $node = $call_graph{$function};
	my $info = '';

	if ($node->{recursive}) {
		$info .= 'R';
	}
	elsif ($node->{calls_recursive}) {
		$info .= 'r';
	}
	else {
		$info .= ' ';
	}

	if (@{$node->{parents} // []} == 0) {
		$info .= '>';
	}
	if ($machine_readable) {
		printf("%s!%d!%d!%d!%d!%d!%d!%d!%d\n",
			$function,
			$call_graph{$function}{max_cost},
			$frame_size{$function}{size} // 0,
			$call_graph{$function}{max_depth},
			$node->{recursive} || 0,
			$node->{calls_recursive} || 0,
			$node->{parents} || 0,
			$call_graph{$function}{unknown_child} ? 1 : 0,
			defined $frame_size{$function}{size} ? 1 : 0,
		);
	}
	else {
		printf("%2s %40s %10d%1s %10d%1s %10d\n",
			$info, substr($function, 0, 40),
			$call_graph{$function}{max_cost},
			$call_graph{$function}{unknown_child} ? '+' : ' ',
			$frame_size{$function}{size} // 0,
			defined $frame_size{$function}{size} ? ' ' : '?',
			$call_graph{$function}{max_depth});
	}
}
