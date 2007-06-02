#ifndef Rectangle3D_h
#define Rectangle3D_h

#include "vector3d.h"
#include "transform3D.h"

namespace Quokka3D
{
    class Rectangle3D
    {
    private:
        Vector3D m_origin;
        Vector3D m_directionU;
        Vector3D m_directionV;
        Vector3D m_normal;
        float m_width;
        float m_height;

    public:
        Rectangle3D() ;
        Rectangle3D(const Vector3D& origin, 
                    const Vector3D& directionU, 
                    const Vector3D& directionV, 
                    float width, float height);
    

        // Yes, the following 3 functions return a public reference to
        // private data. That's what Brackeen's original Java code did
        // so I'm doing it here for the time being.
        Vector3D& getOrigin()     { return m_origin; }
        Vector3D& getDirectionU() { return m_directionU; }
        Vector3D& getDirectionV() { return m_directionV; }

        float getWidth() { return m_width; }
        void setWidth(float width) { m_width = width; }
        float getHeight() { return m_height; }
        void setHeight(float height) { m_height = height; }
        
        Vector3D getNormal() 
        {
            if (m_normal == Vector3D()) // if normal not set ie still 0 (efficient?)
            {
                calcNormal();
            }
            return m_normal;
        }

        void setNormal(const Vector3D& n) { m_normal = Vector3D(n); }

        void add(const Vector3D& u) 
        {
            m_origin += u;
            // don't translate direction vectors or size
        }

        void subtract(const Vector3D& u) 
        {
            m_origin -= u;
            // don't translate direction vectors or size
        }

        void add(Transform3D& xform) {
            addRotation(xform);
            add(xform.getLocation());
        }

        void subtract(Transform3D& xform) {
            subtract(xform.getLocation());
            subtractRotation(xform);
        }

        void addRotation(Transform3D& xform) {
            m_origin.addRotation(xform);
            m_directionU.addRotation(xform);
            m_directionV.addRotation(xform);
        }

        void subtractRotation(Transform3D& xform) {
            m_origin.subtractRotation(xform);
            m_directionU.subtractRotation(xform);
            m_directionV.subtractRotation(xform);
        }

    protected:
        Vector3D calcNormal() 
        {
            if (m_normal == Vector3D())  // if normal not set ie still 0 (efficient?)
            {
                m_normal = Vector3D();
            }
            m_normal.cross(m_directionU, m_directionV);
            m_normal.normalize();
            return m_normal;
        }



   
    };

} // Quokka3D

#endif // Rectangle3D_h