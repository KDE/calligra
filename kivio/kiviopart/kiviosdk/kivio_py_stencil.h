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

#ifndef KIVIO_PY_STENCIL_H
#define KIVIO_PY_STENCIL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "kivio_stencil.h"

#define HAVE_PYTHON

#ifdef HAVE_PYTHON

#include <qlist.h>
#include <qcolor.h>
#include <qfont.h>

#include <Python.h>

class KivioPyStencilSpawner;
class KivioPainter;
class KivioFillStyle;


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
    virtual KivioCollisionType checkForCollision( KivioPoint *pPoint, float );
    virtual int resizeHandlePositions();

    virtual void paint( KivioIntraStencilData * );
    virtual void paintOutline( KivioIntraStencilData * );
    virtual void paint( KivioIntraStencilData * , bool outlined);
    virtual void paintConnectorTargets( KivioIntraStencilData * );
    virtual KivioConnectorTarget *connectToTarget( KivioConnectorPoint *p, float threshHold );
    virtual KivioConnectorTarget *connectToTarget( KivioConnectorPoint *p, int targetID );

    virtual int generateIds( int );

    virtual QColor fgColor();
    virtual void setFGColor( QColor );

    virtual void setBGColor( QColor );
    virtual QColor bgColor();

    virtual void setLineWidth( float );
    virtual float lineWidth();

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
    float old_x, old_y, old_w, old_h;

    virtual void rescaleShapes( PyObject * ); // find recursive for x,y,w,h in object( dict or list ) and update them

    /**
    *   set style settings from dict, return tru if need to use fill... methods or false if draw...
    */
    virtual void setStyle( KivioIntraStencilData *d, PyObject *style, int &fillStyle );
    QColor readColor( PyObject *color ); // used by setStyle


    QList<KivioConnectorTarget> *m_pConnectorTargets;

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

