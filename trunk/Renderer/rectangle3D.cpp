#include "rectangle3D.h"

using namespace Quokka3D;

/*
        Creates a rectangle at the origin with a width and height
        of zero.
*/
Rectangle3D::Rectangle3D()
{
    m_origin = Vector3D();
    m_directionU = Vector3D(1.0f, 0.0f, 0.0f);
    m_directionV = Vector3D(0.0f, 1.0f, 0.0f);
    m_width = 0.0f;
    m_height = 0.0f;
    
}


/*
        Creates a new Rectangle3D with the specified origin,
        direction of the base (directionU) and direction of
        the side (directionV).
*/
Rectangle3D::Rectangle3D(const Vector3D& origin, 
                         const Vector3D& directionU, 
                         const Vector3D& directionV, 
                         float width, float height)
{
    m_origin = Vector3D(origin);
    m_directionU = Vector3D(directionU);
    m_directionU.normalize();
    m_directionV = Vector3D(directionV);
    m_directionV.normalize();
    m_width = width;
    m_height = height;
}