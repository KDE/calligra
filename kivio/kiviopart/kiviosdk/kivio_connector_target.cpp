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
#include "kivio_connector_point.h"
#include "kivio_connector_target.h"
#include "kivio_point.h"
#include "kivio_stencil.h"
#include "kivio_intra_stencil_data.h"

#include <kdebug.h>

KivioConnectorTarget::KivioConnectorTarget()
    : m_pConnectors(NULL)
{
    m_position.setX( 0.0f );
    m_position.setY( 0.0f );
    m_pConnectors = new QPtrList<KivioConnectorPoint>;
    m_pConnectors->setAutoDelete(false);
    m_id = -1;
    m_xOffset = m_yOffset = 0;
}

KivioConnectorTarget::KivioConnectorTarget(double x, double y)
{
    m_position.setX( x );
    m_position.setY( y );
    m_pConnectors = new QPtrList<KivioConnectorPoint>;
    m_pConnectors->setAutoDelete(false);

    m_id = -1;
    m_xOffset = m_yOffset = 0;
}

KivioConnectorTarget::KivioConnectorTarget(double x, double y, double xOffset, double yOffset)
{
    m_position.setX( x );
    m_position.setY( y );
    m_pConnectors = new QPtrList<KivioConnectorPoint>;
    m_pConnectors->setAutoDelete(false);

    m_id = -1;
    setOffsets(xOffset, yOffset);
}

/**
 * Duplicates this target.
 *
 * Duplicates this target, only the position.  The connections
 * are not preserved.
 */
KivioConnectorTarget *KivioConnectorTarget::duplicate()
{
    KivioConnectorTarget *pTarget = new KivioConnectorTarget( m_position.x(), m_position.y(), m_xOffset, m_yOffset );

    return pTarget;
}


/**
 * Kill this object and disconnects all KivioConnectorPoints from this.
 *
 * This is a slightly odd function in that it does not call the disconnect() method
 * of KivioConnectorPoint.  The reason for this is that FIX ME DAVE!
 */
KivioConnectorTarget::~KivioConnectorTarget()
{
    // Iterate through all connectors diconnecting them from this
    KivioConnectorPoint *point;

    if( m_pConnectors )
    {
        point = m_pConnectors->first();
        point = m_pConnectors->take();

        while( point )
        {
	   kdDebug(43000) << "KivioConnectorTarget:: -> diconnecting" << endl;

            // Disconnect the point.  But tell the point to not call
            // KivioConnectorTarget::removeConnectorFromList() because it will cause our
            // position in the list to be screwed up.
            point->disconnect( false );

            point = m_pConnectors->take();
        }

        delete m_pConnectors;
        m_pConnectors = NULL;
    }
}


/**
 * Loads this object from an XML file.
 */
bool KivioConnectorTarget::loadXML( const QDomElement &e )
{
    if( e.tagName().compare( "KivioConnectorTarget" ) != 0 )
    {
       kdDebug(43000) << "Attempted to load KivioConnectorTarget from non-KivioConnectorTarget element" << endl;
        return false;
    }

    m_position.setX(XmlReadFloat( e, "x", 1.0f ));
    m_position.setY(XmlReadFloat( e, "y", 1.0f ));

    m_id = XmlReadInt( e, "id", -1 );

    return true;
}


/**
 * Saves this object to an XML file.
 */
QDomElement KivioConnectorTarget::saveXML( QDomDocument &doc )
{
    QDomElement e;

    e = doc.createElement("KivioConnectorTarget");

    XmlWriteFloat( e, "x", m_position.x() );
    XmlWriteFloat( e, "y", m_position.y() );

    if( m_id != -1 )
        XmlWriteInt( e, "id", m_id );

    return e;
}


/**
 * Adds a KivioConnectorPoint to the list of connections.
 */
void KivioConnectorTarget::addConnectorPointToList( KivioConnectorPoint *p )
{
    if( p )
        m_pConnectors->append(p);
}


/**
 * Removes a KivioConnectorPoint from the list of connections.
 *
 * Removes a KivioConnectorPoint from the list of connections. Note, it does
 * not disconnect the point on the KivioConnectorPoint side.  This function
 * is mainly used by KivioConnectorPoint, and probably shouldn't be used by
 * anything/one else unless you really know what you are doing.
 */
bool KivioConnectorTarget::removeConnectorPointFromList( KivioConnectorPoint *p )
{
    if( !p )
        return false;


    return m_pConnectors->remove(p);
}


/**
 * Sets a new position and updates all connected points.
 */
void KivioConnectorTarget::setX( float _x )
{
    m_position.setX(_x);

    KivioConnectorPoint *pPoint = m_pConnectors->first();
    while( pPoint )
    {
        pPoint->setX( _x, true );

        pPoint = m_pConnectors->next();
    }
}


/**
 * Sets a new position and updates all connected points.
 */

void KivioConnectorTarget::setY( float _y )
{
    m_position.setY(_y);

    KivioConnectorPoint *pPoint = m_pConnectors->first();
    while( pPoint )
    {
        pPoint->setY( _y, true );

        pPoint = m_pConnectors->next();
    }
}


/**
 * Sets a new position and updates all connected points.
 */

void KivioConnectorTarget::setPosition( float _x, float _y )
{
    m_position.setX( _x );
    m_position.setY( _y );

    KivioConnectorPoint *pPoint = m_pConnectors->first();
    while( pPoint )
    {
        pPoint->setPosition( _x, _y, true );

        pPoint = m_pConnectors->next();
    }
}


/**
 * Issues paintOutline requests to all connected stencils.
 */
void KivioConnectorTarget::paintOutline( KivioIntraStencilData *pData )
{
    KivioConnectorPoint *pPoint;
    KivioStencil *pStencil;

    pPoint = m_pConnectors->first();
    while( pPoint )
    {
        pStencil = pPoint->stencil();

        if( pStencil )
            pStencil->paintOutline( pData );

        pPoint = m_pConnectors->next();
    }
}


/**
 * Tests is this target has any connections.
 */
bool KivioConnectorTarget::hasConnections()
{
    KivioConnectorPoint *pPoint = m_pConnectors->first();

    if( pPoint )
        return true;

    return false;
}


/**
 * Sets this target's unique ID, for generateIds() during a saveXML().
 */
void KivioConnectorTarget::setId( int i )
{
    KivioConnectorPoint *pPoint = m_pConnectors->first();
    m_id= i;

    while( pPoint )
    {
        pPoint->setTargetId( i );

        pPoint = m_pConnectors->next();
    }
}

void KivioConnectorTarget::setOffsets(double x, double y)
{
  m_xOffset = x;
  m_yOffset = y;
}
