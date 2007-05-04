#ifndef Polygon3D_h
#define Polygon3D_h

#include "vector3d.h"



namespace Quokka3D
{

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

        int getNumVertices() const { return m_numVertices; }
        void project(ViewWindow&);
        void add(Transform3D&);
        void subtract(Transform3D&);
        void addRotation(Transform3D&);
        void subtractRotation(Transform3D&);
        Vector3D& calcNormal();
        Vector3D getNormal() const { return m_normal; }
        void setNormal(const Vector3D& v) { m_normal = v; }
        bool isFacing(const Vector3D&) const;
        void ensureCapacity(int length);
        bool clip(float);

    protected:
        void insertVertex(int index, float x, float y, float z);
        void deleteVertex(int index);

    private:
        Vec3DArray m_vec3DArray ;   // A polygon is a vector(an array really) of Vector3D
        int m_numVertices;       // The number of vertices in the polygon
        Vector3D m_normal;          // The normalized normal vector for the polygon


    };  // Polygon3D
} // Quokka3D

// Output operators for Polygon3D
std::ostream& operator<< ( std::ostream&, Quokka3D::Polygon3D& );
std::ostream& operator<< ( std::ostream&, const Quokka3D::Polygon3D& );

#endif // Polygon3D_h