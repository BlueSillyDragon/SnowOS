#include "inc/sys/spinlock.hpp"
#include <cstdint>
#include <cstddef>
#include <limine.h>
#include <requests.hpp>
#include <inc/utils/helpers.hpp>
#include <inc/klibc/string.hpp>
#include <flanterm.h>
#include <flanterm_backends/fb.h>
#include <inc/io/serial.hpp>
#include <inc/io/kprintf.hpp>
#include <inc/io/terminal.hpp>
#include <inc/io/krnl_colors.hpp>
#include <inc/io/krnl_font.hpp>
#include <inc/io/logo.hpp>
#include <inc/sys/gdt.hpp>
#include <inc/sys/idt.hpp>
#include <inc/sys/tss.hpp>
#include <inc/mm/pmm.hpp>
#include <inc/mm/vmm.hpp>
#include <inc/mm/slab.hpp>
#include <uacpi/uacpi.h>
#include <uacpi/tables.h>
#include <uacpi/acpi.h>
#include <inc/utils/mmio.hpp>
#include <inc/sys/cpuid.hpp>
#include <inc/sys/hpet.hpp>
#include <inc/sys/apic.hpp>
#include <inc/sys/smp.hpp>
#include <inc/sched/scheduler.hpp>

#define KERNEL_MAJOR 0
#define KERNEL_MINOR 1
#define KERNEL_PATCH 0  

// The following stubs are required by the Itanium C++ ABI (the one we use,
// regardless of the "Itanium" nomenclature).
// Like the memory functions above, these stubs can be moved to a different .cpp file,
// but should not be removed, unless you know what you are doing.
extern "C" {
    int __cxa_atexit(void (*)(void *), void *, void *) { return 0; }
    void __cxa_pure_virtual() { hcf(); }
    void *__dso_handle;
}

// Extern declarations for global constructors array.
extern void (*__init_array[])();
extern void (*__init_array_end[])();

uint64_t hhdm;

uint32_t testColor = 0x23272E;

uint64_t rsdp;


void *threadA(void *args);
void *threadB(void *args);
void *threadC(void *args);
void *threadD(void *args);
void *threadE(void *args);

extern "C" void kernelMain()
{
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    // Call global constructors.
    for (std::size_t i = 0; &__init_array[i] != __init_array_end; i++) {
        __init_array[i]();
    }

    if (hhdm_request.response == nullptr)
    {
        hcf();
    }

    hhdm = hhdm_request.response->offset;

    serialInit();

    // Ensure we got a framebuffer.
    if (framebuffer_request.response == nullptr
     || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    // Fetch the first framebuffer.
    limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    struct flanterm_context *ftCtx = termInit(framebuffer, KRNL_WHITE, KRNL_DARK_GREY);

    setFtCtx(ftCtx);

    kprintf(NONE, kernelLogo);
    kprintf(NONE, "\n\tYuki Version %d.%d.%d\n\n", KERNEL_MAJOR, KERNEL_MINOR, KERNEL_PATCH);

    initGdt();
    initIdt();
    initTss();

    initPmm(memmap_request.response, hhdm);
    initVmm(memmap_request.response, executable_request.response, hhdm);
    initSlab(hhdm);

    rsdp = (uint64_t)rsdp_request.response->address;

    void *tempBuffer = reinterpret_cast<void *>(pmmAlloc() + hhdm);

    uacpi_status ret = uacpi_setup_early_table_access(tempBuffer, 0x1000);

    enableHpet();
    enableLapicTimer();

    initScheduler(hhdm);

    int rc = newProcess(threadA);
    rc = newProcess(threadB);
    rc = newProcess(threadC);
    rc = newProcess(threadD);
    rc = newProcess(threadE);

    kprintf(YUKI, "Done!\n");

    // We're done, just hang...
    hcf();
}

void *threadA(void *args) {
    bool myBool = true;

    TicketSpinlock::lock();

    kprintf(NONE, "This\n");
    kprintf(NONE, "should\n");
    kprintf(NONE, "take\n");
    kprintf(NONE, "a\n");
    kprintf(NONE, "while\n");
    kprintf(NONE, "to\n");
    kprintf(NONE, "complete\n");
    kprintf(NONE, "SnowOS is awesome!\n");

    TicketSpinlock::unlock();

    return nullptr;
}

void *threadB(void *args) {
    bool myBool = true;

    kprintf(NONE, "Hello from thread B!\n");

    return nullptr;
}

void *threadC(void *args) {
    bool myBool = true;

    kprintf(NONE, "Hello from thread C!\n");

    return nullptr;
}

void *threadD(void *args) {
    bool myBool = true;

    kprintf(NONE, "Hello from thread D!\n");

    return nullptr;
}

void *threadE(void *args) {
    bool myBool = true;

    kprintf(NONE, "Hello from thread E!\n");
    
    return nullptr;
}