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
#ifndef KIVIO_PY_STENCIL_SPAWNER_H
#define KIVIO_PY_STENCIL_SPAWNER_H

#include <qdom.h>
#include <qptrlist.h>
#include <qstring.h>

#include "kivio_stencil_spawner.h"
#include <config.h>

#ifdef HAVE_PYTHON

class KivioStencilSpawnerSet;
class KivioStencil;
class KivioPyStencil;
class KivioConnectorTarget;
class QPainter;
class QPixmap;
class QRect;

class KivioPyStencilSpawner : public KivioStencilSpawner
{
protected:
    KivioPyStencil *m_pStencil;
    QString m_filename;

    // Target list
    QPtrList<KivioConnectorTarget>*m_pTargets;

protected:
    void loadShape( QDomNode & );
    QString readDesc( const QString & );

public:
    KivioPyStencilSpawner( KivioStencilSpawnerSet * );
    virtual ~KivioPyStencilSpawner();

    virtual bool load( const QString & );

    virtual QDomElement saveXML( QDomDocument & );

    virtual QString &filename() { return m_filename; }

    virtual KivioStencil *newStencil();

    QPtrList <KivioConnectorTarget> *targets() { return m_pTargets; }
};

#else // HAVE_PYTHON

  #define KivioPyStencilSpawner KivioStencilSpawner

#endif // HAVE_PYTHON


#endif



