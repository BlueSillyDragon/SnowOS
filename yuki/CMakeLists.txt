set(SOURCES "kernel.cpp"
"utils/helpers.cpp"
"io/terminal.cpp"
"io/kprintf.cpp"
"io/serial.cpp"
"klibc/string.cpp"
"tinyubsan.cpp"
"sys/gdt.cpp"
"sys/idt.cpp"
"sys/tss.cpp"
"sys/interrupts.cpp"
"sys/gdt.asm"
"sys/idt.asm"
"sys/isr.asm"
"sys/tss.asm"
"sys/spinlock.cpp"
"mm/pmm.cpp"
"mm/paging.cpp"
"mm/vmm.cpp"
"mm/slab.cpp"
"kernel_api.cpp"
"sys/cpuid.cpp"
"utils/mmio.cpp"
"sys/hpet.cpp"
"sys/apic.cpp"
"sys/smp.cpp"
"sched/scheduler.cpp"
"sched/switch.asm")

include(uACPI/uacpi.cmake)

add_executable(yuki ${UACPI_SOURCES} ${SOURCES} flanterm/src/flanterm.c flanterm/src/flanterm_backends/fb.c)

target_sources(yuki
PRIVATE ${UACPI_SOURCES})

target_include_directories(yuki
    PRIVATE freestnd-c-hdrs/x86_64/include
    PRIVATE freestnd-cxx-hdrs/x86_64/include
    PRIVATE ../yuki
    PRIVATE flanterm/src
    PRIVATE ${UACPI_INCLUDES}
)

target_compile_options(yuki
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wall>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-ggdb>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++23>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-target x86_64-unknown-none>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-O2>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fsanitize=undefined>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wno-builtin-declaration-mismatch>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-DUACPI_BAREBONES_MODE=true>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-DLIMINE_API_REVISION=3>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-nostdinc>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-ffreestanding>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-stack-protector>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-stack-check>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-PIC>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-ffunction-sections>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fdata-sections>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-I ../yuki>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-m64>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-march=x86-64>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-mno-80387>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-mno-mmx>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-mno-sse>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-mno-sse2>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-mno-red-zone>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-mcmodel=kernel>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-MMD>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-MP>)

target_compile_options(yuki
PRIVATE $<$<COMPILE_LANGUAGE:C>:-Wall>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-ggdb>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-std=gnu23>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-target x86_64-unknown-none>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-O2>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-fsanitize=undefined>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-Wno-builtin-declaration-mismatch>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-DUACPI_BAREBONES_MODE=true>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-DLIMINE_API_REVISION=3>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-DDO_UNIT_TESTS>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-Wextra>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-nostdinc>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-ffreestanding>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-fno-stack-protector>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-fno-stack-check>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-fno-PIC>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-ffunction-sections>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-fdata-sections>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-I ../yuki>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-m64>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-march=x86-64>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-mno-80387>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-mno-mmx>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-mno-sse>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-mno-sse2>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-mno-red-zone>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-mcmodel=kernel>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-MMD>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-MP>)

target_link_options(yuki
PRIVATE -Wl,-m,elf_x86_64
PRIVATE -Wl,--build-id=none
PRIVATE -nostdlib
PRIVATE -static
PRIVATE -z max-page-size=0x1000
PRIVATE -Wl,--gc-sections
PRIVATE -T ../yuki/linker.ld)