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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "kivio_config.h"
#include "kivio_custom_drag_data.h"
#include "kivio_fill_style.h"
#include "kivio_intra_stencil_data.h"
#include "kivio_painter.h"
#include "kivio_point.h"
#include "kivio_rect.h"
#include "kivio_stencil.h"

#include <kozoomhandler.h>
#include <KIvioStencilIface.h>

KivioStencil::KivioStencil()
    : m_pSpawner(NULL),
      m_pProtection(NULL),
      m_pCanProtect(NULL)
{
  m_x = m_y = 0.0f;
  m_w = m_h = 72.0f;

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
    if( m_pProtection )
    {
        delete m_pProtection;
        m_pProtection = NULL;
    }

    if( m_pCanProtect )
    {
        delete m_pCanProtect;
        m_pCanProtect = NULL;
    }


    m_pSpawner = NULL;
}

KivioRect KivioStencil::rect()
{
    return KivioRect( m_x, m_y, m_w, m_h );
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
  double newX, newY, newW, newH;

  // Handle Width
  //const double HW = 6.0f;
  //const double HWP1 = HW+1.0f;

  // Handle Width Over 2
  //const double HWo2 = HW/2.0f;

  KoZoomHandler* zoomHandler = pData->zoomHandler;

  KivioPainter *painter = pData->painter;

  newX = zoomHandler->zoomItX(m_x);
  newY = zoomHandler->zoomItY(m_y);
  newW = zoomHandler->zoomItX(m_w);
  newH = zoomHandler->zoomItY(m_h);

  // top left, top right, bottom left, bottom right
  if( m_pProtection->at( kpWidth ) ||
    m_pProtection->at( kpHeight ) )
  {
    painter->drawHandle( newX,                newY,               KivioPainter::cpfLock );
    painter->drawHandle( newX + newW,         newY,               KivioPainter::cpfLock );
    painter->drawHandle( newX,                newY + newH,        KivioPainter::cpfLock );
    painter->drawHandle( newX + newW,         newY + newH,        KivioPainter::cpfLock );
  }
  else
  {
    painter->drawHandle( newX,                newY,               0 );
    painter->drawHandle( newX + newW,         newY,               0 );
    painter->drawHandle( newX,                newY + newH,        0 );
    painter->drawHandle( newX + newW,         newY + newH,        0 );
  }

  // Top/bottom
  if( m_pProtection->at( kpHeight ) ||
    m_pProtection->at( kpAspect ) )
  {
    painter->drawHandle( newX + newW/2.0f,    newY,               KivioPainter::cpfLock );
    painter->drawHandle( newX + newW/2.0f,    newY + newH,        KivioPainter::cpfLock );
  }
  else
  {
    painter->drawHandle( newX + newW/2.0f,    newY,               0 );
    painter->drawHandle( newX + newW/2.0f,    newY + newH,        0 );
  }

    // left, right
  if( m_pProtection->at( kpWidth ) ||
    m_pProtection->at( kpAspect ) )
  {
    painter->drawHandle( newX,                newY + newH/2.0f,   KivioPainter::cpfLock );
    painter->drawHandle( newX + newW,         newY + newH/2.0f,   KivioPainter::cpfLock );
  }
  else
  {
    painter->drawHandle( newX,                newY + newH/2.0f,   0 );
    painter->drawHandle( newX + newW,         newY + newH/2.0f,   0 );
  }
}

KivioCollisionType KivioStencil::checkForCollision( KivioPoint *, double )
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
