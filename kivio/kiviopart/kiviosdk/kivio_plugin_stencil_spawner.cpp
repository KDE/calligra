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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <dlfcn.h>

#include <config.h>
#include "kivio_plugin_stencil_spawner.h"

#include "kivio_intra_stencil_data.h"
#include "kivio_point.h"
#include "kivio_common.h"
#include "kivio_stencil.h"

#include <qdom.h>

#include <kdebug.h>

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
    const char *error;

    if ( !( f.right(4) == ".ksp" ) )
    {
       kdDebug() << "KivioPluginStencilSpawner::load() - " << f << " is not a KSP file" << endl;
        return false;
    }

    m_handle = dlopen( f.ascii(), RTLD_NOW | RTLD_GLOBAL );
    if( !m_handle )
    {
       kdDebug() << "KivioPluginStencilSpawner::load() - " << f << " is not a dll" << endl;
        return false;
    }

    pNewStencil = pNewStencil = (NewStencilFunc)dlsym( m_handle, "NewStencil" );
    if( (error=dlerror())!=NULL )
    {
       kdDebug() << "KivioPluginStencilSpawner::load() - " << f << " - dlsym failed for NewStencil(): " << error << endl;
        dlclose(m_handle);
        m_handle = NULL;
        return false;
    }

    pGetIcon = pGetIcon = (GetIconFunc)dlsym( m_handle, "GetIcon" );
    if( (error=dlerror())!=NULL )
    {
       kdDebug() << "KivioPluginStencilSpawner::load() - " << f << " - dlsym failed for GetIcon(): " << error << endl;
        dlclose(m_handle);
        m_handle = NULL;
        return false;
    }

    pGetSpawnerInfo = (GetSpawnerInfoFunc)dlsym( m_handle, "GetSpawnerInfo" );
    if( (error=dlerror())!=NULL )
    {
       kdDebug() << "KivioPluginStencilSpawner::load() - " << f << " - dlsym failed for GetIcon(): " << error << endl;
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
    QDomElement spawnE = doc.createElement("KivioPluginStencilSpawner");

    XmlWriteString( spawnE, "id", m_pInfo->id() );

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
