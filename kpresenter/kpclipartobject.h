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

#ifndef kpclipartobject_h
#define kpclipartobject_h

#include <qdatetime.h>

#include <koPictureCollection.h>

#include <kpobject.h>

class QPicture;

/******************************************************************/
/* Class: KPClipartObject                                         */
/******************************************************************/

class KPClipartObject : public KP2DObject
{
public:
    KPClipartObject( KoPictureCollection *_clipartCollection );
    KPClipartObject( KoPictureCollection *_clipartCollection, const KoPictureKey & key );
    virtual ~KPClipartObject() {}

    KPClipartObject &operator=( const KPClipartObject & );

    void setClipart( const KoPictureKey & key );

    void reload() { setClipart( m_clipart.getKey() ); }

    virtual ObjType getType() const
    { return OT_CLIPART; }
    virtual QString getTypeString() const
    { return i18n("Clipart"); }

    QString getFileName() const
    { return m_clipart.getKey().filename(); }

    virtual QDomDocumentFragment save( QDomDocument& doc, double offset );
    virtual double load(const QDomElement &element);

    virtual void draw( QPainter *_painter, KoZoomHandler*_zoomHandler,
		       SelectionMode selectionMode, bool drawContour = FALSE );

    KoPictureKey getKey() const
    { return m_clipart.getKey(); }
    KoPicture clipart() const { return m_clipart ; }
protected:
    KoPicture m_clipart;

    KoPictureCollection *clipartCollection;

};

#endif
