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

#ifndef TEXT_H
#define TEXT_H

#include <qxml.h>
#include "svgmain.h"
#include "style.h"

class GText;
class SVGTextElement : public SVGComposite {
public:
    SVGTextElement( SVGComposite *par = 0 ) : SVGComposite( par ), xpos(0), ypos(0) {}
    virtual void setAttributes( const QXmlAttributes& );
    virtual GObject *build();
    virtual void processSubStyle( QString &command, QString &param );
    void setStyleProperties( GObject *obj );
    virtual void addContent( const QString & );
private:
    int xpos, ypos;
    float x, y;
    GText *obj;
};

#endif
