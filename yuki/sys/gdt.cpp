#include <cstdint>
#include <inc/io/terminal.hpp>
#include <inc/sys/gdt.hpp>

extern "C" void reloadSegs(void);

gdtr_t gdtr;
gdt_t gdt;

void initGdt()
{
    // Set the segments
    gdt.null_segment = 0x0;
    gdt.kernel_code = KERNEL_CS;
    gdt.kernel_data = KERNEL_DS;
    gdt.user_code = USER_CS;
    gdt.user_data = USER_DS;

    // Set the GDTR
    gdtr.base = reinterpret_cast<std::uint64_t>(&gdt);
    gdtr.limit = (sizeof(gdt) - 1);

    // Load the GDT and reload segment registers
    __asm__ volatile ("lgdt %0" :: "m"(gdtr));
    reloadSegs();
}