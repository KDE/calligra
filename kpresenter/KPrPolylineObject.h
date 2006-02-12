// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2001 Toshitaka Fujioka <fujioka@kde.org>

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

#ifndef kppolylineobject_h
#define kppolylineobject_h

#include "KPrPointObject.h"
#include <KoStyleStack.h>

#define RAD_FACTOR 180.0 / M_PI

class QPainter;
class DCOPObject;

class KPrPolylineObject : public KPrPointObject
{
public:
    KPrPolylineObject();
    KPrPolylineObject( const KoPointArray &_points, const KoSize &_size,
                      const KoPen &_pen, LineEnd _lineBegin, LineEnd _lineEnd );
    virtual ~KPrPolylineObject() {}

    virtual DCOPObject* dcopObject();

    KPrPolylineObject &operator=( const KPrPolylineObject & );

    virtual ObjType getType() const { return OT_POLYLINE; }
    virtual QString getTypeString() const { return i18n("Polyline"); }

    virtual QDomDocumentFragment save( QDomDocument& doc, double offset );
    virtual double load( const QDomElement &element );
    virtual void loadOasis(const QDomElement &element, KoOasisContext & context, KPrLoadingInfo *info);

protected:
    virtual const char * getOasisElementName() const;
    virtual bool saveOasisObjectAttributes( KPOasisSaveContext &sc ) const;
};

#endif
