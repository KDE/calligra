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

#include <config.h>
#include "kivio_plugin_stencil_spawner.h"
#include <kparts/part.h>
#include <kparts/componentfactory.h>
#include "kiviostencilfactory.h"
#include "kivio_intra_stencil_data.h"
#include "kivio_common.h"
#include "kivio_stencil.h"

#include <qdom.h>
#include <qfileinfo.h>
#include <kdebug.h>

KivioPluginStencilSpawner::KivioPluginStencilSpawner( KivioStencilSpawnerSet *pSet )
    : KivioStencilSpawner( pSet ),
      m_handle(NULL)
{
    pNewStencil = NULL;
}

KivioPluginStencilSpawner::~KivioPluginStencilSpawner()
{
}

bool KivioPluginStencilSpawner::load( const QString &f )
{
  QFileInfo lib(f);
  
  if( lib.exists())
  {
    m_fileName = lib.baseName();
  } else {
    return false;
  }

  fac = KParts::ComponentFactory::createInstanceFromLibrary<KivioStencilFactory>(m_fileName.local8Bit());

  if( !fac)
  {
    kdDebug(43000) << "Failed to load: " << m_fileName << endl;
    return false;
  }
  
  // Get the icon
  m_pIcon = fac->GetIcon();

  // Get the info
  m_pSInfo = fac->GetSpawnerInfo();

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
    KivioStencil *pStencil = fac->NewStencil();

    pStencil->setSpawner(this);
    return pStencil;
}


KivioStencil *KivioPluginStencilSpawner::newStencil(const QString& arg)
{
    KivioStencil *pStencil = fac->NewStencil(arg);

    pStencil->setSpawner(this);
    return pStencil;
}
KivioStencilSpawnerInfo *KivioPluginStencilSpawner::info()
{
    return m_pSInfo;
}
