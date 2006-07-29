#include "kivio_base_target_stencil.h"
#include "kivio_common.h"
#include "kivio_connector_point.h"
#include "kivio_connector_target.h"
#include "kivio_fill_style.h"
#include "kivio_intra_stencil_data.h"
#include "kivio_line_style.h"
#include "kivio_painter.h"
#include "kivio_stencil_spawner.h"
#include "kivio_stencil_spawner_info.h"
#include "kivio_stencil_spawner_set.h"
#include "kivio_text_style.h"

#include <KoZoomHandler.h>

KivioBaseTargetStencil::KivioBaseTargetStencil()
    : KivioStencil()
{
    m_pFillStyle = new KivioFillStyle();
    m_pLineStyle = new KivioLineStyle();
    m_pTextStyle = new KivioTextStyle();

    m_pTargets = new QPtrList<KivioConnectorTarget>;
    m_pTargets->setAutoDelete(true);
}

KivioBaseTargetStencil::~KivioBaseTargetStencil()
{
    delete m_pFillStyle;
    delete m_pLineStyle;
    delete m_pTextStyle;
    delete m_pTargets;
}




//////////////////////////////////////////////////////////////////////////////
//
// KivioLineStyle
//
//////////////////////////////////////////////////////////////////////////////
QColor KivioBaseTargetStencil::fgColor()
{
    return m_pLineStyle->color();
}

void KivioBaseTargetStencil::setFGColor( QColor c )
{
    m_pLineStyle->setColor(c);
}

void KivioBaseTargetStencil::setLineWidth( double l )
{
    m_pLineStyle->setWidth(l);
}

double KivioBaseTargetStencil::lineWidth()
{
    return m_pLineStyle->width();
}



//////////////////////////////////////////////////////////////////////////////
//
// KivioFillStyle
//
//////////////////////////////////////////////////////////////////////////////
void KivioBaseTargetStencil::setBGColor( QColor c )
{
    m_pFillStyle->setColor(c);
}

QColor KivioBaseTargetStencil::bgColor()
{
    return m_pFillStyle->color();
}

KivioFillStyle *KivioBaseTargetStencil::fillStyle()
{
    return m_pFillStyle;
}






//////////////////////////////////////////////////////////////////////////////
//
// KivioTextStyle
//
//////////////////////////////////////////////////////////////////////////////
QColor KivioBaseTargetStencil::textColor()
{
    return m_pTextStyle->color();
}
void KivioBaseTargetStencil::setTextColor( QColor c )
{
    m_pTextStyle->setColor(c);
}

QFont KivioBaseTargetStencil::textFont()
{
    return m_pTextStyle->font();
}
void KivioBaseTargetStencil::setTextFont( const QFont &f )
{
    m_pTextStyle->setFont(f);
}

int KivioBaseTargetStencil::hTextAlign()
{
    return m_pTextStyle->hTextAlign();
}

int KivioBaseTargetStencil::vTextAlign()
{
    return m_pTextStyle->vTextAlign();
}

void KivioBaseTargetStencil::setHTextAlign(int a)
{
    m_pTextStyle->setHTextAlign(a);
}

void KivioBaseTargetStencil::setVTextAlign(int a)
{
    m_pTextStyle->setVTextAlign(a);
}

void KivioBaseTargetStencil::setText( const QString &a )
{
    m_pTextStyle->setText(a);
}

QString KivioBaseTargetStencil::text()
{
    return m_pTextStyle->text();
}



//////////////////////////////////////////////////////////////////////////////
//
// Paint Routines
//
//////////////////////////////////////////////////////////////////////////////

void KivioBaseTargetStencil::paint( KivioIntraStencilData * )
{
    ;
}

void KivioBaseTargetStencil::paintOutline( KivioIntraStencilData * )
{
    ;
}

void KivioBaseTargetStencil::paintConnectorTargets( KivioIntraStencilData *pData )
{
  QPixmap *targetPic;
  KivioPainter *painter;
  double x, y;

  // We don't draw these if we are selected!!!
  if( isSelected() == true )
    return;

  // Obtain the graphic used for KivioConnectorTargets
  targetPic = KivioConfig::config()->connectorTargetPixmap();


  KoZoomHandler* zoomHandler = pData->zoomHandler;
  painter = pData->painter;

  KivioConnectorTarget *pTarget;
  pTarget = m_pTargets->first();
  while( pTarget )
  {
    x = zoomHandler->zoomItX(pTarget->x());
    y = zoomHandler->zoomItY(pTarget->y());

    painter->drawPixmap( x-3, y-3, *targetPic );

    pTarget = m_pTargets->next();
  }
}





