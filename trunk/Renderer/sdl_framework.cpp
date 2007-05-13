#include "sdl_framework.h"
//#include <windows.h> // For the WaitMessage() function.

/** Default constructor. **/
SDL_Framework::SDL_Framework()
{
    m_lLastTick		= 0;
    m_iWidth		= 640;
    m_iHeight		= 480;
    m_czTitle		= 0;

    m_pScreen		= 0;

    m_iFPSTickCounter	= 0;
    m_iFPSCounter		= 0;
    m_iCurrentFPS		= 0;

    m_bMinimized		= false;
}

/** Destructor. **/
SDL_Framework::~SDL_Framework()
{
    //OutputDebugString("In ~SDL_Framework");
    SDL_Quit();
}

/** Sets the height and width of the window.
@param iWidth The width of the window
@param iHeight The height of the window
**/
void SDL_Framework::SetSize(const int& iWidth, const int& iHeight)
{
    m_iWidth  = iWidth;
    m_iHeight = iHeight;
    m_pScreen = SDL_SetVideoMode( iWidth, iHeight, 0, SDL_SWSURFACE);
}

/** Initialize SDL, the window and the additional data. **/
void SDL_Framework::Init()
{
    // Register SDL_Quit to be called at exit; makes sure things are cleaned up when we quit.
    // atexit( SDL_Quit ); // SDL_Quit() is called in the destructor

    // Initialize SDL's subsystems - in this case, only video.
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) 
    {
        fprintf( stderr, "Unable to init SDL: %s\n", SDL_GetError() );
        exit( 1 );
    }

    // Attempt to create a window with the specified height and width.
    SetSize( m_iWidth, m_iHeight );

    // If we fail, return error.
    if ( m_pScreen == NULL ) 
    {
        fprintf( stderr, "Unable to set up video: %s\n", SDL_GetError() );
        exit( 1 );
    }

    AdditionalInit();
}

/** The main loop. **/
void SDL_Framework::Start()
{
    m_lLastTick = SDL_GetTicks();
    m_bQuit = false;

    // Main loop: loop forever.
    while ( !m_bQuit )
    {
        // Handle mouse and keyboard input
        HandleInput();

        if ( m_bMinimized ) {
            // Release some system resources if the app. is minimized.
            //WaitMessage(); // pause the application until focus in regained
        } else {
            // Do some thinking
            DoUpdate();

            // Render stuff
            DoRender();
        }
    }

    End();
}

/** Handles all controller inputs.
@remark This function is called once per frame.
**/
void SDL_Framework::HandleInput()
{
    // Poll for events, and handle the ones we care about.
    SDL_Event event;
    while ( SDL_PollEvent( &event ) ) 
    {
        switch ( event.type ) 
        {
        case SDL_KEYDOWN:
            // If escape is pressed set the Quit-flag
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                m_bQuit = true;
                break;
            }

            KeyDown( event.key.keysym.sym );
            break;

        case SDL_KEYUP:
            KeyUp( event.key.keysym.sym );
            break;

        case SDL_QUIT:
            m_bQuit = true;
            break;

        case SDL_MOUSEMOTION:
            MouseMoved(
                event.button.button, 
                event.motion.x, 
                event.motion.y, 
                event.motion.xrel, 
                event.motion.yrel);
            break;

        case SDL_MOUSEBUTTONUP:
            MouseButtonUp(
                event.button.button, 
                event.motion.x, 
                event.motion.y, 
                event.motion.xrel, 
                event.motion.yrel);
            break;

        case SDL_MOUSEBUTTONDOWN:
            MouseButtonDown(
                event.button.button, 
                event.motion.x, 
                event.motion.y, 
                event.motion.xrel, 
                event.motion.yrel);
            break;

        case SDL_ACTIVEEVENT:
            if ( event.active.state & SDL_APPACTIVE ) {
                if ( event.active.gain ) {
                    m_bMinimized = false;
                    WindowActive();
                } else {
                    m_bMinimized = true;
                    WindowInactive();
                }
            }
            break;
        } // switch
    } // while (handling input)
}

/** Handles the updating routine. **/
void SDL_Framework::DoUpdate() 
{
    long iElapsedTicks = SDL_GetTicks() - m_lLastTick;
    m_lLastTick = SDL_GetTicks();

    Update( iElapsedTicks );

    m_iFPSTickCounter += iElapsedTicks;
}

