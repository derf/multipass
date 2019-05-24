.global asm_save_toc
.global asm_load_toc

stack_backup:
    .space 2048

sp_backup:
    .space 2

asm_save_toc:
    .irp reg,4,5,6,7,8,9,10,11
    push r\reg
    .endr

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
    ret

asm_load_toc:
    mov #stack_backup, r10
    mov #1c00h, r11

load_sram_word:
    mov @r10+, 0(r11)
    add #2, r11
    cmp #1c00h+2048, r11
    jlo load_sram_word

    mov &sp_backup, r1
    .irp reg,11,10,9,8,7,6,5,4
    pop r\reg
    .endr

    ret