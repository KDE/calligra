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

#include "kivio_py_stencil_spawner.h"

#ifdef HAVE_PYTHON

#include "kivio_common.h"
#include "kivio_connector_target.h"
#include "kivio_py_stencil.h"
#include "kivio_stencil_spawner_set.h"
#include "kivio_stencil_spawner.h"
#include "kivio_stencil_spawner_info.h"

#include <iostream>
#include <qdom.h>
#include <qfile.h>
#include <qiodevice.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qpixmap.h>
#include <qrect.h>
#include <qfileinfo.h>

#include <kdebug.h>

KivioPyStencilSpawner::KivioPyStencilSpawner( KivioStencilSpawnerSet *p )
    : KivioStencilSpawner( p ),
      m_pStencil(NULL)
{
    m_pStencil = new KivioPyStencil();
	m_pStencil->setSpawner(this);

    m_pTargets = new QPtrList<KivioConnectorTarget>;
    m_pTargets->setAutoDelete(true);
}

KivioPyStencilSpawner::~KivioPyStencilSpawner()
{
    if( m_pStencil )
    {
        delete m_pStencil;
        m_pStencil = NULL;
    }

    if( m_pTargets )
    {
        delete m_pTargets;
        m_pTargets = NULL;
    }

    kdDebug(43000) << "* PyStencilSpawner "<< m_pInfo->id() << " deleted" << endl;
}

QDomElement KivioPyStencilSpawner::saveXML( QDomDocument &doc )
{
    QDomElement spawnE = doc.createElement("KivioPyStencilSpawner");

    XmlWriteString( spawnE, "id", m_pInfo->id() );

    return spawnE;
}

bool KivioPyStencilSpawner::load( const QString &file )
{
    KivioConnectorTarget *pTarget;
    QDomDocument d("test");

    m_filename = QString(file);
    QFile f(file);

    if( f.open( IO_ReadOnly )==false )
    {
       kdDebug(43000) << "KivioPyStencilSpawner::load() - Error opening stencil: " << file << endl;
        return false;
    }

    d.setContent(&f);

    QDomElement root = d.documentElement();
    QDomElement e;
    QDomNode node = root.firstChild();
    QString nodeName;

    while( !node.isNull() )
    {
        nodeName = node.nodeName();

        if( nodeName.compare("KivioPyStencilSpawnerInfo")==0 )
        {
            m_pInfo->loadXML( (const QDomElement)node.toElement() );
        }
        else if( nodeName.compare("init")==0 )
        {
		   m_pStencil->setSpawner(this);
		   if ( ! m_pStencil->init( node.toElement().text() ) ) {
              return false;
           }
           // init connector targets list
           PyObject *targets = PyDict_GetItemString(m_pStencil->vars,"connector_targets");
           int size = PyList_Size( targets );
           for ( int i=0; i<size; i++ ) {

                PyObject *target = PyList_GetItem( targets, i );
                float x = m_pStencil->getDoubleFromDict( target,"x");
                float y = m_pStencil->getDoubleFromDict( target,"y");
                pTarget = new KivioConnectorTarget(x,y);
                m_pStencil->m_pConnectorTargets->append( pTarget );
                m_pTargets->append(pTarget->duplicate());
           }
           m_defWidth  = m_pStencil->getDoubleFromDict( m_pStencil->vars, "w");
           m_defHeight = m_pStencil->getDoubleFromDict( m_pStencil->vars, "h");

        }

        else if( nodeName.compare("resize")==0 )
        {
            e = node.toElement();
            m_pStencil->resizeCode = node.toElement().text();
        }
        else
        {
           kdDebug(43000) << "KivioPyStencilSpawner::load() - Unknown node " << nodeName << " while loading " << file << endl;
        }

        node = node.nextSibling();
    }

    // Now load the xpm
    QFileInfo finfo(file);
    QString pixFile = finfo.dirPath(true) + "/" + finfo.baseName() + ".xpm";

    if(!m_icon.load( pixFile )) {
      QString pixFile = finfo.dirPath(true) + "/" + finfo.baseName() + ".png";
      m_icon.load( pixFile );
    }

    f.close();

    return true;
}



/**
 * Returns a new stencil, with default width/height of the spawner settings.
*/
KivioStencil *KivioPyStencilSpawner::newStencil()
{

    KivioStencil *pNewStencil = m_pStencil->duplicate();

    return pNewStencil;
}

#endif // HAVE_PYTHON
