#pragma once
#include <stdint.h>

using handle_t = uint32_t;
static const handle_t INVALID_HANDLE = 0xFFFFFFFF;
static const handle_t CURRENT_THREAD = 0xFFFF8000;
static const handle_t CURRENT_PROCESS = 0xFFFF8001;