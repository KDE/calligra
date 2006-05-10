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
#ifndef KIVIO_BASE_TARGET_STENCIL_H
#define KIVIO_BASE_TARGET_STENCIL_H

#include <QFont>
#include <QColor>
#include <qdom.h>
#include <qptrlist.h>

class KivioCustomDragData;
class KivioConnectorPoint;
class KivioConnectorTarget;
class KivioFillStyle;
class KivioIntraStencilData;
class KivioLineStyle;
class KivioPage;
class KivioStencilSpawner;
class KivioTextStyle;

#include "kivio_stencil.h"


class KivioBaseTargetStencil : public KivioStencil
{
protected:
    KivioLineStyle *m_pLineStyle;
    KivioFillStyle *m_pFillStyle;
    KivioTextStyle *m_pTextStyle;

    QPtrList<KivioConnectorTarget> *m_pTargets;

    virtual QDomElement createRootElement( QDomDocument & );

    virtual bool saveCustom( QDomElement &, QDomDocument & );
    virtual QDomElement saveProperties( QDomDocument & );
    virtual QDomElement saveTargets( QDomDocument & );

    virtual bool loadCustom( const QDomElement & );
    virtual bool loadProperties( const QDomElement & );
    virtual bool loadTargets( const QDomElement & );

    virtual void copyBasicInto( KivioBaseTargetStencil * );

public:
    KivioBaseTargetStencil();
    virtual ~KivioBaseTargetStencil();

    virtual KivioStencil *duplicate() { return NULL; }


    //////////////////////////////
    //
    // KivioLineStyle
    //
    //////////////////////////////
    virtual QColor fgColor();
    virtual void setFGColor( QColor c );

    virtual void setLineWidth( double l );
    virtual double lineWidth();



    //////////////////////////////
    //
    // KivioFillStyle
    //
    //////////////////////////////
    virtual void setBGColor( QColor c );
    virtual QColor bgColor();

    virtual KivioFillStyle *fillStyle();



    //////////////////////////////
    //
    // KivioTextStyle
    //
    //////////////////////////////
    virtual QColor textColor();
    virtual void setTextColor( QColor c );

    virtual QFont textFont();
    virtual void setTextFont( const QFont &f );

    virtual int hTextAlign();
    virtual int vTextAlign();

    virtual void setHTextAlign(int a);
    virtual void setVTextAlign(int a);

    virtual void setText( const QString &a );
    virtual QString text();



    ///////////////////////////////
    //
    // Paint Routines
    //
    ///////////////////////////////
    virtual void paint( KivioIntraStencilData * );
    virtual void paintOutline( KivioIntraStencilData * );
    virtual void paintConnectorTargets( KivioIntraStencilData * );



    ///////////////////////////////
    //
    // File I/O
    //
    ///////////////////////////////
    virtual bool loadXML( const QDomElement & );
    virtual QDomElement saveXML( QDomDocument & );



    ///////////////////////////////
    //
    // Geometry
    //
    ///////////////////////////////
    virtual void updateGeometry();



    ///////////////////////////////
    //
    // Connection Routines
    //
    ///////////////////////////////
    // This attempts to connect based on position
    virtual KivioConnectorTarget *connectToTarget( KivioConnectorPoint *, double );

    // This attempts to connect based on a targetID.  This should  ***ONLY*** be used
    // right after a load
    virtual KivioConnectorTarget *connectToTarget( KivioConnectorPoint *, int );


    ///////////////////////////////
    //
    // ID Generation
    //
    ///////////////////////////////
    virtual int generateIds( int );



    virtual int resizeHandlePositions();
};

#endif


