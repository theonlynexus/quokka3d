#include <limits>
#include <cmath>
#include <cassert>
#include <sstream>
#include "math3d.h"
#include "viewwindow.h"

using namespace Soft3D;
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

    for (size_t i = 0; i < p.getNumVertices(); ++i)
    {
        os << fixed << i << ": " << p[i] << "\n";
    }
    
    os << "[/poly]";
    return os;
}


ostream& operator<< (ostream &os, const Polygon3D& p)
{
    os << "[poly: " << p.getNumVertices() << " vertices]\n";

    for (size_t i = 0; i < p.getNumVertices(); ++i)
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



//************************************************************************ 
// 
// Polygon3D members
//
//************************************************************************ 
Polygon3D::Polygon3D()
{
    m_numVertices = 0;
}


Polygon3D::Polygon3D(Vector3D& v0, Vector3D& v1, Vector3D& v2)
{
    m_numVertices = 3;
    m_vec3DArray.push_back(v0);
    m_vec3DArray.push_back(v1);
    m_vec3DArray.push_back(v2);
}


Polygon3D::Polygon3D(Vector3D& v0, Vector3D& v1, Vector3D& v2, Vector3D& v3)
{
    m_numVertices = 4;
    m_vec3DArray.push_back(v0);
    m_vec3DArray.push_back(v1);
    m_vec3DArray.push_back(v2);
    m_vec3DArray.push_back(v3);
}


Polygon3D::Polygon3D(Vec3DArray& v)
{
    m_numVertices = v.size();
    m_vec3DArray = v;           // possibly slow?
}


// transform each vertex by adding the vector
Polygon3D& Polygon3D::operator += (const Vector3D& v)
{
    for (size_t i=0; i!=m_numVertices; i++) 
    {
           m_vec3DArray[i] += v;
    }
    return *this;
}


// transform each vertex by subtracting the vector
Polygon3D& Polygon3D::operator -= (const Vector3D& v)
{
    for (size_t i=0; i!=m_numVertices; i++) 
    {
           m_vec3DArray[i] -= v;
    }
    return *this;
}


void Polygon3D::add(Transform3D& xform) 
{
      //addRotation(xform);
        
    *this += xform.getLocation();
}


void Polygon3D::subtract(Transform3D& xform) 
{
    
    *this -= xform.getLocation();
    // subtractRotation(xform);
}


void Polygon3D::project(ViewWindow& view)
{
    for (size_t i=0; i!=m_numVertices; i++) 
    {
        view.project(m_vec3DArray[i]);
    }
}


/*
    Calculates the unit-vector normal of this polygon.
    This method uses the first, second, and third vertices
    to calcuate the normal, so if these vertices are
    collinear, this method will not work. In this case,
    you can get the normal from the bounding rectangle.
    Use setNormal() to explicitly set the normal.
*/
Vector3D& Polygon3D::calcNormal() 
{
    Vector3D temp1(m_vec3DArray[2]);
    Vector3D temp2(m_vec3DArray[0]);

    temp1 -= m_vec3DArray[1];
    temp2 -= m_vec3DArray[1];
    m_normal.cross(temp1, temp2);
    m_normal.normalize();

    return m_normal;
}


/*
    Tests if this polygon is facing the specified location v
*/
bool Polygon3D::isFacing(const Vector3D& v) const
{
    Vector3D temp(v);
    temp -= m_vec3DArray[0];

    return m_normal.dot(temp) >= 0.0f;
}