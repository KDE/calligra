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

#include <kpobject.h>
#include <kpclipartcollection.h>

#include <qdatetime.h>

class KPGradient;
class QPicture;

/******************************************************************/
/* Class: KPClipartObject                                         */
/******************************************************************/

class KPClipartObject : public KP2DObject
{
public:
    KPClipartObject( KPClipartCollection *_clipartCollection );
    KPClipartObject( KPClipartCollection *_clipartCollection, const QString &_filename, QDateTime _lastModified );
    virtual ~KPClipartObject() {}

    KPClipartObject &operator=( const KPClipartObject & );

    virtual void setFileName( const QString &_filename, QDateTime _lastModified );

    void reload()
    { setFileName( key.filename, key.lastModified ); }

    virtual ObjType getType() const
    { return OT_CLIPART; }
    QString getFileName() const
    { return key.filename; }

    virtual QDomDocumentFragment save( QDomDocument& doc );
    virtual void load(const QDomElement &element);

    virtual void draw( QPainter *_painter, int _diffx, int _diffy );

    KPClipartCollection::Key getKey() const
    { return key; }

protected:
    QPicture *picture;
    KPClipartCollection::Key key;

    KPClipartCollection *clipartCollection;

};

#endif
