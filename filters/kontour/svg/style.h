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

#ifndef STYLE_H
#define STYLE_H

#include <qfont.h>
#include "GObject.h"
#include <qdict.h>

class ColorTable : public QDict<unsigned int> {
public:
    ColorTable();
    ~ColorTable();
};


class StyleProperty {
public:
    StyleProperty();
    void styleCopy( const StyleProperty & );
    ~StyleProperty();

    GObject::OutlineInfo outlineInfo;
    GObject::FillInfo    fillInfo;
    QFont font;
//protected:
    void setStyleProperties( GObject *obj );
    virtual void processSubStyle( QString &, QString & ) {};
    void processStyle();

    QString style;
};


#endif
