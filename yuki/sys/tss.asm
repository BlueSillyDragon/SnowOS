global loadTss

loadTss:
    mov ax, 0x28
    ltr ax
    ret