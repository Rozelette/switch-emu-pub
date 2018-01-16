#include <chrono>

#include "time.h"

namespace kernel::time {

uint64_t getTimeInNanoseconds() {
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

} // kernel::time