%macro isr_err_stub 1
isr_stub_%+%1:
    push rax
    push rbx
    push rcx
    push rdx
    call interruptHandler
    pop rdx
    pop rcx
    pop rbx
    pop rax
    iretq
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
    push rax
    push rbx
    push rcx
    push rdx
    call interruptHandler
    iretq
%endmacro

%macro isr_syscall_stub 1
isr_stub_%+%1:
    push rax
    call syscallHandler
    pop rax
    iretq
%endmacro

%macro timer_int_stub 1
isr_stub_%+%1:
    push rax
    call timerHandler
    pop rax
    iretq
%endmacro

%macro irq_stub 1
isr_stub_%+%1:
    push rax
    call irqHandler
    pop rax
    iretq
%endmacro

extern interruptHandler
extern syscallHandler
extern timerHandler
extern irqHandler
isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31

irq_stub 32     ; IRQ 0 | PIT Interrupt
irq_stub 33     ; IRQ 1 | Keyboard Interrupt
irq_stub 34     ; IRQ 2 | Cascade (never raised)
irq_stub 35     ; IRQ 3 | COM2 (if enabled)
irq_stub 36     ; IRQ 4 | COM1 (if enabled)
irq_stub 37     ; IRQ 5 | LPT2 (if enabled)
irq_stub 38     ; IRQ 6 | Floppy Disk

timer_int_stub 39
timer_int_stub 40

isr_syscall_stub 41

global isr_stub_table
isr_stub_table:
%assign i 0 
%rep    33 
    dq isr_stub_%+i
%assign i i+1 
%endrep
