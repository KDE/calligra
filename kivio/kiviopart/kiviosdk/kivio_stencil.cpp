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
#include "kivio_config.h"
#include "kivio_custom_drag_data.h"
#include "kivio_fill_style.h"
#include "kivio_intra_stencil_data.h"
#include "kivio_painter.h"
#include "kivio_point.h"
#include "kivio_rect.h"
#include "kivio_stencil.h"

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
    for( int i=0; i<NUM_PROTECTIONS; i++ )
    {
        m_pProtection->clearBit(i);
        m_pCanProtect->setBit(i);
    }
    
}

KivioStencil::~KivioStencil()
{
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
    float newX, newY, newW, newH;

    // Handle Width
    const float HW = 6.0f;
    const float HWP1 = HW+1.0f;

    // Handle Width Over 2
    const float HWo2 = HW/2.0f;

    float scale = pData->scale;

    KivioPainter *painter = pData->painter;

    newX = m_x * scale;
    newY = m_y * scale;
    newW = m_w * scale;
    newH = m_h * scale;

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

KivioCollisionType KivioStencil::checkForCollision( KivioPoint *, float )
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

KivioConnectorTarget *KivioStencil::connectToTarget( KivioConnectorPoint *, float )
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

void KivioStencil::updateConnectorPoints(KivioConnectorPoint *, float, float)
{
   // Default to just calling updateGeometry
   updateGeometry();
}
