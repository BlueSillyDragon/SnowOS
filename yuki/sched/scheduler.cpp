#include <cstdint>
#include <inc/klibc/string.hpp>
#include <inc/io/kprintf.hpp>
#include <inc/mm/vmm.hpp>
#include <inc/mm/pmm.hpp>
#include <inc/mm/slab.hpp>
#include <inc/sched/scheduler.hpp>
#include <inc/sys/apic.hpp>

uint64_t nextPid = 1000;
uint64_t nextTid = 0;
uint64_t offsHddm;

enum PROC_STATE {READY, RUNNING, BLOCKED};

struct Thread {
    Thread *prevThread;
    
    uint64_t tid;
    
    uint64_t rbx;
    uint64_t rbp;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;

    uint64_t rsp;
    uint64_t rip;

    Thread *nextThread;
};

struct Process {
    uint64_t pid;

    PROC_STATE state;

    pagemap_t pagemap;

    Thread *thread;

    Process *nextProc;
};

Thread *queueHead = nullptr;
Thread *currentThread = nullptr;

extern "C" void setupContext(void *newCtx);
extern "C" void contextSwitch(void *oldCtx, void *newCtx);

int newProcess(void *(*startFunction) (void *)) {
    __asm__ __volatile__ (" cli ");
    kprintf(SCHEDULER, "Creating new thread with PID: %lu\n", nextPid);

    Process *proc = (Process *)kmalloc(sizeof(Process));
    memset(proc, 0, sizeof(Process));

    proc->pid = nextPid;

    kprintf(SCHEDULER, "Creating new thread for Process: %lu\n", proc->pid);

    Thread *thread = (Thread *)kmalloc(sizeof(Thread));
    memset(thread, 0, sizeof(Thread));

    thread->rsp = (pmmAlloc() + offsHddm) + 0xfff;

    thread->tid = nextTid;
    nextTid++;

    thread->rip = (uint64_t)startFunction;

    setupContext(thread);

    thread->nextThread = queueHead;
    
    if (queueHead != nullptr) {
        queueHead->prevThread = thread;
    }

    queueHead = thread;

    currentThread = thread;

    nextPid++;
    __asm__ __volatile__ (" sti ");
    return 0;
}

void initScheduler(uint64_t hhdm) {
    kprintf(SCHEDULER, "Initializng Scheduler...\n");
    offsHddm = hhdm;
}

// Decide which process to run next, and perform a context switch
extern "C" void yield() {
    Thread *oldThread = currentThread;
    if (queueHead == nullptr || currentThread == nullptr) {
        kprintf(SCHEDULER, "Nothing to do.\n");
    } else if (currentThread->nextThread == nullptr) {
        currentThread = queueHead;
    } else {
        currentThread = currentThread->nextThread;
    }

    kprintf(SCHEDULER, "Running Thread: %lu\n", currentThread->tid);
    contextSwitch(oldThread, currentThread);
}