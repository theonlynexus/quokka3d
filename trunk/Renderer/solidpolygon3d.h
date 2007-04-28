#ifndef solidpolygon3d_h
#define solidpolygon3d_h

#include "math3d.h"

namespace Quokka3D
{
    class SolidPolygon3D : public Polygon3D
    {
    public:
        SolidPolygon3D() : Polygon3D() {}
        SolidPolygon3D(Vector3D& v0, Vector3D& v1, Vector3D& v2) : Polygon3D(v0, v1, v2) {}
        SolidPolygon3D(Vector3D& v0, Vector3D& v1, Vector3D& v2, Vector3D& v3) : Polygon3D(v0, v1, v2, v3) {}
        SolidPolygon3D(Vec3DArray& v) : Polygon3D(v) {}

        unsigned int getColor() const { return m_color; }
        void setColor(unsigned int color) { m_color = color; }



    protected:
    private:
         unsigned int m_color;   // 32-bit color XRGB
    };

}

#endif // solidpolygon3d_h