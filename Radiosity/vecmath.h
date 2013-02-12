////////////////////////////////////////////////////////////
//
// File:  vecmath.h
// Authors:  R. Bailey
// Contributors: Sean Strout
// Last modified: 4/1/11
//
// Description:  This file holds definitions for 3-D vector and
//		3-D point classes.
//
////////////////////////////////////////////////////////////


#ifndef _VECMATH_H_
#define _VECMATH_H_

#include <iostream>
#include <cmath>

using namespace std;

class Point3;

class Vector3 {
public:
    Vector3() : x(0), y(0), z(0) {}
    Vector3(const Vector3& v) : x(v.x), y(v.y), z(v.z) {}
    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    
    Vector3& operator=(const Vector3& a) {
        x = a.x; y = a.y; z = a.z;
        return *this;
    }
    
    double operator[](int n) const { return ((float *) this)[n]; }
    
    Vector3& operator+=(const Vector3& a) {
        x += a.x; y += a.y; z += a.z;
        return *this;
    }
    
    Vector3& operator-=(const Vector3& a) {
        x -= a.x; y -= a.y; z -= a.z;
        return *this;
    }
    
    Vector3& operator*=(double s) {
        x *= s; y *= s; z *= s;
        return *this;
    }
    
    Vector3 operator-() const {
        return Vector3(-x, -y, -z);
    }
    
    Vector3 operator+() const {
        return *this;
    }
    
    float length() const {
        return (float) sqrt(x * x + y * y + z * z);
    }
    
    float lengthSquared() const {
        return x * x + y * y + z * z;
    }
    
    void normalize() {
        float s = 1.0 / (float) sqrt(x * x + y * y + z * z);
        x *= s; y *= s; z *= s;
    }
    
    float x, y, z;
};

// **** Vector3 operators ****

inline Vector3 operator+(const Vector3& a, const Vector3& b) {
    return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline Vector3 operator+(const Vector3& a, float f) {
    return Vector3(a.x + f, a.y + f, a.z + f);
}

inline Vector3 operator-(const Vector3& a, const Vector3& b) {
    return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline Vector3 operator*(double s, const Vector3& v) {
    return Vector3(s * v.x, s * v.y, s * v.z);
}

inline Vector3 operator*(const Vector3& v, double s) {
    return Vector3(s * v.x, s * v.y, s * v.z);
}

// dot product
inline float operator*(const Vector3& a, const Vector3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// cross product
inline Vector3 operator^(const Vector3& a, const Vector3& b) {
    return Vector3(a.y * b.z - a.z * b.y,
                   a.z * b.x - a.x * b.z,
                   a.x * b.y - a.y * b.x);
}

inline bool operator==(const Vector3& a, const Vector3& b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

inline bool operator!=(const Vector3& a, const Vector3& b) {
    return a.x != b.x || a.y != b.y || a.z != b.z;
}

inline Vector3 operator/(const Vector3& v, double s) {
    double is = 1 / s;
    return Vector3(is * v.x, is * v.y, is * v.z);
}

inline ostream& operator<<(ostream& os, const Vector3& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

#endif /* _VECMATH_H_ */
