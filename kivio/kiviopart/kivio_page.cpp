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
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include <qpainter.h>
#include <qdrawutil.h>
#include <qkeycode.h>
#include <QRegExp>
#include <QPoint>
#include <qcursor.h>
#include <qptrstack.h>
#include <qbuffer.h>
#include <qmessagebox.h>
#include <qclipboard.h>
#include <qpicture.h>
#include <qdom.h>
#include <qtextstream.h>
#include <qdragobject.h>
#include <qmime.h>
#include <qsortedlist.h>
#include <qvaluelist.h>

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kmessagebox.h>
#include <KoUnit.h>
#include <KoPoint.h>
#include <KoRect.h>
#include <KoZoomHandler.h>
#include <kapplication.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoDom.h>
#include <KoStore.h>
#include <KoOasisStyles.h>
#include <KoGenStyles.h>

#include "kivio_page.h"
#include "kivio_map.h"
#include "kivio_doc.h"
#include "kivio_canvas.h"
#include "kivio_view.h"
#include "kivio_config.h"

#include "kivio_common.h"
#include "kivio_connector_point.h"
#include "kivio_group_stencil.h"
#include "kivio_intra_stencil_data.h"
#include "kivio_layer.h"
#include "kivio_painter.h"
#include "kivio_stencil.h"
#include "kivio_1d_stencil.h"
#include "KIvioPageIface.h"
#include "kivio_command.h"
#include "kivioglobal.h"
#include "kiviodragobject.h"

#include <fixx11h.h> // for None

int KivioPage::s_id = 0L;
QIntDict<KivioPage>* KivioPage::s_mapPages;

KivioPage::KivioPage( KivioMap *_map, const QString &pageName, const char *_name )
: QObject( _map, _name ),
  m_pCurLayer(NULL)
{
  if ( s_mapPages == 0L )
    s_mapPages = new QIntDict<KivioPage>;
  m_id = s_id++;
  s_mapPages->insert( m_id, this );

  m_dcop = 0;

  m_pMap = _map;
  m_pDoc = _map->doc();

  // Make sure the layers auto-delete themselves
  m_pCurLayer = new KivioLayer(this);
  m_pCurLayer->setName(i18n("Layer 1"));
  m_lstLayers.append( m_pCurLayer );
  m_lstLayers.setAutoDelete(true);

  m_lstSelection.setAutoDelete(false);

  m_strName = pageName;

  setHidden(false);
  // Get a unique name so that we can offer scripting
  if ( !_name ) {
    QCString s;
    s.sprintf("Page%i", s_id );
    setName( s.data() );
  }

  m_pPageLayout = Kivio::Config::defaultPageLayout();
}

DCOPObject* KivioPage::dcopObject()
{
    if ( !m_dcop )
        m_dcop = new KIvioPageIface( this );
    return m_dcop;
}


KivioPage::~KivioPage()
{
  kDebug(43000)<<" KivioPage::~KivioPage()************ :"<<this<<endl;
  s_mapPages->remove(m_id);
  delete m_dcop;
}

KivioPage* KivioPage::find( int _id )
{
  if ( !s_mapPages )
    return 0L;

  return (*s_mapPages)[ _id ];
}

void KivioPage::print( QPainter &/*painter*/, KPrinter */*_printer*/ )
{
}

void KivioPage::printPage( QPainter &/*_painter*/, const QRect& /*page_range*/, const QRect& /*view*/ )
{
}

QDomElement KivioPage::save( QDomDocument& doc )
{
    // Write the name and 'hide' flag first as attributes
    QDomElement page = doc.createElement( "KivioPage" );
    page.setAttribute( "name", m_strName );
    page.setAttribute( "hide", (int)m_bPageHide );

    // Create a child element for the page layout
    QDomElement layoutE = saveLayout( doc );
    page.appendChild( layoutE );

    // Save guides
    QDomElement guidesElement = doc.createElement("GuidesLayout");
    page.appendChild(guidesElement);
    saveGuideLines(guidesElement);

    // Iterate through all layers saving them as child elements
    KivioLayer *pLayer = m_lstLayers.first();
    while( pLayer )
    {
        QDomElement layerE = pLayer->saveXML(doc);
        if( layerE.isNull() )
        {
	   kDebug(43000) << "KivioPage::save() - Oh shit.  KivioLayer::saveXML() returned a bad element!" << endl;
        }
	else
	{
	   page.appendChild( layerE );
	}

        pLayer = m_lstLayers.next();
    }

    return page;
}

void KivioPage::saveOasis(KoStore* /*store*/, KoXmlWriter* docWriter, KoGenStyles* styles)
{
  docWriter->startElement("draw:page");
  docWriter->addAttribute("draw:name", m_strName);

  if(m_pPageLayout == Kivio::Config::defaultPageLayout()) {
    docWriter->addAttribute("draw:master-page-name", "Standard");
  } else {
    KoGenStyle pageLayout = m_pPageLayout.saveOasis();
    QString layoutName = styles->lookup(pageLayout, "PL");

    KoGenStyle masterPage(KoGenStyle::STYLE_MASTER);
    masterPage.addAttribute("style:page-layout-name", layoutName);
    QString masterName = styles->lookup(masterPage, "MP");

    docWriter->addAttribute("draw:master-page-name", masterName);
  }

  // TODO OASIS: Save guidelines!

  QBuffer layerBuffer;
  layerBuffer.open(QIODevice::WriteOnly);
  KoXmlWriter layerWriter(&layerBuffer);
  layerWriter.startElement("draw:layer-set");

  // Iterate through all layers
  KivioLayer* layer = m_lstLayers.first();

  while(layer) {
    layer->saveOasis(&layerWriter);
    layer = m_lstLayers.next();
  }

  layerWriter.endElement(); // draw:layer-set
  QString layerSet = QString::fromUtf8(layerBuffer.buffer(), layerBuffer.buffer().size());
  KoGenStyle pageStyle(Kivio::STYLE_PAGE, "drawing-page");
  pageStyle.addChildElement("draw:layer-set", layerSet);
  QString styleName = styles->lookup(pageStyle, "PS");
  docWriter->addAttribute("draw:style-name", styleName);

  docWriter->endElement(); // draw:page
}

