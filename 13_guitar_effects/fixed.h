#ifndef FIXED_POINT_HPP
#define FIXED_POINT_HPP

#include <cstdint>

template <typename T, int fractional_bits>
class fixed_point {
public:
    fixed_point(T value = 0) : value(value) {}

    bool operator>(const fixed_point& other) const {
        return value > other.value;
    }

    bool operator>=(const fixed_point& other) const {
        return value >= other.value;
    }

    bool operator<(const fixed_point& other) const {
        return value < other.value;
    }

    bool operator<=(const fixed_point& other) const {
        return value <= other.value;
    }

    fixed_point operator+(const fixed_point& other) const {
        return fixed_point(value + other.value);
    }

    fixed_point operator-(const fixed_point& other) const {
        return fixed_point(value - other.value);
    }

    fixed_point operator-() const {
        return fixed_point(-value);
    }

    fixed_point operator*(const fixed_point& other) const {
        return fixed_point((static_cast<int64_t>(value) * other.value) >> fractional_bits);
    }

    fixed_point operator/(const fixed_point& other) const {
        return fixed_point((static_cast<int64_t>(value) << fractional_bits) / other.value);
    }

    fixed_point operator>>(int shift) const {
        return fixed_point(value >> shift);
    }

    T get() const {
        return value;
    }

    void set(T int_value) {
        value = int_value;
    }

    T to_int() const {
        return value >> fractional_bits;
    }

    float to_float() const {
        return static_cast<float>(value) / (1 << fractional_bits);
    }

    static fixed_point from_int(T int_value) {
        return fixed_point(int_value << fractional_bits);
    }

    static fixed_point from_float(float float_value) {
        return fixed_point(static_cast<T>(float_value * (1 << fractional_bits)));
    }

private:
    T value;
};


#endif // FIXED_POINT_HPP


