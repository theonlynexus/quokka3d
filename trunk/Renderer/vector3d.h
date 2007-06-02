// ***************************************************************
//  math3d   version:  1.0   ·  date: 04/21/2007
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************
#ifndef MATH3D_H
#define MATH3D_H

// Avoid opening the header again for MS VC++
#ifdef _MSC_VER
#pragma once
#endif


#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <cassert>


//===========================================================================
//
// Global templates for operator overloads
// (See "More Effective C++", Item 22)
//
//===========================================================================
template<typename T>
const T operator+(const T& lhs, const T& rhs)
{ 
    return T(lhs) += rhs;
}


template<typename T>
const T operator-(const T& lhs, const T& rhs)
{ 
    return T(lhs) -= rhs;
}


template<typename T>
const T operator/(const T& lhs, float rhs) 
{
    assert(rhs > FLT_EPSILON);          // check for divide-by-zero
    return T(lhs) /= rhs;
}


template<typename T>
const T operator*(const T& lhs, float rhs)
{
    return T(lhs) *= rhs;
}


template<typename T>
const T operator*(float lhs, const T& rhs)
{
    return T(rhs) *= lhs;
}


namespace Quokka3D
{
    //===========================================================================
    //
    // Forward declarations
    //
    //===========================================================================
    // class Vector3D;
    class Transform3D;


    //===========================================================================
    //
    // General purpose global utilities
    //
    //===========================================================================
    const float PI = 3.141592654f;

    // Returns true if lhs == rhs within tolerance
    inline bool float_equals(float lhs, float rhs)
    {
        return fabs(lhs - rhs) < FLT_EPSILON ? true : false;
    }

    inline float DegToRad(float degree) 
    {
        return degree * (PI / 180.0f);
    }

    inline float RadToDeg(float radian) 
    {
        return radian * (180.0f / PI);
    }


    //===========================================================================
    //
    // Math Classes
    //
    //===========================================================================
    struct Rectangle
    {
        int x, y, width, height;
    };


    class Transformable 
    {
    public:

        //virtual Vector3D operator+(const Vector3D&) const = 0;
        //virtual Vector3D operator-(const Vector3D&) const = 0;
        //virtual Vector3D operator*(const float) const = 0;
        //virtual Vector3D operator/(const float) const = 0;
        // virtual void add(Transform3D xform) = 0;
        // virtual void subtract(Transform3D xform) = 0;
        // virtual void addRotation(Transform3D xform) = 0;
        // virtual void subtractRotation(Transform3D xform) = 0;
    };


    //*****************************************************************
    //
    // The Vector3D class represents a 3D vector or vertex consisting
    // of 3 coordinates.
    //
    //*****************************************************************
    class Vector3D : public Transformable
    {
    public:
        float x;
        float y; 
        float z;

        Vector3D();

        // Default copy constructor should work (bitwise copy)
        // Vector3D(const Vector3D& vec) {}

        Vector3D(float x, float y, float z);

        // assignment operators
        Vector3D& operator += (const Vector3D&);
        Vector3D& operator -= (const Vector3D&);
        Vector3D& operator *= (float);
        Vector3D& operator /= (float);

        // equality operators
        bool operator == (const Vector3D&) const;
        bool operator != (const Vector3D&) const;

        std::string toString() const;
        // void setTo(float x, float y, float z);

        float length() const;
        void  normalize();
        float dot(const Vector3D&) const;
        void  cross(const Vector3D&, const Vector3D&);

        void add(Transform3D&);
        void subtract(Transform3D&);
        void addRotation(Transform3D&);
        void subtractRotation(Transform3D&);

    private:
        void rotateX(float cosAngle, float sinAngle);
        void rotateY(float cosAngle, float sinAngle);
        void rotateZ(float cosAngle, float sinAngle);


    };  // Vector3D


    //*****************************************************************
    //
    // Inline functions of the Transform3D class 
    //
    //*****************************************************************


    inline float Vector3D::length() const 
    { 
        return sqrt(x*x + y*y + z*z); 
    }

    inline void Vector3D::normalize() 
    { 
        *this /= length(); 
    }

    inline float Vector3D::dot(const Vector3D& v) const 
    {  
        return x*v.x + y*v.y + z*v.z; 
    }

    //    Sets this vector to the cross product of the two
    //    specified vectors.
    inline void Vector3D::cross(const Vector3D& u, const Vector3D& v) 
    {
        float x1 = u.y * v.z - u.z * v.y;
        float y1 = u.z * v.x - u.x * v.z;
        float z1 = u.x * v.y - u.y * v.x;
        this->x = x1;
        this->y = y1;
        this->z = z1;
    }






} // Quokka3D




// Output operators for Vector3D
std::ostream& operator<< ( std::ostream&, Quokka3D::Vector3D& );
std::ostream& operator<< ( std::ostream&, const Quokka3D::Vector3D&);



#endif // MATH3D_H