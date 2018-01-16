#pragma once
#include "log.h"

#define UNREACHABLE() \
    logger::error("UNREACHABLE executed in %s at line %d", __FILE__, __LINE__)