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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "kivio_common.h"
#include "kivio_connector_point.h"
#include "kivio_connector_target.h"
#include "kivio_group_stencil.h"
#include "kivio_intra_stencil_data.h"
#include "kivio_layer.h"
#include "kivio_painter.h"
#include "kivio_stencil.h"
#include "kivio_stencil_spawner.h"
#include "kivio_stencil_spawner_info.h"
#include "kivio_stencil_spawner_set.h"
#include "KIvioLayerIface.h"
#include "kivio_doc.h"
#include "kivio_page.h"
#include "object.h"

#include <qdom.h>

#include <klocale.h>
#include <kdebug.h>
#include <kozoomhandler.h>
#include <koPoint.h>
#include <koStore.h>
#include <koxmlwriter.h>

KivioLayer::KivioLayer( KivioPage *pPage )
{
    m_pPage = pPage;
    m_name = i18n("Untitled Layer");

    m_flags = 0;
    m_dcop = 0;
    setVisible(true);
    setConnectable(false);
}

DCOPObject* KivioLayer::dcopObject()
{
    if ( !m_dcop )
        m_dcop = new KIvioLayerIface( this );
    return m_dcop;
}

KivioLayer::~KivioLayer()
{
  QValueList<Kivio::Object*>::iterator it = m_objectList.begin();
  QValueList<Kivio::Object*>::iterator itEnd = m_objectList.end();
  Kivio::Object* object = 0;

  while(it != itEnd) {
    object = *it;
    it = m_objectList.remove(it);
    delete object;
  }

  delete m_dcop;
  m_dcop = 0;
}

bool KivioLayer::addStencil( Kivio::Object *pStencil )
{
  m_objectList.append(pStencil);

  return true;
}

bool KivioLayer::removeStencil( Kivio::Object *pStencil )
{
  QValueList<Kivio::Object*>::iterator it = m_objectList.find(pStencil);
  bool ok = (it != m_objectList.end());

  if(ok) {
    m_objectList.remove(it);
    delete pStencil;
    pStencil = 0;
  }

  return ok;
}


/**
 * Returns a new stencil of the type described by stencilE
 *
 * @param stencilE The XML element to load from
 *
 * This will search through all Kivio::ObjectSpawnerSets and attempt to
 * locate the stencil described by stencilE.  If it finds it, it allocates
 * a new copy of it, loads the data from stencilE into it, and returns
 * it.
 *
 * @returns A new, loaded stencil.
 */
Kivio::Object *KivioLayer::loadSMLStencil( const QDomElement &stencilE )
{
  //FIXME Port to Object code
//     QString setId, _id;
// 
//     kdDebug(43000) << "KivioLayer::loadSMLStencil() " << setId << " " << _id << endl;
// 
//     setId = XmlReadString( stencilE, "setId", "" );
//     _id = XmlReadString( stencilE, "id", "" );
// 
// 
//     if( setId.length() == 0 ||
//         _id.length() == 0 )
//     {
//         return NULL;
//     }
// 
// 
//     // Locate the spawner set
//     Kivio::ObjectSpawner *pSpawner = m_pPage->doc()->findStencilSpawner(setId,_id);
//     if( pSpawner )
//     {
//         Kivio::Object *pStencil = pSpawner->newStencil();
//         pStencil->loadXML( stencilE );
// 
//         return pStencil;
//     }

    return NULL;
}

/**
 * Returns a new stencil of the type described by stencilE
 *
 * @param stencilE The XML element to load from
 *
 * This will search through all Kivio::ObjectSpawnerSets and attempt to
 * locate the stencil described by stencilE.  If it finds it, it allocates
 * a new copy of it, loads the data from stencilE into it, and returns
 * it.
 *
 * @returns A new, loaded stencil.
 */
Kivio::Object *KivioLayer::loadGroupStencil( const QDomElement &stencilE )
{
  //FIXME Port to Object code
/*   kdDebug(43000) << "KivioLayer::loadGroupStencil()" << endl;

    KivioGroupStencil *pStencil = new KivioGroupStencil();

    if(pStencil->loadXML( stencilE, this )==false)
    {
        delete pStencil;
        return NULL;
    }

    return pStencil;*/

  return 0;
}

Kivio::Object *KivioLayer::loadPluginStencil( const QDomElement &stencilE )
{
  //FIXME Port to Object code
//     QString setId, _id;
// 
//     kdDebug(43000) << "KivioLayer::loadPluginStencil() " << setId.ascii() << " / " << _id << endl;
// 
// 
//     setId = XmlReadString( stencilE, "setId", "" );
//     _id = XmlReadString( stencilE, "id", "" );
// 
// 
//     if( setId.length() == 0 ||
//         _id.length() == 0 )
//         return NULL;
// 
// 
//     // Locate the spawner set
//     Kivio::ObjectSpawner *pSpawner = m_pPage->doc()->findStencilSpawner(setId, _id);
//     if( pSpawner )
//     {
//         Kivio::Object *pStencil = pSpawner->newStencil();
//         pStencil->loadXML( stencilE );
// 
//         return pStencil;
//     }

    return NULL;
}

