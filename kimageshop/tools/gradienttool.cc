/*
 *  brushtool.h - part of KImageShop
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

#include "kimageshop_doc.h"
#include "gradienttool.h"
#include "gradient.h"

GradientTool::GradientTool( KImageShopDoc *_doc, Gradient *_gradient )
  : Tool( _doc )
  , m_gradient( _gradient )
{
}

GradientTool::~GradientTool()
{
}

void GradientTool::mousePress( const KImageShop::MouseEvent &_event )
{
  m_dragging = true;
  m_dragStart = QPoint( _event.posX, _event.posY );
}

void GradientTool::mouseMove( const KImageShop::MouseEvent &_event )
{
  QPoint pos( _event.posX, _event.posY );

  // draw line from m_dragStart to pos
  // how can we delete the old one ?
}

void GradientTool::mouseRelease( const KImageShop::MouseEvent &_event )
{
  m_dragging = false;
}

