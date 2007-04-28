#include "polygonrenderer.h"

namespace Quokka3D
{
    PolygonRenderer::PolygonRenderer(Transform3D& camera, ViewWindow& viewWindow)
    {
        init(camera, viewWindow, true);
    }

    PolygonRenderer::PolygonRenderer(Transform3D& camera, ViewWindow& viewWindow, bool clearViewEveryFrame)
    {
        init(camera, viewWindow, clearViewEveryFrame);
    }

    void PolygonRenderer::init(Transform3D& camera, ViewWindow& viewWindow, bool clearViewEveryFrame)
    {
        m_scanConverter = ScanConverter(viewWindow);
        m_camera = camera;
        m_clearViewEveryFrame = clearViewEveryFrame;
    }

    bool PolygonRenderer::draw(Polygon3D& poly)
    {
        if (poly.isFacing(m_camera.getLocation()))
        {
            m_sourcePolygon = poly; // save the source poly in case data is needed later
            m_destPolygon = poly;
            m_destPolygon.subtract(m_camera);
            bool visible = m_destPolygon.clip(-1.0f);
            if (visible)
            {
                m_destPolygon.project(m_viewWindow);
                visible = m_scanConverter.convert(m_destPolygon);
                if (visible)
                {
                    drawCurrentPolygon();
                    return true;
                }
            }
        }
        return false;
    }

} //Quokka3D
