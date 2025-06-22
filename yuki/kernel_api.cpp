#include "uacpi/log.h"
#include <cstdarg>
#include <cstdint>
#include <inc/io/kprintf.hpp>
#include <uacpi/kernel_api.h>
#include <uacpi/status.h>
#include <inc/mm/vmm.hpp>

extern "C" uint64_t rsdp;

uacpi_status uacpi_kernel_get_rsdp(uacpi_phys_addr *out_rsdp_address) {
    if (rsdp == 0) {
        kprintf(ERROR, "RSDP could not be retrieved!\n");
        return UACPI_STATUS_INTERNAL_ERROR;
    }
    *out_rsdp_address = rsdp;
    return UACPI_STATUS_OK;
}

void *uacpi_kernel_map(uacpi_phys_addr addr, uacpi_size len) {
    return vmmMapPhys(addr, len);
}

void uacpi_kernel_unmap(void *addr, uacpi_size len) {
    vmmUnmapVirt(addr, len);
}

void uacpi_kernel_log(uacpi_log_level level, const uacpi_char* string) {
    switch (level) {
        case UACPI_LOG_INFO:
            kprintf(NONE, "[ \033[90muAcpi Info\033[0m ] %s", string);
            break;
        case UACPI_LOG_TRACE:
            kprintf(NONE, "[ \033[90muAcpi Trace\033[0m ] %s", string);
            break;
        case UACPI_LOG_DEBUG:
            kprintf(NONE, "[ \033[90muAcpi Debug\033[0m ] %s", string);
            break;
        case UACPI_LOG_WARN:
            kprintf(NONE, "[ \033[90muAcpi Warning\033[0m ] %s", string);
            break;
        case UACPI_LOG_ERROR:
            kprintf(NONE, "[ \033[90muAcpi Error\033[0m ] %s", string);
            break;
    }
}