/* This file is part of the KDE libraries
    Copyright (C) 2001, 2002, 2003, 2004 Laurent Montel <montel@kde.org>

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



#ifndef KCHART_PARAMS_IFACE_H
#define KCHART_PARAMS_IFACE_H

#include <dcopobject.h>
#include <dcopref.h>
#include <QString>
#include <QSize>
namespace KChart
{

class KChartParams;

class KChartParamsIface : virtual public DCOPObject
{
    K_DCOP
public:
    KChartParamsIface( KChartParams *_params );

k_dcop:
    virtual QString chartType()const;
    virtual void setChartType( const QString & );
    
    //bar chart config
    virtual bool threeDBars();
    virtual void setThreeDBars( bool threeDBars );
    virtual void setThreeDBarsShadowColors( bool shadow );
    virtual bool threeDBarsShadowColors() const;
    virtual void setThreeDBarAngle( uint angle );
    virtual uint threeDBarAngle() const;
    virtual void setThreeDBarDepth( double depth );
    virtual double threeDBarDepth() const;
    virtual QString barChartSubType() const;
    virtual void setBarChartSubType( const QString & );

    //line chart config
    virtual bool lineMarker() const;
    virtual void setLineMarker( bool );
    virtual void setLineWidth( uint width );
    virtual uint lineWidth() const;
    virtual void setLineChartSubType( const QString & );
    virtual QString lineChartSubType() const;
    virtual bool threeDLines() const;
    virtual void setThreeDLines(bool b);
    virtual void setThreeDLineDepth( int depth );
    virtual int threeDLineDepth() const;
    virtual int threeDLineXRotation() const;
    virtual void setThreeDLineXRotation( int degrees );
    virtual void setThreeDLineYRotation( int degrees );
    virtual int threeDLineYRotation() const;
    virtual QSize lineMarkerSize() const;
    virtual void setLineMarkerSize( QSize size );


    //pie config
    virtual bool threeDPies();
    virtual void setThreeDPieHeight( int pixels );
    virtual int threeDPieHeight();
    virtual void setPieStart( int degrees );
    virtual int pieStart();
    virtual void setExplode( bool explode );
    virtual bool explode() const;
    virtual void setExplodeFactor( double factor );
    virtual double explodeFactor() const;
    virtual void setThreeDPies( bool threeDPies );

    //area chart
    virtual void setAreaChartSubType(const QString &);
    virtual QString areaChartSubType() const;
    virtual void setAreaLocation(const QString & );
    virtual QString areaLocation() const;

    //hilo chart
    virtual void setHiLoChartSubType(const QString &);
    virtual QString hiLoChartSubType() const;
    virtual bool hiLoChartPrintHighValues() const;
    virtual bool hiLoChartLowValuesUseFontRelSize() const;
    virtual int hiLoChartLowValuesFontRelSize() const;
    virtual bool hiLoChartPrintLowValues() const;
    virtual int hiLoChartOpenValuesFontRelSize() const;
    virtual bool hiLoChartPrintCloseValues() const;
    virtual bool hiLoChartCloseValuesUseFontRelSize() const;
    virtual int hiLoChartCloseValuesFontRelSize() const;


    //polar chart
    virtual void setPolarLineWidth( int width  );
    virtual int polarLineWidth() const;
    virtual void setPolarZeroDegreePos( int degrees );
    virtual int polarZeroDegreePos() const;
    virtual void setPolarRotateCircularLabels( bool rotateCircularLabels );
    virtual bool polarRotateCircularLabels() const;
    virtual QSize polarMarkerSize() const;
    virtual void setPolarMarkerSize( QSize size  );

    //ring chart
    virtual int ringStart() const;
    virtual void setRingStart( int degrees );


    //legend
    virtual void hideLegend();
    virtual void setLegendPosition(const QString &);
    virtual QString legendPostion() const;
    virtual void setLegendTitleText( const QString& text );
    virtual void setLegendSpacing( uint space );
    virtual uint legendSpacing();
    //legend font
    virtual void setLegendFontRelSize( int legendFontRelSize );
    virtual int legendFontRelSize() const;
    virtual void setLegendFontUseRelSize( bool legendFontUseRelSize );
    virtual bool legendFontUseRelSize() const;
    virtual void setLegendTitleFontUseRelSize( bool legendTitleFontUseRelSize );
    virtual bool legendTitleFontUseRelSize() const;
    virtual void setLegendTitleFontRelSize( int legendTitleFontRelSize );
    virtual int legendTitleFontRelSize() const;
    

    virtual bool showGrid();
    
    virtual QString header1Text() const;
    virtual void setHeader1Text( const QString& text );
    virtual void setHeader2Text( const QString& text );
    virtual QString header2Text() const;

    virtual uint outlineDataLineWidth() const;
    virtual void setOutlineDataLineWidth( uint width );

    virtual void setThreeDShadowColors( bool shadow );
    virtual bool threeDShadowColors();

private:
    KChartParams *params;

};

}  //KChart namespace

#endif
