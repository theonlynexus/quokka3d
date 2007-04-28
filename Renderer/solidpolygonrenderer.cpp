#include "solidpolygonrenderer.h"
#include "solidpolygon3d.h"
#include "primitives.h"

namespace Quokka3D
{
    /*
    Draws the current polygon. At this point, the current
    polygon is transformed, clipped, projected,
    scan-converted, and visible.
    */
    void SolidPolygonRenderer::drawCurrentPolygon()
    {
        // set the color
        /*if (sourcePolygon instanceof SolidPolygon3D) {
            g.setColor(((SolidPolygon3D)sourcePolygon).getColor());
        }
        else {
            g.setColor(Color.GREEN);
        }*/

        // draw the scans
        
        int y = m_scanConverter.getTopBoundary();
        while (y <= m_scanConverter.getBottomBoundary()) 
        {
            ScanConverter::Scan scan = m_scanConverter[y];
           // ScanConverter.Scan scan = scanConverter.getScan(y);
            if (scan.isValid())
            {
                //line_fast(scan.left, y, scan.right, y, m_sourcePolygon.getColor());
            }
                /*if (scan.isValid()) 
                {
                    g.drawLine(scan.left, y, scan.right, y);
                } */
            y++;
        }

    }

}