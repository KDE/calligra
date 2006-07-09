/* This file is part of the KDE project
   Copyright (C) 2001,2002,2003,2004 Laurent Montel <montel@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KChartParamsIface.h"

#include "kchart_params.h"

#include <dcopclient.h>

namespace KChart
{

KChartParamsIface::KChartParamsIface( KChartParams *params_ )
    : DCOPObject()
{
    params = params_;
}

QString KChartParamsIface::chartType()const
{
    return params->chartTypeToString(params->chartType());
}

void KChartParamsIface::setChartType( const QString &chart )
{
    params->setChartType(params->stringToChartType( chart ));
}

QString KChartParamsIface::barChartSubType() const
{
    return params->barChartSubTypeToString(params->barChartSubType());
}

void KChartParamsIface::setBarChartSubType( const QString &sub )
{
    params->setBarChartSubType(params->stringToBarChartSubType( sub ));
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

void KChartParamsIface::setExplode( bool explode )
{
    params->setExplode(explode);
}

bool KChartParamsIface::explode() const
{
    return params->explode();
}

void KChartParamsIface::setExplodeFactor( double factor )
{
    params->setExplodeFactor(factor);
}

double KChartParamsIface::explodeFactor() const
{
    return params->explodeFactor();
}

void KChartParamsIface::setThreeDPies( bool threeDPies )
{
    params->setThreeDPies(threeDPies);
}

void KChartParamsIface::hideLegend()
{
    params->setLegendPosition(KDChartParams::NoLegend);
}

void KChartParamsIface::setLegendPosition(const QString & pos)
{
    params->setLegendPosition(params->stringToLegendPosition(pos));
}

QString KChartParamsIface::legendPostion() const
{
    return params->legendPositionToString(params->legendPosition());
}

void KChartParamsIface::setLegendTitleText( const QString& text )
{
    params->setLegendTitleText( text );
}

void KChartParamsIface::setLegendSpacing( uint space )
{
    params->setLegendSpacing(space );
}

void KChartParamsIface::setLegendFontRelSize( int legendFontRelSize )
{
    params->setLegendFontRelSize(legendFontRelSize);
}

int KChartParamsIface::legendFontRelSize() const
{
    return params->legendFontRelSize();
}

void KChartParamsIface::setLegendFontUseRelSize( bool legendFontUseRelSize )
{
    params->setLegendFontUseRelSize(legendFontUseRelSize);
}

bool KChartParamsIface::legendFontUseRelSize() const
{
    return params->legendFontUseRelSize();
}

void KChartParamsIface::setLegendTitleFontUseRelSize( bool legendTitleFontUseRelSize )
{
    params->setLegendTitleFontUseRelSize(legendTitleFontUseRelSize);
}

bool KChartParamsIface::legendTitleFontUseRelSize() const
{
    return params->legendTitleFontUseRelSize();
}

void KChartParamsIface::setLegendTitleFontRelSize( int legendTitleFontRelSize )
{
    params->setLegendTitleFontRelSize(legendTitleFontRelSize);
}

int KChartParamsIface::legendTitleFontRelSize() const
{
    return params->legendTitleFontRelSize();
}

uint KChartParamsIface::legendSpacing()
{
    return params->legendSpacing();
}

void KChartParamsIface::setLineChartSubType( const QString & sub)
{
    params->setLineChartSubType(params->stringToLineChartSubType(sub));
}

QString KChartParamsIface::lineChartSubType() const
{
    return params->lineChartSubTypeToString(params->lineChartSubType());
}


bool KChartParamsIface::lineMarker() const
{
    return params->lineMarker();
}

void KChartParamsIface::setLineMarker( bool b)
{
    params->setLineMarker(b);
}

void KChartParamsIface::setLineWidth( uint width )
{
    params->setLineWidth( width );
}

uint KChartParamsIface::lineWidth() const
{
    return params->lineWidth();
}

bool KChartParamsIface::threeDLines() const
{
    return params->threeDLines();
}

void KChartParamsIface::setThreeDLines(bool b)
{
    params->setThreeDLines(b);
}

int KChartParamsIface::threeDLineDepth() const
{
    return params->threeDLineDepth();
}

void KChartParamsIface::setThreeDLineDepth( int depth )
{
    params->setThreeDLineDepth( depth );
}

int KChartParamsIface::threeDLineXRotation() const
{
    return params->threeDLineXRotation();
}

void KChartParamsIface::setThreeDLineXRotation( int degrees )
{
    params->setThreeDLineXRotation( degrees );
}

void KChartParamsIface::setThreeDLineYRotation( int degrees )
{
    params->setThreeDLineYRotation(degrees );
}

int KChartParamsIface::threeDLineYRotation() const
{
    return params->threeDLineYRotation();
}

QSize KChartParamsIface::lineMarkerSize() const
{
    return params->lineMarkerSize();
}

void KChartParamsIface::setLineMarkerSize( QSize size )
{
    params->setLineMarkerSize( size );
}


bool KChartParamsIface::showGrid()
{
    return params->showGrid();
}

QString KChartParamsIface::header1Text() const
{
    return params->header1Text();
}

void KChartParamsIface::setHeader1Text( const QString& text )
{
    params->setHeader1Text(text);
}

void KChartParamsIface::setHeader2Text( const QString& text )
{
    params->setHeader2Text(text);
}

QString KChartParamsIface::header2Text() const
{
    return params->header2Text();
}


void KChartParamsIface::setOutlineDataLineWidth( uint width )
{
    params->setOutlineDataLineWidth( width );
}

uint KChartParamsIface::outlineDataLineWidth() const
{
    return params->outlineDataLineWidth();
}

void KChartParamsIface::setAreaChartSubType(const QString &sub)
{
    params->setAreaChartSubType(params->stringToAreaChartSubType(sub));
}

QString KChartParamsIface::areaChartSubType() const
{
    return params->areaChartSubTypeToString(params->areaChartSubType());
}

void KChartParamsIface::setAreaLocation(const QString & loc)
{
    params->setAreaLocation(params->stringToAreaLocation(loc));
}

QString KChartParamsIface::areaLocation() const
{
    return params->areaLocationToString( params->areaLocation());
}

void KChartParamsIface::setHiLoChartSubType(const QString &sub)
{
    params->setHiLoChartSubType(params->stringToHiLoChartSubType(sub));
}

QString KChartParamsIface::hiLoChartSubType() const
{
    return params->hiLoChartSubTypeToString(params->hiLoChartSubType());
}

bool KChartParamsIface::hiLoChartPrintHighValues() const
{
    return params->hiLoChartPrintHighValues();
}

bool KChartParamsIface::hiLoChartLowValuesUseFontRelSize() const
{
    return params->hiLoChartLowValuesUseFontRelSize();
}

int KChartParamsIface::hiLoChartLowValuesFontRelSize() const
{
    return params->hiLoChartLowValuesFontRelSize();
}

bool KChartParamsIface::hiLoChartPrintLowValues() const
{
    return params->hiLoChartPrintLowValues();
}

int KChartParamsIface::hiLoChartOpenValuesFontRelSize() const
{
    return params->hiLoChartOpenValuesFontRelSize();
}

bool KChartParamsIface::hiLoChartPrintCloseValues() const
{
    return params->hiLoChartPrintCloseValues();
}

bool KChartParamsIface::hiLoChartCloseValuesUseFontRelSize() const
{
    return params->hiLoChartCloseValuesUseFontRelSize();
}

int KChartParamsIface::hiLoChartCloseValuesFontRelSize() const
{
    return params->hiLoChartCloseValuesFontRelSize();
}

void KChartParamsIface::setPolarLineWidth( int width  )
{
    params->setPolarLineWidth( width  );
}

int KChartParamsIface::polarLineWidth() const
{
    return params->polarLineWidth();
}

void KChartParamsIface::setPolarZeroDegreePos( int degrees )
{
    params->setPolarZeroDegreePos( degrees );
}

int KChartParamsIface::polarZeroDegreePos() const
{
    return params->polarZeroDegreePos();
}

void KChartParamsIface::setPolarRotateCircularLabels( bool rotateCircularLabels )
{
    params->setPolarRotateCircularLabels( rotateCircularLabels );
}

bool KChartParamsIface::polarRotateCircularLabels() const
{
    return params->polarRotateCircularLabels();
}

QSize KChartParamsIface::polarMarkerSize() const
{
    return params->polarMarkerSize();
}

void KChartParamsIface::setPolarMarkerSize( QSize size  )
{
    params->setPolarMarkerSize(size);
}

int KChartParamsIface::ringStart() const
{
    return params->ringStart();
}

void KChartParamsIface::setRingStart( int degrees )
{
    params->setRingStart( degrees );
}


void KChartParamsIface::setThreeDShadowColors( bool shadow )
{
    params->setThreeDShadowColors(shadow);
}
bool KChartParamsIface::threeDShadowColors()
{
    return params->threeDShadowColors();
}

}  //KChart namespace
