#ifndef solidpolygon3d_h
#define solidpolygon3d_h

#include "vector3d.h"
#include "polygon3D.h"


namespace Quokka3D
{
    class SolidPolygon3D : public Polygon3D
    {
    public:
        SolidPolygon3D() : Polygon3D() {}
        SolidPolygon3D(const Vector3D& v0, const Vector3D& v1, const Vector3D& v2) : Polygon3D(v0, v1, v2) {}
        SolidPolygon3D(const Vector3D& v0, const Vector3D& v1, const Vector3D& v2, const Vector3D& v3) : Polygon3D(v0, v1, v2, v3) {}
        SolidPolygon3D(const Vec3DArray& v) : Polygon3D(v) {}

        unsigned int getColor() const { return m_color; }
        void setColor(unsigned int color) { m_color = color; }

        



    protected:
    private:
         unsigned int m_color;   // 32-bit color XRGB
         

    };

}

#endif // solidpolygon3d_h