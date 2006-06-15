/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef kppartobject_h
#define kppartobject_h

#include <KoChild.h>

#include "KPrObject.h"

class KPrChild;
class KoXmlWriter;
class KoStore;

class KPrPartObject : public KPr2DObject
{
    Q_OBJECT
public:
    KPrPartObject( KPrChild *_child );
    virtual ~KPrPartObject() {}

    KPrPartObject &operator=( const KPrPartObject & );
    virtual void rotate( float _angle );

    virtual ObjType getType() const { return OT_PART; }
    virtual QString getTypeString() const { return i18n("Embedded Object"); }

    virtual void draw( QPainter *_painter, KoTextZoomHandler *_zoomhandler,
                       int pageNum, SelectionMode selectionMode, bool drawContour = false );

    void activate( QWidget *_widget );
    void deactivate();

    KPrChild *getChild() const { return child; }
    void enableDrawing( bool f ) { _enableDrawing = f; }
    virtual void loadOasis(const QDomElement &element, KoOasisContext &context, KPrLoadingInfo *info);

public slots:
    void slot_changed( KoChild *_koChild );

protected:
    virtual const char * getOasisElementName() const;
    virtual bool saveOasisObjectAttributes( KPOasisSaveContext &sc ) const;

    void updateChildGeometry();
    virtual void paint( QPainter *_painter,KoTextZoomHandler*_zoomHandler,
                        int /* pageNum */, bool drawingShadow, bool drawContour = false );

    bool _enableDrawing;
    KPrChild *child;
};

#endif
