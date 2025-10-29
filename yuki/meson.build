incdirs = include_directories(
    '../common/include/freestnd-c-hdrs/x86_64/include',
    '../common/include/freestnd-cxx-hdrs/x86_64/include',
    'source/hal/flanterm/src',
    '../Limine',
    'include',
)

sources = files(
    'source/ke/kernel.cpp',
    'source/ke/interrupts.cpp',
    'source/hal/hal.cpp',
    'source/hal/gdt.asm',
    'source/hal/isr.asm',
    'source/hal/flanterm/src/flanterm.c',
    'source/hal/flanterm/src/flanterm_backends/fb.c',
)

add_project_arguments(
    '--target=x86_64-unknown-windows-gnu',
    '-nostdinc',
    '-ffreestanding',
    '-DLIMINE_API_REVISION=4',
    '-fno-stack-protector',
    '-fno-stack-check',
    '-fno-PIC',
    '-ffunction-sections',
    '-fdata-sections',
    '-m64',
    '-march=x86-64',
    '-mno-80387',
    '-mno-mmx',
    '-mno-sse',
    '-mno-sse2',
    '-mno-red-zone',
    '-mcmodel=kernel',
    '-fdeclspec',
    '-MMD',
    '-MP',

    language: 'cpp'
)

add_project_arguments(
    '-fwin64',

    language: 'nasm'
)

add_project_link_arguments(
    '-Wl,/subsystem:NATIVE',
    '-Wl,/entry:KeMain',
    '-Wl,/largeaddressaware',
    '-Wl,/base:0xFFFFFFFF80000000',
    '-Wl,/nodefaultlib',
    '-Wl,/kernel',
    '-Wl,/verbose',
    '-Wl,/debug',
    '-static',

    language: 'cpp'
)

executable('yuki.exe', sources, include_directories: incdirs)