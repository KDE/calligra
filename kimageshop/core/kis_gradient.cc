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

KisGradient::KisGradient(KImageEffect::GradientType _type, 
    KImageEffect::RGBComponent _rgbComponent, 
    KImageEffect::Lighting _lighting,
    KImageEffect::ModulationType _modulationType)
{
    type = _type;
    rgbComponent = _rgbComponent;
    lighting = _lighting;
    modulationType = _modulationType;
}

KisGradient::~KisGradient()
{
}


bool KisGradient::paintGradient(QImage *image)
{
    return true;
}

bool KisGradient::paintGradient(KisSelection *selection)
{
    return true;
}

