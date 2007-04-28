#ifndef scanconverter_h__
#define scanconverter_h__

#include <vector>
#include <limits>
#include "viewwindow.h"
#include "math3d.h"

namespace Quokka3D
{
    
    /*
        The ScanConverter class converts a projected polygon into a
        series of horizontal scans for drawing. Uses fixed-point integers
        for speed.
    */
    
    class ScanConverter
    {
    private:
        void clearCurrentScan();

        static const int SCALE_BITS = 16;
        static const int SCALE = 1 << SCALE_BITS;
        static const int SCALE_MASK = SCALE - 1;

    public:
        ScanConverter() {}
        ScanConverter(ViewWindow& view) { m_view = view; m_top = m_bottom = 0; }

        int getTopBoundary() const { return m_top; }
        int getBottomBoundary() const { return m_bottom; }
        void ensureCapacity();
        bool convert(Polygon3D& polygon);

        // Nested class representing a horizontal scan line
        static class Scan 
        {
        public:
            int left;
            int right;

            void setBoundary(int x) { if (x < left) left = x; if (x-1 > right) right = x-1; }
            void clear() { left = INT_MAX; right = INT_MIN; }
            bool isValid() const { return (left <= right); }
            void setTo(int left, int right) { this->left = left; this->right; }
            bool equals(int left, int right) { return (this->left == left && this->right == right); }
        };

        Scan& operator[](const size_t y) { return m_scans[y]; }
        const Scan& operator[](const size_t y) const { return m_scans[y]; }
        
    protected:
        ViewWindow m_view;
        std::vector<Scan> m_scans;
        int m_top;
        int m_bottom;

    
    };

} // Quokka3D
#endif // scanconverter_h__