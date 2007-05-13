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

std::vector<TrueColorPixel> pixels(width * height);    // screen is a linear sequence of pixels

class Application : public Listener
{
public:

    Application()
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
        mouse_x = width/2.0f;
        mouse_y = height/2.0f;
        firstRun = true;
        x = 0.0f;
        y = 100.0f;
        z = -500.0f;
        numFrames = 0;
        diff_x = 0.0f;
        diff_y = 0.0f;
        mouseMoved = false;
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


    // timeDelta is in seconds
    void update(double timeDelta)
    {
        float distanceChange = 500.0 * timeDelta;
        float angleChange = 0.1 * timeDelta;

        if (keyW == true)
        {
            polygonRenderer->getCamera().getLocation().x -= distanceChange * polygonRenderer->getCamera().getSinAngleY();  
            polygonRenderer->getCamera().getLocation().z -= distanceChange * polygonRenderer->getCamera().getCosAngleY();
        }
        if (keyS == true)
        {
            polygonRenderer->getCamera().getLocation().x += distanceChange * polygonRenderer->getCamera().getSinAngleY();  
            polygonRenderer->getCamera().getLocation().z += distanceChange * polygonRenderer->getCamera().getCosAngleY();  
        }
        if (keyA == true)
        {
            polygonRenderer->getCamera().getLocation().x -= distanceChange * polygonRenderer->getCamera().getCosAngleY();  
            polygonRenderer->getCamera().getLocation().z += distanceChange * polygonRenderer->getCamera().getSinAngleY();  
        }
        if (keyD == true)
        {
            polygonRenderer->getCamera().getLocation().x += distanceChange * polygonRenderer->getCamera().getCosAngleY();  
            polygonRenderer->getCamera().getLocation().z -= distanceChange * polygonRenderer->getCamera().getSinAngleY();  
        }
        if (keyUp == true)
        {
            polygonRenderer->getCamera().getLocation().y += distanceChange;     
        }
        if (keyDown == true)
        {
            polygonRenderer->getCamera().getLocation().y -= distanceChange;                
        }
        if (keyRotLeft == true)
        {
            polygonRenderer->getCamera().rotateAngleY(angleChange);  
        }
        if (keyRotRight == true)
        {
            polygonRenderer->getCamera().rotateAngleY(-angleChange);             
        }
        if (keyTiltLeft == true)
        {
            polygonRenderer->getCamera().rotateAngleZ(angleChange);  
        }
        if (keyTiltRight == true)
        {
            polygonRenderer->getCamera().rotateAngleZ(-angleChange);             
        }
        if (mouseMoved) 
        {
            polygonRenderer->getCamera().rotateAngleY(-diff_x * angleChange); 
            polygonRenderer->getCamera().rotateAngleX(-diff_y * angleChange); 
            mouseMoved = false;
        }
        

    }

    void render(Display& display)
    {
        polygonRenderer->startFrame();

        for (int i=0; i!=polys.size(); ++i)
        {
            polygonRenderer->draw(&polys[i]);
        }
        display.update(pixels);
        
    }


    int run()
    {
        Display display( "Fullscreen Example", width, height, Output::Windowed, Mode::TrueColor );

        // register listener
        display.listener(this);
        
        createPolygons();
        ViewWindow view(0, 0, width, height, DegToRad(75));
        Transform3D camera(x, y, z);
        polygonRenderer = new SolidPolygonRenderer(camera, view); //remember to delete
        
        double time = timer.time();
        while (!quit)
        {    
            
            //const double delta = timer.delta();
            //const double res = timer.resolution();
            
            update(timer.delta());

            

            //cout << "Facing: " << polygonRenderer->m_numFacing << "  Clipped: " << polygonRenderer->m_numClipped << endl;
            
            // quick and dirty fps calculator, calculate every 0.5s
            time = timer.time();
            if (time > 0.5)
            {
                cout << (double)numFrames / time << endl;
                numFrames = 0;
                timer.reset();
            }
            
            render(display); 
            
            polygonRenderer->resetCounters();
           
            numFrames++;

            
        }

        delete polygonRenderer;

        return 0;
    }


protected:

    void handleKeys(const Key& key)
    {
        switch(key)
        {
        case Key::W : 
            keyW = !keyW;
            break;
        case Key::S : 
            keyS = !keyS;
            break;
        case Key::A : 
            keyA = !keyA;
            break;
        case Key::D : 
            keyD = !keyD;
            break;
        case Key::Up : 
            keyUp = !keyUp;
            break;
        case Key::Down :  
            keyDown = !keyDown;
            break;
        case Key::Left : 
            keyRotLeft = !keyRotLeft;
            break;
        case Key::Right : 
            keyRotRight = !keyRotRight;
            break;
        case Key::Z : 
            keyTiltLeft = !keyTiltLeft;
            break;
        case Key::X : 
            keyTiltRight = !keyTiltRight;
            break;

        default:
            break;
        }
    }


    void onKeyDown( DisplayInterface & my_display, Key key )
    {
        handleKeys(key);

        if (key == Key::Escape)
        {
            quit = true;
        }

        //return false;       // disable default key handlers
    }


    void onKeyUp( DisplayInterface & display, Key key )
    {
        handleKeys(key);
    }


    void onKeyPressed( DisplayInterface & my_display, Key key )
    {
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
        //cout << mouse.x << " " << mouse.y << endl;
        mouseMoved = true;
        if (firstRun)
        {
            curr_mouse_x = mouse.x;
            curr_mouse_y = mouse.y;
            firstRun = false;
        }
     
        diff_x = mouse.x - curr_mouse_x;
        diff_y = mouse.y - curr_mouse_y;
        curr_mouse_x = mouse.x;
        curr_mouse_y = mouse.y;

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
    float x, y, z, angleY;  // camera location and current rotation angle
    vector<SolidPolygon3D> polys;
    PolygonRenderer* polygonRenderer ;
    bool keyW, keyS, keyA, keyD, keyUp, keyDown, keyRotLeft, keyRotRight, keyTiltLeft, keyTiltRight;
    float mouse_x, mouse_y, curr_mouse_x, curr_mouse_y, diff_x, diff_y;
    Timer timer;
    int numFrames;
    bool firstRun, mouseMoved;
};


int main(int argc, char* argv[])
{
    Application app;
    app.run();
}