QDomElement KivioPage::saveLayout( QDomDocument &doc )
{
    QDomElement e = doc.createElement("PageLayout");
    Kivio::savePageLayout(e, m_pPageLayout);

    return e;
}

bool KivioPage::loadLayout( const QDomElement &e )
{
    m_pPageLayout = Kivio::loadPageLayout(e);
    return true;
}

bool KivioPage::isLoading()
{
  return m_pDoc->isLoading();
}

bool KivioPage::loadXML( const QDomElement& pageE )
{
    m_strName = pageE.attribute("name");
    if (m_strName.isEmpty())
        return false;


    m_bPageHide = (int)pageE.attribute("hide").toInt();

    // Clear the layer list
    KivioLayer *pLayer;
    m_lstLayers.clear();

    QDomNode node = pageE.firstChild();
    while( !node.isNull() )
    {
       if( node.nodeName() == "KivioLayer" )
       {
	  pLayer = new KivioLayer(this);
	  if( pLayer->loadXML( node.toElement() )==false )
	  {
	     delete pLayer;
	     pLayer = NULL;
	  }
	  else
	  {
	     m_lstLayers.append( pLayer );
	     pLayer = NULL;
	  }
       }
       else if( node.nodeName() == "PageLayout" )
       {
	  loadLayout( node.toElement() );
       }
       else if ( node.nodeName() == "GuidesLayout" ) {
	  loadGuideLines(node.toElement());
       }
       else
       {
	  kDebug(43000) << "KivioLayer::loadXML() - unknown node found, " <<  node.nodeName() << endl;
       }

       node = node.nextSibling();
    }

    m_pCurLayer = m_lstLayers.first();
    if( !m_pCurLayer )
    {
       kDebug(43000) << "KivioLayer::loadXML() - No layers loaded!! BIGGGGG PROBLEMS!" << endl;
    }

    // Now that we are done loading, fix all the connections
    KivioLayer *pLayerBak;

    pLayer = m_lstLayers.first();
    while( pLayer )
    {
       pLayerBak = pLayer;

       kDebug(43000) << "KivioLayer::loadXML() - loading layer connections" << endl;
       pLayer->searchForConnections(this);

       m_lstLayers.find( pLayerBak );

       pLayer = m_lstLayers.next();
    }

  return true;
}

bool KivioPage::loadOasis(const QDomElement& page, KoOasisStyles& oasisStyles)
{
  m_strName = page.attributeNS( KoXmlNS::draw, "name", QString::null);
  QDomElement* masterPage = oasisStyles.masterPages()[page.attributeNS( KoXmlNS::draw, "master-page-name", QString::null)];

  if(!masterPage) {
    kDebug(430000) << "Couldn't find the master page! " << page.attributeNS( KoXmlNS::draw, "master-page-name", QString::null) << endl;
    return false;
  }

  const QDomElement *pageLayout = oasisStyles.findStyle(masterPage->attributeNS( KoXmlNS::style, "page-layout-name", QString::null ) );

  if(!pageLayout) {
    kDebug(430000) << "Couldn't find the pagelayout!" << endl;
    return false;
  }

  m_pPageLayout.loadOasis(*pageLayout);

  if(m_pPageLayout.ptWidth <= 1e-13 || m_pPageLayout.ptHeight <= 1e-13) {
    kDebug(430000) << "Non valid pagelayout!" << endl;
    return false;
  }

  const QDomElement* style = oasisStyles.findStyle(page.attributeNS( KoXmlNS::draw, "style-name", QString::null) ); // Find the page style

  if(!style) {
    return false;
  }

  QDomNode styleNode = KoDom::namedItemNS( *style, KoXmlNS::style, "properties");
  styleNode = KoDom::namedItemNS( styleNode, KoXmlNS::draw, "layer-set");
  QDomNode currentNode = styleNode.firstChild();

  // Load the layers
  while(!currentNode.isNull()) {
    if(currentNode.nodeName() == "draw:layer") {
      KivioLayer* layer = new KivioLayer(this);
      layer->loadOasis(currentNode.toElement());
      m_lstLayers.append(layer);
    }

    currentNode = currentNode.nextSibling();
  }

  return true;
}

void KivioPage::update()
{
}

KivioPage* KivioPage::findPage( const QString& name )
{
  if (!m_pMap)
    return 0L;

  return m_pMap->findPage(name);
}

bool KivioPage::setPageName( const QString& name, bool init )
{
  if ( map()->findPage( name ) )
    return false;

  if ( m_strName == name )
    return true;

  QString old_name = m_strName;
  m_strName = name;

  if (init)
    return true;

  emit m_pDoc->sig_pageNameChanged(this, old_name);

  return true;
}

/**
 * Paints the page.
 *
 * An important note is that layers are drawn first to last.  So the last layer is the
 * most visible.
 */
