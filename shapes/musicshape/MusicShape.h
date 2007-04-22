/* This file is part of the KDE project
   Copyright 2007 Montel Laurent <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef VIDEO_SHAPE
#define VIDEO_SHAPE


#include <KoShape.h>
#include <kurl.h>
#define MusicShapeId "MusicShape"

class QFrame;

class MusicShape : public KoShape
{
public:
    explicit MusicShape(const KUrl&url = KUrl());
    virtual ~MusicShape();

    virtual void paint( QPainter& painter, const KoViewConverter& converter );
    
    virtual void resize( const QSizeF &newSize );
private:
};


#endif // VIDEO_FLAKE