//////////////////////////////////////////////////////////////////////////////
//
// File I/O
//
//////////////////////////////////////////////////////////////////////////////
bool KivioBaseTargetStencil::loadXML( const QDomElement &e )
{
   QDomNode node;
   QString name;

   node = e.firstChild();
   while( !node.isNull() )
   {
      name = node.nodeName();

      if( name == "KivioStencilProperties" )
      {
	 loadProperties(node.toElement());
      }

      node = node.nextSibling();
   }

   updateGeometry();

   return true;
}

QDomElement KivioBaseTargetStencil::createRootElement( QDomDocument &doc )
{
   QDomElement e = doc.createElement("KivioPluginStencil");

   XmlWriteString( e, "id", m_pSpawner->info()->id() );
   XmlWriteString( e, "setId", m_pSpawner->set()->id() );

   return e;
}

QDomElement KivioBaseTargetStencil::saveXML( QDomDocument &doc )
{
   QDomElement e = createRootElement( doc );

   e.appendChild( saveProperties(doc) );

   return e;
}


QDomElement KivioBaseTargetStencil::saveProperties( QDomDocument &doc )
{
    QDomElement baseE = doc.createElement("KivioStencilProperties");

    QDomElement geoE = doc.createElement("Geometry");
    XmlWriteFloat( geoE, "x", m_x );
    XmlWriteFloat( geoE, "y", m_y );
    XmlWriteFloat( geoE, "w", m_w );
    XmlWriteFloat( geoE, "h", m_h );
    baseE.appendChild( geoE );

    baseE.appendChild( m_pLineStyle->saveXML(doc) );
    baseE.appendChild( m_pFillStyle->saveXML(doc) );
    baseE.appendChild( m_pTextStyle->saveXML(doc) );
    baseE.appendChild( saveTargets(doc) );

    // Only save custom data if this returns true (means there is custom data)
    QDomElement customE = doc.createElement("CustomData");
    if( saveCustom( customE, doc )==true )
    {
       baseE.appendChild( customE );
    }

    return baseE;
}

bool KivioBaseTargetStencil::loadProperties( const QDomElement &e )
{
    QDomNode node;
    QDomElement nodeE;
    QString nodeName;

    node = e.firstChild();
    while( !node.isNull() )
    {
        nodeName = node.nodeName();
        nodeE = node.toElement();

        if( nodeName == "KivioFillStyle" )
        {
            m_pFillStyle->loadXML( node.toElement() );
        }
        else if( nodeName == "KivioLineStyle" )
        {
            m_pLineStyle->loadXML( node.toElement() );
        }
        else if( nodeName == "KivioTextStyle" )
        {
            m_pTextStyle->loadXML( node.toElement() );
        }
        else if( nodeName == "KivioTargetList" )
        {
            loadTargets( node.toElement() );
        }
        else if( nodeName == "CustomData" )
        {
            loadCustom( node.toElement() );
        }
        else if( nodeName == "Geometry" )
        {
            m_x = XmlReadFloat( nodeE, "x", 0.0f );
            m_y = XmlReadFloat( nodeE, "y", 0.0f );
            m_w = XmlReadFloat( nodeE, "w", 72.0f );
            m_h = XmlReadFloat( nodeE, "h", 72.0f );
        }

        node = node.nextSibling();
    }

    return true;
}

bool KivioBaseTargetStencil::saveCustom( QDomElement &, QDomDocument & )
{
   return false;
}

bool KivioBaseTargetStencil::loadCustom( const QDomElement & )
{
    return true;
}

QDomElement KivioBaseTargetStencil::saveTargets( QDomDocument &doc )
{
    KivioConnectorTarget *p;

    QDomElement e = doc.createElement("TargetList");

    p = m_pTargets->first();
    while( p )
    {
        e.appendChild( p->saveXML(doc) );

        p = m_pTargets->next();
    }

    return e;
}

bool KivioBaseTargetStencil::loadTargets( const QDomElement &e )
{
    KivioConnectorTarget *p;
    QDomNode node;
    QDomElement targetE;
    QString nodeName;

    m_pTargets->clear();

    node = e.firstChild();
    while( !node.isNull() )
    {
        nodeName = node.nodeName();
        targetE = node.toElement();

        if( nodeName == "KivioConnectorTarget" )
        {
            p = new KivioConnectorTarget();
            p->loadXML( targetE );
            m_pTargets->append(p);
            p = NULL;
        }


        node = node.nextSibling();
    }


    return true;
}



