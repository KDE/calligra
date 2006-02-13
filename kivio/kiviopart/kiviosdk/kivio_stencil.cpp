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
#include "kivio_custom_drag_data.h"
#include "kivio_fill_style.h"
#include "kivio_intra_stencil_data.h"
#include "kivio_painter.h"
#include "kivio_stencil.h"

#include <KoZoomHandler.h>
#include <KoRect.h>
#include <KIvioStencilIface.h>

KivioStencil::KivioStencil()
    : m_pSpawner(NULL),
      m_pProtection(NULL),
      m_pCanProtect(NULL)
{
  m_x = m_y = 0.0f;
  m_w = m_h = 72.0f;

  m_rotation = 0;

  m_hidden = false;

  m_selected = false;

  m_pProtection = new QBitArray(NUM_PROTECTIONS);
  m_pCanProtect = new QBitArray(NUM_PROTECTIONS);

  for( int i = 0; i < NUM_PROTECTIONS; i++ )
  {
    m_pProtection->clearBit(i);
    m_pCanProtect->setBit(i);
  }

  m_type = kstNormal;
  m_connected = false;

  iface = new KivioStencilIface(this);
}

KivioStencil::~KivioStencil()
{
    kdDebug(43000)<<" KivioStencil::~KivioStencil() !"<<this<<endl;
    delete m_pProtection;
    m_pProtection = NULL;
    delete m_pCanProtect;
    m_pCanProtect = NULL;


    m_pSpawner = NULL;
}

KoRect KivioStencil::rect()
{
    return KoRect( m_x, m_y, m_w, m_h );
}


bool KivioStencil::loadXML( const QDomElement & )
{
    return false;
}

QDomElement KivioStencil::saveXML( QDomDocument &doc )
{
    return doc.createElement("");
}

void KivioStencil::paint( KivioIntraStencilData */*pData*/ )
{
}

/*
 * Paints the outline of the stencil, no filling is done.
 */
void KivioStencil::paintOutline( KivioIntraStencilData * )
{
}

void KivioStencil::paintConnectorTargets( KivioIntraStencilData * )
{
}

void KivioStencil::paintSelectionHandles( KivioIntraStencilData *pData )
{
  int newX, newY, newW, newH;

  // Handle Width
  //const double HW = 6.0f;
  //const double HWP1 = HW+1.0f;

  // Handle Width Over 2
  //const double HWo2 = HW/2.0f;

  KoZoomHandler* zoomHandler = pData->zoomHandler;

  KivioPainter *painter = pData->painter;

  newX = zoomHandler->zoomItX(m_x - qRound(lineWidth() / 2));
  newY = zoomHandler->zoomItY(m_y - qRound(lineWidth() / 2));
  newW = zoomHandler->zoomItX(m_w + lineWidth() + 1);
  newH = zoomHandler->zoomItY(m_h + lineWidth() + 1);

  painter->saveState();
  painter->setTranslation(newX, newY);
  rotatePainter(pData);  // Rotate the painter if needed

  painter->drawSelectionBox(QRect(0, 0, newW, newH));

  // top left, top right, bottom left, bottom right
  if( m_pProtection->at( kpWidth ) ||
    m_pProtection->at( kpHeight ) )
  {
    painter->drawHandle( 0, 0,  KivioPainter::cpfLock );
    painter->drawHandle( newW, 0, KivioPainter::cpfLock );
    painter->drawHandle( 0, newH, KivioPainter::cpfLock );
    painter->drawHandle( newW, newH, KivioPainter::cpfLock );
  }
  else
  {
    painter->drawHandle( 0, 0, 0 );
    painter->drawHandle( newW, 0, 0 );
    painter->drawHandle( 0, newH, 0 );
    painter->drawHandle( newW, newH, 0 );
  }

  // Top/bottom
  if( m_pProtection->at( kpHeight ) ||
    m_pProtection->at( kpAspect ) )
  {
    painter->drawHandle( newW / 2, 0, KivioPainter::cpfLock );
    painter->drawHandle( newW / 2, newH, KivioPainter::cpfLock );
  }
  else
  {
    painter->drawHandle( newW / 2, 0, 0 );
    painter->drawHandle( newW / 2, newH, 0 );
  }

    // left, right
  if( m_pProtection->at( kpWidth ) ||
    m_pProtection->at( kpAspect ) )
  {
    painter->drawHandle( 0, newH / 2, KivioPainter::cpfLock );
    painter->drawHandle( newW, newH / 2, KivioPainter::cpfLock );
  }
  else
  {
    painter->drawHandle( 0, newH / 2, 0 );
    painter->drawHandle( newW, newH / 2, 0 );
  }
  painter->restoreState();
}

KivioCollisionType KivioStencil::checkForCollision( KoPoint *, double )
{
    return kctNone;
}

void KivioStencil::addToGroup( KivioStencil */*pStencil*/ )
{

}

void KivioStencil::customDrag( KivioCustomDragData * )
{
}

void KivioStencil::updateGeometry()
{
}

KivioConnectorTarget *KivioStencil::connectToTarget( KivioConnectorPoint *, double )
{
    return NULL;
}

KivioConnectorTarget *KivioStencil::connectToTarget( KivioConnectorPoint *, int )
{
    return NULL;
}

KoPoint KivioStencil::snapToTarget( const KoPoint& p, double /*thresh*/, bool& hit )
{
  KoPoint retVal = p;
  hit = false;
  return retVal;
}

int KivioStencil::generateIds(int nextAvailable)
{
    return nextAvailable;
}

void KivioStencil::searchForConnections( KivioPage * )
{
}

void KivioStencil::searchForConnections( KivioPage *, double )
{
}

void KivioStencil::updateConnectorPoints(KivioConnectorPoint *, double, double)
{
   // Default to just calling updateGeometry
   updateGeometry();
}

void KivioStencil::rotatePainter(KivioIntraStencilData *pData)
{
  if(m_rotation != 0) {
    QWMatrix m;
    m.translate(pData->zoomHandler->zoomItX(m_pinPoint.x()), pData->zoomHandler->zoomItY(m_pinPoint.y()));
    m.rotate(m_rotation);
    m.translate(pData->zoomHandler->zoomItX(-m_pinPoint.x()), pData->zoomHandler->zoomItY(-m_pinPoint.y()));
    pData->painter->setWorldMatrix(m, true);
  }
}

KoRect KivioStencil::calculateBoundingBox()
{
   KoRect r;
   return r;
}

void KivioStencil::setRotation(int d)
{
  m_rotation = d;
  m_pinPoint.setCoords(m_w / 2.0, m_h / 2.0);
  updateGeometry();
}

void KivioStencil::move(double xOffset, double yOffset)
{
  setX(x() + xOffset);
  setY(y() + yOffset);
}

bool KivioStencil::isInRect(const KoRect& rect)
{
  bool retVal;
  retVal = rect.contains(m_x, m_y);
  retVal = retVal && rect.contains(m_x + m_w, m_y + m_h);

  return retVal;
}
