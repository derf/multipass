.global asm_save_all
.global asm_load_all
.global asm_load_mem

stack_backup:
    .space 2048

sp_backup:
    .space 2

backup_cookie:
    .space 2

asm_save_all:
    dint
    .irp reg,4,5,6,7,8,9,10,11
    push r\reg
    .endr

    mov #1234h, r10
    mov r10, &backup_cookie

    mov r1, &sp_backup

    mov #1c00h, r10
    mov #stack_backup, r11

save_sram_word:
    mov @r10+, 0(r11)
    add #2, r11
    cmp #1c00h+2048, r10
    jlo save_sram_word

    pop r11
    pop r10
    add #12, r1
    eint
    ret

asm_load_all:
    dint

    push r11
    mov &backup_cookie, r11
    cmp #1234h, r11
    jeq do_load_all
    pop r11
    eint
    ret

do_load_all:
    ; restore SRAM from backup
    mov #stack_backup, r10
    mov #1c00h, r11
load_sram_word:
    mov @r10+, 0(r11)
    add #2, r11
    cmp #1c00h+2048, r11
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

asm_load_mem:
    dint

    push r11
    mov &backup_cookie, r11
    cmp #1234h, r11
    jeq do_load_mem
    pop r11
    eint
    ret

do_load_mem:
    push r10
    push r9
    ; restore SRAM from backup, excluding current stack frame
    mov r1, r9
    sub 6, r9
    mov #stack_backup, r10
    mov #1c00h, r11
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
