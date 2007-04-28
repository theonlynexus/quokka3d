#ifndef VIEWWINDOW_H
#define VIEWWINDOW_H

#include "math3D.h"

namespace Quokka3D 
{
    class ViewWindow
    {
    public:
        ViewWindow() {m_angle = m_distanceToCamera = 0.0f; }
        ViewWindow(int left, int top, int width, int height, float angle);
        void setBounds(int left, int top, int width, int height);
        void setAngle(float angle) { m_angle = angle; m_distanceToCamera = (m_bounds.width/2) / tan(m_angle/2.0f); }
        float getAngle() const { return m_angle; }
        int getWidth() const { return m_bounds.width; }
        int getHeight() const { return m_bounds.height; }
        int getTopOffset() const {  return m_bounds.y; }
        int getLeftOffset() const {  return m_bounds.x; }
        float getDistance() const { return m_distanceToCamera; }
        float convertFromViewXToScreenX(float x) const { return x + m_bounds.x + m_bounds.width/2; }   
        float convertFromViewYToScreenY(float y) const { return -y + m_bounds.y + m_bounds.height/2; }   
        float convertFromScreenXToViewX(float x) const { return x - m_bounds.x - m_bounds.width/2; }
        float convertFromScreenYToViewY(float y) const { return -y + m_bounds.y + m_bounds.height/2; }
        void project(Vector3D&) const; // projects the specified vector to the screen

    private:
        Rectangle m_bounds;         // rectangular bounds for the view window on the screen
        float m_angle;              // horizontal view angle
        float m_distanceToCamera;   // distance from camera to view window
    };
}

#endif  //VIEWWINDOW_H