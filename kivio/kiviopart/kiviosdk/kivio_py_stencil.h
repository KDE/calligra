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

#ifndef KIVIO_PY_STENCIL_H
#define KIVIO_PY_STENCIL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "kivio_stencil.h"

#ifdef HAVE_PYTHON

#include <qptrlist.h>
#include <QColor>
#include <QFont>

/* slots gets defined in Qt 3.2 and conflicts with python 2.3 headers */
#undef slots
#include <Python.h>
#define slots

class KivioPyStencilSpawner;
class KivioPainter;
class KivioFillStyle;
class KivioView;

class KivioPyStencil : public KivioStencil
{

friend class KivioPyStencilSpawner;

public:
    KivioPyStencil();
    virtual ~KivioPyStencil();


    virtual bool loadXML( const QDomElement & );
    virtual QDomElement saveXML( QDomDocument & );
    virtual void loadConnectorTargetListXML( const QDomElement & );

    virtual void updateGeometry();
    virtual KivioStencil *duplicate();
    virtual KivioCollisionType checkForCollision( KoPoint *pPoint, double );
    virtual int resizeHandlePositions();

    virtual void paint( KivioIntraStencilData * );
    virtual void paintOutline( KivioIntraStencilData * );
    virtual void paint( KivioIntraStencilData * , bool outlined);
    virtual void paintConnectorTargets( KivioIntraStencilData * );
    virtual KivioConnectorTarget *connectToTarget( KivioConnectorPoint *p, double threshHold );
    virtual KivioConnectorTarget *connectToTarget( KivioConnectorPoint *p, int targetID );

    virtual int generateIds( int );

    virtual QColor fgColor();
    virtual void setFGColor( QColor );

    virtual void setBGColor( QColor );
    virtual QColor bgColor();

    virtual void setLineWidth( double );
    virtual double lineWidth();

    // FOnt stuff
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


protected:
    PyObject *vars, *globals;
    QString resizeCode; // python code to be runed when resize
    double old_x, old_y, old_w, old_h;

    virtual void rescaleShapes( PyObject * ); // find recursive for x,y,w,h in object( dict or list ) and update them

    /**
    *   set style settings from dict, return tru if need to use fill... methods or false if draw...
    */
    virtual void setStyle( KivioIntraStencilData *d, PyObject *style, int &fillStyle );
    QColor readColor( PyObject *color ); // used by setStyle


    QPtrList<KivioConnectorTarget> *m_pConnectorTargets;

    /**
    *   init stencil using initCode, return 1, if ok, or 0 if was some error ( maybe python syntax error )
    */
    int init( QString initCode);

    int runPython(QString code);
    double getDoubleFromDict( PyObject *dict, const char* key );
    QString getStringFromDict( PyObject *dict, const char* key );

    void PyDebug( PyObject * ); // show var
};

#else // HAVE_PYTHON

  #define KivioPyStencil KivioStencil

#endif // HAVE_PYTHON


#endif // KIVIO_PY_STENCIL_H

