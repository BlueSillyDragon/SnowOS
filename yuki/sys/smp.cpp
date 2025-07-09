#include <limine.h>
#include <cstdint>
#include <inc/io/kprintf.hpp>
#include <inc/sys/gdt.hpp>
#include <inc/sys/idt.hpp>
#include <inc/sys/apic.hpp>
#include <inc/mm/vmm.hpp>
#include <inc/sys/smp.hpp>

void cpuInit() {
    kprintf(YUKI, "Initializing CPU...\n");
    loadGdt();
    loadIdt();
    setCr3();
    enableLapicTimer();
    
    for (;;) {
        __asm__ __volatile__ (" hlt ");
    }
}

void startCpus(limine_mp_response *mp_response) {
    kprintf(YUKI, "Waking up other CPUs...\n");
    if (mp_response->cpu_count == 1) {
        kprintf(YUKI, "Only 1 CPU in system, nothing to do\n");
    } else {
        kprintf(YUKI, "There are %d CPUs in the system\n", mp_response->cpu_count);

        for (uint64_t i = 0; i < mp_response->cpu_count; i++) {
            kprintf(YUKI, "CPU %lu ID: %lu LAPIC ID: %lu\n", i, mp_response->cpus[i]->processor_id, mp_response->cpus[i]->lapic_id);
            mp_response->cpus[i]->goto_address = reinterpret_cast<limine_goto_address>((&cpuInit));
        }
    }
}