#ifndef polygonrenderer_h
#define polygonrenderer_h

#include "math3d.h"
#include "scanconverter.h"
#include "viewwindow.h"
#include "primitives.h"

namespace Quokka3D
{
    class PolygonRenderer
    {
    public:
        PolygonRenderer() {}
        PolygonRenderer(Transform3D& camera, ViewWindow& viewWindow);
        PolygonRenderer(Transform3D& camera, ViewWindow& viewWindow, bool clearViewEveryFrame);
        Transform3D& getCamera()  { return m_camera; }
        void startFrame() {};
        void endFrame() {};
        bool draw(Polygon3D* poly);

    protected:
        ScanConverter m_scanConverter;
        Transform3D m_camera;
        ViewWindow m_viewWindow;
        bool m_clearViewEveryFrame;
        Polygon3D* m_sourcePolygon;
        Polygon3D m_destPolygon;

        void init(Transform3D& camera, ViewWindow& viewWindow, bool clearViewEveryFrame);
        virtual void drawCurrentPolygon() = 0;

    private:
        
    };

} // Quokka3D
#endif // polygonrenderer_h