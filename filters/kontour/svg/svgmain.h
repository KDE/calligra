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

#ifndef SVGMAIN_H
#define SVGMAIN_H

#include <qlist.h>
#include <qxml.h>
#include "datatypes.h"
#include "transform.h"
#include "style.h"

class GObject;
class GGroup;
class GPixmap;

class SVGComposite;
class SVGSVGElement;
class SVGElement {
public:
    QString id() { return m_id; }
    void setId( const QString &id ) { m_id = id; }
    bool hasIdentifier() { return !m_id.isEmpty(); }

    virtual GObject *build() = 0;
    virtual void setAttributes( const QXmlAttributes& atts );

    bool hasParent() { return parent; };
    virtual void setParent( SVGComposite *par );
    
    SVGSVGElement *ownerSVGElement() { return _ownerSVGElement; };
protected:
    SVGComposite *parent;
private:
    QString m_id;
    SVGSVGElement *_ownerSVGElement;
};


class SVGGraphicElement : public SVGElement,
                          public StyleProperty,
                          public SVGTransformable {
public:
    SVGGraphicElement( SVGComposite *par = 0 );
    virtual ~SVGGraphicElement();
//    virtual GObject *build() = 0;
    virtual void setAttributes( const QXmlAttributes& atts );
    virtual void setParent( SVGComposite *par );
};


class SVGComposite : public SVGElement,
                     public StyleProperty,
                     public SVGTransformable {
public:
    SVGComposite( SVGComposite *par = 0 );
    virtual ~SVGComposite();
    virtual GObject *build();
    void addObject( SVGElement *obj ) { if (obj ) children.append( obj ); };
    virtual QString type() { return "composite"; };
    virtual void setAttributes( const QXmlAttributes& );
    virtual void addContent( const QString & ) {};
protected:
    GGroup *group;
    QList<SVGElement> children;
    
    friend class SVGTextElement;
};


class SVGGElement : public SVGComposite {
public:
    SVGGElement( SVGComposite *par = 0 ) : SVGComposite( par ) {};
    virtual QString type() { return "group"; };
};


class SVGSVGElement : public SVGComposite {
public:
    SVGSVGElement( SVGComposite *par = 0 ) : SVGComposite( par ) {};
    virtual void setAttributes( const QXmlAttributes& );
    virtual QString type() { return "svg"; };
    
    SVGLength width() { return _width; };
    SVGLength height() { return _height; };
private:
    SVGLength _width;
    SVGLength _height;
};


class SVGSymbolElement : public SVGComposite {
public:
    SVGSymbolElement( SVGComposite *par = 0 );
    virtual QString type() { return "symbol"; };
};


class SVGDefsElement : public SVGComposite {
public:
    SVGDefsElement( SVGComposite *par = 0 ) : SVGComposite( par ) {};
    virtual QString type() { return "defs"; };
    virtual GObject *build();
};


class SVGImageElement : public SVGGraphicElement {
public:
    SVGImageElement( SVGComposite *par = 0 ) : SVGGraphicElement(par) {};
    virtual QString type() { return "image"; };
    virtual void setAttributes( const QXmlAttributes & );
    virtual GObject *build();
private:
    SVGLength x, y;
    SVGLength width, height;
    QString uri;
    GPixmap *obj;
    //SVGElementInstance *instanceRoot;
};
#endif
