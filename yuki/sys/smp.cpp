#include <cstdint>
#include <inc/io/kprintf.hpp>
#include <inc/sys/smp.hpp>

void startCpus(limine_mp_response *mp_response) {
    kprintf(YUKI, "Waking up other CPUs...\n");
    if (mp_response->cpu_count == 1) {
        kprintf(YUKI, "Only 1 CPU in system, nothing to do\n");
    } else {
        kprintf(YUKI, "There are %d CPUs in the system\n", mp_response->cpu_count);

        for (uint64_t i = 0; i < mp_response->cpu_count; i++) {
            kprintf(YUKI, "CPU %lu ID: %lu LAPIC ID: %lu\n", i, mp_response->cpus[i]->processor_id, mp_response->cpus[i]->lapic_id);
        }
    }
}