#include "inc/io/kprintf.hpp"
#include <cstdint>
#include <inc/sys/spinlock.hpp>

bool locked = false;
uint32_t nextTicket = 0;
uint32_t servingTicket = 0;

void Spinlock::lock() {
    __asm__ __volatile__ (" cli ");
    while(true) {
        if (!__atomic_exchange_n(&locked, true, __ATOMIC_ACQUIRE)) {
            return;
        }

        while (__atomic_load_n(&locked, __ATOMIC_RELAXED)) {
				__asm__ __volatile__ ("pause" ::: "memory");
		}
    }
    __asm__ __volatile__ (" sti ");
}

bool Spinlock::isLocked() {
    return __atomic_load_n(&locked, __ATOMIC_RELAXED);
}

void Spinlock::unlock() {
    __asm__ __volatile__ (" cli ");
    __atomic_store_n(&locked, false, __ATOMIC_RELEASE);
    __asm__ __volatile__ (" sti ");
}

void TicketSpinlock::lock() {
    auto ticket = __atomic_fetch_add(&nextTicket, 1, __ATOMIC_RELAXED);
		while(__atomic_load_n(&servingTicket, __ATOMIC_ACQUIRE) != ticket) {
			__asm__ __volatile__ ("pause" ::: "memory");
		}
}

bool TicketSpinlock::isLocked() {
    return __atomic_load_n(&servingTicket, __ATOMIC_RELAXED)
			< __atomic_load_n(&nextTicket, __ATOMIC_RELAXED);
}

void TicketSpinlock::unlock() {
    auto current = __atomic_load_n(&servingTicket, __ATOMIC_RELAXED);
		__atomic_store_n(&servingTicket, current + 1, __ATOMIC_RELEASE);
}