/** Handles the rendering and FPS calculations. **/
void SDL_Framework::DoRender()
{
    ++m_iFPSCounter;
    if ( m_iFPSTickCounter >= 1000 )
    {
        m_iCurrentFPS = m_iFPSCounter;
        m_iFPSCounter = 0;
        m_iFPSTickCounter = 0;
    }

    // Clear the window
    SDL_FillRect( m_pScreen, 0, SDL_MapRGB( m_pScreen->format, 0, 0, 0 ) );

    // Lock surface if needed
    if ( SDL_MUSTLOCK( m_pScreen ) )
        if ( SDL_LockSurface( m_pScreen ) < 0 )
            return;

    Render( GetSurface() );

    // Unlock if needed
    if ( SDL_MUSTLOCK( m_pScreen ) ) 
        SDL_UnlockSurface( m_pScreen );

    // Tell SDL to update the whole gScreen
    SDL_Flip( m_pScreen );
}

/** Sets the title of the window 
@param czTitle A character array that contains the text that the window title should be set to.
**/
void SDL_Framework::SetTitle(const char* czTitle)
{
    m_czTitle = czTitle;
    SDL_WM_SetCaption( czTitle, 0 );
}

/** Retrieve the title of the application window.
@return The last set windows title as a character array.
@remark Only the last set title is returned. If another application has changed the window title, then that title won't be returned.
**/
const char* SDL_Framework::GetTitle()
{
    return m_czTitle;
}

/** Retrieve the main screen surface.
@return A pointer to the SDL_Surface surface
@remark The surface is not validated internally.
**/
SDL_Surface* SDL_Framework::GetSurface()
{ 
    return m_pScreen;
}

/** Get the current FPS.
@return The number of drawn frames in the last second.
@remark The FPS is only updated once each second.
**/
int SDL_Framework::GetFPS()
{
    return m_iCurrentFPS;
}




void SDL_Framework::plot_pixel(int x, int y, Uint32 pixel)
{
    int bpp = m_pScreen->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)m_pScreen->pixels + y * m_pScreen->pitch + x * bpp;

    switch(bpp) 
    {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}    



void SDL_Framework::line8( 
    int x1, int y1, 
    int x2, int y2, 
    Uint32 color)
{
    int d;
    int x;
    int y;
    int ax;
    int ay;
    int sx;
    int sy;
    int dx;
    int dy;

    Uint8 *lineAddr;
    Sint32 yOffset;

    dx = x2 - x1;  
    ax = abs(dx) << 1;  
    sx = sign(dx);

    dy = y2 - y1;  
    ay = abs(dy) << 1;  
    sy = sign(dy);
    yOffset = sy * m_pScreen->pitch;

    x = x1;
    y = y1;

    lineAddr = ((Uint8 *)(m_pScreen->pixels)) + (y * m_pScreen->pitch);
    if (ax>ay)
    {                      /* x dominant */
        d = ay - (ax >> 1);
        for (;;)
        {
            *(lineAddr + x) = (Uint8)color;

            if (x == x2)
            {
                return;
            }
            if (d>=0)
            {
                y += sy;
                lineAddr += yOffset;
                d -= ax;
            }
            x += sx;
            d += ay;
        }
    }
    else
    {                      /* y dominant */
        d = ax - (ay >> 1);
        for (;;)
        {
            *(lineAddr + x) = (Uint8)color;

            if (y == y2)
            {
                return;
            }
            if (d>=0) 
            {
                x += sx;
                d -= ay;
            }
            y += sy;
            lineAddr += yOffset;
            d += ax;
        }
    }
}

//----------------------------------------------------------

// Draw lines in 16 bit surfaces. Note that this code will
// also work on 15 bit surfaces.

void SDL_Framework::line16( 
    int x1, int y1, 
    int x2, int y2, 
    Uint32 color)
{
    int d;
    int x;
    int y;
    int ax;
    int ay;
    int sx;
    int sy;
    int dx;
    int dy;

    Uint8 *lineAddr;
    Sint32 yOffset;

    dx = x2 - x1;  
    ax = abs(dx) << 1;  
    sx = sign(dx);

    dy = y2 - y1;  
    ay = abs(dy) << 1;  
    sy = sign(dy);
    yOffset = sy * m_pScreen->pitch;

    x = x1;
    y = y1;

    lineAddr = ((Uint8 *)m_pScreen->pixels) + (y * m_pScreen->pitch);
    if (ax>ay)
    {                      /* x dominant */
        d = ay - (ax >> 1);
        for (;;)
        {
            *((Uint16 *)(lineAddr + (x << 1))) = (Uint16)color;

            if (x == x2)
            {
                return;
            }
            if (d>=0)
            {
                y += sy;
                lineAddr += yOffset;
                d -= ax;
            }
            x += sx;
            d += ay;
        }
    }
    else
    {                      /* y dominant */
        d = ax - (ay >> 1);
        for (;;)
        {
            *((Uint16 *)(lineAddr + (x << 1))) = (Uint16)color;

            if (y == y2)
            {
                return;
            }
            if (d>=0) 
            {
                x += sx;
                d -= ay;
            }
            y += sy;
            lineAddr += yOffset;
            d += ax;
        }
    }
}

