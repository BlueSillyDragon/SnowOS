;
;Snow Operating System
;Copyright (c) BlueSillyDragon 2025
 
;File: hal/gdt.asm

;Description:
;This file holds the ReloadSegments
;Subrountine for GDT initialization

;Author:
;BlueSillyDragon
;October 28th 2025
;

global ReloadSegments

section .text

ReloadSegments:
    push 0x08
    lea rax, [rel .ReloadCS]
    push rax
    retfq
.ReloadCS:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret