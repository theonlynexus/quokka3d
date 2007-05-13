#ifndef solidpolygonrenderer_h
#define solidpolygonrenderer_h

#include "polygonrenderer.h"
#include "sdl_framework.h"

namespace Quokka3D
{
    class SolidPolygonRenderer : public PolygonRenderer
    {
    public:
        SolidPolygonRenderer() {sdl_ptr = NULL;}
        SolidPolygonRenderer(SDL_Framework* sdl, Transform3D& camera, ViewWindow& viewWindow) {sdl_ptr = sdl; init(camera, viewWindow, true); }
        SolidPolygonRenderer(SDL_Framework* sdl, Transform3D& camera, ViewWindow& viewWindow, bool clearViewEveryFrame) 
            { sdl_ptr = sdl; init(camera, viewWindow, clearViewEveryFrame); }


        SDL_Framework* sdl_ptr;

    protected:
        void drawCurrentPolygon();
    	
    private:
       
    };

} // Quokka3D

#endif // solidpolygonrenderer_h