//----------------------------------------------------------

// Draw lines in 24 bit surfaces. 24 bit surfaces require
// special handling because the pixels don't fall on even
// address boundaries. Instead of being able to store a
// single byte, word, or long you have to store 3
// individual bytes. As a result 24 bit graphics is slower
// than the other pixel sizes.

void SDL_Framework::line24( 
    int x1, int y1, 
    int x2, int y2, 
    Uint32 color)
{
    int d;
    int x;
    int y;
    int ax;
    int ay;
    int sx;
    int sy;
    int dx;
    int dy;

    Uint8 *lineAddr;
    Sint32 yOffset;

#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    color <<= 8;
#endif

    dx = x2 - x1;  
    ax = abs(dx) << 1;  
    sx = sign(dx);

    dy = y2 - y1;  
    ay = abs(dy) << 1;  
    sy = sign(dy);
    yOffset = sy * m_pScreen->pitch;

    x = x1;
    y = y1;

    lineAddr = ((Uint8 *)(m_pScreen->pixels)) + (y * m_pScreen->pitch);
    if (ax>ay)
    {                      /* x dominant */
        d = ay - (ax >> 1);
        for (;;)
        {
            Uint8 *p = (lineAddr + (x * 3));
            memcpy(p, &color, 3);

            if (x == x2)
            {
                return;
            }
            if (d>=0)
            {
                y += sy;
                lineAddr += yOffset;
                d -= ax;
            }
            x += sx;
            d += ay;
        }
    }
    else
    {                      /* y dominant */
        d = ax - (ay >> 1);
        for (;;)
        {
            Uint8 *p = (lineAddr + (x * 3));
            memcpy(p, &color, 3);

            if (y == y2)
            {
                return;
            }
            if (d>=0) 
            {
                x += sx;
                d -= ay;
            }
            y += sy;
            lineAddr += yOffset;
            d += ax;
        }
    }
}

//----------------------------------------------------------

// Draw lines in 32 bit surfaces. Note that this routine
// ignores alpha values. It writes them into the surface
// if they are included in the pixel, but does nothing
// else with them.

void SDL_Framework::line32( 
    int x1, int y1, 
    int x2, int y2, 
    Uint32 color)
{
    int d;
    int x;
    int y;
    int ax;
    int ay;
    int sx;
    int sy;
    int dx;
    int dy;

    Uint8 *lineAddr;
    Sint32 yOffset;

    dx = x2 - x1;  
    ax = abs(dx) << 1;  
    sx = sign(dx);

    dy = y2 - y1;  
    ay = abs(dy) << 1;  
    sy = sign(dy);
    yOffset = sy * m_pScreen->pitch;

    x = x1;
    y = y1;

    lineAddr = ((Uint8 *)(m_pScreen->pixels)) + (y * m_pScreen->pitch);
    if (ax>ay)
    {                      /* x dominant */
        d = ay - (ax >> 1);
        for (;;)
        {
            *((Uint32 *)(lineAddr + (x << 2))) = (Uint32)color;

            if (x == x2)
            {
                return;
            }
            if (d>=0)
            {
                y += sy;
                lineAddr += yOffset;
                d -= ax;
            }
            x += sx;
            d += ay;
        }
    }
    else
    {                      /* y dominant */
        d = ax - (ay >> 1);
        for (;;)
        {
            *((Uint32 *)(lineAddr + (x << 2))) = (Uint32)color;

            if (y == y2)
            {
                return;
            }
            if (d>=0) 
            {
                x += sx;
                d -= ay;
            }
            y += sy;
            lineAddr += yOffset;
            d += ax;
        }
    }
}


//----------------------------------------------------------

// Examine the depth of a surface and select a line
// drawing routine optimized for the bytes/pixel of the
// surface.

void SDL_Framework::line( 
    int x1, int y1, 
    int x2, int y2, 
    Uint32 color)
{
    switch (m_pScreen->format->BytesPerPixel)
    {
    case 1:
        line8(x1, y1, x2, y2, color);
        break;
    case 2:
        line16(x1, y1, x2, y2, color);
        break;
    case 3:
        line24(x1, y1, x2, y2, color);
        break;
    case 4:
        line32(x1, y1, x2, y2, color);
        break;
    }
}




