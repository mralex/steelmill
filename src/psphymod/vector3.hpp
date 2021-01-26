#pragma once

#include "simd/vector.hpp"
using rack::simd::float_4;

struct Vector3 {
    float_4 vec;

    Vector3() {
        vec = 0.f;
    }

    Vector3(float x) {
        vec = x;
        vec[3] = 0.f;
    }

    Vector3(float x, float y, float z) {
        vec[0] = x;
        vec[1] = y;
        vec[2] = z;
        vec[3] = 0.f;
    }

    Vector3(float_4 v) {
        vec = v;
    }


    Vector3& operator=(const Vector3& b) { vec = b.vec; return *this; };
    Vector3& operator=(const float b) { vec = b; vec[3] = 0.f; return *this; };

    Vector3 operator+(const Vector3& b) { return Vector3(vec + b.vec); };
    Vector3 operator+(const float b) { return Vector3(vec + float_4(b)); };

    Vector3 operator-(const Vector3& b) { return Vector3(vec - b.vec); };
    Vector3 operator-(const float b) { return Vector3(vec - float_4(b)); };

    Vector3 operator*(const Vector3& b) { return Vector3(vec * b.vec); };
    Vector3 operator*(const float b) { return Vector3(vec * float_4(b)); };

    float square() { 
        float_4 s = vec * vec;
        return s[0] + s[1] + s[2];    
    };

    float x() { return vec[0]; }
    float y() { return vec[1]; }
    float z() { return vec[2]; }
};
