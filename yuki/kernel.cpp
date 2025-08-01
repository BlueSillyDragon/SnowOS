#include "inc/sys/panic.hpp"
#include "uacpi/status.h"
#include <cstdint>
#include <cstddef>
#include <limine.h>
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
#include <inc/sys/spinlock.hpp>
#include <inc/sys/smp.hpp>
#include <inc/sched/scheduler.hpp>

constexpr uint64_t yukiMajor = 0;
constexpr uint64_t yukiMinor = 1;
constexpr uint64_t yukiPatch = 0;


// Limine stuff

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

// Set the base revision to 3, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

namespace {

__attribute__((used, section(".limine_requests")))
volatile LIMINE_BASE_REVISION(3);

}

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

namespace {

__attribute__((used, section(".limine_requests")))
volatile limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
    .response = nullptr
};

__attribute__((used, section(".limine_requests")))
volatile limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
    .response = nullptr
};

__attribute__((used, section(".limine_requests")))
volatile limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0,
    .response = nullptr
};

__attribute__((used, section(".limine_requests")))
volatile limine_executable_address_request executable_request = {
    .id = LIMINE_EXECUTABLE_ADDRESS_REQUEST,
    .revision = 0,
    .response = nullptr
};

__attribute__((used, section(".limine_requests")))
volatile limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0,
    .response = nullptr
};

__attribute__((used, section(".limine_requests")))
volatile limine_mp_request mp_request = {
    .id = LIMINE_MP_REQUEST,
    .revision = 0,
    .response = nullptr,
    .flags = 0
};

}

// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .cpp file, as seen fit.

namespace {

__attribute__((used, section(".limine_requests_start")))
volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
volatile LIMINE_REQUESTS_END_MARKER;

}

// End of Limine stuff

uint64_t hhdm;

uint32_t testColor = 0x23272E;

uint64_t rsdp;

void *threadA(void *args);

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
    kprintf(NONE, "\n\tYuki Version %d.%d.%d\n\n", yukiMajor, yukiMinor, yukiPatch);

    initGdt();
    initIdt();
    initTss();

    initPmm(memmap_request.response, hhdm);
    initVmm(memmap_request.response, executable_request.response);
    initSlab(hhdm);

    rsdp = (uint64_t)rsdp_request.response->address;

    void *tempBuffer = reinterpret_cast<void *>(pmmAlloc() + hhdm);

    uacpi_status ret = uacpi_setup_early_table_access(tempBuffer, 0x1000);

    if (ret != UACPI_STATUS_OK) {
        kernelPanic("Issue Occured getting initializing ACPI tables!");
    }

    enableHpet();
    enableLapicTimer();

    int rc = newProcess(threadA);

    kprintf(YUKI, "Done!\n");

    // We're done, just hang...
    hcf();
}

void *threadA(void *args) {
    bool myBool = true;
    while (myBool) {
        __asm__ __volatile__ ("mov $0xcafebabe, %rax; cli");
        hcf();
    }
    return nullptr;
}

uint64_t getHhdm() {
    return hhdm;
}

uint64_t getRsdp() {
    return rsdp;
}