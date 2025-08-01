#include <cstdint>
#include <inc/utils/helpers.hpp>
#include <inc/klibc/string.hpp>
#include <inc/sched/scheduler.hpp>
#include <inc/mm/pmm.hpp>
#include <inc/mm/paging.hpp>
#include <inc/mm/slab.hpp>
#include <inc/io/kprintf.hpp>

uint64_t nextPid = 0;
uint64_t nextTid = 0;

Thread *readyQueue = nullptr;

Thread *currentThread = nullptr;
Thread *oldThread = nullptr;

extern "C" void setupContext(Thread *ctx);
extern "C" void switchContexts(Thread *oldCtx, Thread *newCtx);

int newProcess(void *(*startFunction) (void *)) {
    Process *proc = reinterpret_cast<Process *>(kmalloc(sizeof(Process)));
    memset(proc, 0, sizeof(Process));

    proc->pid = nextPid;

    // Create the main thread
    Thread *mainThread = reinterpret_cast<Thread *>(kmalloc(sizeof(Thread)));
    memset(mainThread, 0, sizeof(Thread));

    proc->mainThread = mainThread;

    mainThread->rip = (uint64_t)startFunction;
    mainThread->rsp = (pmmAlloc() + getHhdm()) + 0xfff;

    mainThread->tid = nextTid;

    kprintf(SCHEDULER, "Kernel stack allocated at 0x%lx\n", mainThread->rsp);

    setupContext(mainThread);

    if (readyQueue == nullptr) {
        mainThread->prevThread = mainThread;
        mainThread->nextThread = mainThread;
        readyQueue = mainThread;
    } else {
        Thread *lastThread = readyQueue->prevThread;

        mainThread->nextThread = readyQueue;
        mainThread->prevThread = lastThread;
        readyQueue->prevThread = mainThread;
        lastThread->nextThread = mainThread;

        readyQueue = mainThread;
    }

    currentThread = mainThread;

    kprintf(SCHEDULER, "Created new process with PID: %lu\n", proc->pid);

    nextPid++;
    nextTid++;

    return 0;
}

void schedule() {
    if (readyQueue == nullptr) {
        kprintf(SCHEDULER, "No threads on queue, running idle thread UwU\n");
        return;
    }

    oldThread = currentThread;
    currentThread = currentThread->nextThread;

    kprintf(SCHEDULER, "Running thread from ready queue OwO\n");
    kprintf(SCHEDULER, "Old Thread: %lu, New Thread: %lu\n", oldThread->tid, currentThread->tid);

    switchContexts(oldThread, currentThread);
}