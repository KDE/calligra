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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef kppartobject_h
#define kppartobject_h

#include <koChild.h>

#include "kpobject.h"

class KPresenterChild;
class KoXmlWriter;
class KoStore;

class KPPartObject : public QObject, public KP2DObject
{
    Q_OBJECT
public:
    KPPartObject( KPresenterChild *_child );
    virtual ~KPPartObject() {}

    KPPartObject &operator=( const KPPartObject & );
    virtual void rotate( float _angle );

    virtual ObjType getType() const { return OT_PART; }
    virtual QString getTypeString() const { return i18n("Embedded Object"); }

    virtual void draw( QPainter *_painter, KoZoomHandler *_zoomhandler,
                       SelectionMode selectionMode, bool drawContour = FALSE );

    void activate( QWidget *_widget );
    void deactivate();

    KPresenterChild *getChild() const { return child; }
    void enableDrawing( bool f ) { _enableDrawing = f; }
    virtual bool saveOasisPart( KoXmlWriter &xmlWriter, KoStore *store, KoSavingContext& context, int indexObj, int partIndexObj ) const;
    virtual void loadOasis(const QDomElement &element, KoOasisContext &context, KPRLoadingInfo *info);
    virtual bool saveOasis( KoXmlWriter &xmlWriter, KoSavingContext &context, int indexObj ) const { return true;/* use saveOasisPart*/};

public slots:
    void slot_changed( KoChild *_koChild );

protected:
    void updateChildGeometry();
    virtual void paint( QPainter *_painter,KoZoomHandler*_zoomHandler,
                        bool drawingShadow, bool drawContour = FALSE );

    bool _enableDrawing;
    KPresenterChild *child;
};

#endif
