/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2003 theKompany.com & Dave Marotti,
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
#ifndef KIVIO_SML_STENCIL_H
#define KIVIO_SML_STENCIL_H

#include <qptrlist.h>

#include "kivio_stencil.h"
class QDomElement;
class QColor;
class KivioConnectorTarget;
class KivioFillStyle;
class KivioIntraStencilData;
class KivioLineStyle;
class KivioPainter;
class KivioShape;
class KoZoomHandler;

class KivioSMLStencil : public KivioStencil
{
protected:
    friend class KivioSMLStencilSpawner;

    QPtrList<KivioShape> *m_pShapeList;
    KivioShape *m_pSubSelection;
    QPtrList<KivioConnectorTarget> *m_pConnectorTargets;

    // Offsets and scale which get reused in between
    // various drawing routines.  To save time and
    // stack space, we keep them around in the class
    int _xoff, _yoff;
    KoZoomHandler* m_zoomHandler;

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

    bool checkCollisionArc( KivioShape *, KoPoint * );
    bool checkCollisionBezier( KivioShape *, KoPoint * );
    bool checkCollisionOpenPath( KivioShape *, KoPoint * );
    bool checkCollisionClosedPath( KivioShape *, KoPoint * );
    bool checkCollisionPie( KivioShape *, KoPoint * );
    bool checkCollisionEllipse( KivioShape *, KoPoint * );
    bool checkCollisionLineArray( KivioShape *, KoPoint * );
    bool checkCollisionRectangle( KivioShape *, KoPoint * );
    bool checkCollisionRoundRectangle( KivioShape *, KoPoint * );
    bool checkCollisionPolygon( KivioShape *, KoPoint * );
    bool checkCollisionPolyline( KivioShape *, KoPoint * );
    bool checkCollisionTextBox( KivioShape *, KoPoint * );


    KivioShape *locateShape( const QString & );
    void loadConnectorTargetListXML( const QDomElement & );

public:
    KivioSMLStencil();
    virtual ~KivioSMLStencil();


    virtual bool loadXML( const QDomElement & );
    virtual QDomElement saveXML( QDomDocument & );

    virtual QPtrList<KivioShape> *shapeList() { return m_pShapeList; }
    virtual KivioShape *subSelection() { return m_pSubSelection; }

    virtual void paintOutline( KivioIntraStencilData * );
    virtual void paint( KivioIntraStencilData * );
    virtual void paintConnectorTargets( KivioIntraStencilData * );

    virtual KivioStencil *duplicate();

    virtual QColor fgColor();
    virtual QColor bgColor();
    virtual void setFGColor(QColor);
    virtual void setBGColor(QColor);
    virtual void setLineWidth(double);
    virtual double lineWidth();
    virtual void setLinePattern(int p);
    virtual int linePattern();
    virtual void setFillPattern(int p);
    virtual int fillPattern();

    virtual void setTextColor( QColor );
    virtual void setTextFont( const QFont & );

    virtual void setHTextAlign(int);
    virtual void setVTextAlign(int);

    virtual int hTextAlign();
    virtual int vTextAlign();

    virtual QString text();
    virtual void setText( const QString & );

    virtual QFont textFont();
    virtual QColor textColor();

    virtual KivioCollisionType checkForCollision( KoPoint *, double );

    virtual KivioConnectorTarget *connectToTarget( KivioConnectorPoint *, double );
    virtual KivioConnectorTarget *connectToTarget( KivioConnectorPoint *, int );
    
    virtual KoPoint snapToTarget( const KoPoint& p, double thresh, bool& hit );

    virtual void updateGeometry();

    virtual int generateIds( int );

    virtual int resizeHandlePositions();

    virtual KivioLineStyle lineStyle();
    virtual void setLineStyle(KivioLineStyle ls);

    /**
      Returns the name of the text box that is at @param p.
      If there exist no text box at @param p then it returns QString::null.
    */
    virtual QString getTextBoxName(const KoPoint& p);

    virtual void setText(const QString& text, const QString& name);
    virtual QString text(const QString& name);

    virtual void addConnectorTarget(const KoPoint&);
    virtual void removeConnectorTarget(const KoPoint&);

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