void KivioPage::paintContent( KivioPainter& painter, const QRect& rect, bool transparent,
  QPoint p0, KoZoomHandler* zoom, bool drawConnectorTargets, bool drawSelection )
{
  KivioLayer *pLayer = m_lstLayers.first();
  while( pLayer )
  {
      if( pLayer->visible() )
      {
          pLayer->paintContent( painter, rect, transparent, p0, zoom );
      }

      pLayer = m_lstLayers.next();
  }

  // Now the second iteration - connection targets
  // Only draw targets if the zoom is higher than a certain value
  if(zoom->zoom() >= 50) {
    if(drawConnectorTargets) {
      m_pCurLayer->paintConnectorTargets( painter, rect, transparent, p0, zoom );
      pLayer = m_lstLayers.first();

      while(pLayer) {
        if(pLayer->connectable() && (pLayer != m_pCurLayer)) {
          pLayer->paintConnectorTargets( painter, rect, transparent, p0, zoom );
        }

        pLayer = m_lstLayers.next();
      }
    }
  }

  // Now the third iteration - selection handles
  if(drawSelection) {
    m_pCurLayer->paintSelectionHandles( painter, rect, transparent, p0, zoom );
  }
}

void KivioPage::printContent( KivioPainter& painter, int xdpi, int ydpi )
{
  if(!xdpi) {
    xdpi = KoGlobal::dpiX();
  }

  if(!ydpi) {
    ydpi = KoGlobal::dpiY();
  }

  KivioLayer *pLayer = m_lstLayers.first();

  while( pLayer )
  {
      if( pLayer->visible() )
      {
          pLayer->printContent( painter, xdpi, ydpi );
      }

      pLayer = m_lstLayers.next();
  }
}

void KivioPage::printContent(KivioPainter& painter, KoZoomHandler* zoomHandler)
{
  KivioLayer *pLayer = m_lstLayers.first();

  while( pLayer )
  {
    if(pLayer->visible())
    {
      pLayer->printContent(painter, zoomHandler);
    }

    pLayer = m_lstLayers.next();
  }
}

void KivioPage::printSelected( KivioPainter& painter, int xdpi, int ydpi )
{
  if(!xdpi) {
    xdpi = KoGlobal::dpiX();
  }

  if(!ydpi) {
    ydpi = KoGlobal::dpiY();
  }

  KivioStencil *pStencil;
  KivioIntraStencilData data;
  KoZoomHandler zoomHandler;
  zoomHandler.setZoomAndResolution(100, xdpi, ydpi);

  data.painter = &painter;
  data.zoomHandler = &zoomHandler;
  data.printing = true;

  KivioLayer *pLayer = m_lstLayers.first();

  while( pLayer )
  {
    if( pLayer->visible()==true )
    {
      pStencil = pLayer->firstStencil();

      while( pStencil )
      {
        if(pStencil->isSelected())
        {
          pStencil->paint(&data);
        }

        pStencil = pLayer->nextStencil();
      }
    }

    pLayer = m_lstLayers.next();
  }
}


bool KivioPage::addStencil( KivioStencil *pStencil )
{
  if(!pStencil) {
    kDebug(43000) << "KivioPage::addStencil() - Null stencil passed" << endl;
    return false;
  }

  if(!m_pCurLayer) {
    kDebug(43000) << "KivioPage::addStencil() - NULL current layer" << endl;
    return false;
  }

  KivioAddStencilCommand *cmd = new KivioAddStencilCommand(i18n("Add Stencil"), this, m_pCurLayer, pStencil );
  m_pDoc->addCommand(cmd);

  return m_pCurLayer->addStencil( pStencil );
}

void KivioPage::selectStencils( double x, double y, double w, double h )
{
  // Iterate through all stencils of this layer
  KivioStencil *pStencil = m_pCurLayer->stencilList()->first();

  while(pStencil) {
    // Is it in the rectangle?
    if(pStencil->isInRect(KoRect(x, y, w, h))) {
      selectStencil( pStencil );
    }

    pStencil = m_pCurLayer->stencilList()->next();
  }

  m_pDoc->slotSelectionChanged();
}

void KivioPage::selectStencil( KivioStencil *pStencil )
{
  if(!pStencil) {
    kDebug(43000) << "KivioPage::selectStencil - AHHHH! NULL STENCIL!" << endl;
    return;
  }

  // Don't allow reselection
  if(m_lstSelection.findRef(pStencil) != -1) {
    return;
  }

  kDebug(43000) <<"KivioPage::selectStencil - Selecting stencil" << endl;
  pStencil->select();
  m_lstSelection.append(pStencil);
  m_pDoc->slotSelectionChanged();
}

bool KivioPage::unselectStencil( KivioStencil *pStencil )
{
    pStencil->unselect();
    m_pDoc->slotSelectionChanged();
    return m_lstSelection.removeRef( pStencil );
}

void KivioPage::selectAllStencils()
{
  unselectAllStencils();
  KivioStencil* pStencil = m_pCurLayer->stencilList()->first();

  while(pStencil) {
    pStencil->select();
    m_lstSelection.append(pStencil);
    pStencil = m_pCurLayer->stencilList()->next();
  }


    /*
     * The following code is commented out because selections must
     * remain in the current layer.
    */
/*
    KivioLayer* pLayer = m_lstLayers.first();
    while( pLayer )
    {
        if( pLayer->visible() )
        {
            pStencil = pLayer->stencilList()->first();
            while( pStencil )
            {
                pStencil->select();
                m_lstSelection.append(pStencil);

                pStencil = pLayer->stencilList()->next();
            }
        }

        pLayer = m_lstLayers.next();
    }
*/
  m_pDoc->slotSelectionChanged();
}

