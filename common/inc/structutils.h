#pragma once
#include <stddef.h>

#define CHECK_SIZE(type, size) static_assert(sizeof(type) == size, #type " must be " #size " bytes");
#define CHECK_OFFSET(type, field, offset) static_assert(offsetof(type, field) == offset, #type "::" #field " must be at offset " #offset)

#define UNKNOWN_SIZE(type)

#define CAT_WORKAROUND2(a, b) a ## b
#define CAT_WORKAROUND(a, b) CAT_WORKAROUND2(a, b)

#define UNKNOWN(size) char CAT_WORKAROUND(__unk, __COUNTER__) [size]
#define PADDING(size) char CAT_WORKAROUND(__pad, __COUNTER__) [size]