//////////////////////////////////////////////////////////////////////////////
//
// Geometry Routine
//
//////////////////////////////////////////////////////////////////////////////
void KivioBaseTargetStencil::updateGeometry()
{
    ;
}




//////////////////////////////////////////////////////////////////////////////
//
// Target Routines
//
//////////////////////////////////////////////////////////////////////////////
KivioConnectorTarget *KivioBaseTargetStencil::connectToTarget( KivioConnectorPoint *p, double threshHold)
{
    double px = p->x();
    double py = p->y();

    double tx, ty;

    KivioConnectorTarget *pTarget = m_pTargets->first();
    while( pTarget )
    {
        tx = pTarget->x();
        ty = pTarget->y();


        if( px >= tx - threshHold &&
            px <= tx + threshHold &&
            py >= ty - threshHold &&
            py <= ty + threshHold )
        {
            // setTarget calls pTarget->addConnectorPoint() and removes
            // any previous connections from p
            p->setTarget( pTarget );
            return pTarget;
        }

        pTarget = m_pTargets->next();
    }

    return NULL;
}

// we probably don't need targetID in the function since it's stored in p... but I don't remember so
// i'm leaving it here for now.
KivioConnectorTarget *KivioBaseTargetStencil::connectToTarget( KivioConnectorPoint *p, int /*targetID*/ )
{
    int id = p->targetId();

    KivioConnectorTarget *pTarget = m_pTargets->first();
    while( pTarget )
    {
        if( pTarget->id() == id )
        {
            p->setTarget(pTarget);

            return pTarget;
        }

        pTarget = m_pTargets->next();
    }

    return NULL;
}




//////////////////////////////////////////////////////////////////////////////
//
// ID Generation
//
//////////////////////////////////////////////////////////////////////////////
int KivioBaseTargetStencil::generateIds( int nextAvailable )
{
    KivioConnectorTarget *pTarget = m_pTargets->first();

    // Iterate through all the targets
    while( pTarget )
    {
        // If this target has something connected to it
        if( pTarget->hasConnections() )
        {
            // Set it's id to the next available id
            pTarget->setId( nextAvailable );

            // Increment the next available id
            nextAvailable++;
        }
        else
        {
            // Otherwise mark it as unused (-1)
            pTarget->setId( -1 );
        }

        pTarget = m_pTargets->next();
    }

    // Return the next availabe id
    return nextAvailable;
}

void KivioBaseTargetStencil::copyBasicInto( KivioBaseTargetStencil *pStencil )
{
    pStencil->setSpawner( m_pSpawner );

    m_pFillStyle->copyInto( pStencil->m_pFillStyle );
    m_pLineStyle->copyInto( pStencil->m_pLineStyle );
    m_pTextStyle->copyInto( pStencil->m_pTextStyle );

    // Copy the targets
    KivioConnectorTarget *pSrcTarget, *pTgTarget;
    pSrcTarget = m_pTargets->first();
    pTgTarget = pStencil->m_pTargets->first();

    while( pSrcTarget && pTgTarget )
    {
        pTgTarget->setPosition( pSrcTarget->x(), pSrcTarget->y() );

        pSrcTarget = m_pTargets->next();
        pTgTarget = pStencil->m_pTargets->next();
    }

    // Copy the geometry
    pStencil->m_x = m_x;
    pStencil->m_y = m_y;
    pStencil->m_w = m_w;
    pStencil->m_h = m_h;

    *(pStencil->m_pProtection) = *m_pProtection;
    *(pStencil->m_pCanProtect) = *m_pCanProtect;
}

int KivioBaseTargetStencil::resizeHandlePositions()
{
   // Calculate the resize handle positions
   int mask = KIVIO_RESIZE_HANDLE_POSITION_ALL;

   if( m_pProtection->at( kpWidth ) )
   {
      mask &= ~(krhpNE | krhpNW | krhpSW | krhpSE | krhpE | krhpW);
   }

   if( m_pProtection->at( kpHeight) )
   {
      mask &= ~(krhpNE | krhpNW | krhpSW | krhpSE | krhpN | krhpS);
   }

   return mask;
}