void KivioPage::unselectAllStencils()
{
  KivioStencil* pStencil = m_lstSelection.first();

  while(pStencil)
  {
    pStencil->unselect();
    pStencil = m_lstSelection.next();
  }

  m_lstSelection.clear();
  m_pDoc->slotSelectionChanged();
}

bool KivioPage::isStencilSelected(KivioStencil *pStencil)
{
  return m_lstSelection.findRef( pStencil ) == -1;
}

/**
 * Looks for stencils at a given point in the canvas
 *
 * @param pPoint The point to search with
 *
 * This will iterate through all visible layers and check
 * for stencils.  The first one it finds will be returned.
 * If none are found, it will return NULL.
 */
KivioStencil *KivioPage::checkForStencil( KoPoint *pPoint, int *collisionType, double threshold, bool selectedOnly )
{
    KivioStencil *pStencil;
    int colType;


    /*
     * This code is commented out because selecting a stencil should only take place
     * on the current layer.  The following code searches all layers.
    */
    // Start with the last layer since it is the top
    /*
    KivioLayer* pLayer = m_lstLayers.last();
    while( pLayer )
    {
        pStencil = pLayer->checkForStencil( pPoint, &colType );
        if( pStencil )
        {
            *collisionType = colType;
            return pStencil;
        }

        pLayer = m_lstLayers.prev();
    }
    */

    pStencil = m_pCurLayer->checkForStencil( pPoint, &colType, threshold, selectedOnly );
    if( pStencil )
    {
        *collisionType = colType;
        return pStencil;
    }


    *collisionType = kctNone;
    return NULL;
}

void KivioPage::deleteSelectedStencils()
{
  // Make sure none of them have deletion protection
  KivioStencil* pStencil = m_lstSelection.first();

  while(pStencil) {
    if(pStencil->protection()->at(kpDeletion)) {
      KMessageBox::information(NULL, i18n("One of the selected stencils has protection from deletion and cannot be deleted."),
                                i18n("Protection From Deletion") );
      return;
    }

    pStencil = m_lstSelection.next();
  }

  // Iterate through all items in the selection list
  m_lstSelection.first();
  pStencil = m_lstSelection.take();
  KMacroCommand *macro = new KMacroCommand( i18n("Remove Stencil"));
  bool createMacro = false;

  while(pStencil) {
    KivioRemoveStencilCommand *cmd =new KivioRemoveStencilCommand(i18n("Remove Stencil"), this,  m_pCurLayer , pStencil );
    createMacro = true;
    macro->addCommand(cmd);

    if(pStencil->type() == kstConnector) {
      static_cast<Kivio1DStencil*>(pStencil)->disconnectFromTargets();
    }

    pStencil = m_lstSelection.take();
  }

  if (createMacro) {
    macro->execute();
    m_pDoc->addCommand( macro );
  } else {
    delete macro;
  }
}

void KivioPage::groupSelectedStencils()
{
  // Can't group 0 or 1 stencils
  if(!m_pCurLayer || (m_lstSelection.count() <= 1)) {
    return;
  }

  KivioGroupStencil* pGroup = new KivioGroupStencil();

  // Iterate through all items in the selection list, taking them from the layer, then adding
  // them to the group

  KivioStencil* pStencil = m_pCurLayer->firstStencil();
  KivioStencil* pTake = 0;

  while(pStencil) {
    if(pStencil->isSelected()) {
      // Take the stencil out of it's layer
      pTake = m_pCurLayer->takeStencil(pStencil);

      if(!pTake) {
        kDebug(43000) << "KivioPage::groupSelectedStencil() - Failed to take() one of the selected stencils. CRAP!" << endl;
      } else {
        // Add it to the group
        pGroup->addToGroup(pTake);
        pStencil = m_pCurLayer->currentStencil();
      }
    } else {
      pStencil = m_pCurLayer->nextStencil();
    }
  }

  // Unselect the old ones
  unselectAllStencils();

  // Add the group as the selected stencil
  m_pCurLayer->addStencil(pGroup);

  selectStencil(pGroup);

  KivioGroupCommand* cmd = new KivioGroupCommand(i18n("Group Selection"), this, m_pCurLayer, pGroup);
  doc()->addCommand(cmd);
}

void KivioPage::ungroupSelectedStencils()
{
  KivioStencil *pSelStencil, *pStencil;
  QPtrList<KivioStencil> *pList;
  QPtrList<KivioStencil> *pSelectThese = new QPtrList<KivioStencil>;
  KMacroCommand* macro = new KMacroCommand(i18n("Ungroup"));
  bool ungrouped = false;

  pSelectThese->setAutoDelete(false);

  // Iterate through all selected stencils
  pSelStencil = m_lstSelection.first();
  while( pSelStencil )
  {
    // If there is a list, it is a group stencil
    pList = pSelStencil->groupList();
    if(pList)
    {
      pList->first();
      pStencil = pList->first();

      while( pStencil )
      {
        m_pCurLayer->addStencil( pStencil );
        pSelectThese->append( pStencil );

        pStencil = pList->next();
      }

      // Remove the current stencil from the selection list(the group we just disassembled)
      m_lstSelection.take();

      // Remove it permanently from the layer
      if(!m_pCurLayer->takeStencil(pSelStencil))
      {
        kDebug(43000) << "KivioPage::ungroupSelectedStencil() - Failed to locate the group shell for deletion"
          << endl;
      }

      KivioUnGroupCommand* cmd = new KivioUnGroupCommand(i18n("Ungroup"), this, m_pCurLayer,
          static_cast<KivioGroupStencil*>(pSelStencil));
      macro->addCommand(cmd);
      ungrouped = true;
    }

    pSelStencil = m_lstSelection.next();
  }

  // Now iterate through the selectThese list and select
  // those stencils
  pStencil = pSelectThese->first();
  while( pStencil )
  {
    selectStencil( pStencil );

    if(pStencil->type() == kstConnector) {
      pStencil->searchForConnections(this, 4.0);
    }

    pStencil = pSelectThese->next();
  }

  delete pSelectThese;

  if(ungrouped) {
    doc()->addCommand(macro);
  } else {
    delete macro;
  }
}

