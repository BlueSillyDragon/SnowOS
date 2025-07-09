#pragma once

#include <cstdint>

// I don't entirely know how this works lol

namespace Spinlock {
    void lock();
    bool isLocked();
    void unlock();
};

namespace TicketSpinlock {
    void lock();
    bool isLocked();
    void unlock();
};