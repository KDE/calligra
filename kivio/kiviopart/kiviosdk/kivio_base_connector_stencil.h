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
#ifndef KIVIO_BASE_CONNECTOR_STENCIL_H
#define KIVIO_BASE_CONNECTOR_STENCIL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qdom.h>
#include <qptrlist.h>

#include "kivio_stencil.h"

class KivioArrowHead;
class KivioConnectorPoint;
class KivioCustomDragData;
class KivioFillStyle;
class KivioLineStyle;
class KivioIntraStencilData;
class KivioStencil;

class KivioBaseConnectorStencil : public KivioStencil
{
protected:
    QPtrList <KivioConnectorPoint> *m_pConnectorPoints;

    KivioFillStyle *m_pFillStyle;
    KivioLineStyle *m_pLineStyle;


    // Utility routines
    bool boolAllTrue( bool *, int );
    bool boolContainsFalse( bool *, int );

    QDomElement saveConnectors( QDomDocument &doc );
    bool loadConnectors( const QDomElement & );

    QDomElement saveProperties( QDomDocument &doc );
    bool loadProperties( const QDomElement & );

public:
    KivioBaseConnectorStencil();
    virtual ~KivioBaseConnectorStencil();

    // fg color functions
    virtual void setFGColor( QColor c );
    virtual QColor fgColor();


    // bg color functions
    virtual void setBGColor( QColor c );
    virtual QColor bgColor();


    // line width functions
    virtual void setLineWidth( double f );
    virtual double lineWidth();


    // position functions
    virtual void setX( double );
    virtual void setY( double );
    virtual void setPosition( double, double );


    // required for connector tool
    virtual void setStartPoint( double, double );
    virtual void setEndPoint( double, double );


    // painting functions
    virtual void paint( KivioIntraStencilData * );
    virtual void paintOutline( KivioIntraStencilData * );
    virtual void paintConnectorTargets( KivioIntraStencilData * );
    virtual void paintSelectionHandles( KivioIntraStencilData * );


    // collision detection
    virtual KivioCollisionType checkForCollision( KoPoint *, double );


    // custom dragging
    virtual void customDrag( KivioCustomDragData * );


    // geometry management
    virtual void updateGeometry();


    // file i/o routines
    virtual bool loadXML( const QDomElement & );
    virtual QDomElement saveXML( QDomDocument & );


    // duplication routines
    virtual KivioStencil* duplicate();


    // connection search routines
    virtual void searchForConnections( KivioPage * );


    // returns which resize handles are valid
    virtual int resizeHandlePositions();

};

#endif

