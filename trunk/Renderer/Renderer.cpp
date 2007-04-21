// Renderer.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "math3D.h"
#include "primitives.h"
#include "viewwindow.h"
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
        z = -300.0f;
        x = 0.0f;
    }


    int run()
    {
        Display display( "Fullscreen Example", width, height, Output::Windowed, Mode::TrueColor );

        Vector3D v0(-50.0f, -50.0f, 0.0f), v1(50.0f, -50.0f, 0.0f), v2(0.0f, 50.0f, 0.0f), v3(0.0f, -50.0f, 100.0f);

        Polygon3D poly(v0, v1, v2, v3);
        

        //Vector3D v4(1,2,3), v5(10,9,7);
        //v4 *= 4;
        //v4 /= 2.0f;
        //v5 = 2 * (v5 - v4) ;
        //cout << v5;

        // register listener
        display.listener(this);
    
        Transform3D transform(x, 0.0f, z);
        ViewWindow view(0, 0, width, height, DegToRad(75));
        Polygon3D transformedPolygon;
        Vector3D vt0;
        Vector3D vt1;
        Vector3D vt2;
        Vector3D vt3;

        

        while (!quit)
        {
            transform.setLocation(Vector3D(x, 0.0f, z));
            transformedPolygon = poly;
            transformedPolygon.add(transform);
            transformedPolygon.project(view);

            vt0 = transformedPolygon[0];
            vt1 = transformedPolygon[1];
            vt2 = transformedPolygon[2];
            vt3 = transformedPolygon[3];

            cls();

            line_fast((int)vt0.x, (int)vt0.y, (int)vt1.x, (int)vt1.y, MAKE_RGB32(255, 0, 0));
            line_fast((int)vt1.x, (int)vt1.y, (int)vt2.x, (int)vt2.y, MAKE_RGB32(255, 255, 0));
            line_fast((int)vt2.x, (int)vt2.y, (int)vt0.x, (int)vt0.y, MAKE_RGB32(255, 0, 255));
            line_fast((int)vt0.x, (int)vt0.y, (int)vt3.x, (int)vt3.y, MAKE_RGB32(0, 255, 0));
            line_fast((int)vt1.x, (int)vt1.y, (int)vt3.x, (int)vt3.y, MAKE_RGB32(0, 0, 255));
            line_fast((int)vt2.x, (int)vt2.y, (int)vt3.x, (int)vt3.y, MAKE_RGB32(255,255, 255));
            
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



