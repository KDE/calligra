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
#include "kivio_connector_point.h"
#include "kivio_connector_target.h"
#include "kivio_group_stencil.h"
#include "kivio_intra_stencil_data.h"
#include "kivio_layer.h"
#include "kivio_painter.h"
#include "kivio_point.h"
#include "kivio_stencil.h"
#include "kivio_stencil_spawner.h"
#include "kivio_stencil_spawner_info.h"
#include "kivio_stencil_spawner_set.h"

#include "kivio_doc.h"
#include "kivio_page.h"

#include <klocale.h>
#include <kdebug.h>

KivioLayer::KivioLayer( KivioPage *pPage )
    : m_pStencilList(NULL)
{
    m_pPage = pPage;
    m_name = i18n("Untitled Layer");

    m_pStencilList = new QList<KivioStencil>;
    m_pStencilList->setAutoDelete(true);

    m_flags = 0;

    setVisible(true);
    setConnectable(false);
}

KivioLayer::~KivioLayer()
{
   kdDebug() << "KivioLayer() - In the end there can be only one" << endl;

    if( m_pStencilList )
    {
        delete m_pStencilList;
        m_pStencilList = NULL;
    }
}

bool KivioLayer::addStencil( KivioStencil *pStencil )
{
    m_pStencilList->append( pStencil );

    return true;
}

bool KivioLayer::removeStencil( KivioStencil *pStencil )
{
    return m_pStencilList->remove( pStencil );
}


/**
 * Returns a new stencil of the type described by stencilE
 *
 * @param stencilE The XML element to load from
 *
 * This will search through all KivioStencilSpawnerSets and attempt to
 * locate the stencil described by stencilE.  If it finds it, it allocates
 * a new copy of it, loads the data from stencilE into it, and returns
 * it.
 *
 * @returns A new, loaded stencil.
 */
KivioStencil *KivioLayer::loadSMLStencil( const QDomElement &stencilE )
{
    QString setName, title;

    setName = XmlReadString( stencilE, "setName", "" );
    title = XmlReadString( stencilE, "title", "" );

    // Can't locate a stencil if it doesn't have a title or set name
    if( setName.length() == 0 ||
        title.length() == 0 )
        return NULL;

    kdDebug() << "KivioLayer::loadSMLStencil() " << setName << " " << title << endl;

    // Locate the spawner set
    KivioStencilSpawner *pSpawner = m_pPage->doc()->findStencilSpawner(setName,title);
    if( pSpawner )
    {
        KivioStencil *pStencil = pSpawner->newStencil();
        pStencil->loadXML( stencilE );

        return pStencil;
    }

    return NULL;
}

/**
 * Returns a new stencil of the type described by stencilE
 *
 * @param stencilE The XML element to load from
 *
 * This will search through all KivioStencilSpawnerSets and attempt to
 * locate the stencil described by stencilE.  If it finds it, it allocates
 * a new copy of it, loads the data from stencilE into it, and returns
 * it.
 *
 * @returns A new, loaded stencil.
 */
KivioStencil *KivioLayer::loadGroupStencil( const QDomElement &stencilE )
{
   kdDebug() << "KivioLayer::loadGroupStencil()" << endl;

    KivioGroupStencil *pStencil = new KivioGroupStencil();

    if(pStencil->loadXML( stencilE, this )==false)
    {
        delete pStencil;
        return NULL;
    }

    return pStencil;
}

KivioStencil *KivioLayer::loadPluginStencil( const QDomElement &stencilE )
{
    QString setName, title;

    setName = XmlReadString( stencilE, "setName", "" );
    title = XmlReadString( stencilE, "title", "" );

    // Can't locate a stencil if it doesn't have a title or set name
    if( setName.length() == 0 ||
        title.length() == 0 )
        return NULL;

    kdDebug() << "KivioLayer::loadPluginStencil() " << setName.ascii() << " " <<  title << endl;
    
    // Locate the spawner set
    KivioStencilSpawner *pSpawner = m_pPage->doc()->findStencilSpawner(setName,title);
    if( pSpawner )
    {
        KivioStencil *pStencil = pSpawner->newStencil();
        pStencil->loadXML( stencilE );

        return pStencil;
    }
    
    return NULL;
}

bool KivioLayer::loadXML( const QDomElement &layerE )
{
    m_flags = XmlReadInt( layerE, "flags", 1 );
    m_name = XmlReadString( layerE, "name", "layerX" );

    QDomNode node;
    node = layerE.firstChild();
    while( !node.isNull() )
    {
        QString name = node.nodeName();
        if( name == "KivioSMLStencil" )
        {
            KivioStencil *pStencil = loadSMLStencil( node.toElement() );
            if( pStencil )
            {
                pStencil->updateGeometry();
                m_pStencilList->append( pStencil );
            }
            else
            {
		  kdWarning() << "KivioLayer::loadXML() - Unknown KivioSMLStencil (title=" <<
		  XmlReadString( node.toElement(), "title", "" ) << " set=" <<  
		  XmlReadString( node.toElement(), "setName", "" ) << ") found." << endl;
            }
        }
        else if( name == "KivioGroupStencil" )
        {
            KivioStencil *pStencil = loadGroupStencil( node.toElement() );
            if( pStencil )
            {
                m_pStencilList->append(pStencil);
            }
            else
            {
	       kdDebug() << "KivioLayer::loadXML() - Unable to load KivioGroupStencil" << endl;
            }
        }
        else if( name == "KivioPluginStencil" )
        {
            KivioStencil *pStencil = loadPluginStencil( node.toElement() );
            if( pStencil )
            {
                m_pStencilList->append(pStencil);
            }
            else
            {
	       kdDebug() << "KivioLayer - Unable to load KivioPluginStencil" << endl;
            }
        }

        node = node.nextSibling();
    }

    return true;
}

