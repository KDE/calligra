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

#ifndef kpgroupobject_h
#define kpgroupobject_h

class KPresenterDoc;

#include <kpobject.h>

/******************************************************************/
/* Class: KPGroupObject                                           */
/******************************************************************/

class KPGroupObject : public KPObject
{
public:
    KPGroupObject();
    KPGroupObject( const QList<KPObject> &objs );
    virtual ~KPGroupObject() {}

    KPGroupObject &operator=( const KPGroupObject & );

    virtual void setSize( int _width, int _height );
    virtual void setOrig( QPoint _point );
    virtual void setOrig( int _x, int _y );
    virtual void moveBy( QPoint _point );
    virtual void moveBy( int _dx, int _dy );
    virtual void resizeBy( QSize _size );
    virtual void resizeBy( int _dx, int _dy );

    virtual void rotate( float _angle );
    virtual void setShadowDistance( int _distance );
    virtual void setShadowDirection( ShadowDirection _direction );
    virtual void setShadowColor( QColor _color );
    virtual void setEffect( Effect _effect );
    virtual void setEffect2( Effect2 _effect2 );
    virtual void setPresNum( int _presNum );
    virtual void setDisappear( bool b );
    virtual void setDisappearNum( int num );
    virtual void setEffect3( Effect3 _effect3);

    virtual ObjType getType() const
    { return OT_GROUP; }

    virtual QDomDocumentFragment save( QDomDocument& doc );
    virtual void load( KOMLParser& parser, QValueList<KOMLAttrib>& lst, KPresenterDoc *doc );

    virtual void draw( QPainter *_painter, int _diffx, int _diffy );

    void setUpdateObjects( bool b ) {
        updateObjs = b;
    }

    QList<KPObject> getObjects() {
        return objects;
    }

    virtual void zoom( float _fakt );
    virtual void zoomOrig();
    virtual void setOwnClipping( bool _ownClipping );
    virtual void setSubPresStep( int _subPresStep );
    virtual void doSpecificEffects( bool _specEffects, bool _onlyCurrStep = true );

protected:
    void updateSizes( float fx, float fy );
    void updateCoords( int dx, int dy );

    QList<KPObject> objects;
    bool updateObjs;

};

#endif
