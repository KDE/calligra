// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2005-2006 Thorsten Zachmann <zachmann@kde.org>

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

#include "KPrBrush.h"

#include <KoGenStyles.h>
#include <KoOasisStyles.h>
#include <KoOasisContext.h>
#include <KoXmlNS.h>
#include "KPrDocument.h"

KPrBrush::KPrBrush()
    : KoBrush()
{
}

KPrBrush::KPrBrush( const QBrush &brush, const QColor &gColor1, const QColor &gColor2,
                    BCType gType, FillType fillType, bool unbalanced,
                    int xfactor, int yfactor )
    : KoBrush( brush, gColor1,gColor2, gType, fillType, unbalanced, xfactor, yfactor )
{
}

void KPrBrush::saveOasisFillStyle( KoGenStyle &styleObjectAuto, KoGenStyles& mainStyles ) const
{
    switch ( getFillType() )
    {
        case FT_BRUSH:
        {
            if( getBrush().style() != Qt::NoBrush )
            {
                KoOasisStyles::saveOasisFillStyle( styleObjectAuto, mainStyles, getBrush() );
            }
            else
            {
                styleObjectAuto.addProperty( "draw:fill","none" );
            }
            break;
        }
        case FT_GRADIENT:
            styleObjectAuto.addProperty( "draw:fill","gradient" );
            styleObjectAuto.addProperty( "draw:fill-gradient-name", saveOasisGradientStyle( mainStyles ) );
            break;
    }
}


QString KPrBrush::saveOasisGradientStyle( KoGenStyles& mainStyles ) const
{
    KoGenStyle gradientStyle( KPrDocument::STYLE_GRADIENT /*no family name*/);
    gradientStyle.addAttribute( "draw:start-color", getGColor1().name() );
    gradientStyle.addAttribute( "draw:end-color", getGColor2().name() );

    QString unbalancedx( "50%" );
    QString unbalancedy( "50%" );

    if ( getGUnbalanced() )
    {
        unbalancedx = QString( "%1%" ).arg( getGXFactor() / 4 + 50 );
        unbalancedy = QString( "%1%" ).arg( getGYFactor() / 4 + 50 );
    }
    gradientStyle.addAttribute( "draw:cx", unbalancedx );
    gradientStyle.addAttribute( "draw:cy", unbalancedy );

    switch( getGType() )
    {
        case BCT_PLAIN:
            gradientStyle.addAttribute( "draw:angle", 0 );
            gradientStyle.addAttribute( "draw:style", "linear" );
            break;
        case BCT_GHORZ:
            gradientStyle.addAttribute( "draw:angle", 0 );
            gradientStyle.addAttribute( "draw:style", "linear" );
            break;
        case BCT_GVERT:
            gradientStyle.addAttribute( "draw:angle", 900 );
            gradientStyle.addAttribute( "draw:style", "linear" );
            break;
        case BCT_GDIAGONAL1:
            gradientStyle.addAttribute( "draw:angle", 450 );
            gradientStyle.addAttribute( "draw:style", "linear" );
            break;
        case BCT_GDIAGONAL2:
            gradientStyle.addAttribute( "draw:angle", 135 );
            gradientStyle.addAttribute( "draw:style", "linear" );
            break;
        case BCT_GCIRCLE:
            gradientStyle.addAttribute( "draw:angle", 0 );
            gradientStyle.addAttribute( "draw:style", "radial" );
            break;
        case BCT_GRECT:
            gradientStyle.addAttribute( "draw:angle", 0 );
            gradientStyle.addAttribute( "draw:style", "square" );
            break;
        case BCT_GPIPECROSS:
            gradientStyle.addAttribute( "draw:angle", 0 );
            gradientStyle.addAttribute( "draw:style", "axial" );
            break;
        case BCT_GPYRAMID: //todo fixme ! it doesn't work !
            gradientStyle.addAttribute( "draw:angle", 0 );
            gradientStyle.addAttribute( "draw:style", 0 );
            break;
    }

    return mainStyles.lookup( gradientStyle, "gradient" );
}


