/*
    Copyright (C) 2001, Rob Buis <rwlbuis@wanadoo.nl>.
    This file is part of the KDE project

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

DESCRIPTION
    Filter to convert a svg file to a killustrator document using koDocument
*/

#include "datatypes.h"
#include <units.h>
#include <qregexp.h>
#include <qpaintdevice.h>
#include <qpaintdevicemetrics.h>
#include "svgmain.h"
#include <kdebug.h>

//#include <qfontmetrics.h>

static QString UNIT_EMS("em");
static QString UNIT_MM("mm");
static QString UNIT_CM("cm");
static QString UNIT_INCH("in");
static QString UNIT_PICA("pc");
static QString UNIT_POINT("pt");
static QString UNIT_PERC("%");

/********************* Data structures *******************************/

SVGLength::SVGLength() 
{
    _unitType = SVG_LENGTHTYPE_UNKNOWN;
    _value = 0;
    _valueInSpecifiedUnits = 0;
    _valueAsString = "0";
    elem  = NULL;
    style = NULL;
    
//    QPaintDeviceMetrics metr( dummy.device() );
//    kdDebug() << " dpi : " << metr.logicalDpiY() << endl;
}

// experimental
SVGLength::SVGLength( float value )
{
    _unitType = SVG_LENGTHTYPE_UNKNOWN;
    _value = value;
    _valueInSpecifiedUnits = value;
    elem  = NULL;
    style = NULL;
    //_valueAsString = "0";
}

SVGLength::SVGLength( const SVGLength &len )
{
    _unitType              = len._unitType;
    _value                 = len._value;
    _valueInSpecifiedUnits = len._valueInSpecifiedUnits;
    _valueAsString         = len._valueAsString;
}


void SVGLength::setContext( SVGElement *el, StyleProperty *st )
{
    elem  = el;
    style = st;
}


float SVGLength::value()
{
    return _value;
}


void SVGLength::setValue( float value )
{
    _value = value;
}


float SVGLength::valueInSpecifiedUnits()
{
    return _valueInSpecifiedUnits;
}


void SVGLength::setValueInSpecifiedUnits( float valueInSpecifiedUnits )
{
    _valueInSpecifiedUnits = valueInSpecifiedUnits;
}


QString SVGLength::valueAsString()
{
    return _valueAsString;
}


void SVGLength::setValueAsString( const QString &valueAsString )
{
    _valueAsString = valueAsString;
    _value = convertNumToPt( _valueAsString );
}


void SVGLength::newValueSpecifiedUnits( unsigned short unitType, float valueInSpecifiedUnits )
{
    _valueInSpecifiedUnits = valueInSpecifiedUnits;
    _valueAsString.setNum( _valueInSpecifiedUnits );
    switch( _unitType )
    {
    case SVG_LENGTHTYPE_EMS:    _valueAsString.append( UNIT_EMS );
                                break;
    case SVG_LENGTHTYPE_MM:     _valueAsString.append( UNIT_MM );
                                break;
    case SVG_LENGTHTYPE_CM:     _valueAsString.append( UNIT_CM );
                                break;
    case SVG_LENGTHTYPE_IN:     _valueAsString.append( UNIT_INCH );
                                break;
    case SVG_LENGTHTYPE_PC:     _valueAsString.append( UNIT_PICA );
                                break;
    case SVG_LENGTHTYPE_PT:     _valueAsString.append( UNIT_POINT );
                                break;
//    default:
    }
    kdDebug() << "_valueAsString : " << _valueAsString.local8Bit() << endl;
    _value = convertNumToPt( _valueAsString );
}


void SVGLength::convertToSpecifiedUnits( unsigned short unitType )
{
//    _unitType = unitType;
//    _value = convertNumToPt( _valueAsString );
//    _valueInSpecifiedUnits = _value;
}


float SVGLength::convertNumToPt( QString s ) {
    bool bOK;

    if( s.isEmpty() ) return -1.0;
    //kdDebug() << "Converting: " << s.local8Bit() << " to : " << s.toFloat(&bOK) << endl;
    //kdDebug()<< "bOK: " << bOK << endl;
    float convNum = s.toFloat(&bOK);
    _valueInSpecifiedUnits = convNum;
    if( !bOK ) {
        QRegExp reg("[0-9 -.]");
	s.replace(reg, "");
	if(s.compare(UNIT_MM) == 0) {
            convNum = cvtMmToPt(convNum);
	    _unitType = SVG_LENGTHTYPE_MM;
	}
	else if(s.compare(UNIT_CM) == 0) {
            convNum = cvtCmToPt(convNum);
	    _unitType = SVG_LENGTHTYPE_CM;
 	}
	else if(s.compare(UNIT_INCH) == 0) {
            convNum = cvtInchToPt(convNum);
	    _unitType = SVG_LENGTHTYPE_IN;
 	}
	else if(s.compare(UNIT_PICA) == 0) {
            convNum = cvtPicaToPt(convNum);
	    _unitType = SVG_LENGTHTYPE_PC;
 	}
	else if(s.compare(UNIT_POINT) == 0) {
            _unitType = SVG_LENGTHTYPE_PT;
	}
	else if(s.compare(UNIT_PERC) == 0) {
            _unitType = SVG_LENGTHTYPE_PERCENTAGE;
	    if( elem )
	    {
	        if( SVGSVGElement *svgsvg = elem->ownerSVGElement() )
		{
                    convNum = ( convNum * svgsvg->width().value() ) / 100.0;
		    kdDebug() << " Parent width : " << svgsvg->width().value() << endl;
		}
	    }
	}
	else if( s.compare(UNIT_EMS) == 0 ) {
	    _unitType = SVG_LENGTHTYPE_EMS;
	    if( style )
                convNum *= style->font.pixelSize();
	}
    }
    return convNum;
}


SVGPoint::SVGPoint() : _x( 0.0 ), _y( 0.0 )
{
}


SVGPoint::SVGPoint( float x, float y ) : _x( x ), _y( y )
{
}


float SVGPoint::x()
{
    return _x; 
}


void SVGPoint::setX( float x )
{
    _x = x;
}


float SVGPoint::y()
{
    return _y;
}


void SVGPoint::setY( float y )
{
    _y = y;
}


SVGNumber::SVGNumber()
{
    _value = 0;
}


SVGNumber::SVGNumber( float value )
{
    _value = value;
}


float SVGNumber::value()
{
    return _value;
}


void SVGNumber::setValue( float value )
{
    _value = value;
}


unsigned long SVGList::numberOfItems() {
    return list.count();
}


void SVGList::clear() {
    list.clear();
}


SVGListItem *SVGList::getItem( unsigned long index ) {
    return list.at( index );
}


SVGListItem *SVGList::appendItem( SVGListItem *item ) {
    list.append( item );
    return item;
}
