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
#include "kivio_stencil.h"

KivioConnectorPoint::KivioConnectorPoint()
{
    m_x = m_y = 0.0f;
    m_pTarget = NULL;
    m_pStencil = NULL;
    m_targetId = -1;
    m_connectable = true;
}

KivioConnectorPoint::KivioConnectorPoint( KivioStencil *pParent, bool conn )
{
    m_x = m_y = 0.0f;
    m_pTarget = NULL;
    m_pStencil = pParent;
    m_targetId = -1;
    m_connectable = conn;
}

KivioConnectorPoint::~KivioConnectorPoint()
{
    if( m_pTarget )
    {
        m_pTarget->removeConnectorPointFromList( this );
        m_pTarget = NULL;
    }

    m_pStencil = NULL;
}

/**
 * Sets the target to a value.
 *
 * @param pTarget The target to connect to
 *
 * This sets the KivioConnectorTarget we are attached to.  In addition to
 * setting the pointer, it also tells the target to add this as a connected
 * point.
 */
void KivioConnectorPoint::setTarget( KivioConnectorTarget *pTarget  )
{
   if( m_connectable == false )
   {
      return;
   }

    if( m_pTarget )
    {
         m_pTarget->removeConnectorPointFromList( this );
    }

    
    m_pTarget = pTarget;

    m_x = pTarget->x();
    m_y = pTarget->y();

    m_pTarget->addConnectorPointToList( this );
}


/**
 * Sets the x coordinate.
 *
 * @param newX The new value
 * @param updateStencil Whether or not the stencil's geometry should be updated (def=true)
 *
 * Sets the X coordinate to a new value, and optionally
 * updates the stencil's geometry if updateStencil is true.
 */
void KivioConnectorPoint::setX( float newX, bool updateStencil )
{
   float oldX = m_x;
    m_x = newX;

    if( updateStencil && m_pStencil )
        m_pStencil->updateConnectorPoints(this, oldX, m_y);
}


/**
 * Sets the y coordinate.
 *
 * @param newY The new value
 * @param updateStencil Whether or not the stencil's geometry should be updated (def=true)
 *
 * Sets the Y coordinate to a new value, and optionally
 * updates the stencil's geometry if updateStencil is true.
 */
void KivioConnectorPoint::setY( float newY, bool updateStencil )
{
   float oldY = m_y;
    m_y = newY;

    if( updateStencil && m_pStencil )
        m_pStencil->updateConnectorPoints(this, m_x, oldY);
}


/**
 * Sets the coordinates.
 *
 * @param newX The new x value
 * @param newY The new y value
 * @param updateStencil Whether or not the stencil's geometry should be updated (def=true)
 *
 * Sets the X & Y coordinates to new values, and optionally
 * updates the stencil's geometry if updateStencil is true.
 */
 void KivioConnectorPoint::setPosition( float newX, float newY, bool updateStencil )
{
   float oldX = m_x;
   float oldY = m_y;

    m_x = newX;
    m_y = newY;

    if( updateStencil && m_pStencil )
        m_pStencil->updateConnectorPoints(this, oldX, oldY);
}


/**
 * Disconnects from the target, with optional behavior.
 *
 * @param removeFromTargetList Whether we should tell the target to remove us from it's list or not. (Default=true).
 *
 * Call this function if you want this connector to disconnect itself from it's target.  If removeFromTargetList is
 * true, then we call KivioConnectorTarget::removeConnectorPointFromList() which causes the target to look for
 * us in his list of connections and remove us.
 */
void KivioConnectorPoint::disconnect( bool removeFromTargetList )
{
    if( m_pTarget )
    {
        if( removeFromTargetList == true )
            m_pTarget->removeConnectorPointFromList(this);
            
        m_pTarget = NULL;
    }

}


/**
 * Loads this object from an XML file.
 *
 * @param e The QDomElement to load from.
 *
 * This will load the necessary values from an XML file.
 */
bool KivioConnectorPoint::loadXML( const QDomElement &e )
{
    m_x = XmlReadFloat( e, "x", 1.0f );
    m_y = XmlReadFloat( e, "y", 1.0f );
    m_targetId = XmlReadInt( e, "targetId", -1 );
    m_connectable = (bool)XmlReadInt( e, "connectable", (int)true );

    return true;
}


/**
 * Saves this object to an XML file.
 *
 * @param doc The QDomDocument to save to.
 *
 * This creates a new QDomElement and saves its data to it.
 *
 * @returns The new element created and saved to.
 */
QDomElement KivioConnectorPoint::saveXML( QDomDocument &doc )
{
    QDomElement e = doc.createElement("KivioConnectorPoint");

    XmlWriteFloat( e, "x", m_x );
    XmlWriteFloat( e, "y", m_y );
    XmlWriteInt( e, "connectable", (int)m_connectable );

    if( m_targetId != -1 )
        XmlWriteInt( e, "targetId", m_targetId );

    return e;
}
