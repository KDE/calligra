/*
 *  kis_gradient.cc - part of Krayon
 *
 *  Copyright (c) 2001 John Califf <jcaliff@compuzone.net>
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

#include <kimageeffect.h>
#include "kis_selection.h"
#include "kis_gradient.h"
#include "kis_color.h"



KisGradient::KisGradient()
{
    mEffect = KImageEffect::VerticalGradient;
    setNull();
}


KisGradient::~KisGradient()
{
}


void KisGradient::setNull()
{
    mGradientWidth  = 0;
    mGradientHeight = 0;
    gradArray.resize(0);
}

/*
    mapKdeGradient - preferred method of mapping a predefined 
    kde gradient to a QImage.  This works well for 2 color
    gradients - I see no need for more colors although different
    blending methods and periodicity needs to be considred later
    for our native gradients and gimp imports 

    Below are the predefined types of kde gradients. Nice!

    enum GradientType 
    { VerticalGradient, HorizontalGradient,
      DiagonalGradient, CrossDiagonalGradient,
      PyramidGradient, RectangleGradient,
      PipeCrossGradient, EllipticGradient };
*/

void KisGradient::mapKdeGradient(QRect gradR, 
            KisColor startColor, KisColor endColor )
{
    mGradientWidth  = gradR.width();
    mGradientHeight = gradR.height();

    gradArray.resize(mGradientWidth * mGradientHeight);
    gradArray.fill(0); 

    // use uniform 256x256 size for maximum smoothness - scale
    // up to desired size later - this size should actually
    // be determinded by the difference in color values
    // for the channel with the max difference - it can never
    // be greater than 255, and no further smoothness can be
    // gained with an image larger than 256x256 without going
    // to 64 bit color or using custom dithering per scanline
    
    QSize size(256, 256);
    QColor ca(startColor.R(), startColor.G(), startColor.B());
    QColor cb(endColor.R(), endColor.G(), endColor.B());

    // use gradient effect selected with gradient dialog
    QImage tmpImage = KImageEffect::gradient(size, ca, cb, mEffect, 0);
    
    // scale to desired size to prevent banding (inherent dithering)
    gradImage = tmpImage.smoothScale(mGradientWidth, mGradientHeight);

}


void KisGradient::mapVertGradient( QRect gradR, 
            KisColor startColor, KisColor endColor )
{
    mGradientWidth  = gradR.width();
    mGradientHeight = gradR.height();

    gradArray.resize(mGradientWidth * mGradientHeight);
    gradArray.fill(0); 

    uint color = 0;
    
    // draw gradient within rectanguar area defined above
    int length = gradR.height();
    
    int rDiff = ( endColor.R() - startColor.R() );
    int gDiff = ( endColor.G() - startColor.G() );
    int bDiff = ( endColor.B() - startColor.B() );
  
    int rl = startColor.R();
    int gl = startColor.G();
    int bl = startColor.B();
 
    float rlFloat = (float)rl;
    float glFloat = (float)gl;
    float blFloat = (float)bl;

    int y1 = 0;
    int y2 = gradR.height();
    int x1 = 0;
    int x2 = gradR.width();
    
    // gradient defined vertically
    for( int y = y1 ; y < y2 ; y++ )
    {
        // calc color
        float rlFinFloat 
            = rlFloat + ((float) (y - y1) * (float)rDiff) / (float)length;
        float glFinFloat 
            = glFloat + ((float) (y - y1) * (float)gDiff) / (float)length;
        float blFinFloat 
            = blFloat + ((float) (y - y1) * (float)bDiff) / (float)length;

        uint red   = (uint)rlFinFloat;
        uint green = (uint)glFinFloat;
        uint blue  = (uint)blFinFloat;
        
        color = (0xff000000) | (red << 16) | (green << 8) | (blue); 

        // draw uniform horizontal line of color - 
        for( int x = x1 ; x < x2 ; x++ )
        {
            gradArray[y * (x2 - x1) + (x - x1)] = color;
        }
    }
}


void KisGradient::mapHorGradient( QRect gradR, 
            KisColor startColor, 
            KisColor endColor )
{
    mGradientWidth  = gradR.width();
    mGradientHeight = gradR.height();

    gradArray.resize(mGradientWidth * mGradientHeight);
    gradArray.fill(0); 

    // draw gradient within rectanguar area defined above
    int length = gradR.width();
    
}
