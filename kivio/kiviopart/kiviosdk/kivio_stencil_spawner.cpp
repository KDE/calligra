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
#include "kivio_stencil_spawner.h"
#include "kivio_stencil_spawner_set.h"
#include "kivio_stencil_spawner_info.h"
#include "kivio_stencil.h"


KivioStencilSpawner::KivioStencilSpawner( KivioStencilSpawnerSet *p )
    : m_pInfo(NULL)
{
    m_pInfo = new KivioStencilSpawnerInfo();
    m_pSet = p;
    m_fileName = "";
    m_defWidth = 72.0f;
    m_defHeight = 72.0f;
}

KivioStencilSpawner::~KivioStencilSpawner()
{
    if( m_pInfo )
    {
        delete m_pInfo;
        m_pInfo = NULL;
    }

    m_pSet = NULL;
    m_fileName = "";
}


bool KivioStencilSpawner::load( const QString & )
{
    return false;
}

bool KivioStencilSpawner::loadXML( const QString &, QDomDocument & )
{
    return false;
}


KivioStencil *KivioStencilSpawner::newStencil()
{
    return NULL;
}

KivioStencil *KivioStencilSpawner::newStencil(const QString& /*name*/)
{
    return NULL;
}

QDomElement KivioStencilSpawner::saveXML( QDomDocument &doc )
{
    return doc.createElement("KivioStencilSpawner");
}
