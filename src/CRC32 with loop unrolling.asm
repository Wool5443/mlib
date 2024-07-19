global CRC32

section .text

;----------------------------------------------
; Enrty: rdi -> data, rsi - size
; Return: CRC32 hash
;----------------------------------------------
CRC32:
    cld

    mov  rcx, rsi ; rcx = size
    mov  r8, rsi
    and  r8, 0b111 ; r8 = size % 8

    mov  rsi, rdi ; rsi -> data
    xor  rdx, rdx ; rdx holds the result
 
    shr  rcx, 3 ; rcx = size / 8
    jz   .skip8

    .loop8:
        lodsq
        crc32 rdx, rax
        loop .loop8

    .skip8:

    test r8, r8
    jz   .end

    jmp  [table + r8 * 8 - 8]

    .1:
        lodsb
        crc32 rdx, al
        jmp  .end
    .2:
        lodsw
        crc32 edx, ax
        jmp  .end
    .3:
        lodsw
        crc32 edx, ax
        lodsb
        crc32 rdx, al
        jmp  .end
    .4:
        lodsd
        crc32 edx, eax
        jmp  .end
    .5:
        lodsd
        crc32 edx, eax
        lodsb
        crc32 rdx, al
        jmp  .end
    .6:
        lodsd
        crc32 edx, eax
        lodsw
        crc32 edx, ax
        jmp  .end
    .7:
        lodsd
        crc32 edx, eax
        lodsw
        crc32 edx, ax
        lodsb
        crc32 rdx, al

    .end:
    mov  rax, rdx

    ret

    nop
    nop
table:
    dq  CRC32.1
    dq  CRC32.2
    dq  CRC32.3
    dq  CRC32.4
    dq  CRC32.5
    dq  CRC32.6
    dq  CRC32.7
    nop
    nop
