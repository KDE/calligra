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
#ifndef KIVIO_CONNECTOR_TARGET_H
#define KIVIO_CONNECTOR_TARGET_H

#include <qdom.h>
#include <qptrlist.h>

#include <KoPoint.h>

class KivioIntraStencilData;
class KivioPainter;
class KivioStencil;
class KivioConnectorPoint;

class KivioConnectorTarget
{
protected:
    KoPoint m_position;
    double m_xOffset, m_yOffset;
    int m_id;
    QPtrList<KivioConnectorPoint> *m_pConnectors;

public:
    KivioConnectorTarget();
    KivioConnectorTarget( double, double );
    KivioConnectorTarget( double, double, double, double );
    virtual ~KivioConnectorTarget();

    bool loadXML( const QDomElement & );
    QDomElement saveXML( QDomDocument & );

    KivioConnectorTarget *duplicate();

    inline float x() { return m_position.x(); }
    inline float y() { return m_position.y(); }
    inline KoPoint position() { return m_position; }

    void setId( int id );
    inline int id() { return m_id; }

    void setX( float );
    void setY( float );
    void setPosition( float, float );

    void addConnectorPointToList( KivioConnectorPoint * );
    bool removeConnectorPointFromList( KivioConnectorPoint * );

    void paintOutline( KivioIntraStencilData * );
    
    void disconnectPoint( KivioConnectorPoint * );
    
    bool hasConnections();
    
    void setOffsets(double x, double y);
    double xOffset() { return m_xOffset; }
    double yOffset() { return m_yOffset; }
};

#endif

