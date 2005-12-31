/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2004 theKompany.com & Dave Marotti
 *                         Peter Simonsson
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
#ifndef KIVIO_GROUP_STENCIL_H
#define KIVIO_GROUP_STENCIL_H

#include "kivio_stencil.h"

#include <qdom.h>

class KivioIntraStencilData;
class KivioLayer;
class KivioPage;
class KivioPainter;

class KivioGroupStencil : public KivioStencil
{
  protected:
    // The list of child stencils
    QPtrList<KivioStencil> *m_pGroupList;
    int checkForCollision(const KoPoint& p);

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
    virtual QColor textColor();
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
    virtual void setLineWidth( double );


    // Collision detection
    virtual KivioCollisionType checkForCollision( KoPoint *, double );


    // Group operations
    virtual void addToGroup( KivioStencil * );
    virtual QPtrList<KivioStencil> *groupList() { return m_pGroupList; }


    // Position / Dimension operations
    virtual void setX( double );
    virtual void setY( double );
    virtual void setW( double );
    virtual void setH( double );
    virtual void setPosition( double f1, double f2 );
    virtual void setDimensions( double f1, double f2 );


    // ID generation
    virtual int generateIds( int );


    // Connection operations
    virtual KivioConnectorTarget *connectToTarget( KivioConnectorPoint *, double );
    virtual KivioConnectorTarget *connectToTarget( KivioConnectorPoint *, int );

    virtual void searchForConnections( KivioPage * );

    virtual int resizeHandlePositions();

    /**
      Returns the name of the text box that is at @param p.
      If there exist no text box at @param p then it returns QString::null.
    */
    virtual QString getTextBoxName(const KoPoint& p);

    virtual void setText(const QString& text, const QString& name);
    virtual QString text(const QString& name);

    virtual bool hasTextBox() const;

    virtual QColor textColor(const QString& textBoxName);
    virtual void setTextColor(const QString& textBoxName, const QColor& color);

    virtual QFont textFont(const QString& textBoxName);
    virtual void setTextFont(const QString& textBoxName, const QFont& font);

    virtual int hTextAlign(const QString& textBoxName);
    virtual int vTextAlign(const QString& textBoxName);

    virtual void setHTextAlign(const QString& textBoxName, int align);
    virtual void setVTextAlign(const QString& textBoxName, int align);
};

#endif

