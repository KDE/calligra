/*
 *  kis_gradient.h - part of Krayon
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

#ifndef __kis_gradient_h__
#define __kis_gradient_h__


#include <kimageeffect.h>

class KisSelection;

class KisGradient 
{

public:

    KisGradient(KImageEffect::GradientType _type, 
                KImageEffect::RGBComponent _rgbComponent, 
                KImageEffect::Lighting _lighting,
                KImageEffect::ModulationType _modulationType);
    
    ~KisGradient();

    bool paintGradient(QImage *image);
    bool paintGradient(KisSelection *selection);
    
private:

    KImageEffect::GradientType type; 
    KImageEffect::RGBComponent rgbComponent; 
    KImageEffect::Lighting lighting;
    KImageEffect::ModulationType modulationType;
  
};

#endif

