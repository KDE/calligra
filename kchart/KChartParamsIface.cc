/* This file is part of the KDE project
   Copyright (C) 2001 Laurent Montel <lmontel@mandrakesoft.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "KChartParamsIface.h"

#include "kchart_params.h"

#include <dcopclient.h>

KChartParamsIface::KChartParamsIface( KChartParams *params_ )
    : DCOPObject()
{
    params = params_;
}

void KChartParamsIface::setChartType( /*ChartType chartType*/ )
{
    //todo
}


bool KChartParamsIface::threeDBars()
{
    return params->threeDBars();
}

void KChartParamsIface::setThreeDBars( bool threeDBars )
{
    params->setThreeDBars(threeDBars);
}

void KChartParamsIface::setThreeDBarsShadowColors( bool shadow )
{
    params->setThreeDBarsShadowColors(shadow);
}

bool KChartParamsIface::threeDBarsShadowColors() const
{
    return params->threeDBarsShadowColors();
}

void KChartParamsIface::setThreeDBarAngle( uint angle )
{
    params->setThreeDBarAngle(angle);
}

uint KChartParamsIface::threeDBarAngle() const
{
    return params->threeDBarAngle();
}

void KChartParamsIface::setThreeDBarDepth( double depth )
{
    params->setThreeDBarDepth(depth);
}

double KChartParamsIface::threeDBarDepth() const
{
    return params->threeDBarDepth();
}


bool KChartParamsIface::threeDPies()
{
    return params->threeDPies();
}

void KChartParamsIface::setThreeDPieHeight( int pixels )
{
    params->setThreeDPieHeight(pixels);
}

int KChartParamsIface::threeDPieHeight()
{
    return params->threeDPieHeight();
}

void KChartParamsIface::setPieStart( int degrees )
{
    params->setPieStart(degrees);
}

int KChartParamsIface::pieStart()
{
    return params->pieStart();
}

