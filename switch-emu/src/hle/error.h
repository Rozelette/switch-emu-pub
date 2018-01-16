#pragma once

#define ERR_SUCCESS 0

#define ERR_CONS(module, desc) ((module) | ((desc) << 8))

// TODO fill out
#define ERR_MODULE_SM 0x15