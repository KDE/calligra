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
 
#include "kis_selection.h"
#include "kis_gradient.h"
#include "kis_color.h"

KisGradient::KisGradient()
{
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


void KisGradient::mapVertGradient( QRect gradR, 
            KisColor startColor, 
            KisColor endColor )
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