void KivioPage::bringToFront()
{
    KivioStencil *pStencil, *pMove;
    KivioLayer *pLayer;

    QPtrList <KivioStencil> newList;

    pLayer = m_pCurLayer;

    newList.setAutoDelete(false);

    /*
     * We iterate through all stencils since order must be maintained
     * amongst the selection during the move.
     */
    pStencil = pLayer->firstStencil();
    while( pStencil )
    {
        if( isStencilSelected( pStencil )==true )
        {
            pMove = pLayer->takeStencil();
            if( pMove )
            {
                newList.append(pMove);
                pStencil = pLayer->currentStencil();
            }
            else  // In the case of error, the outside else won't execute
            {
                pStencil = pLayer->nextStencil();
            }
        }
        else
        {
            pStencil = pLayer->nextStencil();
        }
    }

    // push them back in, in reverse order
    pStencil = newList.last();
    while( pStencil )
    {
        pLayer->stencilList()->insert(0, pStencil);

        pStencil = newList.prev();
    }
}

void KivioPage::sendToBack()
{
    KivioStencil *pStencil, *pMove;
    KivioLayer *pLayer;

    QPtrList <KivioStencil> newList;

    pLayer = m_pCurLayer;

    newList.setAutoDelete(false);

    /*
     * We iterate through all stencils since order must be maintained
     * amongst the selection during the move.
     */
    pStencil = pLayer->firstStencil();
    while( pStencil )
    {
        if( isStencilSelected( pStencil )==true )
        {
            pMove = pLayer->takeStencil();
            if( pMove )
            {
                newList.append(pMove);
                pStencil = pLayer->currentStencil();
            }
            else  // In the case of error, the outside else won't execute
            {
                pStencil = pLayer->nextStencil();
            }
        }
        else
        {
            pStencil = pLayer->nextStencil();
        }
    }

    // push them back in, in reverse order
    pStencil = newList.first();
    while( pStencil )
    {
        pLayer->stencilList()->append(pStencil);

        pStencil = newList.next();
    }
}

void KivioPage::copy()
{
  if(m_lstSelection.count() <= 0) {
    return;
  }

  // push to clipbaord
  KivioDragObject* kdo = new KivioDragObject();
  kdo->setStencilList(m_lstSelection);
  kdo->setStencilRect(getRectForAllSelectedStencils());
  QApplication::clipboard()->setData(kdo, QClipboard::Clipboard);
}

void KivioPage::cut()
{
  KivioStencil *pStencil;
  KivioLayer *pLayer;
  bool safe=true;

  if( m_lstSelection.count() <= 0 )
      return;

  pLayer = m_pCurLayer;

  // Make sure none of them are protected from deletion
  pStencil = pLayer->firstStencil();
  while( pStencil )
  {
    if( isStencilSelected( pStencil )==true )
    {
      if( pStencil->protection()->at(kpDeletion)==true )
      {
	safe=false;
      }
    }

    pStencil = pLayer->nextStencil();
  }

  if( safe==false )
  {
    KMessageBox::information(NULL, i18n("One of the stencils has protection from deletion. You cannot cut or delete this stencil."), i18n("Protection From Delete") );

    return;
  }

  copy();
  deleteSelectedStencils();
}

void KivioPage::paste(KivioView* view)
{
  QPtrList<KivioStencil> list;
  list.setAutoDelete(false);
  KivioDragObject kdo;

  if(kdo.decode(QApplication::clipboard()->data(QClipboard::Clipboard), list, this)) {
    unselectAllStencils();
    KivioStencil* stencil = list.first();

    while(stencil) {
      addStencil(stencil);
      selectStencil(stencil);
      stencil = list.next();
    }

    view->canvasWidget()->startPasteMoving();
  }
}

int KivioPage::generateStencilIds(int next)
{
    KivioLayer *pLayer;

    pLayer = m_lstLayers.first();
    while( pLayer )
    {

        next = pLayer->generateStencilIds( next );

        pLayer = m_lstLayers.next();
    }

    return next;
}

KivioLayer *KivioPage::firstLayer()
{
    return m_lstLayers.first();
}

KivioLayer *KivioPage::nextLayer()
{
    return m_lstLayers.next();
}

KivioLayer *KivioPage::prevLayer()
{
    return m_lstLayers.prev();
}

KivioLayer *KivioPage::lastLayer()
{
    return m_lstLayers.last();
}

