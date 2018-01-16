#pragma once
#include <stdint.h>
#include <limits>
// TODO asserts?

template<typename T>
constexpr bool testBit(T val, size_t pos) {
    return (val & (static_cast<T>(1) << pos)) != 0;
}

template<size_t pos, typename T>
constexpr bool testBit(T val) {
    return (val & (static_cast<T>(1) << (pos))) != 0;
}

template<typename T>
constexpr T bitmask(T size) {
    return (size == sizeof(T) * 8)? std::numeric_limits<T>::max() : (static_cast<T>(1) << size) - 1;
}

template<size_t size, typename T>
constexpr T bitmask() {
    return (static_cast<T>(1) << (size)) - 1;
}

template<>
constexpr uint32_t bitmask<32, uint32_t>() {
    return 0xFFFFFFFF;
}

template<>
constexpr uint64_t bitmask<64, uint64_t>() {
    return 0xFFFFFFFFFFFFFFFF;
}

template<typename T>
constexpr T bitRange(T val, T start, T end) {
    return (val >> start) & bitmask(end - start + 1);
}

template<size_t start, size_t end, typename T>
constexpr T bitRange(T val) {
    return (val >> start) & bitmask<end - start + 1, T>();
}

template<typename T>
constexpr T signExtend(T val, T size) {
    return testBit(val, size - 1)? val | ~bitmask(size) : val;
}

template<unsigned size, typename T>
constexpr T signExtend(T val) {
    return testBit<size - 1>(val)? val | ~bitmask<size, T>() : val;
}

template<typename T>
constexpr T constLog2(T num) {
    return (num == 1)? 0 : 1 + constLog2(num >> 1);
}
