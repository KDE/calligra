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

#ifndef SVGIMPORT_H
#define SVGIMPORT_H

#include <qxml.h>
#include <koFilter.h>
#include "svgmain.h"

class SVGHandler : public QXmlDefaultHandler
{
public:
    SVGHandler();
    virtual ~SVGHandler();
 
    // return the error protocol if parsing failed
    QString errorProtocol();
 
    // overloaded handler functions
    bool startDocument();
    bool startElement( const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts );
    bool endElement( const QString &namespaceURI, const QString &localName, const QString &qName );
    bool characters( const QString &ch );
 
    //QString errorString();
 
    bool fatalError( const QXmlParseException & );
private:
    enum State { Normal, Defs };
    int state;
};


class SVGImport : public KoFilter
{
    Q_OBJECT

public:
    SVGImport( KoFilter *parent, const char *name );
    virtual ~SVGImport();
    virtual bool filterImport( const QString &file, KoDocument *,
                               const QString &from, const QString &to,
                               const QString &config=QString::null );
};

class SVGElementInstance;
class SVGUseElement : public SVGComposite {
public:
    SVGUseElement( SVGComposite *par = 0 ) : SVGComposite( par ) {};
    virtual QString type() { return "use"; };
    virtual void setAttributes( const QXmlAttributes & );
    virtual GObject *build();
private:
    SVGLength x, y;
    SVGLength width, height;
    SVGElementInstance *instanceRoot;
};


class SVGElementInstance {
public:
    SVGElementInstance( SVGComposite *, QString );
    virtual GObject *build();
private:
    SVGElement *correspondingElement;
};
#endif
