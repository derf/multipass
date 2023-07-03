;
; Copyright 2020 Birte Kristina Friesel
;
; SPDX-License-Identifier: BSD-2-Clause
;

.global asm_save_all
.global asm_load_all
.global asm_load_mem

#define SRAM_BASE #1c00h
#define SRAM_SIZE 4096

; SRAM and stack pointer backup space
; two backup areas allow for consistency in case of a power loss during backup
sp_backup1:
    .space 2
sram_backup1:
    .space SRAM_SIZE
sp_backup2:
    .space 2
sram_backup2:
    .space SRAM_SIZE

; Backup Cookie: Do we have valid data (and if yes, where?)
; or has the FRAM been wiped?
backup_cookie:
    .space 2

; save entire SRAM and CPU register state to persistent FRAM.
; Must be called with interrupts disabled
asm_save_all:

    ; r4 to r11 are callee saved -> push them to the stack.
    ; As we will save the entire SRAM (including stack), they are
    ; included in the SRAM backup and can be popped when restoring it.
    .irp reg,4,5,6,7,8,9,10,11
    push r\reg
    .endr

    mov &backup_cookie, r10 ; content of backup_cookie -> r10
    mov #sp_backup1, r11 ; address of sp_backup1 -> r11
    cmp r10, r11 ; backup_cookie == addr. of sp_backup1?
    jne do_save_all ; if not, the previous backup went to space 2 or never happened -> write backup to space 1
    ; otherwise, the previous backup went to space 1 -> write backup to space 2

    mov #sp_backup2, r11

do_save_all:
    dint
    mov r1, 0(r11) ; store stack pointer in sp_backup(1|2)
    mov r11, r9 ; backup location -> r9
    mov SRAM_BASE, r10 ; SRAM area start -> r10

save_sram_word:
    add #2, r11
    mov @r10+, 0(r11)
    cmp SRAM_BASE+SRAM_SIZE, r10
    jlo save_sram_word

    mov r9, &backup_cookie ; save backup location (addr. of sp_backup(1|2)) in backup_cookie

    eint

    ; revert changes to callee-saved registers
    pop r11
    pop r10
    pop r9
    ; remove unchanged registers from stack
    add #10, r1

    ret

; load entire SRAM and CPU register state from persistent FRAM,
; if it contains valid backup data. Execution will resume at the
; last place where asm_save_all() was called as if nothing in between
; had happened. Does not take the state of hardware peripherals into account.
asm_load_all:

    ; check if we have backup data
    push r11
    push r10
    mov &backup_cookie, r10

    ; ... in location 1?
    mov #sp_backup1, r11
    cmp r10, r11
    jeq do_load_all

    ; ... in location 2?
    mov #sp_backup2, r11
    cmp r10, r11
    jeq do_load_all

    ; no? -> too bad, resume with normal startup
    pop r10
    pop r11
    ret

do_load_all:
    dint
    ; restore stack pointer
    mov @r11, r1
    add #2, r11
    ; restore SRAM from backup
    mov SRAM_BASE, r10
load_sram_word:
    mov @r11+, 0(r10)
    add #2, r10
    cmp SRAM_BASE+SRAM_SIZE, r10
    jlo load_sram_word

    ; restore registers
    .irp reg,11,10,9,8,7,6,5,4
    pop r\reg
    .endr

    eint

    ; The return address on the stack has been restored from FRAM backup
    ; -> execution will continue where asm_save_all was called
    ret

; Load global objects from persistent FRAM, if it contains valid backup data.
; Stack and CPU registers are left as-is, the program flow is not altered.
asm_load_mem:

    ; check if we have backup data
    push r11
    push r10
    mov &backup_cookie, r10

    ; ... in location 1?
    mov #sp_backup1, r11
    cmp r10, r11
    jeq do_load_mem

    ; ... in location 2?
    mov #sp_backup2, r11
    cmp r10, r11
    jeq do_load_mem

    ; no? -> too bad, resume with normal startup
    pop r10
    pop r11
    ret

do_load_mem:
    dint
    ; restore SRAM from backup, excluding stack
    ; -> everything from SRAM start (inclusive) to @sp (exclusive). Reminder: SP == R1 on MSP430
    add #2, r11
    mov SRAM_BASE, r10
load_sram_word2:
    mov @r11+, 0(r10)
    add #2, r10
    cmp r1, r10
    jlo load_sram_word2

    pop r10
    pop r11

    eint
    ret
