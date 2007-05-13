
#include <stdlib.h>
#include <iostream>
#include "vector3d.h"
#include "primitives.h"
#include "viewwindow.h"
#include "solidpolygon3d.h"
#include "polygonrenderer.h"
#include "solidpolygonrenderer.h"
#include "sdl_framework.h"

using namespace Quokka3D;
using namespace std;


//#define MAKE_RGB32(r, g, b) ((((BYTE)(b)|((WORD)((BYTE)(g))<< 8 ))|(((DWORD)(BYTE)(r))<< 16 )))
Uint32 black;
Uint32 red;
Uint32 green;
Uint32 blue;



class CMyEngine: public SDL_Framework
{

private:
    bool quit;
    float x, y, z, angleY;  // camera location and current rotation angle
    vector<SolidPolygon3D> polys;
    PolygonRenderer* polygonRenderer ;
    bool keyW, keyS, keyA, keyD, keyUp, keyDown, keyRotLeft, keyRotRight, keyTiltLeft, keyTiltRight;
    float mouse_x, mouse_y, curr_mouse_x, curr_mouse_y, diff_x, diff_y;
//    Timer timer;
    int numFrames;
    bool firstRun, mouseMoved;


public:
    

    void createPolygons();
    void AdditionalInit ();
    void Think	    ( const int& iElapsedTime );
    void Render	    ( SDL_Surface* pDestSurface );

    void KeyUp  	    (const int& iKeyEnum);
    void KeyDown	    (const int& iKeyEnum);

    void MouseMoved     (const int& iButton, 
        const int& iX, 
        const int& iY, 
        const int& iRelX, 
        const int& iRelY);

    void MouseButtonUp  (const int& iButton, 
        const int& iX, 
        const int& iY, 
        const int& iRelX, 
        const int& iRelY);

    void MouseButtonDown(const int& iButton, 
        const int& iX, 
        const int& iY, 
        const int& iRelX, 
        const int& iRelY);

    void WindowInactive();
    void WindowActive();

    void End();
};




// Entry point
int main(int argc, char* argv[])  // <- this must match exactly, since SDL rewrites it
{
    CMyEngine Engine;

    Engine.SetTitle( "Loading..." );
    Engine.Init();

    Engine.SetTitle( "SDL Testing!" );
    Engine.Start();

    Engine.SetTitle( "Quitting..." );

    return 0;
}


// Create a house (convex polyhedra)
// All faces must use anti-clockwise winding order
void CMyEngine::createPolygons() {
    SolidPolygon3D poly;

    // walls
    poly = SolidPolygon3D(
        Vector3D(-200, 0, -1000),
        Vector3D(200, 0, -1000),
        Vector3D(200, 250, -1000),
        Vector3D(-200, 250, -1000));
    poly.setColor(red);
    polys.push_back(poly);
    poly = SolidPolygon3D(
        Vector3D(-200, 0, -1400),
        Vector3D(-200, 250, -1400),
        Vector3D(200, 250, -1400),
        Vector3D(200, 0, -1400));
    poly.setColor(green);
    polys.push_back(poly);
    poly = SolidPolygon3D(
        Vector3D(-200, 0, -1400),
        Vector3D(-200, 0, -1000),
        Vector3D(-200, 250, -1000),
        Vector3D(-200, 250, -1400));
    poly.setColor(blue);
    polys.push_back(poly);
    poly = SolidPolygon3D(
        Vector3D(200, 0, -1000),
        Vector3D(200, 0, -1400),
        Vector3D(200, 250, -1400),
        Vector3D(200, 250, -1000));
    poly.setColor(red);
    polys.push_back(poly);

    // door and windows
    poly = SolidPolygon3D(
        Vector3D(0, 0, -1000),
        Vector3D(75, 0, -1000),
        Vector3D(75, 125, -1000),
        Vector3D(0, 125, -1000));
    poly.setColor(green);
    polys.push_back(poly);
    poly = SolidPolygon3D(
        Vector3D(-150, 150, -1000),
        Vector3D(-100, 150, -1000),
        Vector3D(-100, 200, -1000),
        Vector3D(-150, 200, -1000));
    poly.setColor(blue);
    polys.push_back(poly);

    // roof
    poly = SolidPolygon3D(
        Vector3D(-200, 250, -1000),
        Vector3D(200, 250, -1000),
        Vector3D(75, 400, -1200),
        Vector3D(-75, 400, -1200));
    poly.setColor(blue);
    polys.push_back(poly);
    poly = SolidPolygon3D(
        Vector3D(-200, 250, -1400),
        Vector3D(-200, 250, -1000),
        Vector3D(-75, 400, -1200));
    poly.setColor(blue);
    polys.push_back(poly);
    poly = SolidPolygon3D(
        Vector3D(200, 250, -1400),
        Vector3D(-200, 250, -1400),
        Vector3D(-75, 400, -1200),
        Vector3D(75, 400, -1200));
    poly.setColor(blue);
    polys.push_back(poly);
    poly = SolidPolygon3D(
        Vector3D(200, 250, -1000),
        Vector3D(200, 250, -1400),
        Vector3D(75, 400, -1200));
    poly.setColor(blue);
    polys.push_back(poly);
}


