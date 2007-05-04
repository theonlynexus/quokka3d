// Renderer.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "vector3d.h"
#include "primitives.h"
#include "viewwindow.h"
#include "solidpolygon3d.h"
#include "polygonrenderer.h"
#include "solidpolygonrenderer.h"
#include "PixelToaster.h"


using namespace std;
using namespace Quokka3D;
using namespace PixelToaster;



#define MAKE_RGB32(r, g, b) ((((BYTE)(b)|((WORD)((BYTE)(g))<< 8 ))|(((DWORD)(BYTE)(r))<< 16 )))

const int width = 640;
const int height = 480;

vector<TrueColorPixel> pixels( width * height );    // screen is a linear sequence of pixels

class Application : public Listener
{
public:

    Application()
    {
        quit = false;
        z = -500.0f;
        x = 0.0f;
    }


    // Create a house (convex polyhedra)
    // All faces must use anti-clockwise winding order
    void createPolygons() {
        SolidPolygon3D poly;

        // walls
        poly = SolidPolygon3D(
            Vector3D(-200, 0, -1000),
            Vector3D(200, 0, -1000),
            Vector3D(200, 250, -1000),
            Vector3D(-200, 250, -1000));
        poly.setColor(MAKE_RGB32(255, 255, 0));
        polys.push_back(poly);
        poly = SolidPolygon3D(
            Vector3D(-200, 0, -1400),
            Vector3D(-200, 250, -1400),
            Vector3D(200, 250, -1400),
            Vector3D(200, 0, -1400));
        poly.setColor(MAKE_RGB32(128, 128, 0));
        polys.push_back(poly);
        poly = SolidPolygon3D(
            Vector3D(-200, 0, -1400),
            Vector3D(-200, 0, -1000),
            Vector3D(-200, 250, -1000),
            Vector3D(-200, 250, -1400));
        poly.setColor(MAKE_RGB32(128, 128, 0));
        polys.push_back(poly);
        poly = SolidPolygon3D(
            Vector3D(200, 0, -1000),
            Vector3D(200, 0, -1400),
            Vector3D(200, 250, -1400),
            Vector3D(200, 250, -1000));
        poly.setColor(MAKE_RGB32(128, 128, 0));
        polys.push_back(poly);

        // door and windows
        poly = SolidPolygon3D(
            Vector3D(0, 0, -1000),
            Vector3D(75, 0, -1000),
            Vector3D(75, 125, -1000),
            Vector3D(0, 125, -1000));
        poly.setColor(MAKE_RGB32(2, 40, 90));
        polys.push_back(poly);
        poly = SolidPolygon3D(
            Vector3D(-150, 150, -1000),
            Vector3D(-100, 150, -1000),
            Vector3D(-100, 200, -1000),
            Vector3D(-150, 200, -1000));
        poly.setColor(MAKE_RGB32(25, 40, 40));
        polys.push_back(poly);

        // roof
        poly = SolidPolygon3D(
            Vector3D(-200, 250, -1000),
            Vector3D(200, 250, -1000),
            Vector3D(75, 400, -1200),
            Vector3D(-75, 400, -1200));
        poly.setColor(MAKE_RGB32(220, 0, 0));
        polys.push_back(poly);
        poly = SolidPolygon3D(
            Vector3D(-200, 250, -1400),
            Vector3D(-200, 250, -1000),
            Vector3D(-75, 400, -1200));
        poly.setColor(MAKE_RGB32(128, 0, 0));
        polys.push_back(poly);
        poly = SolidPolygon3D(
            Vector3D(200, 250, -1400),
            Vector3D(-200, 250, -1400),
            Vector3D(-75, 400, -1200),
            Vector3D(75, 400, -1200));
        poly.setColor(MAKE_RGB32(128, 0, 0));
        polys.push_back(poly);
        poly = SolidPolygon3D(
            Vector3D(200, 250, -1000),
            Vector3D(200, 250, -1400),
            Vector3D(75, 400, -1200));
        poly.setColor(MAKE_RGB32(128, 0, 0));
        polys.push_back(poly);
    }

    int run()
    {
        Display display( "Fullscreen Example", width, height, Output::Windowed, Mode::TrueColor );

        // register listener
        display.listener(this);
        
        createPolygons();
        ViewWindow view(0, 0, width, height, DegToRad(75));
        Transform3D camera(x, 200.0f, z);
        PolygonRenderer& polygonRenderer = SolidPolygonRenderer(camera, view);
        
        while (!quit)
        {    
            polygonRenderer.getCamera().getLocation().x = x;

#ifdef _DEBUG
            cout << polygonRenderer.getCamera().getLocation() << endl;
#endif // _DEBUG

            polygonRenderer.getCamera().getLocation().z = z;

            cls();

            for (int i=0; i!=polys.size(); ++i)
            {
                polygonRenderer.draw(&polys[i]);
            }

            display.update(pixels);            
        }

        return 0;
    }


protected:

    void onKeyDown( DisplayInterface & my_display, Key key )
    {
        if ( key == Key::Escape )
            quit = true;

        //return false;       // disable default key handlers
    }

    void onKeyPressed( DisplayInterface & my_display, Key key )
    {
        if (key == Key::Up) 
        {
            z += 10.0f;    
        }

        if (key == Key::Down) 
        {
            z -= 10.0f;    
        }

        if (key == Key::Left) 
        {
            x -= 10.0f;    
        }

        if (key == Key::Right) 
        {
            x += 10.0f;    
        }
    }

    void onKeyUp( DisplayInterface & display, Key key )
    {
        // ...
    }

    void onMouseButtonDown( DisplayInterface & display, Mouse mouse )
    {
        // ...
    }

    void onMouseButtonUp( DisplayInterface & display, Mouse mouse )
    {
        // ...
    }

    void onMouseMove( DisplayInterface & display, Mouse mouse )
    {
        // ...
    }

    void onActivate( DisplayInterface & display, bool active )
    {
        // ...
    }

    bool onClose( DisplayInterface & display )
    {
        return quit = true;         // returning true indicates that we want the display close to proceed
    }

private:

    //Display display;//  ( "Fullscreen Example", width, height, Output::Windowed, Mode::TrueColor );
    bool quit;
    float z, x;
    vector<SolidPolygon3D> polys;
    

};


int main(int argc, char* argv[])
{
    Application app;
    app.run();
}



