;
;Snow Operating System
;Copyright (c) BlueSillyDragon 2025
 
;File: hal/isr.asm

;Description:
;This file holds the ISR Stubs for the
;Interrupt Descriptor Table

;Author:
;BlueSillyDragon
;October 28th 2025
;

%macro IsrErrorStub 1
ISR_STUB_%+%1:
    push rax
    push rbx
    push rcx
    push rdx
    call KeInterruptHandler
    pop rdx
    pop rcx
    pop rbx
    pop rax
    iretq
%endmacro

%macro IsrNoErrorStub 1
ISR_STUB_%+%1:
    push rax
    push rbx
    push rcx
    push rdx
    call KeInterruptHandler
    pop rdx
    pop rcx
    pop rbx
    pop rax
    iretq
%endmacro

extern KeInterruptHandler
IsrNoErrorStub  0
IsrNoErrorStub  1
IsrNoErrorStub  2
IsrNoErrorStub  3
IsrNoErrorStub  4
IsrNoErrorStub  5
IsrNoErrorStub  6
IsrNoErrorStub  7
IsrErrorStub    8
IsrNoErrorStub  9
IsrErrorStub    10
IsrErrorStub    11
IsrErrorStub    12
IsrErrorStub    13
IsrErrorStub    14
IsrNoErrorStub  15
IsrNoErrorStub  16
IsrErrorStub    17
IsrNoErrorStub  18
IsrNoErrorStub  19
IsrNoErrorStub  20
IsrNoErrorStub  21
IsrNoErrorStub  22
IsrNoErrorStub  23
IsrNoErrorStub  24
IsrNoErrorStub  25
IsrNoErrorStub  26
IsrNoErrorStub  27
IsrNoErrorStub  28
IsrNoErrorStub  29
IsrErrorStub    30
IsrNoErrorStub  31

global IsrStubTable

section .text

IsrStubTable:
%assign i 0 
%rep    32
    dq ISR_STUB_%+i
%assign i i+1 
%endrep