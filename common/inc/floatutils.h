#pragma once
#include "types.h"

// TODO makeFloat for halfFloat

inline singleFloat makeFloat(float f) {
    singleFloat ret;
    ret.val = f;
    return ret;
}

inline doubleFloat makeFloat(double f) {
    doubleFloat ret;
    ret.val = f;
    return ret;
}

inline singleFloat makeFloat(singleFloat f) {
    return f;
}

inline doubleFloat makeFloat(doubleFloat f) {
    return f;
}

template<typename T>
inline bool isPositive(T f) {
    return !(makeFloat(f).sign);
}

template<typename T>
inline bool isNegative(T f) {
    return !isPositive(f);
}

template<typename T>
inline bool isZero(T f) {
    auto val = makeFloat(f);
    return (val.mantissa == 0) && (val.exponent == 0);
}

template<typename T>
inline bool isPositiveZero(T f) {
    return isPositive(f) && isZero(f);
}

template<typename T>
inline bool isNegativeZero(T f) {
    return isNegative(f) && isZero(f);
}

template<typename T>
inline bool isInfinity(T f) {
    auto val = makeFloat(f);
    return (val.mantissa == 0) && (val.exponent == val.maxExp);
}

template<typename T>
inline bool isPositiveInfinity(T f) {
    return isPositive(f) && isInfinity(f);
}

template<typename T>
inline bool isNegativeInfinity(T f) {
    return isNegative(f) && isInfinity(f);
}

template<typename T>
inline bool isNaN(T f) {
    auto val = makeFloat(f);
    return (val.mantissa != 0) && (val.exponent == val.maxExp);
}

template<typename T>
inline bool isQuiet(T f) {
    auto val = makeFloat(f);
    return !!(f.isQuiet);
}

template<typename T>
inline bool isSignalingNaN(T f) {
    return isNaN(f) && !isQuiet(f);
}

template<typename T>
inline bool isQuietNaN(T f) {
    return isNaN(f) && isQuiet(f);
}

template<typename T>
inline T generateSNaN() {
    auto temp = makeFloat(static_cast<T>(0));
    temp.mantissa = 0;
    temp.exponent = temp.maxExp;
    temp.isQuiet = 0;
    return temp.val;
}

template<typename T>
inline T generateQNaN() {
    auto temp = makeFloat(static_cast<T>(0));
    temp.mantissa = 0;
    temp.exponent = temp.maxExp;
    temp.isQuiet = 1;
    return temp.val;
}