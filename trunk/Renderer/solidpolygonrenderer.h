#ifndef solidpolygonrenderer_h
#define solidpolygonrenderer_h

#include "polygonrenderer.h"

namespace Quokka3D
{
    class SolidPolygonRenderer : public PolygonRenderer
    {
    public:
        SolidPolygonRenderer() {}
        SolidPolygonRenderer(const Transform3D& camera, const ViewWindow& viewWindow) { init(camera, viewWindow, true); }
        SolidPolygonRenderer(const Transform3D& camera, const ViewWindow& viewWindow, bool clearViewEveryFrame) 
            { init(camera, viewWindow, clearViewEveryFrame); }



    protected:
        void drawCurrentPolygon();
    	
    private:
       
    };

} // Quokka3D

#endif // solidpolygonrenderer_h