bool KivioLayer::loadXML( const QDomElement &layerE )
{
  //FIXME Port to Object code
//     m_flags = XmlReadInt( layerE, "flags", 1 );
//     m_name = XmlReadString( layerE, "name", "layerX" );
// 
//     QDomNode node;
//     node = layerE.firstChild();
//     while( !node.isNull() )
//     {
//         QString name = node.nodeName();
//         if( name == "KivioSMLStencil" || name == "KivioPyStencil" )
//         {
//             Kivio::Object *pStencil = loadSMLStencil( node.toElement() );
//             if( pStencil )
//             {
//                 pStencil->updateGeometry();
//                 m_pStencilList->append( pStencil );
//             }
//             else
//             {
// 	       kdWarning(43000) << "KivioLayer::loadXML() - Unknown KivioSMLStencil (id=" <<
// 		  XmlReadString( node.toElement(), "id", "" ) << " set=" <<
// 		  XmlReadString( node.toElement(), "setId", "" ) << ") found." << endl;
//             }
//         }
//         else if( name == "KivioGroupStencil" )
//         {
//             Kivio::Object *pStencil = loadGroupStencil( node.toElement() );
//             if( pStencil )
//             {
//                 m_pStencilList->append(pStencil);
//             }
//             else
//             {
// 	       kdWarning(43000) << "KivioLayer::loadXML() - Unable to load KivioGroupStencil" << endl;
//             }
//         }
//         else if( name == "KivioPluginStencil" )
//         {
//             Kivio::Object *pStencil = loadPluginStencil( node.toElement() );
//             if( pStencil )
//             {
//                 m_pStencilList->append(pStencil);
//             }
//             else
//             {
// 	       kdWarning(43000) << "KivioLayer - Unable to load KivioPluginStencil" << endl;
// 	       kdWarning(43000) << "KivioLayer::loadXML() - Unable to load KivioPluginStencil (id=" <<
// 		  XmlReadString( node.toElement(), "id", "" ) << " set=" <<
// 		  XmlReadString( node.toElement(), "setId", "" ) << ") found." << endl;
//             }
//         }
// 
//         node = node.nextSibling();
//     }

    return true;
}

void KivioLayer::loadOasis(const QDomElement& layer)
{
  m_name = layer.attribute("draw:name");
  // TODO OASIS Load flags
}


QDomElement KivioLayer::saveXML( QDomDocument &doc )
{
    QDomElement e = doc.createElement("KivioLayer");

    //FIXME Port to Object code
//     XmlWriteInt( e, "flags", m_flags );
//     XmlWriteString( e, "name", m_name );
// 
//     Kivio::Object *pStencil = m_pStencilList->first();
//     while( pStencil )
//     {
//         e.appendChild( pStencil->saveXML( doc ) );
// 
//         pStencil = m_pStencilList->next();
//     }

    return e;
}

void KivioLayer::saveOasis(KoXmlWriter* layerWriter)
{
  layerWriter->startElement("draw:layer");
  layerWriter->addAttribute("draw:name", m_name);
  // TODO OASIS Save flags
  layerWriter->endElement(); // draw:layer
}

void KivioLayer::paintContent( QPainter& painter, const QRect&, bool, QPoint,
  KoZoomHandler* zoom )
{
  QValueList<Kivio::Object*>::iterator itEnd = m_objectList.end();

  for(QValueList<Kivio::Object*>::iterator it = m_objectList.begin(); it != itEnd; ++it) {
    (*it)->paint(painter, zoom);
  }
}

void KivioLayer::printContent( QPainter& painter, int xdpi, int ydpi )
{
  if(!xdpi) {
    xdpi = KoGlobal::dpiX();
  }

  if(!ydpi) {
    ydpi = KoGlobal::dpiY();
  }

  KoZoomHandler zoomHandler;
  zoomHandler.setZoomAndResolution(100, xdpi, ydpi);
  QValueList<Kivio::Object*>::iterator itEnd = m_objectList.end();

  for(QValueList<Kivio::Object*>::iterator it = m_objectList.begin(); it != itEnd; ++it) {
    (*it)->paint(painter, &zoomHandler);
  }
}

