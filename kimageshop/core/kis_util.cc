/*
 *  kis_util.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter <me@kde.org>
 *                1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <kglobal.h>
#include <klocale.h>
#include <kimageio.h>

#include "kis_global.h"
#include "kis_util.h"

// A number which can be added to any image coordinate to make it positive
// Used to make numbers round towards + or - infinity regardless of sign
const long BIGNUM = (TILE_SIZE*10000);


void KisUtil::printRect( const QRect& r, const QString& name )
{
  qDebug("%s:: l:%d t:%d r:%d b:%d w:%d h:%d", name.latin1(), r.left(), r.top(), r.right(),
	 r.bottom(), r.width(), r.height());
}

void KisUtil::printPoint( const QPoint& p, const QString& name )
{
  qDebug("%s:: x:%d y:%d", name.latin1(), p.x(), p.y()); 
}

void KisUtil::enlargeRectToContainPoint( QRect& r, QPoint p )
{
  if (r.contains(p))
    {
      qDebug("KisUtil::enlargeRectToContainPoint: point already contained\n");	
      return;
    }
  if (p.x()<r.left())   r.setLeft(p.x());
  if (p.x()>r.right())  r.setRight(p.x());
  if (p.y()<r.top())    r.setTop(p.y());
  if (p.y()>r.bottom()) r.setBottom(p.y());
}

// Find a rectangle which encloses r whose coordinates are divisible
// by TILE_SIZE (ie no remainder)
QRect KisUtil::findTileExtents( QRect r )
{
  r.setLeft(((r.left()+BIGNUM)/TILE_SIZE)*TILE_SIZE-BIGNUM);
  r.setTop(((r.top()+BIGNUM)  /TILE_SIZE)*TILE_SIZE-BIGNUM);
  r.setBottom(((r.bottom()+TILE_SIZE+BIGNUM)/TILE_SIZE)*TILE_SIZE-BIGNUM-1);
  r.setRight(((r.right()+TILE_SIZE+BIGNUM)  /TILE_SIZE)*TILE_SIZE-BIGNUM-1);
  return(r);
}

QString KisUtil::channelIdtoString(cId cid)
{
  switch (cid)
	{
	case ci_Indexed:
	  return i18n("indexed");
	  break;
	case ci_Alpha:
	  return ("alpha");
	  break;
	case ci_Red:
	  return i18n("red");
	  break;
	case ci_Green:
	  return i18n("green");
	  break;
	case ci_Blue:
	  return i18n("blue");
	  break;
	case ci_Cyan:
	  return i18n("cyan");
	  break;
	case ci_Magenta:
	  return i18n("magenta");
	  break;
	case ci_Yellow:
	  return i18n("yellow");
	  break;
	case ci_Black:
	  return i18n("black");
	  break;
	case ci_L:
	  return "L";
	  break;
	case ci_a:
	  return "a";
	  break;
	case ci_b:
	  return "b";
	  break;
	default:
	  return "unknown";
	  break;
	}
}

QString KisUtil::readFilters()
{
    return KImageIO::pattern( KImageIO::Reading ) + "\n*.kis|KImageShop picture\n";
}

QString KisUtil::writeFilters()
{
    return KisUtil::readFilters();
}

/*
 * Function:	fb_stretch_image()
 * Arguments:	source and destination surface, x and y positioners,
 * 		width and height to scale to.
 * Description: Stretches an image to specified width and height.
 */
#if 0
void fb_stretch_image(fb_surface *src, 
			     fb_surface *dest, 
			     int x, int y, 
			     int w, int h)
{
    int xpos = x, ypos = y;
    int r, g, b;
    float ratio_x;
    float ratio_y;
    int x1, y1;

    register unsigned char *ptr1, *ptr2;

    if (w == src->width && h == src->height)
         fb_draw_image(src, dest, x, y);

    ratio_x = (float)src->width / (float)w;
    ratio_y = (float)src->height / (float)h;

    ptr1 = (unsigned char *)(src->mem_pointer +
                                 src->mem_offset +
                                 ypos * src->line_size +
                                 xpos * src->pixel_size);

    ptr2 = (unsigned char *)(dest->mem_pointer +
                                 dest->mem_offset +
                                 (ypos - y) * dest->line_size);

    for (ypos = y; ypos < y + h; ypos++)
        for (xpos = x; xpos < x + w; xpos++) 
        {
            x1 = (int)((xpos - x) * ratio_x);
            y1 = (int)((ypos - y) * ratio_y);

            fb_getpixel(x1, y1, &r, &g, &b, src);
            fb_putpixel(xpos, ypos, r, g, b, dest);
        }
}
#endif


QImage KisUtil::roughScaleQImage(QImage & src, int width, int height)
{
    QImage dest(width, height, src.depth());

    int xpos, ypos;
    int x1, y1;    
    int x = 0, y = 0;
    
    float ratio_x = (float)src.width() / (float)width;
    float ratio_y = (float)src.height() / (float)height;
    
    for (ypos = y; ypos < y + height; ypos++)
    {
        for (xpos = x; xpos < x + width; xpos++) 
        {
            x1 = (int)((xpos - x) * ratio_x);
            y1 = (int)((ypos - y) * ratio_y);

            QRgb value = src.pixel(x1, y1);
            dest.setPixel(xpos, ypos, value);
        }
    }
    
    return dest;
}


