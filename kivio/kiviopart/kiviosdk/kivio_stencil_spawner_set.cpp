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
#include "kivio_common.h"
#include "kivio_plugin_stencil_spawner.h"
#include "kivio_sml_stencil_spawner.h"
#include "kivio_stencil_spawner.h"
#include "kivio_stencil_spawner_set.h"

#include <qdir.h>

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
    qDebug("* StencilSpawnerSet %s deleted\n", m_name.ascii() );
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
    qDebug("+SAVE KivioStencilSpawnerSet");
    QDomElement spawnE = doc.createElement("KivioStencilSpawnerSet");

    XmlWriteString( spawnE, "desc", m_name );

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
    m_name = readDesc( dirName );

    d.setNameFilter("*.sml *.ksp");

    for( int i=0; i<(int)d.count(); i++ )
    {
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
        qDebug("-LOAD KivioStencilSpawnerSet::loadDir() - Found a PLUGIN! %s", fileName.ascii() );
        pSpawner = new KivioPluginStencilSpawner(this);
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

QString KivioStencilSpawnerSet::readDesc( const QString &dir )
{
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
}

KivioStencilSpawner* KivioStencilSpawnerSet::find( const QString& title)
{
    KivioStencilSpawner *pSpawner = m_pSpawners->first();
    while( pSpawner )
    {
        // If the title matches, this is it!
        qDebug(QString("FIND %1 - %2").arg(title).arg(pSpawner->info()->title()).latin1()); // arghl! (Simon)
        if( pSpawner->info()->title() == title )
        {
            return pSpawner;
        }
    
        pSpawner = m_pSpawners->next();
    }
    
    return NULL;
}

