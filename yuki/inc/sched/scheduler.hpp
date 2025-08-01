#pragma once

#include <cstdint>
#include <inc/mm/paging.hpp>

struct Thread {
    Thread *prevThread;

    uint64_t tid;

    uint64_t rsp;
    uint64_t rip;

    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rdi;
    uint64_t rsi;

    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;

    Thread *nextThread;
}__attribute__((packed));

struct Process {
    uint64_t pid;
    Pagemap pagemap;

    Thread *mainThread;

    Process *nextProcess;
};

int newProcess(void *(*startFunction) (void *));
void initScheduler(uint64_t hhdm);
void schedule();