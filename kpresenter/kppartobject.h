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

#include <koMainWindow.h>
#include <koChild.h>

#include <kpobject.h>

#include <qpixmap.h>

class KPresenterChild;
class KPresenterFrame;
class KPGradient;

/******************************************************************/
/* Class: KPPartObject                                            */
/******************************************************************/

class KPPartObject : public QObject, public KP2DObject
{
    Q_OBJECT
public:
    KPPartObject( KPresenterChild *_child );
    virtual ~KPPartObject() {}

    KPPartObject &operator=( const KPPartObject & );

    virtual QDomDocumentFragment save( QDomDocument& doc, double offset );
    virtual double load(const QDomElement &element);

    virtual void rotate( float _angle );
    virtual void setSize( double _width, double _height );
    virtual void setOrig( const KoPoint &_point );
    virtual void setOrig( double _x, double _y );
    virtual void moveBy( const KoPoint &_point );
    virtual void moveBy( double _dx, double _dy );
    virtual void resizeBy( double _dx, double _dy );

    virtual ObjType getType() const
    { return OT_PART; }
    virtual QString getTypeString() const
    { return i18n("Embedded object"); }

    virtual void draw( QPainter *_painter, KoZoomHandler *_zoomhandler,
		       bool drawSelection, bool drawContour = FALSE );

    void activate( QWidget *_widget );
    void deactivate();

    KPresenterChild *getChild() const { return child; }

    void enableDrawing( bool f ) { _enableDrawing = f; }

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
