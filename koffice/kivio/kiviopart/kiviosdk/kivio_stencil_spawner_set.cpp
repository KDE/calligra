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
#include "kivio_common.h"
#include "kivio_plugin_stencil_spawner.h"
#include "kivio_sml_stencil_spawner.h"
#include "kivio_dia_stencil_spawner.h"
#include "kivio_py_stencil_spawner.h"
#include "kivio_stencil_spawner.h"
#include "kivio_stencil_spawner_set.h"

#include <qdir.h>
#include <qfile.h>
#include <qstring.h>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>

KivioStencilSpawnerSet::KivioStencilSpawnerSet(const QString& name)
    : m_pSpawners(NULL),
      m_pSelected(NULL)
{
  m_hidden = false;
  m_dir = "";
  m_name = name.isEmpty() ? QString("Untitled") : name;

  m_pSpawners = new QPtrList<KivioStencilSpawner>;
  m_pSpawners->setAutoDelete(true);
}

KivioStencilSpawnerSet::~KivioStencilSpawnerSet()
{
  delete m_pSpawners;
  m_pSpawners = NULL;
  kdDebug(43000) << "KivioStencilSpawnerSet::~KivioStencilSpawnerSet() - " <<  m_name << endl;
}


/**
 * Loads a stencil spawners from data defined in an XML file
 *
 * @param e The @ref QDomElement to load from
 *
 * This is a function that needs to be implemented by all descendant
 * classes.  It should search the known stencil-paths for the spawner
 * specified, and load it.
 */
bool KivioStencilSpawnerSet::loadXML( const QDomElement & )
{
  return false;
}


/**
 * Saves a spawner to a @ref QDomElement
 *
 * @param doc The document to save to
 *
 * All descendant classes should implement this function.  It should
 * save the necessary data to a @ref QDomElement such that when
 * it is loaded again, it can find the spawner and load it from
 * the local filesystem.
 */
QDomElement KivioStencilSpawnerSet::saveXML( QDomDocument &doc )
{
  QDomElement spawnE = doc.createElement("KivioStencilSpawnerSet");

  XmlWriteString( spawnE, "id", m_id );
  XmlWriteString(spawnE, "hidden", (m_hidden ? "true" : "false"));

  KivioStencilSpawner *pSpawner = m_pSpawners->first();
  while( pSpawner )
  {
    spawnE.appendChild( pSpawner->saveXML( doc ) );

    pSpawner = m_pSpawners->next();
  }


  return spawnE;
}

bool KivioStencilSpawnerSet::loadDir( const QString &dirName )
{
  QDir d(dirName);
  QString fileName;

  m_dir = dirName;
  m_name = readTitle( dirName );
  m_id = readId( dirName );

  d.setNameFilter("*.so *.sml *.ksp *.spy *.shape");
  m_files = d.entryList();

  return true;
}

KivioStencilSpawner* KivioStencilSpawnerSet::loadFile( const QString &fileName )
{
    for (KivioStencilSpawner* ss = m_pSpawners->first(); ss; ss = m_pSpawners->next() )
        if (ss->fileName() == fileName)
            return ss;

    KivioStencilSpawner *pSpawner;

    if( fileName.contains( ".sml", false ) )
    {
        pSpawner = new KivioSMLStencilSpawner(this);
    }
    else if( fileName.contains( ".ksp", false ) )
    {
        pSpawner = new KivioPluginStencilSpawner(this);
    }
    else if( fileName.contains( ".so", false ) )
    {
        pSpawner = new KivioPluginStencilSpawner(this);
    }
    else if( fileName.contains( ".spy", false ) )
    {
        pSpawner = new KivioPyStencilSpawner(this);
    }
    else if( fileName.contains( ".shape", false ) )
    {
	pSpawner = new KivioDiaStencilSpawner(this);
    }
    else
    {
        return 0;
    }

    if( pSpawner->load( fileName )==true )
        m_pSpawners->append( pSpawner );
    else
    {
        delete pSpawner;
        return 0;
    }

    return pSpawner;
}

