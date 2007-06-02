#include "vector3d.h"
#include "polygon3D.h"
#include "transform3D.h"
#include "viewwindow.h"

using namespace Quokka3D;
using namespace std;

//************************************************************************ 
// 
// Polygon3D members
//
//************************************************************************ 
Polygon3D::Polygon3D()
{
    m_numVertices = 0;
}


Polygon3D::Polygon3D(const Vector3D& v0, const Vector3D& v1, const Vector3D& v2)
{
    m_numVertices = 3;
    m_vec3DArray.push_back(v0);
    m_vec3DArray.push_back(v1);
    m_vec3DArray.push_back(v2);
    calcNormal();
}


Polygon3D::Polygon3D(const Vector3D& v0, const Vector3D& v1, const Vector3D& v2, const Vector3D& v3)
{
    m_numVertices = 4;
    m_vec3DArray.push_back(v0);
    m_vec3DArray.push_back(v1);
    m_vec3DArray.push_back(v2);
    m_vec3DArray.push_back(v3);
    calcNormal();
}


Polygon3D::Polygon3D(const Vec3DArray& v)
{
    m_numVertices = (int)v.size();
    m_vec3DArray = v;           // possibly slow?
    calcNormal();
}


// transform each vertex by adding the vector
Polygon3D& Polygon3D::operator += (const Vector3D& v)
{
    for (int i=0; i!=m_numVertices; i++) 
    {
        m_vec3DArray[i] += v;
    }
    return *this;
}


// transform each vertex by subtracting the vector
Polygon3D& Polygon3D::operator -= (const Vector3D& v)
{
    for (int i = 0; i != m_numVertices; i++) 
    {
        m_vec3DArray[i] -= v;
    }
    return *this;
}


void Polygon3D::add(Transform3D& xform) 
{
    // rotate
    addRotation(xform);

    // translate
    *this += xform.getLocation();
}


void Polygon3D::subtract(Transform3D& xform) 
{
    // translate
    *this -= xform.getLocation();

    // rotate
    subtractRotation(xform);
}


void Polygon3D::addRotation(Transform3D& xform) {
    for (int i=0; i!=m_numVertices; i++) {
        m_vec3DArray[i].addRotation(xform);
    }
    m_normal.addRotation(xform);
}


void Polygon3D::subtractRotation(Transform3D& xform) {
    for (int i=0; i!=m_numVertices; i++) {
        m_vec3DArray[i].subtractRotation(xform);
    }
    m_normal.subtractRotation(xform);
}


void Polygon3D::project(ViewWindow& view)
{
    for (int i = 0; i != m_numVertices; i++) 
    {
        view.project(m_vec3DArray[i]);
    }
}


/*
Calculates the unit-vector normal of this polygon.
This method uses the first, second, and third vertices
to calculate the normal, so if these vertices are
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


// Increase the size of the vertex array by length
void Polygon3D::ensureCapacity(int length) 
{
    if ((int)m_vec3DArray.size() < length) 
    {
        m_vec3DArray.resize(length);

        /*Vector3D[] newV = new Vector3D[length];
        System.arraycopy(v,0,newV,0,v.length);
        for (int i=v.length; i<newV.length; i++) {
        newV[i] = new Vector3D();
        }
        v = newV;*/
    }
}


/**
Clips this polygon so that all vertices are in front of
the clip plane, clipZ (in other words, all vertices
have z <= clipZ).
The value of clipZ should not be 0, as this causes
divide-by-zero problems.
Returns true if the polygon is at least partially in
front of the clip plane.
*/
bool Polygon3D::clip(float clipZ) 
{
    ensureCapacity(m_numVertices * 3);

    bool isCompletelyHidden = true;

    // insert vertices so all edges are either completely
    // in front or behind the clip plane
    for (int i=0; i != m_numVertices; i++) {
        int next = (i + 1) % m_numVertices;
        Vector3D v1(m_vec3DArray[i]);
        Vector3D v2(m_vec3DArray[next]);

        if (v1.z < clipZ) 
        {
            isCompletelyHidden = false;
        }
        // ensure v1.z < v2.z
        if (v1.z > v2.z) 
        {
            std::swap(v1, v2);
        }
        if (v1.z < clipZ && v2.z > clipZ) 
        {
            float scale = (clipZ - v1.z) / (v2.z - v1.z);
            insertVertex(next,
                v1.x + scale * (v2.x - v1.x) ,
                v1.y + scale * (v2.y - v1.y),
                clipZ);
            // skip the vertex we just created
            i++;
        }
    }

    if (isCompletelyHidden) 
    {
        return false;
    }

    // delete all vertices that have z > clipZ
    for (int i = m_numVertices-1; i >= 0; i--) {
        if (m_vec3DArray[i].z > clipZ) {
            deleteVertex(i);
        }
    }

    return (m_numVertices >= 3);
}


/**
Inserts a new vertex at the specified index.
*/
// TODO: use std::vector insert for this?
void Polygon3D::insertVertex(int index, float x, float y, float z)
{
    Vector3D newVertex(m_vec3DArray[m_vec3DArray.size()-1]);

    newVertex.x = x;
    newVertex.y = y;
    newVertex.z = z;

    for (int i = (int)m_vec3DArray.size()-1; i > index; i--) {
        m_vec3DArray[i] = m_vec3DArray[i-1];
    }
    m_vec3DArray[index] = newVertex;
    m_numVertices++;
}


/**
Delete the vertex at the specified index.
*/
// TODO: use std:vector erase for this?
void Polygon3D::deleteVertex(int index) 
{
    Vector3D deleted(m_vec3DArray[index]);

    for (int i = index; i < (int)m_vec3DArray.size()-1; i++) {
        m_vec3DArray[i] = m_vec3DArray[i+1];
    }
    m_vec3DArray[m_vec3DArray.size()-1] = deleted;
    m_numVertices--;
}


/**
Inserts a vertex into this polygon at the specified index.
The exact vertex in inserted (not a copy).
*/
// TODO: use std::vector insert for this?
//void Polygon3D::insertVertex(int index, Vector3D& vertex) 
//{
//    Vector3D[] newV = new Vector3D[numVertices+1];
//    System.arraycopy(v,0,newV,0,index);
//    newV[index] = vertex;
//    System.arraycopy(v,index,newV,index+1,numVertices-index);
//    v = newV;
//    numVertices++;
//}