QDomElement KivioLayer::saveXML( QDomDocument &doc )
{
    QDomElement e = doc.createElement("KivioLayer");

    XmlWriteInt( e, "flags", m_flags );
    XmlWriteString( e, "name", m_name );

    KivioStencil *pStencil = m_pStencilList->first();
    while( pStencil )
    {
        e.appendChild( pStencil->saveXML( doc ) );

        pStencil = m_pStencilList->next();
    }

    return e;
}

void KivioLayer::paintContent( KivioPainter& painter, const QRect&, bool , QPoint , float zoom )
{
    KivioStencil *pStencil = m_pStencilList->first();
    KivioIntraStencilData data;

    painter.setFGColor( QColor(0,0,0) );

    data.painter = &painter;
    data.scale = zoom;

    while( pStencil )
    {
        pStencil->paint( &data );

        pStencil = m_pStencilList->next();
    }
}

void KivioLayer::printContent( KivioPainter& painter )
{
    KivioStencil *pStencil = m_pStencilList->first();
    KivioIntraStencilData data;

    painter.setFGColor( QColor(0,0,0) );

    data.painter = &painter;
    data.scale = 1.0f;
    data.printing = true;

    while( pStencil )
    {
        pStencil->paint( &data );

        pStencil = m_pStencilList->next();
    }
}

void KivioLayer::paintConnectorTargets( KivioPainter& painter, const QRect&, bool, QPoint, float zoom )
{
    KivioIntraStencilData data;

    painter.setFGColor( QColor(0,0,0) );

    data.painter = &painter;
    data.scale = zoom;

    KivioStencil *pStencil = m_pStencilList->first();
    while( pStencil )
    {
        pStencil->paintConnectorTargets( &data );

        pStencil = m_pStencilList->next();
    }
}

void KivioLayer::paintSelectionHandles( KivioPainter& painter, const QRect&, bool, QPoint, float zoom )
{
    KivioIntraStencilData data;

    painter.setFGColor( QColor(0,0,0) );

    data.painter = &painter;
    data.scale = zoom;
    
    KivioStencil *pStencil = m_pStencilList->first();
    while( pStencil )
    {
        if( pStencil->isSelected() )
            pStencil->paintSelectionHandles( &data );

        pStencil = m_pStencilList->next();
    }
}

KivioStencil *KivioLayer::checkForStencil( KivioPoint *pPoint, int *collisionType, float threshhold )
{
    KivioStencil *pStencil;
    int colType;

    pStencil = m_pStencilList->last();
    while( pStencil )
    {
        if( (colType = pStencil->checkForCollision( pPoint, threshhold )) != kctNone )
        {
            // Assign the collision type and return
            *collisionType = colType;
            return pStencil;
        }

        pStencil = m_pStencilList->prev();
    }

    *collisionType = kctNone;

    return NULL;
}

void KivioLayer::setVisible( bool f )
{
    if( f==true )
    {
        m_flags = m_flags | FLOW_LAYER_VISIBLE;
    }
    else
    {
        m_flags = m_flags & (~FLOW_LAYER_VISIBLE);
    }
}

void KivioLayer::setConnectable( bool f )
{
    if( f==true )
    {
        m_flags = m_flags | FLOW_LAYER_CONNECTABLE;
    }
    else
    {
        m_flags = m_flags & (~FLOW_LAYER_CONNECTABLE);
    }
}

int KivioLayer::generateStencilIds( int next )
{
    KivioStencil *pStencil;
    
    pStencil = m_pStencilList->first();
    while( pStencil )
    {
        next = pStencil->generateIds( next );
    
        pStencil = m_pStencilList->next();
    }
    
    return next;
}

void KivioLayer::searchForConnections( KivioPage *p )
{
    KivioStencil *pStencil;
    KivioStencil *pCur;
    
    pStencil = m_pStencilList->first();
    while( pStencil )
    {
        // Backup the current list position
        pCur = pStencil;
        
        pStencil->searchForConnections( p );
        
        // Restore it
        m_pStencilList->find( pCur );
    
        pStencil = m_pStencilList->next();
    }
}

KivioStencil *KivioLayer::takeStencil( KivioStencil *p )
{
    m_pStencilList->find( p );
    
    return m_pStencilList->take();
}


/**
 * Attempts to connect a KivioConnectorPoint to a KivioConnectorTarget of each stencil.
 *
 * @param p The point to attempt the connection with
 * @param thresh The threshold to use
 * @returns The KivioConnectorTarget the point connected to
 */
KivioConnectorTarget *KivioLayer::connectPointToTarget( KivioConnectorPoint *p, float thresh )
{
    KivioConnectorTarget *pTarget;

    KivioStencil *pStencil = m_pStencilList->last();
    while( pStencil )
    {
        // Don't allow the connector point to connect to the stencil that owns it
        if( pStencil != p->stencil() )
        {
            pTarget = pStencil->connectToTarget(p, thresh);
            if( pTarget )
            {
                return pTarget;
            }
        }

        pStencil = m_pStencilList->prev();
    }

    return NULL;
}
