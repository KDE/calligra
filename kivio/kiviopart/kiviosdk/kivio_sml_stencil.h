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
#ifndef KIVIO_SML_STENCIL_H
#define KIVIO_SML_STENCIL_H

#include <qdom.h>
#include <qlist.h>

#include "kivio_stencil.h"

class QColor;
class KivioConnectorTarget;
class KivioFillStyle;
class KivioIntraStencilData;
class KivioLineStyle;
class KivioPainter;
class KivioShape;

class KivioSMLStencil : public KivioStencil
{
protected:
    friend class KivioSMLStencilSpawner;

    QList<KivioShape> *m_pShapeList;
    KivioShape *m_pSubSelection;
    QList<KivioConnectorTarget> *m_pConnectorTargets;
//    QList<KivioConnectorTarget> *m_pOriginalConnectorTargets;

    // Offsets and scale which get reused in between
    // various drawing routines.  To save time and
    // stack space, we keep them around in the class
    float _scale, _xoff, _yoff;


// Drawing routines
protected:
    void drawOutlineArc( KivioShape *, KivioIntraStencilData * );
    void drawOutlineBezier( KivioShape *, KivioIntraStencilData * );
    void drawOutlineOpenPath( KivioShape *, KivioIntraStencilData * );
    void drawOutlineClosedPath( KivioShape *, KivioIntraStencilData * );
    void drawOutlinePie( KivioShape *, KivioIntraStencilData * );
    void drawOutlineEllipse( KivioShape *, KivioIntraStencilData * );
    void drawOutlineLineArray( KivioShape *, KivioIntraStencilData * );
    void drawOutlineRectangle( KivioShape *, KivioIntraStencilData * );
    void drawOutlineRoundRectangle( KivioShape *, KivioIntraStencilData * );
    void drawOutlinePolygon( KivioShape *, KivioIntraStencilData * );
    void drawOutlinePolyline( KivioShape *, KivioIntraStencilData * );
    void drawOutlineTextBox( KivioShape *, KivioIntraStencilData * );

    void drawArc( KivioShape *, KivioIntraStencilData * );
    void drawBezier( KivioShape *, KivioIntraStencilData * );
    void drawOpenPath( KivioShape *, KivioIntraStencilData * );
    void drawClosedPath( KivioShape *, KivioIntraStencilData * );
    void drawPie( KivioShape *, KivioIntraStencilData * );
    void drawEllipse( KivioShape *, KivioIntraStencilData * );
    void drawLineArray( KivioShape *, KivioIntraStencilData * );
    void drawRectangle( KivioShape *, KivioIntraStencilData * );
    void drawRoundRectangle( KivioShape *, KivioIntraStencilData * );
    void drawPolygon( KivioShape *, KivioIntraStencilData * );
    void drawPolyline( KivioShape *, KivioIntraStencilData * );
    void drawTextBox( KivioShape *, KivioIntraStencilData * );

    bool checkCollisionArc( KivioShape *, KivioPoint * );
    bool checkCollisionBezier( KivioShape *, KivioPoint * );
    bool checkCollisionOpenPath( KivioShape *, KivioPoint * );
    bool checkCollisionClosedPath( KivioShape *, KivioPoint * );
    bool checkCollisionPie( KivioShape *, KivioPoint * );
    bool checkCollisionEllipse( KivioShape *, KivioPoint * );
    bool checkCollisionLineArray( KivioShape *, KivioPoint * );
    bool checkCollisionRectangle( KivioShape *, KivioPoint * );
    bool checkCollisionRoundRectangle( KivioShape *, KivioPoint * );
    bool checkCollisionPolygon( KivioShape *, KivioPoint * );
    bool checkCollisionPolyline( KivioShape *, KivioPoint * );
    bool checkCollisionTextBox( KivioShape *, KivioPoint * );


    KivioShape *locateShape( const QString & );
    void loadConnectorTargetListXML( const QDomElement & );

public:
    KivioSMLStencil();
    virtual ~KivioSMLStencil();
    
    
    virtual bool loadXML( const QDomElement & );
    virtual QDomElement saveXML( QDomDocument & );

    virtual QList<KivioShape> *shapeList() { return m_pShapeList; }
    virtual KivioShape *subSelection() { return m_pSubSelection; }

    virtual void paintOutline( KivioIntraStencilData * );
    virtual void paint( KivioIntraStencilData * );
    virtual void paintConnectorTargets( KivioIntraStencilData * );

    virtual KivioStencil *duplicate();

    virtual QColor fgColor();
    virtual QColor bgColor();
    virtual void setFGColor(QColor);
    virtual void setBGColor(QColor);
    virtual void setLineWidth(float);
    virtual float lineWidth();

    virtual void setTextColor( QColor );
    virtual void setTextFont( const QFont & );

    virtual void setHTextAlign(int);
    virtual void setVTextAlign(int);

    virtual int hTextAlign();
    virtual int vTextAlign();

    virtual QString text();
    virtual void setText( const QString & );

    virtual QFont textFont();

    virtual KivioCollisionType checkForCollision( KivioPoint *, float );

    virtual KivioConnectorTarget *connectToTarget( KivioConnectorPoint *, float );
    virtual KivioConnectorTarget *connectToTarget( KivioConnectorPoint *, int );

    virtual void updateGeometry();

    virtual int generateIds( int );

    virtual int resizeHandlePositions();

};

#endif


