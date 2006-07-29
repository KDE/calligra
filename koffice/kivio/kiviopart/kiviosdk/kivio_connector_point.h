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
#ifndef KIVIO_CONNECTOR_POINT_H
#define KIVIO_CONNECTOR_POINT_H

#include <qdom.h>

#include <KoPoint.h>
#include <koffice_export.h>
class KivioConnectorTarget;
class KivioStencil;


class KIVIO_EXPORT KivioConnectorPoint
{
protected:
    // The position of this connector point
    KoPoint m_pos;

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
    int targetId() const { return m_targetId; }

    double x() const { return m_pos.x(); }
    double y() const { return m_pos.y(); }
    KoPoint position() const { return m_pos; }

    void setX( double, bool updateStencil = true );
    void setY( double, bool updateStencil = true );
    void setPosition( double, double, bool updateStencil = true );
    void moveBy( double _x, double _y, bool updateStencil = true );

    bool connectable() const { return m_connectable; }
    void setConnectable( bool b ) { m_connectable = b; if( b==false ) { disconnect(); } }

    void disconnect(bool removeFromTargetList = true);
    bool isConnected();
};

#endif