QString KivioStencilSpawnerSet::readTitle( const QString &dir )
{
  QDomDocument d("StencilSPawnerSet");
  QDomElement root, nodeElement;
  QDomNode node;
  QString nodeName;
  QString title, origTitle;
  QFile f(dir+"/desc");

  if( f.open( IO_ReadOnly )==false )
  {
    kdDebug(43000) << "KivioStencilSpawnerSet::readTitle() - Error opening stencil set title: " <<
        dir << "/desc" << endl;
    return dir.right(dir.length() - dir.findRev('/')-1);
  }

  d.setContent(&f);

  root = d.documentElement();
  node = root.firstChild();

  while( !node.isNull() )
  {
    nodeName = node.nodeName();
    nodeElement = node.toElement();

    if( nodeName.compare("Title")==0 && nodeElement.hasAttribute("lang"))
    {
      if(nodeElement.attribute("lang") == KGlobal::locale()->language()) {
        title = XmlReadString( nodeElement, "data", dir );
      }
    }
    else if( nodeName.compare("Title")==0 && !nodeElement.hasAttribute("lang"))
    {
      origTitle = XmlReadString( nodeElement, "data", dir );
    }

    node = node.nextSibling();
  }

  if(title.isEmpty()) {
    title = i18n( "Stencils", origTitle.utf8() );
  }

  return title;
}

QString KivioStencilSpawnerSet::readId( const QString &dir )
{
  QDomDocument d("StencilSPawnerSet");
  QDomElement root;
  QDomNode node;
  QString nodeName;
  QString theid;
  QFile f(dir+"/desc");

  if( f.open( IO_ReadOnly )==false )
  {
    kdDebug(43000) << "KivioStencilSpawnerSet::readId() - Error opening stencil set description: " <<
        dir << "/desc" << endl;
    return "";
  }

  d.setContent(&f);

  root = d.documentElement();
  node = root.firstChild();

  while( !node.isNull() )
  {
    nodeName = node.nodeName();

    if( nodeName.compare("Id")==0 )
    {
      theid = XmlReadString( node.toElement(), "data", dir );
      return theid;
    }

    node = node.nextSibling();
  }

  kdDebug(43000) << "KivioStencilSpawnerSet::readId() - No id found in "
  << dir << "/desc" << endl;

  return "";
}

QString KivioStencilSpawnerSet::readDescription(const QString& dir)
{
  QDomDocument d("StencilSPawnerSet");
  QDomElement root, nodeElement;
  QDomNode node;
  QString nodeName;
  QFile f(dir + "/desc");

  if( f.open( IO_ReadOnly )==false )
  {
    kdDebug(43000) << "KivioStencilSpawnerSet::readId() - Error opening stencil set description: " <<
      dir << "/desc" << endl;
    return "";
  }

  d.setContent(&f);

  root = d.documentElement();
  node = root.firstChild();
  QString description, origDesc;

  while( !node.isNull() )
  {
    nodeName = node.nodeName();
    nodeElement = node.toElement();

    if( nodeName.compare("Description")==0 && nodeElement.hasAttribute("lang"))
    {
      if(nodeElement.attribute("lang") == KGlobal::locale()->language()) {
        description = nodeElement.text();
      }
    }
    else if( nodeName.compare("Description")==0 && !nodeElement.hasAttribute("lang"))
    {
      origDesc = nodeElement.text();
    }

    node = node.nextSibling();
  }

  if(description.isEmpty() && !origDesc.isEmpty()) {
    description = i18n( "Stencils", origDesc.utf8() );
  }

  return description;
}


KivioStencilSpawner* KivioStencilSpawnerSet::find( const QString& id)
{
  if(!m_pSpawners || m_pSpawners->isEmpty()) {
    return 0;
  }

  KivioStencilSpawner* pSpawner = m_pSpawners->first();

  while( pSpawner )
  {
      // If the id matches, this is it!
      if( pSpawner->info()->id() == id )
      {
          return pSpawner;
      }

      pSpawner = m_pSpawners->next();
  }

  return NULL;
}

void KivioStencilSpawnerSet::addSpawner(KivioStencilSpawner* spawner)
{
  if(spawner) {
    m_pSpawners->append(spawner);
  }
}
