/*
 *  gradienttool.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Michael Koch <koch@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qpainter.h>

#include "kimageshop_doc.h"
#include "gradienttool.h"
#include "gradient.h"

GradientTool::GradientTool( KImageShopDoc *_doc, KImageShopView *_view, CanvasView *_cv, Gradient *_gradient )
  : Tool( _doc )
  , m_pView (_view )
  , m_pCanvasView( _cv )
  , m_gradient( _gradient )
  , m_dragging( false )
  , m_helpWidget( 0 )
{
}

GradientTool::~GradientTool()
{
}

void GradientTool::mousePress( const KImageShop::MouseEvent &_event )
{
  m_dragging = true;
  m_dragStart = QPoint( _event.posX, _event.posY );
  m_pointEnd = m_dragStart;

/*
  if( !m_helpWidget )
  {
    m_helpWidget = new QWidget( m_pCanvasView );
    m_helpWidget->resize( m_pCanvasView->size() );
    m_helpWidget->setBackgroundMode( QWidget::NoBackground );
    m_helpWidget->show();
  }
*/
//m_pCanvasView->getTransWidget()->show();
}

void GradientTool::mouseMove( const KImageShop::MouseEvent &_event )
{

  if( m_dragging )
  {
    QPainter p;
    p.begin( m_pCanvasView );
    p.setRasterOp ( Qt::NotROP );
//    p.eraseRect( m_pCanvasView->rect() );
    p.setPen( QPen( Qt::black, 1 ) );
    p.setBrush( Qt::NoBrush );
    p.drawLine( m_dragStart, m_pointEnd );
    p.setRasterOp ( Qt::CopyROP );
    p.end();

    m_pointEnd = QPoint( _event.posX, _event.posY );

    p.begin( m_pCanvasView );
    p.setRasterOp ( Qt::NotROP );
//    p.eraseRect( m_pCanvasView->rect() );
    p.setPen( QPen( Qt::black, 1 ) );
    p.setBrush( Qt::NoBrush );
    p.drawLine( m_dragStart, m_pointEnd );
    p.setRasterOp ( Qt::CopyROP );
    p.end();

/*
    QPainter p( m_helpWidget );
    m_helpWidget->show();

    p.flush();
*/
  }
}

void GradientTool::mouseRelease( const KImageShop::MouseEvent &_event )
{
  m_dragging = false;

  QPainter p;
  p.begin( m_pCanvasView );
  p.setRasterOp ( Qt::NotROP );
//  p.eraseRect( m_pCanvasView->rect() );
  p.setPen( QPen( Qt::black, 1 ) );
  p.setBrush( Qt::NoBrush );
  p.drawLine( m_dragStart, m_pointEnd );
  p.setRasterOp ( Qt::CopyROP );
  p.end();

  if ( m_helpWidget )
  { // destroy the transparent widget, used for showing the rectangle (zoom)
    delete m_helpWidget;
    m_helpWidget = 0L;
  }
}