void CMyEngine::AdditionalInit()
{
    quit = false;
    keyW = false;
    keyS = false;
    keyA = false;
    keyD = false;
    keyUp = false;
    keyDown = false;
    keyRotLeft = false;
    keyRotRight = false;
    keyTiltLeft = false;
    keyTiltRight = false;
    
    firstRun = true;
    x = 0.0f;
    y = 100.0f;
    z = -500.0f;
    numFrames = 0;
    diff_x = 0.0f;
    diff_y = 0.0f;
    mouseMoved = false;
    black = SDL_MapRGB(m_pScreen->format, 0x00, 0x00, 0x00);
    red = SDL_MapRGB(m_pScreen->format, 0xff, 0x00, 0x00);
    green = SDL_MapRGB(m_pScreen->format, 0x00, 0xff, 0x00);
    blue = SDL_MapRGB(m_pScreen->format, 0x00, 0x00, 0xff);

    createPolygons();
    ViewWindow view(0, 0,   m_iWidth, m_iHeight, DegToRad(75));
    Transform3D camera(x, y, z);
    polygonRenderer = new SolidPolygonRenderer(this, camera, view); //remember to delete
}

void CMyEngine::Think( const int& iElapsedTime )
{
    //cout << "thinking...\n";
}

void CMyEngine::Render( SDL_Surface* pDestSurface )
{
    polygonRenderer->startFrame();

    for (int i=0; i!=polys.size(); ++i)
    {
        polygonRenderer->draw(&polys[i]);
    }
}

void CMyEngine::KeyDown(const int& iKeyEnum)
{        
    switch (iKeyEnum)
    {
    case SDLK_LEFT:
        cout << "<- pressed\n";
        break;
    case SDLK_RIGHT:
        cout << "-> pressed\n";
        break;
    case SDLK_UP:
        
        break;
    case SDLK_DOWN:
        
        break;
    }
}


void CMyEngine::KeyUp(const int& iKeyEnum)
{
    switch (iKeyEnum)
    {
    case SDLK_LEFT:
        // Left arrow released
        break;
    case SDLK_RIGHT:
        // Right arrow released
        break;
    case SDLK_UP:
        // Up arrow released
        break;
    case SDLK_DOWN:
        // Down arrow released
        break;
    }
}

void CMyEngine::MouseMoved(const int& iButton, 
                           const int& iX, 
                           const int& iY, 
                           const int& iRelX, 
                           const int& iRelY)
{
    // Handle mouse movement

    // iX and iY are absolute screen positions
    // iRelX and iRelY are screen position relative to last detected mouse movement
    cout << "iRelx: " << iRelX << "  iRely: " << iRelY << endl;

}

void CMyEngine::MouseButtonUp(const int& iButton, 
                              const int& iX, 
                              const int& iY, 
                              const int& iRelX, 
                              const int& iRelY)
{
    // Handle mouse button released
}

void CMyEngine::MouseButtonDown(const int& iButton, 
                                const int& iX, 
                                const int& iY, 
                                const int& iRelX, 
                                const int& iRelY)
{
    // Handle mouse button pressed
}

void CMyEngine::WindowInactive()
{
    // Pause game
}

void CMyEngine::WindowActive()
{
    // Un-pause game
}


void CMyEngine::End()
{
    delete polygonRenderer;
}
