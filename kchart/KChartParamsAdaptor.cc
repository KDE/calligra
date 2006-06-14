/* This file is part of the KDE project
   Copyright (C) 2001,2002,2003,2004 Laurent Montel <montel@kde.org>
   Copyright (C) 2006 Fredrik Edemar <f_edemar@linux.se>

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

#include "KChartParamsAdaptor.h"

#include "kchart_params.h"

namespace KChart
{

KChartParamsAdaptor::KChartParamsAdaptor( KChartParams *params_ )
    : QDBusAbstractAdaptor( params_ )
{
    setAutoRelaySignals(true);
    params = params_;
}

QString KChartParamsAdaptor::chartType()const
{
    return params->chartTypeToString(params->chartType());
}

void KChartParamsAdaptor::setChartType( const QString &chart )
{
    params->setChartType(params->stringToChartType( chart ));
}

QString KChartParamsAdaptor::barChartSubType() const
{
    return params->barChartSubTypeToString(params->barChartSubType());
}

void KChartParamsAdaptor::setBarChartSubType( const QString &sub )
{
    params->setBarChartSubType(params->stringToBarChartSubType( sub ));
}

bool KChartParamsAdaptor::threeDBars()
{
    return params->threeDBars();
}

void KChartParamsAdaptor::setThreeDBars( bool threeDBars )
{
    params->setThreeDBars(threeDBars);
}

void KChartParamsAdaptor::setThreeDBarsShadowColors( bool shadow )
{
    params->setThreeDBarsShadowColors(shadow);
}

bool KChartParamsAdaptor::threeDBarsShadowColors() const
{
    return params->threeDBarsShadowColors();
}

void KChartParamsAdaptor::setThreeDBarAngle( uint angle )
{
    params->setThreeDBarAngle(angle);
}

uint KChartParamsAdaptor::threeDBarAngle() const
{
    return params->threeDBarAngle();
}

void KChartParamsAdaptor::setThreeDBarDepth( double depth )
{
    params->setThreeDBarDepth(depth);
}

double KChartParamsAdaptor::threeDBarDepth() const
{
    return params->threeDBarDepth();
}


bool KChartParamsAdaptor::threeDPies()
{
    return params->threeDPies();
}

void KChartParamsAdaptor::setThreeDPieHeight( int pixels )
{
    params->setThreeDPieHeight(pixels);
}

int KChartParamsAdaptor::threeDPieHeight()
{
    return params->threeDPieHeight();
}

void KChartParamsAdaptor::setPieStart( int degrees )
{
    params->setPieStart(degrees);
}

int KChartParamsAdaptor::pieStart()
{
    return params->pieStart();
}

void KChartParamsAdaptor::setExplode( bool explode )
{
    params->setExplode(explode);
}

bool KChartParamsAdaptor::explode() const
{
    return params->explode();
}

void KChartParamsAdaptor::setExplodeFactor( double factor )
{
    params->setExplodeFactor(factor);
}

double KChartParamsAdaptor::explodeFactor() const
{
    return params->explodeFactor();
}

void KChartParamsAdaptor::setThreeDPies( bool threeDPies )
{
    params->setThreeDPies(threeDPies);
}

void KChartParamsAdaptor::hideLegend()
{
    params->setLegendPosition(KDChartParams::NoLegend);
}

void KChartParamsAdaptor::setLegendPosition(const QString & pos)
{
    params->setLegendPosition(params->stringToLegendPosition(pos));
}

QString KChartParamsAdaptor::legendPostion() const
{
    return params->legendPositionToString(params->legendPosition());
}

void KChartParamsAdaptor::setLegendTitleText( const QString& text )
{
    params->setLegendTitleText( text );
}

void KChartParamsAdaptor::setLegendSpacing( uint space )
{
    params->setLegendSpacing(space );
}

void KChartParamsAdaptor::setLegendFontRelSize( int legendFontRelSize )
{
    params->setLegendFontRelSize(legendFontRelSize);
}

int KChartParamsAdaptor::legendFontRelSize() const
{
    return params->legendFontRelSize();
}

void KChartParamsAdaptor::setLegendFontUseRelSize( bool legendFontUseRelSize )
{
    params->setLegendFontUseRelSize(legendFontUseRelSize);
}

bool KChartParamsAdaptor::legendFontUseRelSize() const
{
    return params->legendFontUseRelSize();
}

void KChartParamsAdaptor::setLegendTitleFontUseRelSize( bool legendTitleFontUseRelSize )
{
    params->setLegendTitleFontUseRelSize(legendTitleFontUseRelSize);
}

bool KChartParamsAdaptor::legendTitleFontUseRelSize() const
{
    return params->legendTitleFontUseRelSize();
}

void KChartParamsAdaptor::setLegendTitleFontRelSize( int legendTitleFontRelSize )
{
    params->setLegendTitleFontRelSize(legendTitleFontRelSize);
}

int KChartParamsAdaptor::legendTitleFontRelSize() const
{
    return params->legendTitleFontRelSize();
}

uint KChartParamsAdaptor::legendSpacing()
{
    return params->legendSpacing();
}

void KChartParamsAdaptor::setLineChartSubType( const QString & sub)
{
    params->setLineChartSubType(params->stringToLineChartSubType(sub));
}

QString KChartParamsAdaptor::lineChartSubType() const
{
    return params->lineChartSubTypeToString(params->lineChartSubType());
}


bool KChartParamsAdaptor::lineMarker() const
{
    return params->lineMarker();
}

void KChartParamsAdaptor::setLineMarker( bool b)
{
    params->setLineMarker(b);
}

void KChartParamsAdaptor::setLineWidth( uint width )
{
    params->setLineWidth( width );
}

uint KChartParamsAdaptor::lineWidth() const
{
    return params->lineWidth();
}

bool KChartParamsAdaptor::threeDLines() const
{
    return params->threeDLines();
}

void KChartParamsAdaptor::setThreeDLines(bool b)
{
    params->setThreeDLines(b);
}

int KChartParamsAdaptor::threeDLineDepth() const
{
    return params->threeDLineDepth();
}

void KChartParamsAdaptor::setThreeDLineDepth( int depth )
{
    params->setThreeDLineDepth( depth );
}

int KChartParamsAdaptor::threeDLineXRotation() const
{
    return params->threeDLineXRotation();
}

void KChartParamsAdaptor::setThreeDLineXRotation( int degrees )
{
    params->setThreeDLineXRotation( degrees );
}

void KChartParamsAdaptor::setThreeDLineYRotation( int degrees )
{
    params->setThreeDLineYRotation(degrees );
}

int KChartParamsAdaptor::threeDLineYRotation() const
{
    return params->threeDLineYRotation();
}

QSize KChartParamsAdaptor::lineMarkerSize() const
{
    return params->lineMarkerSize();
}

void KChartParamsAdaptor::setLineMarkerSize( QSize size )
{
    params->setLineMarkerSize( size );
}


bool KChartParamsAdaptor::showGrid()
{
    return params->showGrid();
}

QString KChartParamsAdaptor::header1Text() const
{
    return params->header1Text();
}

void KChartParamsAdaptor::setHeader1Text( const QString& text )
{
    params->setHeader1Text(text);
}

void KChartParamsAdaptor::setHeader2Text( const QString& text )
{
    params->setHeader2Text(text);
}

QString KChartParamsAdaptor::header2Text() const
{
    return params->header2Text();
}


void KChartParamsAdaptor::setOutlineDataLineWidth( uint width )
{
    params->setOutlineDataLineWidth( width );
}

uint KChartParamsAdaptor::outlineDataLineWidth() const
{
    return params->outlineDataLineWidth();
}

void KChartParamsAdaptor::setAreaChartSubType(const QString &sub)
{
    params->setAreaChartSubType(params->stringToAreaChartSubType(sub));
}

QString KChartParamsAdaptor::areaChartSubType() const
{
    return params->areaChartSubTypeToString(params->areaChartSubType());
}

void KChartParamsAdaptor::setAreaLocation(const QString & loc)
{
    params->setAreaLocation(params->stringToAreaLocation(loc));
}

QString KChartParamsAdaptor::areaLocation() const
{
    return params->areaLocationToString( params->areaLocation());
}

void KChartParamsAdaptor::setHiLoChartSubType(const QString &sub)
{
    params->setHiLoChartSubType(params->stringToHiLoChartSubType(sub));
}

QString KChartParamsAdaptor::hiLoChartSubType() const
{
    return params->hiLoChartSubTypeToString(params->hiLoChartSubType());
}

bool KChartParamsAdaptor::hiLoChartPrintHighValues() const
{
    return params->hiLoChartPrintHighValues();
}

bool KChartParamsAdaptor::hiLoChartLowValuesUseFontRelSize() const
{
    return params->hiLoChartLowValuesUseFontRelSize();
}

int KChartParamsAdaptor::hiLoChartLowValuesFontRelSize() const
{
    return params->hiLoChartLowValuesFontRelSize();
}

bool KChartParamsAdaptor::hiLoChartPrintLowValues() const
{
    return params->hiLoChartPrintLowValues();
}

int KChartParamsAdaptor::hiLoChartOpenValuesFontRelSize() const
{
    return params->hiLoChartOpenValuesFontRelSize();
}

bool KChartParamsAdaptor::hiLoChartPrintCloseValues() const
{
    return params->hiLoChartPrintCloseValues();
}

bool KChartParamsAdaptor::hiLoChartCloseValuesUseFontRelSize() const
{
    return params->hiLoChartCloseValuesUseFontRelSize();
}

int KChartParamsAdaptor::hiLoChartCloseValuesFontRelSize() const
{
    return params->hiLoChartCloseValuesFontRelSize();
}

void KChartParamsAdaptor::setPolarLineWidth( int width  )
{
    params->setPolarLineWidth( width  );
}

int KChartParamsAdaptor::polarLineWidth() const
{
    return params->polarLineWidth();
}

void KChartParamsAdaptor::setPolarZeroDegreePos( int degrees )
{
    params->setPolarZeroDegreePos( degrees );
}

int KChartParamsAdaptor::polarZeroDegreePos() const
{
    return params->polarZeroDegreePos();
}

void KChartParamsAdaptor::setPolarRotateCircularLabels( bool rotateCircularLabels )
{
    params->setPolarRotateCircularLabels( rotateCircularLabels );
}

bool KChartParamsAdaptor::polarRotateCircularLabels() const
{
    return params->polarRotateCircularLabels();
}

QSize KChartParamsAdaptor::polarMarkerSize() const
{
    return params->polarMarkerSize();
}

void KChartParamsAdaptor::setPolarMarkerSize( QSize size  )
{
    params->setPolarMarkerSize(size);
}

int KChartParamsAdaptor::ringStart() const
{
    return params->ringStart();
}

void KChartParamsAdaptor::setRingStart( int degrees )
{
    params->setRingStart( degrees );
}


void KChartParamsAdaptor::setThreeDShadowColors( bool shadow )
{
    params->setThreeDShadowColors(shadow);
}
bool KChartParamsAdaptor::threeDShadowColors()
{
    return params->threeDShadowColors();
}

}  //KChart namespace

#include "KChartParamsAdaptor.moc"