void KPrBrush::loadOasisFillStyle( KoOasisContext &context, const char * propertyType )
{
    KoStyleStack &styleStack = context.styleStack();
    styleStack.setTypeProperties( propertyType );

    if ( styleStack.hasAttributeNS( KoXmlNS::draw, "fill" ) )
    {
        const QString fill = styleStack.attributeNS( KoXmlNS::draw, "fill" );
        kdDebug(33001) << " load object gradient fill type :" << fill << endl;

        if ( fill == "solid" || fill == "hatch" )
        {
            setBrush( KoOasisStyles::loadOasisFillStyle( styleStack, fill, context.oasisStyles() ) );
        }
        else if ( fill == "gradient" )
        {
            QString style = styleStack.attributeNS( KoXmlNS::draw, "fill-gradient-name" );
            QDomElement* draw = context.oasisStyles().drawStyles()[style];

            if ( draw )
            {
                setGColor1( draw->attributeNS( KoXmlNS::draw, "start-color", QString::null ) );
                setGColor2( draw->attributeNS( KoXmlNS::draw, "end-color", QString::null ) );

                QString type = draw->attributeNS( KoXmlNS::draw, "style", QString::null );
                kdDebug()<<" type :"<<type<<endl;
                if ( type == "linear" )
                {
                    int angle = draw->attributeNS( KoXmlNS::draw, "angle", QString::null ).toInt() / 10;

                    // make sure the angle is between 0 and 359
                    angle = abs( angle );
                    angle -= ( (int) ( angle / 360 ) ) * 360;

                    // What we are trying to do here is to find out if the given
                    // angle belongs to a horizontal, vertical or diagonal gradient.
                    int lower, upper, nearAngle = 0;
                    for ( lower = 0, upper = 45; upper < 360; lower += 45, upper += 45 )
                    {
                        if ( upper >= angle )
                        {
                            int distanceToUpper = abs( angle - upper );
                            int distanceToLower = abs( angle - lower );
                            nearAngle = distanceToUpper > distanceToLower ? lower : upper;
                            break;
                        }
                    }
                    // nearAngle should now be one of: 0, 45, 90, 135, 180...
                    if ( nearAngle == 0 || nearAngle == 180 )
                        setGType( BCT_GHORZ ); // horizontal
                    else if ( nearAngle == 90 || nearAngle == 270 )
                        setGType( BCT_GVERT ); // vertical
                    else if ( nearAngle == 45 || nearAngle == 225 )
                        setGType( BCT_GDIAGONAL1 ); // diagonal 1
                    else if ( nearAngle == 135 || nearAngle == 315 )
                        setGType( BCT_GDIAGONAL2 ); // diagonal 2

                    if ( nearAngle == 180 || nearAngle == 270 || nearAngle == 225 || nearAngle == 315 )
                    {
                        setGColor1( draw->attributeNS( KoXmlNS::draw, "end-color", QString::null ) );
                        setGColor2( draw->attributeNS( KoXmlNS::draw, "start-color", QString::null ) );
                    }
                }
                else if ( type == "radial" || type == "ellipsoid" )
                    setGType( BCT_GCIRCLE ); // circle
                else if ( type == "square" || type == "rectangular" )
                    setGType( BCT_GRECT ); // rectangle
                else if ( type == "axial" )
                    setGType( BCT_GPIPECROSS ); // pipecross
                else //safe
                    setGType( BCT_PLAIN ); // plain

                // Hard to map between x- and y-center settings of ooimpress
                // and (un-)balanced settings of kpresenter. Let's try it.
                int x, y;
                if ( draw->hasAttributeNS( KoXmlNS::draw, "cx" ) )
                    x = draw->attributeNS( KoXmlNS::draw, "cx", QString::null ).remove( '%' ).toInt();
                else
                    x = 50;

                if ( draw->hasAttributeNS( KoXmlNS::draw, "cy" ) )
                    y = draw->attributeNS( KoXmlNS::draw, "cy", QString::null ).remove( '%' ).toInt();
                else
                    y = 50;

                if ( x == 50 && y == 50 )
                {
                    setGUnbalanced( false );
                    setGXFactor( 100 );
                    setGYFactor( 100 );
                }
                else
                {
                    setGUnbalanced( true );
                    // map 0 - 100% to -200 - 200
                    setGXFactor( 4 * x - 200 );
                    setGYFactor( 4 * y - 200 );
                }
            }

            // We have to set a brush with brushstyle != no background fill
            // otherwise the properties dialog for the object won't
            // display the preview for the gradient.
            QBrush tmpBrush;
            tmpBrush.setStyle( static_cast<Qt::BrushStyle>( 1 ) );
            setBrush( tmpBrush );
            setFillType( FT_GRADIENT );
        }
        else if ( fill == "none" )
        {
            //nothing
        }
        else if ( fill == "bitmap" )
        {
            //todo
            //not implementer in kpresenter...
            //the drawing object is filled with the bitmap specified by the draw:fill-image-name attribute.
            //QBrush implement setPixmap
            //easy just add pixmap and store it.
        }
    }
}
