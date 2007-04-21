#ifndef MATH3D_H
#define MATH3D_H

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


namespace Soft3D
{
    //===========================================================================
    //
    // Forward declarations
    //
    //===========================================================================
    class Vector3D;
    class Transform3D;


    //===========================================================================
    //
    // General purpose global utilities
    //
    //===========================================================================
    // #define PI ((float)3.141592654f)
  
    // #define DegToRad( degree ) ((degree) * (PI / 180.0f))
    // #define RadToDeg( radian ) ((radian) * (180.0f / PI))

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

        float length() const { return sqrt(x*x + y*y + z*z); }
        void  normalize() { *this /= length(); }
        float dot(const Vector3D& v) const {  return x*v.x + y*v.y + z*v.z; }
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


    /*
        Sets this vector to the cross product of the two
        specified vectors. Either of the specified vectors can
        be this vector.
    */
    inline void Vector3D::cross(const Vector3D& u, const Vector3D& v) 
    {
        x = u.y * v.z - u.z * v.y;
        y = u.z * v.x - u.x * v.z;
        z = u.x * v.y - u.y * v.x;
    }


    //*****************************************************************
    //
    // The Transform3D class represents a rotation and translation
    //
    //*****************************************************************
    class Transform3D
    {
    public:
        Transform3D() { setAngle(0.0f, 0.0f, 0.0f); };
        Transform3D(float x, float y, float z) { m_location = Vector3D(x, y, z); setAngle(0.0f, 0.0f, 0.0f); }

        // default copy ctor and assignment should be ok as no pointers are used

        Vector3D getLocation() const { return m_location; }

        void setLocation(const Vector3D& v) { m_location = v; } 

        float getCosAngleX() const { return m_cosAngleX; }

        float getSinAngleX() const { return m_sinAngleX; }

        float getCosAngleY() const { return m_cosAngleY; }

        float getSinAngleY() const { return m_sinAngleY; }

        float getCosAngleZ() const { return m_cosAngleZ; }

        float getSinAngleZ() const { return m_sinAngleZ; }

        float getAngleX() const { return atan2(m_sinAngleX, m_cosAngleX); }

        float getAngleY() const { return atan2(m_sinAngleY, m_cosAngleY); }

        float getAngleZ() const { return atan2(m_sinAngleZ, m_cosAngleZ); }

        void setAngleX(float angleX) { m_cosAngleX = cos(angleX); m_sinAngleX = sin(angleX); }

        void setAngleY(float angleY) { m_cosAngleY = cos(angleY); m_sinAngleY = sin(angleY); }

        void setAngleZ(float angleZ) { m_cosAngleZ = cos(angleZ); m_sinAngleZ = sin(angleZ); }

        void setAngle(float angleX, float angleY, float angleZ) { setAngleX(angleX); setAngleY(angleY); setAngleZ(angleZ); }

        void rotateAngleX(float angle) { if (!float_equals(angle, 0.0f)) setAngleX(getAngleX() + angle); }

        void rotateAngleY(float angle) { if (!float_equals(angle, 0.0f)) setAngleY(getAngleY() + angle); }

        void rotateAngleZ(float angle) { if (!float_equals(angle, 0.0f)) setAngleZ(getAngleZ() + angle); }

        void rotateAngle(float angleX, float angleY, float angleZ)
        {
            rotateAngleX(angleX);
            rotateAngleY(angleY);
            rotateAngleZ(angleZ);
        }

    protected:
        Vector3D m_location;

    private:
        float m_cosAngleX;
        float m_sinAngleX;
        float m_cosAngleY;
        float m_sinAngleY;
        float m_cosAngleZ;
        float m_sinAngleZ;

    }; // Transform3D


    typedef std::vector<Vector3D> Vec3DArray;
    class ViewWindow;  // forward declaration for use in Polygon3D class

    //*****************************************************************
    //
    // The Polygon3D class represents a 3D polygon consisting of an
    // array (vector) of vertices. All vertices are assumed to be on
    // the same plane.
    //
    //*****************************************************************
    class Polygon3D
    {
    public:
        Polygon3D();
        Polygon3D(Vector3D&, Vector3D&, Vector3D&);
        Polygon3D(Vector3D&, Vector3D&, Vector3D&, Vector3D&);
        Polygon3D(Vec3DArray&);

        // default assignment and copy ctor should work ok
        
        Vector3D& operator[](const size_t idx) { return m_vec3DArray[idx]; }
        const Vector3D& operator[](const size_t idx) const { return m_vec3DArray[idx]; }

        // assignment operators
        Polygon3D& operator += (const Vector3D&);
        Polygon3D& operator -= (const Vector3D&);

        size_t getNumVertices() const { return m_numVertices; }
        void project(ViewWindow&);
        void add(Transform3D&);
        void subtract(Transform3D&);
        Vector3D& calcNormal();
        Vector3D getNormal() const { return m_normal; }
        void setNormal(const Vector3D& v) { m_normal = v; }
        bool isFacing(const Vector3D& v) const;


    private:
        Vec3DArray m_vec3DArray ;   // A polygon is a vector(an array really) of Vector3D
        size_t m_numVertices;       // The number of vertices in the polygon
        Vector3D m_normal;          // The normalized normal vector for the polygon

    };  // Polygon3D


} // Soft3D




// Output operators for Vector3D
std::ostream& operator<< ( std::ostream&, Soft3D::Vector3D& );
std::ostream& operator<< ( std::ostream&, const Soft3D::Vector3D&);

// Output operators for Polygon3D
std::ostream& operator<< ( std::ostream&, Soft3D::Polygon3D& );
std::ostream& operator<< ( std::ostream&, const Soft3D::Polygon3D& );

#endif // MATH3D_H