bool KivioPage::removeCurrentLayer()
{
    KivioLayer *pLayer;

    // NOOOOOOO!
    if( m_lstLayers.count() <= 1 )
        return false;

    pLayer = m_lstLayers.first();

    if( pLayer != m_pCurLayer )
    {
        if( m_lstLayers.find( m_pCurLayer )==false )
        {
	   kDebug(43000) << "KivioLayer::removeCurrentLayer() - Couldn't find current layer in the list. Bad!" << endl;
            return false;
        }
    }

    pLayer = m_lstLayers.next();
    if( !pLayer )
    {
        (void)m_lstLayers.last();
        pLayer = m_lstLayers.prev();
    }

    if( !pLayer )
    {
       kDebug(43000) << "KivioLayer::removeCurrentLayer() - Couldn't find a next layer." << endl;
        return false;
    }

    KivioRemoveLayerCommand * cmd = new KivioRemoveLayerCommand( i18n("Remove Layer"), this , m_pCurLayer , m_lstLayers.findRef(m_pCurLayer) );
    doc()->addCommand( cmd );
    takeLayer( m_pCurLayer );
/*
    if( m_lstLayers.remove( m_pCurLayer )==false )
    {
       kDebug(43000) << "KivioLayer::removeCurrentLayer() - Couldn't find current layer in the list. Bad!" << endl;
        return false;
    }
*/
    m_pCurLayer = pLayer;


    return true;
}

void KivioPage::takeLayer( KivioLayer *pLayer )
{
    int pos=m_lstLayers.findRef(pLayer);
    m_lstLayers.take( pos );
}

void KivioPage::addLayer( KivioLayer *pLayer )
{
    m_lstLayers.append( pLayer );
}

void KivioPage::insertLayer( int position, KivioLayer *pLayer )
{
    m_lstLayers.insert( position, pLayer );
}

KivioLayer *KivioPage::layerAt( int pos )
{
    return m_lstLayers.at(pos);
}

void KivioPage::alignStencils(AlignData d)
{
    KivioStencil* pStencil = m_lstSelection.first();

    if(!pStencil)
        return;

    if (d.v != AlignData::None || d.h != AlignData::None) {
        KMacroCommand *macro = new KMacroCommand(i18n("Move Stencil"));
        double x = pStencil->x();
        double y = pStencil->y();
        double w = pStencil->w();
        double h = pStencil->h();

        while( pStencil )
        {
            KoRect oldRect = pStencil->rect();

            switch (d.v) {
                case AlignData::Top:
                  pStencil->setY(y);
                  break;
                case AlignData::Center:
                  pStencil->setY(y+h/2-pStencil->h()/2);
                  break;
                case AlignData::Bottom:
                  pStencil->setY(y+h-pStencil->h());
                  break;
                default:
                  break;
            }
            switch (d.h) {
                case AlignData::Left:
                  pStencil->setX(x);
                  break;
                case AlignData::Center:
                  pStencil->setX(x+w/2-pStencil->w()/2);
                  break;
                case AlignData::Right:
                  pStencil->setX(x+w-pStencil->w());
                  break;
                default:
                  break;
            }

            KivioMoveStencilCommand * cmd = new KivioMoveStencilCommand( i18n("Move Stencil"),
              pStencil, oldRect, pStencil->rect(), this);
            macro->addCommand(cmd);
            pStencil = m_lstSelection.next();
        }

        m_pDoc->addCommand(macro);
    }

    if (d.centerOfPage) {
        KMacroCommand *macro = new KMacroCommand(i18n("Move Stencil"));
        double w = m_pPageLayout.ptWidth;
        double h = m_pPageLayout.ptHeight;
        KoRect r = getRectForAllSelectedStencils();
        double dx = ((w - r.width()) / 2.0) - r.x();
        double dy = ((h - r.height()) / 2.0) - r.y();
        pStencil = m_lstSelection.first();

        while( pStencil )
        {
            KoRect oldRect = pStencil->rect();
            pStencil->setPosition(pStencil->x() + dx, pStencil->y() + dy);
            KivioMoveStencilCommand * cmd = new KivioMoveStencilCommand( i18n("Move Stencil"),
              pStencil, oldRect, pStencil->rect(), this);
            macro->addCommand(cmd);
            pStencil = m_lstSelection.next();
        }

        m_pDoc->addCommand(macro);
    }
}

class XYSortedStencilList : public QPtrList<KivioStencil>
{
public:
  XYSortedStencilList(bool sortX) :xsort(sortX) {};

protected:
  int compareItems(QPtrCollection::Item i1, QPtrCollection::Item i2)
  {
    KivioStencil* s1 = (KivioStencil*)i1;
    KivioStencil* s2 = (KivioStencil*)i2;
    if (xsort) {
      if (s1->x() > s2->x())
        return 1;
      if (s1->x() < s2->x())
        return -1;
      return 0;
    }
    if (s1->y() > s2->y())
      return 1;
    if (s1->y() < s2->y())
      return -1;
    return 0;
  }

private:
  bool xsort;
};


