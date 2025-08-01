section .text

global setupContext

; Sets up the context for a thread
; RDI | Thread *ctx
setupContext:
    cli
    ; Save the old RSP and switch to the new one
    mov rdx, rsp
    mov rsp, [rdi + 16]
    mov rax, [rdi + 24]

    push 0x10
    push rsp
    pushfq
    push 0x08
    push rax

    mov [rdi + 16], rsp

    mov rsp, rdx
    
    sti

    ret

global switchContexts

; Saves old context on old stack and switches to new one, then rets
; RDI | Thread *oldCtx
; RSI | Thread *newCtx
switchContexts:
    cli

    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push r15
    push r14
    push r13
    push r12

    mov [rdi + 16], rsp
    mov rsp, [rsi + 16]

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax

    sti

    iretq