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

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <qxml.h>
#include <GObject.h>
#include "datatypes.h"

class SVGTransformList : public SVGList {
};


class SVGTransformable {
protected:
    SVGTransformList translateProps;
    QString transform;
public:
    void performTransformations( GObject *obj );
};


class SVGTransform : public SVGListItem {
public:
    virtual ~SVGTransform() {};
//    void performTransform( GObject &obj ) { obj.transform( transformation, true ); };
    void setSkewX( float angle );
    void setSkewY( float angle );
    void setRotate( float angle, float cx, float cy );
    void setScale( float sx, float sy );
    void setTranslate( float tx, float ty );
    void setMatrix( const QWMatrix & m );
    QWMatrix matrix() { return _matrix; }
protected:
    QWMatrix _matrix;
};

#endif
