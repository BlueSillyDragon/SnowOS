global setupContext

; void setupContext(void *ctx)
; ctx | context to setup

setupContext:
    mov rdx, rsp
    mov rsp, [rdi + 64]
    mov rax, [rdi + 72]
    push 0x10
    push rsp
    pushfq
    push 0x08
    push rax
    
    push 0x0
    push 0x0
    push 0x0
    push 0x0
    push 0x0
    push 0x0

    mov [rdi + 64], rsp

    mov rsp, rdx
    ret

global contextSwitch

; void contextSwitch(void *oldCtx, void *newCtx)
; oldCtx | Context to save
; newCtx | Context to load

extern setCr3

contextSwitch:
    ; Save Registers
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    mov [rdi + 64], rsp
    mov rsp, [rsi + 64]

    ; Load new registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx

    mov rax, 0xcafebabe

    iretq

.returnPoint:
    ret