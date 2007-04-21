#include <cmath>
#include "viewwindow.h"

namespace Quokka3D
{
    /*
        Creates a new ViewWindow with the specified bounds on the
        screen and horizontal view angle.
    */
    ViewWindow::ViewWindow(int left, int top, int width, int height, float angle)
    {
        m_angle = angle;
        setBounds(left, top, width, height);
    }


    /*
        Sets the bounds for this ViewWindow on the screen.
    */
    void ViewWindow::setBounds(int left, int top, int width, int height)
    {
        m_bounds.x = left;
        m_bounds.y = top;
        m_bounds.width = width;
        m_bounds.height = height;
        m_distanceToCamera = (m_bounds.width/2) / tan(m_angle/2.0f);
    }

 
    /*
        Projects the specified vector to the screen.
    */
    void ViewWindow::project(Vector3D& v) const
    {
        // project to view window
        v.x = m_distanceToCamera * v.x / -v.z;
        v.y = m_distanceToCamera * v.y / -v.z;

        // convert to screen coordinates
        v.x = convertFromViewXToScreenX(v.x);
        v.y = convertFromViewYToScreenY(v.y);
    }

}