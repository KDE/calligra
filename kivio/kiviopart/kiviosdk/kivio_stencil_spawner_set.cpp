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

KivioStencilSpawnerSet::KivioStencilSpawnerSet(const QString& name)
    : m_pSpawners(NULL),
      m_pSelected(NULL)
{
    m_dir = "";
    m_name = name.isEmpty() ? QString("Untitled") : name;

    m_pSpawners = new QList<KivioStencilSpawner>;
    m_pSpawners->setAutoDelete(true);
}

KivioStencilSpawnerSet::~KivioStencilSpawnerSet()
{
    if( m_pSpawners )
    {
        delete m_pSpawners;
        m_pSpawners = NULL;
    }
    kdDebug() << "KivioStencilSpawnerSet::~KivioStencilSpawnerSet() - StencilSpawnerSet " <<  m_name << " deleted" << endl;
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

    d.setNameFilter("*.sml *.ksp *.spy *.shape");

    for( int i=0; i<(int)d.count(); i++ )
    {
       kdDebug() << "SpawnerSet: " << fileName << endl;
        fileName = dirName + "/" + d[i];
        loadFile(fileName);
    }

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
   QDomElement root;
   QDomNode node;
   QString nodeName;
   QString title;
   QFile f(dir+"/desc");

   if( f.open( IO_ReadOnly )==false )
   {
      kdDebug() << "KivioStencilSpawnerSet::readTitle() - Error opening stencil set description: " <<
	 dir << "/desc" << endl;
      return "";
   }

   d.setContent(&f);

   root = d.documentElement();
   node = root.firstChild();

   while( !node.isNull() )
   {
      nodeName = node.nodeName();

      if( nodeName.compare("Title")==0 )
      {
	 title = XmlReadString( node.toElement(), "data", dir );
	 return title;
      }
   }

   kdDebug() << "KivioStencilSpawnerSet::readTitle() - No title found in "
	     << dir << "/desc" << endl;

   return "";

/*
    int pos;

    QFile file( dir + "/desc" );

    if( file.exists()==false )
        return "Unknown";

    if( file.open( IO_ReadOnly )==false )
        return "Unknown";

    QString ret;

    file.readLine( ret, 128 );
    file.close();

    pos = ret.find( '\n' );
    if( pos!=-1 )
        ret.truncate( pos );

    return ret;
*/
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
      kdDebug() << "KivioStencilSpawnerSet::readId() - Error opening stencil set description: " <<
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

   kdDebug() << "KivioStencilSpawnerSet::readId() - No id found in "
	     << dir << "/desc" << endl;

   return "";
}

KivioStencilSpawner* KivioStencilSpawnerSet::find( const QString& id)
{
    KivioStencilSpawner *pSpawner = m_pSpawners->first();
    while( pSpawner )
    {
        // If the title matches, this is it!
        if( pSpawner->info()->id() == id )
        {
            return pSpawner;
        }

        pSpawner = m_pSpawners->next();
    }

    return NULL;
}

