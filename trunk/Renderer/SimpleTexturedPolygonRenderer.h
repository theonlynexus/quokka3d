#ifndef SimpleTexturedPolygonRenderer_h
#define SimpleTexturedPolygonRenderer_h

#include "polygonrenderer.h"
#include "rectangle3D.h"
#include "LightPng/LightPng.h"
#include "LightPng/LightZ.h"

namespace Quokka3D
{
    
    class SimpleTexturedPolygonRenderer : public PolygonRenderer
    {
    public:
        SimpleTexturedPolygonRenderer() {}
        SimpleTexturedPolygonRenderer(const Transform3D& camera, 
                                      const ViewWindow& viewWindow,
                                      const std::string& textureFile);
        
        ~SimpleTexturedPolygonRenderer() { delete m_texture; }

        LPNG_Image* loadTexture(const std::string& fileName);
        
    private:
        char* ReadFile( const char *fn, int &dest_size );


    protected:
        void drawCurrentPolygon();
    	
    private:
        Vector3D m_a, m_b, m_c;
        Vector3D m_viewPos;
        Rectangle3D m_textureBounds;
        LPNG_Image *m_texture;            // a pointer to the texture data bits

        

    };

} // Quokka3D

#endif // SimpleTexturedPolygonRenderer_h