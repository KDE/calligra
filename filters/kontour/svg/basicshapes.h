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

#ifndef BASICSHAPES_H
#define BASICSHAPES_H

#include <qxml.h>
#include "datatypes.h"
#include "svgmain.h"

class GOval;
class GPolyline;
class GPolygon;
class SVGRectElement : public SVGGraphicElement {
public:
    SVGRectElement( SVGComposite *par = 0 ) : SVGGraphicElement( par ) {};
    virtual void setAttributes( const QXmlAttributes & );
    virtual GObject *build();
private:
    SVGLength x, y;
    SVGLength rx, ry;
    SVGLength width, height;
    GPolygon *obj;
};


class SVGCircleElement : public SVGGraphicElement {
public:
    SVGCircleElement( SVGComposite *par = 0 ) : SVGGraphicElement( par ) {};
    virtual void setAttributes( const QXmlAttributes & );
    virtual GObject *build();
private:
    SVGLength cx, cy;
    SVGLength r;
    GOval *obj;
};


class SVGEllipseElement : public SVGGraphicElement {
public:
    SVGEllipseElement( SVGComposite *par = 0 ) : SVGGraphicElement( par ) {};
    virtual void setAttributes( const QXmlAttributes & );
    virtual GObject *build();
private:
    SVGLength cx, cy;
    SVGLength rx, ry;
    GOval *obj;
};


class SVGLineElement : public SVGGraphicElement {
public:
    SVGLineElement( SVGComposite *par = 0 ) : SVGGraphicElement( par ) {};
    virtual void setAttributes( const QXmlAttributes & );
    virtual GObject *build();
private:
    SVGLength x1, x2;
    SVGLength y1, y2;
    GPolyline *obj;
};


class SVGPolylineElement : public SVGGraphicElement, 
                           public SVGPointList {
public:
    SVGPolylineElement( SVGComposite *par = 0 ) : SVGGraphicElement( par ) {};
    virtual void setAttributes( const QXmlAttributes & );
    virtual GObject *build();
protected:
    QString points;
    GObject *processPoints( bool bPolygon = false );
};


class SVGPolygonElement : public SVGPolylineElement {
public:
    SVGPolygonElement( SVGComposite *par = 0 ) : SVGPolylineElement( par ) {};
    virtual GObject *build();
};

#endif
