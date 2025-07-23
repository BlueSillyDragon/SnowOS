#pragma once

#include <limine.h>

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