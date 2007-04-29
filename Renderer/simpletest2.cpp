// Renderer.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "math3D.h"
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
        z = 100.0f;
        x = 0.0f;
    }


    int run()
    {
        Display display( "Fullscreen Example", width, height, Output::Windowed, Mode::TrueColor );

        Vector3D v0(-50.0f, -50.0f, 0.0f);
        Vector3D v1( 50.0f, -50.0f, 0.0f);
        Vector3D v2(  0.0f,  50.0f, 0.0f);

        // register listener
        display.listener(this);
        
        ViewWindow view(0, 0, width, height, DegToRad(75));
        Transform3D camera(x, 0.0f, 150.0f);
        camera.rotateAngleY(DegToRad(20.0f));
        PolygonRenderer& polygonRenderer = SolidPolygonRenderer(camera, view);
        SolidPolygon3D poly(v0, v1, v2);
        poly.setColor(MAKE_RGB32(0, 255, 0));

        while (!quit)
        {
            polygonRenderer.draw(&poly);
            display.update( pixels );
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
            z += 2.0f;    
        }

        if (key == Key::Down) 
        {
            z -= 2.0f;    
        }

        if (key == Key::Left) 
        {
            x -= 2.0f;    
        }

        if (key == Key::Right) 
        {
            x += 2.0f;    
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
    

};


int main(int argc, char* argv[])
{
    Application app;
    app.run();


}



