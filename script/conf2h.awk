#! /usr/bin/gawk -f
#
# Copyright 2021 AG Eingebettete Softwaresysteme, Universität Osnabrück
#
# SPDX-License-Identicier: CC0-1.0

BEGIN {
  print "// config.h generated from " ARGV[1] "\n" \
    "#ifndef CONFIG_H\n" \
    "#define CONFIG_H"
}

/^CONFIG_.*?_INSTANCES=/ { next }

/^#/ { sub(/^#/,"//") }

/^CONFIG_.*?=/ {
  if (/=n$/) {
    sub(/^/,"// ");
  } else {
    sub(/^/,"#define ")
    if (/=y$/) {
      sub(/=.*$/,"")
    } else if (/=".*"$/) {
      sub(/="/, " ")
      sub(/"$/, "")
    } else {
      sub(/=/," ")
    }
  }
}

{ print }

END { print "#endif" }
