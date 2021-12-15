#pragma once

#include "util/concepts.h"



#define ARC_CREATE_BITMASK_ENUM(E)                                                                                      \
                                                                                                                        \
    static_assert(ScopedEnum<E>, "E must be a scoped enum type");                                                       \
                                                                                                                        \
    constexpr static E operator&(E a, E b) noexcept {                                                                   \
        return static_cast<E>(static_cast<std::underlying_type_t<E>>(a) & static_cast<std::underlying_type_t<E>>(b));   \
    }                                                                                                                   \
                                                                                                                        \
    constexpr static E operator|(E a, E b) noexcept {                                                                   \
        return static_cast<E>(static_cast<std::underlying_type_t<E>>(a) | static_cast<std::underlying_type_t<E>>(b));   \
    }                                                                                                                   \
                                                                                                                        \
    constexpr static E operator^(E a, E b) noexcept {                                                                   \
        return static_cast<E>(static_cast<std::underlying_type_t<E>>(a) ^ static_cast<std::underlying_type_t<E>>(b));   \
    }                                                                                                                   \
                                                                                                                        \
    constexpr static E operator~(E a) noexcept {                                                                        \
        return static_cast<E>(~static_cast<std::underlying_type_t<E>>(a));                                              \
    }                                                                                                                   \
                                                                                                                        \
    constexpr static E& operator&=(E& a, E b) noexcept {                                                                \
        a = a & b;                                                                                                      \
        return a;                                                                                                       \
    }                                                                                                                   \
                                                                                                                        \
    constexpr static E& operator|=(E& a, E b) noexcept {                                                                \
        a = a | b;                                                                                                      \
        return a;                                                                                                       \
    }                                                                                                                   \
                                                                                                                        \
    constexpr static E& operator^=(E& a, E b) noexcept {                                                                \
        a = a ^ b;                                                                                                      \
        return a;                                                                                                       \
    }