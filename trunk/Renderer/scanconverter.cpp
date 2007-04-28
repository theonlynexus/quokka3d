#include <algorithm>
#include "scanconverter.h"

namespace Quokka3D
{
    void ScanConverter::ensureCapacity()
    {
        int height = m_view.getTopOffset() + m_view.getHeight();
        
        if (m_scans.size() != height) 
        {
            m_scans.resize(height);
            
            // set top and bottom so clearCurrentScan clears all
            m_top = 0;
            m_bottom = height - 1;
        }   
    }


    void ScanConverter::clearCurrentScan()
    {
        for (int i=m_top; i<=m_bottom; i++) {
            m_scans[i].clear();   
        }
        m_top = INT_MAX;
        m_bottom = INT_MIN;

    }


    bool ScanConverter::convert(Polygon3D& polygon)
    {
        ensureCapacity();
        clearCurrentScan();

        int minX = m_view.getLeftOffset();
        int maxX = m_view.getLeftOffset() + m_view.getWidth() - 1;
        int minY = m_view.getTopOffset();
        int maxY = m_view.getTopOffset() + m_view.getHeight() - 1;
        int numVertices = polygon.getNumVertices();

        for (int i=0; i<numVertices; i++) 
        {
            Vector3D v1 = polygon[i];
            Vector3D v2;
            if (i == numVertices - 1) 
            {
                v2 = polygon[0];
            }
            else 
            {
                v2 = polygon[i+1];
            }

            // ensure v1.y < v2.y
            if (v1.y > v2.y) 
            {          
                // TODO: Change this to std::swap?
                Vector3D temp = v1;
                v1 = v2;
                v2 = temp;
            }
            float dy = v2.y - v1.y;

            // ignore horizontal lines
            if (float_equals(dy, 0.0f)) 
            {
                continue;
            }

            // TODO: checkout Brackeen ceil in MoreMath.java if ceil here is too slow
            int startY = std::max((int)ceil(v1.y), minY);
            int endY = std::min((int)ceil(v2.y)-1, maxY);
            m_top = std::min(m_top, startY);
            m_bottom = std::max(m_bottom, endY);
            float dx = v2.x - v1.x;

            // special case: vertical line
            if (float_equals(dx, 0.0f)) 
            {
                int x = (int)ceil(v1.x);
                // ensure x within view bounds
                x = std::min(maxX+1, std::max(x, minX));
                for (int y=startY; y<=endY; y++) 
                {
                    m_scans[y].setBoundary(x);    
                }
            }
            else {
                // scan-convert this edge (line equation)
                float gradient = dx / dy;

                // (slower version)
                /*
                for (int y=startY; y<=endY; y++) {
                int x = MoreMath.ceil(v1.x + (y - v1.y) * gradient);
                // ensure x within view bounds
                x = Math.min(maxX+1, Math.max(x, minX));
                scans[y].setBoundary(x);
                }
                */

                // (faster version)

                // trim start of line
                float startX = v1.x + (startY - v1.y) * gradient;
                if (startX < minX) 
                {
                    int yInt = (int)(v1.y + (minX - v1.x) / gradient);
                    yInt = std::min(yInt, endY);
                    while (startY <= yInt) 
                    {
                        m_scans[startY].setBoundary(minX);
                        ++startY;
                    }
                }
                else if (startX > maxX) 
                {
                    int yInt = (int)(v1.y + (maxX - v1.x) / gradient);
                    yInt = std::min(yInt, endY);
                    while (startY <= yInt) 
                    {
                        m_scans[startY].setBoundary(maxX+1);
                        ++startY;
                    }
                }

                if (startY > endY) 
                {
                    continue;
                }

                // trim back of line
                float endX = v1.x + (endY - v1.y) * gradient;
                if (endX < minX) 
                {
                    int yInt = (int)ceil(v1.y + (minX - v1.x) / gradient);
                    yInt = std::max(yInt, startY);
                    while (endY >= yInt) 
                    {
                        m_scans[endY].setBoundary(minX);
                        --endY;
                    }
                }
                else if (endX > maxX) 
                {
                    int yInt = (int)ceil(v1.y + (maxX - v1.x) / gradient);
                    yInt = std::max(yInt, startY);
                    while (endY >= yInt) 
                    {
                        m_scans[endY].setBoundary(maxX+1);
                        --endY;
                    }
                }

                if (startY > endY) 
                {
                    continue;
                }

                // line equation using integers
                int xScaled = (int)(SCALE * v1.x + SCALE * (startY - v1.y) * dx / dy) + SCALE_MASK;
                int dxScaled = (int)(dx * SCALE / dy);

                for (int y=startY; y<=endY; y++) 
                {
                    m_scans[y].setBoundary(xScaled >> SCALE_BITS);
                    xScaled += dxScaled;
                }
            }
        }

        // check if visible (any valid scans)
        for (int i=m_top; i<=m_bottom; i++) {
            if (m_scans[i].isValid()) {
                return true;
            }
        }
        return false;
    }


} // Quokka3D
