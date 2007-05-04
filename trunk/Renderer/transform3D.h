#ifndef Transform3D_h
#define Transform3D_h

#include "vector3d.h"

namespace Quokka3D
{
    //*****************************************************************
    //
    // The Transform3D class represents a rotation and translation
    //
    //*****************************************************************
    class Transform3D
    {
    public:
        Transform3D() { m_location = Vector3D(0.0f, 0.0f, 0.0f); setAngle(0.0f, 0.0f, 0.0f); };
        Transform3D(float x, float y, float z) { m_location = Vector3D(x, y, z); setAngle(0.0f, 0.0f, 0.0f); }

        // default copy ctor and assignment should be ok as no pointers are used

        Vector3D& getLocation();
        void setLocation(const Vector3D& v);

        float getCosAngleX() const;
        float getSinAngleX() const;
        float getCosAngleY() const;
        float getSinAngleY() const;
        float getCosAngleZ() const;
        float getSinAngleZ() const;
        
        float getAngleX() const;
        float getAngleY() const;
        float getAngleZ() const;

        void setAngleX(float angleX);
        void setAngleY(float angleY);
        void setAngleZ(float angleZ);
        void setAngle(float angleX, float angleY, float angleZ);

        void rotateAngleX(float angle);
        void rotateAngleY(float angle);
        void rotateAngleZ(float angle);
        void rotateAngle(float angleX, float angleY, float angleZ);

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


    //*****************************************************************
    //
    // Inline functions of the Transform3D class 
    //
    //*****************************************************************
    inline Vector3D& Transform3D::getLocation()
    { 
        return m_location; 
    }

    inline void Transform3D::setLocation(const Vector3D& v) 
    { 
        m_location = v; 
    } 

    inline float Transform3D::getCosAngleX() const 
    { 
        return m_cosAngleX; 
    }

    inline float Transform3D::getSinAngleX() const 
    { 
        return m_sinAngleX; 
    }

    inline float Transform3D::getCosAngleY() const 
    { 
        return m_cosAngleY; 
    }

    inline float Transform3D::getSinAngleY() const 
    { 
        return m_sinAngleY; 
    }

    inline float Transform3D::getCosAngleZ() const 
    { 
        return m_cosAngleZ; 
    }

    inline float Transform3D::getSinAngleZ() const 
    { 
        return m_sinAngleZ; 
    }

    inline float Transform3D::getAngleX() const 
    { 
        return atan2(m_sinAngleX, m_cosAngleX); 
    }

    inline float Transform3D::getAngleY() const 
    { 
        return atan2(m_sinAngleY, m_cosAngleY); 
    }

    inline float Transform3D::getAngleZ() const 
    { 
        return atan2(m_sinAngleZ, m_cosAngleZ); 
    }

    inline void Transform3D::setAngleX(float angleX) 
    { 
        m_cosAngleX = cos(angleX); 
        m_sinAngleX = sin(angleX); 
    }

    inline void Transform3D::setAngleY(float angleY) 
    { 
        m_cosAngleY = cos(angleY); 
        m_sinAngleY = sin(angleY); 
    }

    inline void Transform3D::setAngleZ(float angleZ) 
    { 
        m_cosAngleZ = cos(angleZ); 
        m_sinAngleZ = sin(angleZ); 
    }

    inline void Transform3D::setAngle(float angleX, float angleY, float angleZ) 
    { 
        setAngleX(angleX); 
        setAngleY(angleY); 
        setAngleZ(angleZ); 
    }

    inline void Transform3D::rotateAngleX(float angle) 
    { 
        if (!float_equals(angle, 0.0f)) 
            setAngleX(getAngleX() + angle); 
    }

    inline void Transform3D::rotateAngleY(float angle) 
    { 
        if (!float_equals(angle, 0.0f)) 
            setAngleY(getAngleY() + angle); 
    }

    inline void Transform3D::rotateAngleZ(float angle) 
    { 
        if (!float_equals(angle, 0.0f)) 
            setAngleZ(getAngleZ() + angle); 
    }

    inline void Transform3D::rotateAngle(float angleX, float angleY, float angleZ)
    {
        rotateAngleX(angleX);
        rotateAngleY(angleY);
        rotateAngleZ(angleZ);
    }

} // Quokka3D

#endif // Transform3D_h