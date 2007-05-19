#ifndef solidpolygonrenderer_h
#define solidpolygonrenderer_h

#include "polygonrenderer.h"

namespace Quokka3D
{
    class SolidPolygonRenderer : public PolygonRenderer
    {
    public:
        SolidPolygonRenderer() {}
        SolidPolygonRenderer(Transform3D& camera, ViewWindow& viewWindow) { init(camera, viewWindow, true); }
        SolidPolygonRenderer(Transform3D& camera, ViewWindow& viewWindow, bool clearViewEveryFrame) 
            { init(camera, viewWindow, clearViewEveryFrame); }



    protected:
        void drawCurrentPolygon();
    	
    private:
       
    };

} // Quokka3D

#endif // solidpolygonrenderer_h