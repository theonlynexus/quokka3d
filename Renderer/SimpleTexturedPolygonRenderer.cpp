#include "SimpleTexturedPolygonRenderer.h"
#include "primitives.h"
#include "LightPng/LightPng.h"
#include "LightPng/LightZ.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>

using namespace Quokka3D;

SimpleTexturedPolygonRenderer::SimpleTexturedPolygonRenderer(const Transform3D& camera, 
                                                             const ViewWindow& viewWindow, 
                                                             const std::string& textureFile)
{
    init(camera, viewWindow, true); 
    m_a = Vector3D();
    m_b = Vector3D();
    m_c = Vector3D();
    m_viewPos = Vector3D();
    m_textureBounds = Rectangle3D();
    // m_texture = NULL;
    m_texture = loadTexture(textureFile);

}


// Reads a file fully (return null on error)
char* SimpleTexturedPolygonRenderer::ReadFile( const char *fn, int &dest_size )
{
	FILE *f = fopen( fn, "rb" );
	if ( f == 0 )
		return 0;

	fseek( f, 0, SEEK_END );
	dest_size = ftell( f );
	fseek( f, 0, SEEK_SET );

	char *data = new char[ dest_size + 1 ];
	if ( fread( data, 1, dest_size, f ) != dest_size )
	{
		delete [] data;
		data = 0;
	}
	fclose( f );

	return data;  // caller must delete data with delete[]
}


LPNG_Image* SimpleTexturedPolygonRenderer::loadTexture(const std::string& fileName)
{
    int source_len;
    char *source = ReadFile(fileName.c_str(), source_len);
	if ( source == 0 )
	{
        fprintf(stderr, "Error reading file %s\n", fileName.c_str());
        exit(EXIT_FAILURE);
	}

    clock_t before = clock();

	// Does all the hard work decompressing the png in memory
	LPNG_Image *img = LPNG_Create( source, source_len );

    double elapsed = clock() - before;

    printf("PNG_Create()took %.3f seconds\n", elapsed/CLOCKS_PER_SEC);

	if ( img == 0 )
	{
		fprintf(stderr, "Error creating PNG image from file %s\n", fileName.c_str());
		exit(EXIT_FAILURE);
	}

	printf( "Image opened ok\n" );
	printf( "Width : %d\n", img->width );
	printf( "Height: %d\n", img->height );
	printf( "Has palette: %s\n", img->has_palette ? "yes" : "no" );

    delete[] source;

    return img; // caller must delete memory

}

void SimpleTexturedPolygonRenderer::drawCurrentPolygon()
{
        // Calculate texture bounds.
        // Ideally texture bounds are pre-calculated and stored
        // with the polygon. Coordinates are computed here for
        // demonstration purposes.

        // These 3 vars are references because that's what they are
        // in Brackeen's original Java code.
        Vector3D& textureOrigin = m_textureBounds.getOrigin();
        Vector3D& textureDirectionU = m_textureBounds.getDirectionU();
        Vector3D& textureDirectionV = m_textureBounds.getDirectionV();

        textureOrigin = (*m_sourcePolygon)[0];

        textureDirectionU = (*m_sourcePolygon)[3];
        textureDirectionU -= textureOrigin;
        textureDirectionU.normalize();

        textureDirectionV = (*m_sourcePolygon)[1];
        textureDirectionV -= textureOrigin;
        textureDirectionV.normalize();

        // transform the texture bounds
        m_textureBounds.subtract(m_camera);
      

        // start texture-mapping calculations
        m_a.cross(m_textureBounds.getDirectionV(), m_textureBounds.getOrigin());
        m_b.cross(m_textureBounds.getOrigin(), m_textureBounds.getDirectionU());
        m_c.cross(m_textureBounds.getDirectionU(), m_textureBounds.getDirectionV());

        int y = m_scanConverter.getTopBoundary();
        m_viewPos.z = -m_viewWindow.getDistance();

        unsigned char *data = m_texture->data;

        while (y <= m_scanConverter.getBottomBoundary()) 
        {
            ScanConverter::Scan scan = m_scanConverter[y];

            if (scan.isValid()) 
            {
                m_viewPos.y = m_viewWindow.convertFromScreenYToViewY(y);
                for (int x=scan.left; x<=scan.right; x++) 
                {
                    m_viewPos.x = m_viewWindow.convertFromScreenXToViewX(x);

                    // compute the texture location
                    int tx = (int)(m_a.dot(m_viewPos) / m_c.dot(m_viewPos));
                    int ty = (int)(m_b.dot(m_viewPos) / m_c.dot(m_viewPos));

                    //printf("y = %d  x = %d  tx = %d  ty = %d\n", y, x, tx, ty); 

                    // get the color to draw
                    unsigned char *src_color = data + (ty * m_texture->width + tx) * PITCH;
                    unsigned char a, r, g, b;
                    a = *src_color++;
                    r = *src_color++;
                    g = *src_color++;
                    b = *src_color++;

                    TRUECOLOR color = MAKE_RGB32(r,g,b);

                    // At last, after all that hard work, draw the pixel!
                    plot_pixel(x, y, color);
                }
            }
            y++;    // next scan line
        }
}
    
