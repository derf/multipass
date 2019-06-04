.global asm_save_all
.global asm_load_all
.global asm_load_mem

#define SRAM_BASE #1c00h
#define SRAM_SIZE 4096

; SRAM backup space
sram_backup:
    .space SRAM_SIZE

; Stack Pointer backup
sp_backup:
    .space 2

; Backup Cookie: Do we have valid data or has the FRAM been wiped?
backup_cookie:
    .space 2

; save entire SRAM and CPU register state to persistent FRAM
asm_save_all:

    ; r4 to r11 are callee saved -> push them to the stack.
    ; As we will save the entire SRAM (including stack), they are
    ; included in the SRAM backup and can be popped when restoring it.
    .irp reg,4,5,6,7,8,9,10,11
    push r\reg
    .endr

    ; We will soon have valid data
    mov #1234h, r10
    mov r10, &backup_cookie

    mov r1, &sp_backup

    mov SRAM_BASE, r10
    mov #sram_backup, r11

    ; Interrupts may alter global variables in SRAM and thus lead to inconsistencies
    dint

save_sram_word:
    mov @r10+, 0(r11)
    add #2, r11
    cmp SRAM_BASE+SRAM_SIZE, r10
    jlo save_sram_word

    eint

    ; revert changes to callee-saved registers
    pop r11
    pop r10
    ; remove unchanged registers from stack
    add #12, r1

    ret

; load entire SRAM and CPU register stat from persistent FRAM,
; if it contains valid backup data. Execution will resume at the
; last place where asm_save_all() was called is if nothing in between
; had happened. Does not take possible the state of hardware peripherals
; into account.
asm_load_all:

    ; check if we have backup data
    push r11
    mov &backup_cookie, r11
    cmp #1234h, r11

    ; yes? -> load it
    jeq do_load_all

    ; no? -> too bad, resume with normal startup
    pop r11
    ret

    dint

do_load_all:
    ; restore SRAM from backup
    mov #sram_backup, r10
    mov SRAM_BASE, r11
load_sram_word:
    mov @r10+, 0(r11)
    add #2, r11
    cmp SRAM_BASE+SRAM_SIZE, r11
    jlo load_sram_word

    ; restore stack pointer
    mov &sp_backup, r1

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

    push r11
    mov &backup_cookie, r11
    cmp #1234h, r11
    jeq do_load_mem
    pop r11
    ret

    dint

do_load_mem:
    push r10
    push r9
    ; restore SRAM from backup, excluding stack
    ; -> everything from SRAM start (inclusive) to @sp (exclusive). Reminder: SP == R1 on MSP430
    mov r1, r9
    mov #sram_backup, r10
    mov SRAM_BASE, r11
load_sram_word2:
    mov @r10+, 0(r11)
    add #2, r11
    cmp r9, r11
    jlo load_sram_word2

    pop r9
    pop r10
    pop r11

    eint
    ret
