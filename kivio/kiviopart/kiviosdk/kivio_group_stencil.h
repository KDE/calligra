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
#ifndef KIVIO_GROUP_STENCIL_H
#define KIVIO_GROUP_STENCIL_H

#include "kivio_stencil.h"

#include <qdom.h>

class KivioIntraStencilData;
class KivioLayer;
class KivioPage;
class KivioPainter;
class KivioRect;

class KivioGroupStencil : public KivioStencil
{
protected:
//    void drawSelectionHandles( KivioPainter * );

    // The list of child stencils
    QList<KivioStencil> *m_pGroupList;

public:
    KivioGroupStencil();
    virtual ~KivioGroupStencil();

    
    // File I/O routines
    virtual bool loadXML( const QDomElement &, KivioLayer * );
    virtual QDomElement saveXML( QDomDocument & );


    // Painting routines
    virtual void paintOutline( KivioIntraStencilData * );
    virtual void paint( KivioIntraStencilData * );
    virtual void paintConnectorTargets( KivioIntraStencilData * );

    
    // Duplication
    virtual KivioStencil *duplicate();

    
    // FG/Color operations
    virtual void setFGColor( QColor );
    virtual void setBGColor( QColor );
    
    

    // Text operations
    virtual void setTextColor( QColor );

    virtual QFont textFont();
    virtual void setTextFont( const QFont & );

    virtual int hTextAlign();
    virtual int vTextAlign();

    virtual void setHTextAlign(int);
    virtual void setVTextAlign(int);
    
    virtual void setText( const QString & );
    virtual QString text();
    
    
    // Line width
    virtual void setLineWidth( float );

    
    // Collision detection
    virtual KivioCollisionType checkForCollision( KivioPoint *, float );

    
    // Group operations
    virtual void addToGroup( KivioStencil * );
    virtual QList<KivioStencil> *groupList() { return m_pGroupList; }

    
    // Position / Dimension operations
    virtual void setX( float );
    virtual void setY( float );
    virtual void setW( float );
    virtual void setH( float );
    virtual void setPosition( float f1, float f2 );
    virtual void setDimensions( float f1, float f2 );
    
    
    // ID generation
    virtual int generateIds( int );
    

    // Connection operations
    virtual KivioConnectorTarget *connectToTarget( KivioConnectorPoint *, float );
    virtual KivioConnectorTarget *connectToTarget( KivioConnectorPoint *, int );

    virtual void searchForConnections( KivioPage * );

    virtual int resizeHandlePositions();
};

#endif

