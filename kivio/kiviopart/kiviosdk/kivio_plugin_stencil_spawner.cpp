/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <dlfcn.h>

#include "kivio_plugin_stencil_spawner.h"

#include "kivio_intra_stencil_data.h"
#include "kivio_point.h"
#include "kivio_common.h"
#include "kivio_stencil.h"

KivioPluginStencilSpawner::KivioPluginStencilSpawner( KivioStencilSpawnerSet *pSet )
    : KivioStencilSpawner( pSet ),
      m_handle(NULL)
{
    pNewStencil = NULL;
}

KivioPluginStencilSpawner::~KivioPluginStencilSpawner()
{
    if( m_handle )
    {
        dlclose( m_handle );
        m_handle = NULL;
        m_fileName = "";
    }


    pNewStencil = NULL;
}

bool KivioPluginStencilSpawner::load( const QString &f )
{
    char *error;

    if( strstr( f.ascii(), ".ksp" )==NULL )
    {
        qDebug("KivioPluginStencilSpawner::load() - %s is not a KSP file",
                f.ascii() );
        return false;
    }

    m_handle = dlopen( f.ascii(), RTLD_NOW | RTLD_GLOBAL );
    if( !m_handle )
    {
        qDebug("KivioPluginStencilSpawner::load() - %s is not a dll",
                f.ascii() );
        return false;
    }

    pNewStencil = pNewStencil = (NewStencilFunc)dlsym( m_handle, "NewStencil" );
    if( (error=dlerror())!=NULL )
    {
        qDebug("KivioPluginStencilSpawner::load() - %s - dlsym failed for NewStencil(): %s",
            f.ascii(), error );
        dlclose(m_handle);
        m_handle = NULL;
        return false;
    }

    pGetIcon = pGetIcon = (GetIconFunc)dlsym( m_handle, "GetIcon" );
    if( (error=dlerror())!=NULL )
    {
        qDebug("KivioPluginStencilSpawner::load() - %s - dlsym failed for GetIcon(): %s",
            f.ascii(), error );
        dlclose(m_handle);
        m_handle = NULL;
        return false;
    }

    pGetSpawnerInfo = (GetSpawnerInfoFunc)dlsym( m_handle, "GetSpawnerInfo" );
    if( (error=dlerror())!=NULL )
    {
        qDebug("KivioPluginStencilSpawner::load() - %s - dlsym failed for GetSpawnerInfo(): %s",
            f.ascii(), error );
        dlclose(m_handle);
        m_handle = NULL;
        return false;
    }

    // Get the icon
    m_pIcon = pGetIcon();

    // Get the info
    m_pSInfo = pGetSpawnerInfo();


    m_fileName = f;
    m_filename = f;

    return true;
}

QDomElement KivioPluginStencilSpawner::saveXML( QDomDocument &doc )
{
    qDebug("+SAVE KivioPluginStencilSpawner %s", m_filename.ascii());
    QDomElement spawnE = doc.createElement("KivioPluginStencilSpawner");

    if( spawnE.isNull() )
    {
        qDebug("SPAWN ELEMENT IS NULL");
    }

    XmlWriteString( spawnE, "filename", m_filename );

    return spawnE;
}

KivioStencil *KivioPluginStencilSpawner::newStencil()
{
    KivioStencil *pStencil = pNewStencil();

    pStencil->setSpawner(this);
    return pStencil;
}

KivioStencilSpawnerInfo *KivioPluginStencilSpawner::info()
{
    return m_pSInfo;
}