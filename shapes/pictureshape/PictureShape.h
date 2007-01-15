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

#ifndef PICTURE_SHAPE
#define PICTURE_SHAPE


#include <KoShape.h>
#include <kurl.h>
#define PictureShapeId "PictureShape"


class PictureShape : public KoShape
{
public:
    PictureShape(const KUrl&url = KUrl());
    virtual ~PictureShape();

    virtual void paint( QPainter& painter, const KoViewConverter& converter );

    /*
     * Return current url
     */
    KUrl currentUrl() const;
    /*
     * Change video url
     */
    void setCurrentUrl(const KUrl&url);

    virtual void resize( const QSizeF &newSize );
private:
    KUrl m_currentPictureUrl;
};


#endif // PICTURE_FLAKE
