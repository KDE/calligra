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
#ifndef KIVIO_PLUGIN_STENCIL_H
#define KIVIO_PLUGIN_STENCIL_H

class KivioIntraStencilData;
class KivioStencil;
class KivioStencilFactory;
class QDomDocument;

#include "kivio_stencil_spawner.h"
#include "kivio_stencil_spawner_info.h"
#include <qpixmap.h>

//typedef KivioStencil *(*NewStencilFunc)();
//typedef QPixmap *(*GetIconFunc)();
//typedef KivioStencilSpawnerInfo *(*GetSpawnerInfoFunc)();


class KivioPluginStencilSpawner : public KivioStencilSpawner
{
protected:
    KivioStencilFactory *fac;
    void *m_handle;
    QPixmap *m_pIcon;
    QString m_filename;
    KivioStencilSpawnerInfo *m_pSInfo;

    KivioStencil *(*pNewStencil)();
    QPixmap *(*pGetIcon)();
    KivioStencilSpawnerInfo *(*pGetSpawnerInfo)();

public:
    KivioPluginStencilSpawner( KivioStencilSpawnerSet * );
    virtual ~KivioPluginStencilSpawner();

    virtual bool load( const QString & );
    virtual QDomElement saveXML( QDomDocument & );

    virtual KivioStencil *newStencil();
    virtual KivioStencil *newStencil(const QString &arg);
    virtual KivioStencilSpawnerInfo *info();

    virtual QPixmap *icon() { return m_pIcon; }
};

#endif