void KivioLayer::paintConnectorTargets( QPainter& painter, const QRect&, bool, QPoint,
  KoZoomHandler* zoom )
{
  // FIXME this needs to be implemented differently due to how drawing is handled in Qt4
/*    KivioIntraStencilData data;

    painter.setFGColor( QColor(0,0,0) );

    data.painter = &painter;
    data.zoomHandler = zoom;

    Kivio::Object *pStencil = m_pStencilList->first();
    while( pStencil )
    {
        if(!pStencil->hidden()) {
          pStencil->paintConnectorTargets( &data );
        }

        pStencil = m_pStencilList->next();
    }*/
}

void KivioLayer::paintSelectionHandles( QPainter& painter, const QRect&, bool, QPoint, KoZoomHandler* zoom )
{
  // FIXME this needs to be implemented differently due to how drawing is handled in Qt4
/*    KivioIntraStencilData data;

    painter.setFGColor( QColor(0,0,0) );

    data.painter = &painter;
    data.zoomHandler = zoom;

    Kivio::Object *pStencil = m_pStencilList->first();
    while( pStencil )
    {
        if( pStencil->isSelected() && !pStencil->hidden() )
            pStencil->paintSelectionHandles( &data );

        pStencil = m_pStencilList->next();
    }*/
}

Kivio::Object *KivioLayer::checkForStencil( KoPoint *pPoint, int *collisionType, float threshold, bool selectedOnly )
{
  //FIXME Port to Object code
//     Kivio::Object *pStencil;
//     int colType;
// 
//     pStencil = m_pStencilList->last();
//     while( pStencil )
//     {
//       // If we are only supposed to check the selected stencils, then only do that.  Otherwise
//       // check them all.
//       if( (selectedOnly==true && pStencil->isSelected()==true) ||
//         (selectedOnly==false) )
//       {
//         if( (colType = pStencil->checkForCollision( pPoint, threshold )) != kctNone )
//         {
//           // Assign the collision type and return
//           *collisionType = colType;
//           return pStencil;
//         }
//       }
// 
//       pStencil = m_pStencilList->prev();
//     }
// 
//     *collisionType = kctNone;

    return NULL;
}

void KivioLayer::setVisible( bool f )
{
    if( f==true )
    {
        m_flags = m_flags | FLOW_LAYER_VISIBLE;
    }
    else
    {
        m_flags = m_flags & (~FLOW_LAYER_VISIBLE);
    }
}

void KivioLayer::setConnectable( bool f )
{
    if( f==true )
    {
        m_flags = m_flags | FLOW_LAYER_CONNECTABLE;
    }
    else
    {
        m_flags = m_flags & (~FLOW_LAYER_CONNECTABLE);
    }
}

int KivioLayer::generateStencilIds( int next )
{
  //FIXME Port to Object code
//     Kivio::Object *pStencil;
// 
//     pStencil = m_pStencilList->first();
//     while( pStencil )
//     {
//         next = pStencil->generateIds( next );
// 
//         pStencil = m_pStencilList->next();
//     }

    return next;
}

void KivioLayer::searchForConnections( KivioPage *p )
{
  //FIXME Port to Object code
/*    Kivio::Object *pStencil;
    Kivio::Object *pCur;

    pStencil = m_pStencilList->first();
    while( pStencil )
    {
        // Backup the current list position
        pCur = pStencil;

        pStencil->searchForConnections( p );

        // Restore it
        m_pStencilList->find( pCur );

        pStencil = m_pStencilList->next();
    }*/
}

Kivio::Object *KivioLayer::takeStencil( Kivio::Object *p )
{
  QValueList<Kivio::Object*>::iterator it = m_objectList.find(p);
  m_objectList.remove(it);

  return p;
}


/**
 * Attempts to connect a KivioConnectorPoint to a KivioConnectorTarget of each stencil.
 *
 * @param p The point to attempt the connection with
 * @param thresh The threshold to use
 * @returns The KivioConnectorTarget the point connected to
 */
KivioConnectorTarget *KivioLayer::connectPointToTarget( KivioConnectorPoint *p, float thresh )
{
  //FIXME Port to Object code
//     KivioConnectorTarget *pTarget;
// 
//     Kivio::Object *pStencil = m_pStencilList->last();
//     while( pStencil )
//     {
//         // Don't allow the connector point to connect to the stencil that owns it
//         if( pStencil != p->stencil() )
//         {
//             pTarget = pStencil->connectToTarget(p, thresh);
//             if( pTarget )
//             {
//                 return pTarget;
//             }
//         }
// 
//         pStencil = m_pStencilList->prev();
//     }

    return NULL;
}

KoPoint KivioLayer::snapToTarget( const KoPoint& p, double thresh, bool& hit )
{
    KoPoint retVal = p;
    //FIXME Port to Object code
//     Kivio::Object *pStencil = m_pStencilList->last();
// 
//     while( pStencil && !hit)
//     {
//         retVal = pStencil->snapToTarget(p, thresh, hit);
//         pStencil = m_pStencilList->prev();
//     }

    return retVal;
}
