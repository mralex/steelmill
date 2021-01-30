#pragma once

struct Vector3 {
public:
    float x, y, z;

    Vector3() {
        x = y = z = 0.f;
    }

    Vector3(float value) {
        x = y = z = value;
    }

    Vector3(float _x, float _y, float _z) {
        x = _x;
        y = _y;
        z = _z;
    }

    Vector3& operator=(const Vector3& b) { x = b.x; y = b.y; z = b.z; return *this; };
    Vector3& operator=(const float b) { x = b; y = b; z = b; return *this; };

    Vector3 operator+(const Vector3& b) { return Vector3(x + b.x, y + b.y, z + b.z); };
    Vector3 operator+(const float b) { return Vector3(x + b, y + b, z + b); };

    Vector3 operator-(const Vector3& b) { return Vector3(x - b.x, y - b.y, z - b.z); };
    Vector3 operator-(const float b) { return Vector3(x - b, y - b, z - b); };

    Vector3 operator*(const Vector3& b) { return Vector3(x * b.x, y * b.y, z * b.z); };
    Vector3 operator*(const float b) { return Vector3(x * b, y * b, z * b); };

    float square() { 
        return (x * x) + (y * y) + (z * z);
    };
};
