#include <limits>
#include <cmath>
#include <cassert>
#include <sstream>
#include <algorithm>
#include "vector3d.h"
#include "transform3D.h"
#include "viewwindow.h"
#include "polygon3D.h"

using namespace Quokka3D;
using namespace std;



//**************************************************************************
//
// Global functions
//
//**************************************************************************
ostream& operator<< (ostream &os, Vector3D& v)
{
    return os << fixed << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}


ostream& operator<< (ostream &os, const Vector3D& v)
{
    return os << fixed << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}


ostream& operator<< (ostream &os, Polygon3D& p)
{
    os << "[poly: " << p.getNumVertices() << " vertices]\n";

    for (int i = 0; i < p.getNumVertices(); ++i)
    {
        os << fixed << i << ": " << p[i] << "\n";
    }
    
    os << "[/poly]";
    return os;
}


ostream& operator<< (ostream &os, const Polygon3D& p)
{
    os << "[poly: " << p.getNumVertices() << " vertices]\n";

    for (int i = 0; i < p.getNumVertices(); ++i)
    {
        os << fixed << i << ": " << p[i] << "\n";
    }
    
    os << "[/poly]";
    return os;
}


//************************************************************************ 
// 
// Vector3D members
//
//************************************************************************ 
Vector3D::Vector3D() { x = y = z = 0.0f; }

Vector3D::Vector3D(float x1, float y1, float z1) : x(x1), y(y1), z(z1) {}

Vector3D& Vector3D::operator += (const Vector3D& v)
{
    x += v.x;
    y += v.y;
    z += v.z;

    return *this;
}

Vector3D& Vector3D::operator -= (const Vector3D& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;

    return *this;
}

Vector3D& Vector3D::operator *= (float k)
{
    x *= k;
    y *= k;
    z *= k;

    return *this;
}

Vector3D& Vector3D::operator /= (float k)
{
    assert(k > FLT_EPSILON);     // check for divide-by-zero

    x /= k;
    y /= k;
    z /= k;

    return *this;
}

bool Vector3D::operator==(const Vector3D& v) const
{
    return (fabs(x - v.x) < FLT_EPSILON 
        &&  fabs(y - v.y) < FLT_EPSILON 
        &&  fabs(z - v.z) < FLT_EPSILON) 
        ? true : false;
}


bool Vector3D::operator!=(const Vector3D& v) const
{
    return (fabs(x - v.x) > FLT_EPSILON 
        ||  fabs(y - v.y) > FLT_EPSILON 
        ||  fabs(z - v.z) > FLT_EPSILON) 
        ? true : false;
}

string Vector3D::toString() const
{
    ostringstream ostr;

    ostr << fixed << "(" << x << ", " << y << ", " << z << ")" << endl;
    return ostr.str();
}


/**
    Rotate this vector around the x axis the specified amount,
    using pre-computed cosine and sine values of the angle to
    rotate.
*/
void Vector3D::rotateX(float cosAngle, float sinAngle) {
    float newY = y*cosAngle - z*sinAngle;
    float newZ = y*sinAngle + z*cosAngle;
    y = newY;
    z = newZ;
}


/**
    Rotate this vector around the y axis the specified amount,
    using pre-computed cosine and sine values of the angle to
    rotate.
*/
void Vector3D::rotateY(float cosAngle, float sinAngle) {
    float newX = z*sinAngle + x*cosAngle;
    float newZ = z*cosAngle - x*sinAngle;
    x = newX;
    z = newZ;
}


/**
    Rotate this vector around the z axis the specified amount,
    using pre-computed cosine and sine values of the angle to
    rotate.
*/
void Vector3D::rotateZ(float cosAngle, float sinAngle) {
    float newX = x*cosAngle - y*sinAngle;
    float newY = x*sinAngle + y*cosAngle;
    x = newX;
    y = newY;
}


/**
    Adds the specified transform to this vector. This vector
    is first rotated, then translated.
*/
void Vector3D::add(Transform3D& xform) {

    // rotate
    addRotation(xform);

    // translate
    *this += xform.getLocation();
}


/**
    Subtracts the specified transform to this vector. This
    vector translated, then rotated.
*/
void Vector3D::subtract(Transform3D& xform) {

    // translate
    *this -= xform.getLocation();

    // rotate
    subtractRotation(xform);
}


/**
    Rotates this vector with the angle of the specified
    transform.
*/
void Vector3D::addRotation(Transform3D& xform) {
    rotateX(xform.getCosAngleX(), xform.getSinAngleX());
    rotateZ(xform.getCosAngleZ(), xform.getSinAngleZ());
    rotateY(xform.getCosAngleY(), xform.getSinAngleY());
}


/**
    Rotates this vector with the opposite angle of the
    specified transform.
*/
void Vector3D::subtractRotation(Transform3D& xform) {
    // note that sin(-x) == -sin(x) and cos(-x) == cos(x)
    rotateY(xform.getCosAngleY(), -xform.getSinAngleY());
    rotateZ(xform.getCosAngleZ(), -xform.getSinAngleZ());
    rotateX(xform.getCosAngleX(), -xform.getSinAngleX());
}



