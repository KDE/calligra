/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef KIVIO_STENCIL_SPAWNER_H
#define KIVIO_STENCIL_SPAWNER_H

#include <qstring.h>
#include <qpixmap.h>
#include <qdom.h>
#include <qobject.h>

class QPainter;
class QRect;
class KivioStencilSpawnerInfo;
class KivioStencilSpawnerSet;
class KivioStencil;

class KivioStencilSpawner
{
protected:
    QString m_fileName;
    QPixmap m_icon;
    KivioStencilSpawnerSet *m_pSet;
    KivioStencilSpawnerInfo *m_pInfo;

    // Default width and height
    float m_defWidth, m_defHeight;

public:
    KivioStencilSpawner( KivioStencilSpawnerSet * );
    virtual ~KivioStencilSpawner();

    virtual bool load( const QString & );
    virtual bool loadXML( const QString &, QDomDocument & );
    virtual QDomElement saveXML( QDomDocument & );

    virtual KivioStencil *newStencil();
    virtual KivioStencil *newStencil(const QString& args);

    virtual QString fileName() { return m_fileName; }
    virtual KivioStencilSpawnerSet *set() { return m_pSet; }
    virtual KivioStencilSpawnerInfo *info() { return m_pInfo; }

    virtual float defWidth() { return m_defWidth; }
    virtual float defHeight() { return m_defHeight; }

    virtual QPixmap *icon() { return &m_icon; }


//    virtual void drawIcon( QPainter *, QRect *, bool );
};

#endif


