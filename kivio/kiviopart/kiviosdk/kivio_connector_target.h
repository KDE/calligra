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
#ifndef KIVIO_CONNECTOR_TARGET_H
#define KIVIO_CONNECTOR_TARGET_H

#include <qdom.h>
#include <qlist.h>

#include "kivio_point.h"

class KivioIntraStencilData;
class KivioPainter;
class KivioStencil;
class KivioConnectorPoint;


class KivioConnectorTarget
{
protected:
    KivioPoint m_position;
    int m_id;
    QList<KivioConnectorPoint> *m_pConnectors;

public:
    KivioConnectorTarget();
    KivioConnectorTarget( float, float );
    virtual ~KivioConnectorTarget();

    bool loadXML( const QDomElement & );
    QDomElement saveXML( QDomDocument & );

    KivioConnectorTarget *duplicate();

    inline float x() { return m_position.x(); }
    inline float y() { return m_position.y(); }

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
};

#endif

