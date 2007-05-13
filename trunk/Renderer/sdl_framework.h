#ifndef SDL_FRAMEWORK_H
#define SDL_FRAMEWORK_H

#include "SDL.h"
#include <limits>

#define sign(x) (((x)<0) ? -1 : (x)>0 ? 1 : 0)       // macro to return the sign of a number

/** The base engine class. **/
class SDL_Framework  
{
private:
    /** Last iteration's tick value **/
    long m_lLastTick;

    

    /** Has quit been called? **/
    bool m_bQuit;

    /** The title of the window **/
    const char* m_czTitle;

    

    /** Is the window minimized? **/
    bool m_bMinimized;

    /** Variables to calculate the frame rate **/
    /** Tick counter. **/
    int m_iFPSTickCounter;

    /** Frame rate counter. **/
    int m_iFPSCounter;

    /** Stores the last calculated frame rate. **/
    int m_iCurrentFPS;

protected:
    /** Screen surface **/
    SDL_Surface* m_pScreen;

    /** Window width **/
    int m_iWidth;

    /** Window height **/
    int m_iHeight;

    void DoUpdate();
    void DoRender();

    void SetSize(const int& iWidth, const int& iHeight);

    void HandleInput();

public:
    SDL_Framework();
    virtual ~SDL_Framework();

    void Init();
    void Start();

    // Drawing methods
    void line        (int x1, int y1, int x2, int y2, Uint32 color);
    void line8       (int x1, int y1, int x2, int y2, Uint32 color);
    void line16      (int x1, int y1, int x2, int y2, Uint32 color);
    void line24      (int x1, int y1, int x2, int y2, Uint32 color);
    void line32      (int x1, int y1, int x2, int y2, Uint32 color);
    void plot_pixel  (int x, int y, Uint32 pixel);

    /** OVERLOADED - Data that should be initialized when the application starts. **/
    virtual void AdditionalInit	() {}

    /** OVERLOADED - All the games calculation and updating. 
    @param iElapsedTime The time in milliseconds elapsed since the function was called last.
    **/
    virtual void Update		( const int& iElapsedTime ) {}
    /** OVERLOADED - All the games rendering. 
    @param pDestSurface The main screen surface.
    **/
    virtual void Render		( SDL_Surface* pDestSurface ) {}

    /** OVERLOADED - Allocated data that should be cleaned up. **/
    virtual void End		() {}

    /** OVERLOADED - Window is active again. **/
    virtual void WindowActive	() {}

    /** OVERLOADED - Window is inactive. **/
    virtual void WindowInactive	() {}


    /** OVERLOADED - Keyboard key has been released.
    @param iKeyEnum The key number.
    **/
    virtual void KeyUp		(const int& iKeyEnum) {}

    /** OVERLOADED - Keyboard key has been pressed.
    @param iKeyEnum The key number.
    **/
    virtual void KeyDown		(const int& iKeyEnum) {}


    /** OVERLOADED - The mouse has been moved.
    @param iButton	Specifies if a mouse button is pressed.
    @param iX	The mouse position on the X-axis in pixels.
    @param iY	The mouse position on the Y-axis in pixels.
    @param iRelX	The mouse position on the X-axis relative to the last position, in pixels.
    @param iRelY	The mouse position on the Y-axis relative to the last position, in pixels.

    @bug The iButton variable is always NULL.
    **/
    virtual void MouseMoved		(const int& iButton,
        const int& iX, 
        const int& iY, 
        const int& iRelX, 
        const int& iRelY) {}

    /** OVERLOADED - A mouse button has been released.
    @param iButton	Specifies if a mouse button is pressed.
    @param iX	The mouse position on the X-axis in pixels.
    @param iY	The mouse position on the Y-axis in pixels.
    @param iRelX	The mouse position on the X-axis relative to the last position, in pixels.
    @param iRelY	The mouse position on the Y-axis relative to the last position, in pixels.
    **/

    virtual void MouseButtonUp	(const int& iButton, 
        const int& iX, 
        const int& iY, 
        const int& iRelX, 
        const int& iRelY) {}

    /** OVERLOADED - A mouse button has been pressed.
    @param iButton	Specifies if a mouse button is pressed.
    @param iX	The mouse position on the X-axis in pixels.
    @param iY	The mouse position on the Y-axis in pixels.
    @param iRelX	The mouse position on the X-axis relative to the last position, in pixels.
    @param iRelY	The mouse position on the Y-axis relative to the last position, in pixels.
    **/
    virtual void MouseButtonDown	(const int& iButton, 
        const int& iX, 
        const int& iY, 
        const int& iRelX, 
        const int& iRelY) {}

    void		SetTitle	(const char* czTitle);
    const char* 	GetTitle	();

    SDL_Surface* 	GetSurface	();

    int 		GetFPS		();
}; 

#endif // SDL_FRAMEWORK_H