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
#ifndef KIVIO_CONNECTOR_POINT_H
#define KIVIO_CONNECTOR_POINT_H

#include <qdom.h>

class KivioConnectorTarget;
class KivioStencil;


class KivioConnectorPoint
{
protected:
    // The position of this connector point
    float m_x, m_y;

    // The target this connector is connected to
    KivioConnectorTarget *m_pTarget;

    // The stencil this point belongs to
    KivioStencil *m_pStencil;

    int m_targetId;
    
    bool m_connectable;


public:
    KivioConnectorPoint( KivioStencil *, bool conn=true );
    KivioConnectorPoint();
    virtual ~KivioConnectorPoint();

    bool loadXML( const QDomElement & );
    QDomElement saveXML( QDomDocument & );

    KivioConnectorTarget *target() const { return m_pTarget; }
    void setTarget( KivioConnectorTarget * );

    KivioStencil *stencil() const { return m_pStencil; }
    void setStencil( KivioStencil *p ) { m_pStencil=p; }

    void setTargetId( int i ) { m_targetId = i; }
    int targetId() { return m_targetId; }

    float x() { return m_x; }
    float y() { return m_y; }

    void setX( float, bool updateStencil=true );
    void setY( float, bool updateStencil=true );
    void setPosition( float, float, bool updateStencil=true );
    void setConnectable( bool b ) { m_connectable = b; if( b==false ) { disconnect(); } }

    bool connectable() { return m_connectable; }

    void disconnect(bool removeFromTargetList=true);
};

#endif

