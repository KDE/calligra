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
*/

#ifndef DATATYPES_H
#define DATATYPES_H

#include <qlist.h>
#include <qstring.h>
#include <qpainter.h>

class SVGListItem {
public:
    virtual ~SVGListItem() {};
};
class SVGList {
public:
    SVGList() { list.setAutoDelete( true ); }
    unsigned long numberOfItems();
    
    void clear();
    SVGListItem *initialize( SVGListItem * );
    SVGListItem *getItem( unsigned long );
    SVGListItem *insertItemBefore( SVGListItem *, unsigned long );
    SVGListItem *replaceItem( SVGListItem *, unsigned long );
    SVGListItem *removeItem( unsigned long );
    SVGListItem *appendItem( SVGListItem * );
private:
    unsigned long _numberOfItems;
protected:
    QList<SVGListItem> list;
};


class SVGRect {
public:
    float x() { return _x; }
    void setX( float x ) { _x = x; }

    float y() { return _y; }
    void setY( float y ) { _y = y; }
private:
    float _x;
    float _y;
};


class SVGPointList : public SVGList {
};
class SVGPoint : public SVGListItem {
public:
    SVGPoint();
    virtual ~SVGPoint() {};
    SVGPoint( float, float );

    float x();
    void setX( float );
    float y();
    void setY( float );
    
    //SVGPoint matrixTransform( QWMatrix & );
private:
    float _x, _y;
};


class SVGNumber {
public:
    SVGNumber();
    SVGNumber( float );
    
    float value();
    void  setValue( float );
private:
    float _value;
};

class SVGElement;
class StyleProperty;
class SVGLength {
public:
    SVGLength();
    SVGLength( float );
    SVGLength( const SVGLength & );
    void setContext( SVGElement *, StyleProperty * );
    // Length Unit Types
    enum { SVG_LENGTHTYPE_UNKNOWN,
           SVG_LENGTHTYPE_NUMBER,
           SVG_LENGTHTYPE_PERCENTAGE,
           SVG_LENGTHTYPE_EMS,
           SVG_LENGTHTYPE_EXS,
           SVG_LENGTHTYPE_PX,
           SVG_LENGTHTYPE_CM,
           SVG_LENGTHTYPE_MM,
           SVG_LENGTHTYPE_IN,
           SVG_LENGTHTYPE_PT,
           SVG_LENGTHTYPE_PC };
    unsigned short unitType();
    float value();
    void  setValue( float );

    float valueInSpecifiedUnits();
    void  setValueInSpecifiedUnits( float );

    QString valueAsString();
    void    setValueAsString( const QString & );
    
    void newValueSpecifiedUnits( unsigned short, float );
    void convertToSpecifiedUnits( unsigned short );
    float convertNumToPt( QString s );
private:
    unsigned short _unitType;
    float          _value;
    float          _valueInSpecifiedUnits;
    QString        _valueAsString;
    
    SVGElement *elem;
    StyleProperty *style;
protected:
    QPainter dummy;
};

#endif