/*
 * Function:	fb_stretch_image_aa()
 * Arguments:	source and destination surface, x and y positioners,
 * 		new width and height.
 * Returns:	None
 */
#if 0
void fb_stretch_image_aa(fb_surface *src, 
				fb_surface *dest, 
				int x, int y, 
				int w, int h)
{
    int xpos = x, ypos = y;
    float r, g, b;
    int r1, g1, b1;
    int r2, g2, b2;
    int r3, g3, b3;
    int r4, g4, b4;
    float ratio_x;
    float ratio_y;
    float x1, y1;
    float xerr, yerr;
    float xfloat, yfloat;

    if (w == src->width && h == src->height)
          fb_draw_image(src, dest, x, y);

    ratio_x = (float)src->width / (float)w;
    ratio_y = (float)src->height / (float)h;

    for (ypos = y; ypos < y + h; ypos++)
    {
        for (xpos = x; xpos < x + w; xpos++) 
        {
             xfloat = (xpos - x) * ratio_x;
             yfloat = (ypos - y) * ratio_y;

             x1 = (int)xfloat;
             y1 = (int)yfloat;

             xerr = 1.0 - (xfloat - (float)x1);
             yerr = 1.0 - (yfloat - (float)y1);

             fb_getpixel(x1, y1, &r1, &g1, &b1, src);
             fb_getpixel(x1 + 1, y1, &r2, &g2, &b2, src);
             fb_getpixel(x1 + 1, y1 + 1, &r3, &g3, &b3, src);
             fb_getpixel(x1, y1 + 1, &r4, &g4, &b4, src);

             r = (float)r1 * xerr +
                 (float)r2 * (1.0 - xerr) +
                 (float)r3 * (1.0 - xerr) +
                 (float)r4 * xerr;

             r += (float)r1 * yerr +
                  (float)r2 * yerr +
                  (float)r3 * (1.0 - yerr) +
                  (float)r4 * (1.0 - yerr);

              r *= 0.25;

              g = (float)g1 * xerr +
                  (float)g2 * (1.0 - xerr) +
                  (float)g3 * (1.0 - xerr) +
                  (float)g4 * xerr;

              g += (float)g1 * yerr +
                   (float)g2 * yerr +
                   (float)g3 * (1.0 - yerr) +
                   (float)g4 * (1.0 - yerr);

              g *= 0.25;

              b = (float)b1 * xerr +
                  (float)b2 * (1.0 - xerr) +
                  (float)b3 * (1.0 - xerr) +
                  (float)b4 * xerr;

              b += (float)b1 * yerr +
                   (float)b2 * yerr +
                   (float)b3 * (1.0 - yerr) +
                   (float)b4 * (1.0 - yerr);

               b *= 0.25;

               fb_putpixel(xpos, ypos, (int)r, (int)g, (int)b, dest);
		}
    }        
}
#endif

QImage KisUtil::roughScaleQImageAA(QImage & src, int width, int height)
{
    QImage dest;

    int x = 0, y = 0;
    int xpos = x, ypos = y;
    
    float r, g, b;

    int r1, g1, b1;
    int r2, g2, b2;
    int r3, g3, b3;
    int r4, g4, b4;

    float x1, y1;

    float xerr, yerr;
    float xfloat, yfloat;
    
    float ratio_x = (float)src.width() / (float)width;
    float ratio_y = (float)src.height() / (float)height;

    for (ypos = y; ypos < y + height; ypos++)
    {
        for (xpos = x; xpos < x + width; xpos++) 
        {
            xfloat = (xpos - x) * ratio_x;
            yfloat = (ypos - y) * ratio_y;

            x1 = (int)xfloat;
            y1 = (int)yfloat;

            xerr = 1.0 - (xfloat - (float)x1);
            yerr = 1.0 - (yfloat - (float)y1);

             //fb_getpixel(x1, y1, &r1, &g1, &b1, src);
             //fb_getpixel(x1 + 1, y1, &r2, &g2, &b2, src);
             //fb_getpixel(x1 + 1, y1 + 1, &r3, &g3, &b3, src);
             //fb_getpixel(x1, y1 + 1, &r4, &g4, &b4, src);
            
            r = (float)r1 * xerr +
                (float)r2 * (1.0 - xerr) +
                (float)r3 * (1.0 - xerr) +
                (float)r4 * xerr;

            r += (float)r1 * yerr +
                 (float)r2 * yerr +
                 (float)r3 * (1.0 - yerr) +
                 (float)r4 * (1.0 - yerr);

            r *= 0.25;

            g = (float)g1 * xerr +
                (float)g2 * (1.0 - xerr) +
                (float)g3 * (1.0 - xerr) +
                (float)g4 * xerr;

            g += (float)g1 * yerr +
                 (float)g2 * yerr +
                 (float)g3 * (1.0 - yerr) +
                 (float)g4 * (1.0 - yerr);

            g *= 0.25;

            b = (float)b1 * xerr +
                (float)b2 * (1.0 - xerr) +
                (float)b3 * (1.0 - xerr) +
                (float)b4 * xerr;

            b += (float)b1 * yerr +
                 (float)b2 * yerr +
                 (float)b3 * (1.0 - yerr) +
                 (float)b4 * (1.0 - yerr);

            b *= 0.25;

            // fb_putpixel(xpos, ypos, (int)r, (int)g, (int)b, dest);
	    }
    }
    
    return dest;
}