void KivioPage::distributeStencils(DistributeData d)
{
  double x  = 0.0;
  double y  = 0.0;
  double x1 = 0.0;
  double y1 = 0.0;

  KivioStencil* pStencil = m_lstSelection.first();
  if (!pStencil)
    return;

  XYSortedStencilList xSortList(true);
  XYSortedStencilList ySortList(false);
  QValueList<KoRect> oldRects;

  while( pStencil )
  {
      xSortList.append(pStencil);
      ySortList.append(pStencil);
      oldRects.append(pStencil->rect());
      pStencil = m_lstSelection.next();
  }
  xSortList.sort();
  ySortList.sort();

  switch (d.extent) {
    case DistributeData::Page:
      x = m_pPageLayout.ptLeft;
      y = m_pPageLayout.ptTop;
      x1 = m_pPageLayout.ptWidth - m_pPageLayout.ptRight;
      y1 = m_pPageLayout.ptHeight - m_pPageLayout.ptBottom;
      break;
    case DistributeData::Selection:
      pStencil = m_lstSelection.first();
      x = pStencil->x();
      y = pStencil->x();
      x1 = x + pStencil->w();
      y1 = y + pStencil->h();
      while( pStencil )
      {
          x = qMin(x,pStencil->x());
          y = qMin(y,pStencil->y());
          x1 = qMax(x1,pStencil->x() + pStencil->w());
          y1 = qMax(y1,pStencil->y() + pStencil->h());
          pStencil = m_lstSelection.next();
      }
      break;
    default:
      break;
  }

  /*****************************************************/
  KivioStencil* firstx = xSortList.first();
  KivioStencil* lastx = xSortList.last();
  double countx = (double)(xSortList.count()-1);
  double distx = 0.0;
  switch (d.h) {
    case DistributeData::Left:
      x1 = x1 - lastx->w();
      distx = (x1 - x)/countx;
      break;
    case DistributeData::Center:
      x = x + firstx->w()/2;
      x1 = x1 - lastx->w()/2;
      distx = (x1 - x)/countx;
      break;
    case DistributeData::Spacing: {
      double allw = 0.0;
      pStencil = xSortList.first();
      while( pStencil )
      {
          allw = allw + pStencil->w();
          pStencil = xSortList.next();
      }
      distx = (x1-x-allw)/countx;
      break; }
    case DistributeData::Right:
      x = x + firstx->w();
      distx = (x1 - x)/countx;
      break;
    default:
      break;
  }

  double xx = x;
  switch (d.h) {
    case DistributeData::Center:
      pStencil = xSortList.first();
      while( pStencil )
      {
          pStencil->setX(xx - pStencil->w()/2);
          xx = xx + distx;
          pStencil = xSortList.next();
      }
      break;
    case DistributeData::Right:
      pStencil = xSortList.first();
      while( pStencil )
      {
          pStencil->setX(xx - pStencil->w());
          xx = xx + distx;
          pStencil = xSortList.next();
      }
      break;
    case DistributeData::Left:
      pStencil = xSortList.first();
      while( pStencil )
      {
          pStencil->setX(xx);
          xx = xx + distx;
          pStencil = xSortList.next();
      }
      break;
    case DistributeData::Spacing:
      pStencil = xSortList.first();
      while( pStencil )
      {
          pStencil->setX(xx);
          xx = xx + pStencil->w() + distx;
          pStencil = xSortList.next();
      }
      break;
    default:
      break;
  }
  /*****************************************************/
  KivioStencil* firsty = ySortList.first();
  KivioStencil* lasty = ySortList.last();
  double county = (double)(ySortList.count()-1);
  double disty = 0.0;
  switch (d.v) {
    case DistributeData::Top:
      y1 = y1 - lasty->h();
      disty = (y1 - y)/county;
      break;
    case DistributeData::Center:
      y = y + firsty->h()/2;
      y1 = y1 - lasty->h()/2;
      disty = (y1 - y)/countx;
      break;
    case DistributeData::Spacing: {
      double allh = 0.0;
      pStencil = ySortList.first();
      while( pStencil )
      {
          allh = allh + pStencil->h();
          pStencil = ySortList.next();
      }
      disty = (y1-y-allh)/county;
      break; }
    case DistributeData::Bottom:
      y = y + firsty->h();
      disty = (y1 - y)/county;
      break;
    default:
      break;
  }

  double yy = y;
  switch (d.v) {
    case DistributeData::Center:
      pStencil = ySortList.first();
      while( pStencil )
      {
          pStencil->setY(yy - pStencil->h()/2);
          yy = yy + disty;
          pStencil = ySortList.next();
      }
      break;
    case DistributeData::Bottom:
      pStencil = ySortList.first();
      while( pStencil )
      {
          pStencil->setY(yy - pStencil->h());
          yy = yy + disty;
          pStencil = ySortList.next();
      }
      break;
    case DistributeData::Top:
      pStencil = ySortList.first();
      while( pStencil )
      {
          pStencil->setY(yy);
          yy = yy + disty;
          pStencil = ySortList.next();
      }
      break;
    case DistributeData::Spacing:
      pStencil = ySortList.first();
      while( pStencil )
      {
          pStencil->setY(yy);
          yy = yy + pStencil->h() + disty;
          pStencil = ySortList.next();
      }
      break;
    default:
      break;
  }

  if(d.v != DistributeData::None || d.h != DistributeData::None) {
    KMacroCommand *macro = new KMacroCommand(i18n("Move Stencil"));
    QValueListIterator<KoRect> it;
    pStencil = m_lstSelection.first();

    for(it = oldRects.begin(); it != oldRects.end(); ++it) {
      KivioMoveStencilCommand * cmd = new KivioMoveStencilCommand(i18n("Move Stencil"),
        pStencil, (*it), pStencil->rect(), this);
      macro->addCommand( cmd);
      pStencil = m_lstSelection.next();
    }

    m_pDoc->addCommand( macro );
  }
}


/**
 * Returns a rectangle representing the bounds of all the selected stencils (in points).
 */
KoRect KivioPage::getRectForAllSelectedStencils()
{
    KoRect rTotal, r;

    KivioStencil *pStencil = m_lstSelection.first();

    // Get the rect of the first selected stencil
    if( pStencil )
    {
        rTotal = pStencil->rect();

        pStencil = m_lstSelection.next();
    }

    // iterate through all the stencils uniting the rectangles
    while( pStencil )
    {
        r = pStencil->rect();

        rTotal = rTotal.unite( r );

        pStencil = m_lstSelection.next();
    }

    return rTotal;
}

