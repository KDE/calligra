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

#ifndef KCHART_PARAMS_IFACE_H
#define KCHART_PARAMS_IFACE_H

#include <dcopobject.h>
#include <dcopref.h>
#include <qstring.h>

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

    //hilo chart
    virtual void setHiLoChartSubType(const QString &);
    virtual QString hiLoChartSubType() const;

    //legend
    virtual void hideLegend();
    virtual void setLegendPosition(const QString &);
    virtual QString legendPostion() const;
    virtual void setLegendTitleText( const QString& text );
    virtual void setLegendSpacing( uint space );
    virtual uint legendSpacing();

    virtual bool showGrid();
    
    virtual QString header1Text() const;
    virtual void setHeader1Text( const QString& text );
    virtual void setHeader2Text( const QString& text );
    virtual QString header2Text() const;

    virtual uint outlineDataLineWidth() const;
    virtual void setOutlineDataLineWidth( uint width );

private:
    KChartParams *params;

};

#endif