/**
 * Returns a rectangle representing the bounds of all the selected stencils (in points).
 */
KoRect KivioPage::getRectForAllStencils()
{
    KoRect rTotal, r;

    bool firstTime = true;

    KivioLayer *pLayer;
    KivioStencil *pStencil;

    pLayer = m_lstLayers.first();
    while( pLayer )
    {
        pStencil = pLayer->firstStencil();
        while( pStencil )
        {
            if( firstTime==true )
            {
                rTotal = pStencil->rect();
                firstTime = false;
            }
            else
            {
                r = pStencil->rect();
                rTotal = rTotal.unite( r );
            }

            pStencil = pLayer->nextStencil();
        }

        pLayer = m_lstLayers.next();
    }


    return rTotal;
}

void KivioPage::setPaperLayout(const KoPageLayout &l)
{
  m_pPageLayout = l;
  doc()->updateView(this);
  emit sig_pageLayoutChanged(m_pPageLayout);
}

KivioConnectorTarget *KivioPage::connectPointToTarget( KivioConnectorPoint *p, double /*thresh*/)
{
   double oldX, oldY;
   KivioLayer *pLayer, *pCurLayer;
   bool doneSearching = false;
   KivioConnectorTarget *pTarget;

   if( !p )
      return NULL;

   if( p->connectable()==false )
      return NULL;

   oldX = p->x();
   oldY = p->y();

   pCurLayer = curLayer();
   pLayer = firstLayer();

   while( pLayer && doneSearching==false )
   {
      if( pLayer != pCurLayer )
      {
	 if( pLayer->connectable()==false || pLayer->visible()==false )
	 {
	    pLayer = nextLayer();
	    continue;
	 }
      }

      if( (pTarget=pLayer->connectPointToTarget(p, 8.0f )) )
      {
	 return pTarget;
      }

      pLayer = nextLayer();
   }

   return NULL;
}

KoPoint KivioPage::snapToTarget( const KoPoint& p, double thresh, bool& hit )
{
   KivioLayer *pLayer, *pCurLayer;
   KoPoint retVal = p;

   pCurLayer = curLayer();
   pLayer = firstLayer();

   while( pLayer && !hit )
   {
      if( pLayer != pCurLayer )
      {
          if( pLayer->connectable()==false || pLayer->visible()==false )
          {
            pLayer = nextLayer();
            continue;
          }
      }

      retVal = pLayer->snapToTarget(p, thresh, hit);

      pLayer = nextLayer();
   }

   return retVal;
}

void KivioPage::setHidePage(bool _hide)
{
    setHidden(_hide);
    if(_hide)
        emit sig_PageHidden(this);
    else
        emit sig_PageShown(this);
}

void KivioPage::setPaintSelected(bool paint)
{
  KivioStencil *pStencil = m_lstSelection.first();

  while( pStencil )
  {
    pStencil->setHidden(!paint);
    pStencil = m_lstSelection.next();
  }
}

bool KivioPage::checkForStencilTypeInSelection(KivioStencilType type)
{
  KivioStencil *pStencil = m_lstSelection.first();

  while( pStencil )
  {
    if(pStencil->type() == type) {
      return true;
    }

    pStencil = m_lstSelection.next();
  }

  return false;
}

bool KivioPage::checkForTextBoxesInSelection()
{
  KivioStencil *pStencil = m_lstSelection.first();

  while(pStencil) {
    if(pStencil->hasTextBox()) {
      return true;
    }

    pStencil = m_lstSelection.next();
  }

  return false;
}

void KivioPage::setGuideLines(const QValueList<double> hGuideLines, const QValueList<double> vGuideLines)
{
  m_hGuideLines = hGuideLines;
  m_vGuideLines = vGuideLines;
}

void KivioPage::saveGuideLines(QDomElement& element)
{
  QValueList<double>::iterator it;
  QValueList<double>::iterator itEnd = m_hGuideLines.end();

  for(it = m_hGuideLines.begin(); it != itEnd; ++it) {
    QDomElement e = element.ownerDocument().createElement("Guideline");
    element.appendChild(e);
    XmlWriteDouble(e, "pos", *it);
    XmlWriteInt(e, "orient", (int)Qt::Horizontal);
  }

  itEnd = m_vGuideLines.end();

  for(it = m_vGuideLines.begin(); it != itEnd; ++it) {
    QDomElement e = element.ownerDocument().createElement("Guideline");
    element.appendChild(e);
    XmlWriteDouble(e, "pos", *it);
    XmlWriteInt(e, "orient", (int)Qt::Vertical);
  }
}

void KivioPage::loadGuideLines(const QDomElement& element)
{
  m_hGuideLines.clear();
  m_vGuideLines.clear();

  QDomElement e = element.firstChild().toElement();

  for( ; !e.isNull(); e = e.nextSibling().toElement() )
  {
    double pos = XmlReadDouble(e, "pos", 0.0);
    Qt::Orientation orient = (Qt::Orientation)XmlReadInt(e, "orient", 0);
    addGuideLine(orient, pos);
  }
}

void KivioPage::addGuideLine(Qt::Orientation orientation, double position)
{
  if(orientation == Qt::Horizontal) {
    m_hGuideLines.append(position);
  } else {
    m_vGuideLines.append(position);
  }
}

#include "kivio_page.moc"
