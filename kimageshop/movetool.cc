/*
 *  movetool.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter  <me@kde.org>
 *                1999 Michael Koch    <mkoch@kde.org>
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

#include <klocale.h>

#include "movetool.h"
#include "kimageshop_doc.h"

MoveCommand::MoveCommand( KImageShopDoc *_doc, int _layer, QPoint _oldpos, QPoint _newpos )
  : KImageShopCommand( i18n( "Move layer" ), _doc )
  , m_layer( _layer )
  , m_oldPos( _oldpos )
  , m_newPos( _newpos )
{
}

void MoveCommand::execute()
{
  cout << "MoveCommand::execute" << endl;

  moveTo( m_newPos );
}

void MoveCommand::unexecute()
{
  cout << "MoveCommand::unexecute" << endl;

  moveTo( m_oldPos );
}

void MoveCommand::moveTo( QPoint _pos )
{
  cout << "MoveCommand::moveTo" << endl;
  cout << "moveTo : " << _pos.x() << ":" << _pos.y() << endl;

  QRect oldRect;
  QRect newRect;
  QRect updateRect;

  m_pDoc->setCurrentLayer( m_layer );

  oldRect = m_pDoc->getCurrentLayer()->imageExtents();
  newRect = QRect( _pos, oldRect.size() );

  m_pDoc->moveLayer( _pos.x(), _pos.y() );
  updateRect = oldRect.unite( newRect );
  m_pDoc->compositeImage( updateRect );  

  m_pDoc->slotUpdateViews( updateRect );
}

MoveTool::MoveTool( KImageShopDoc *doc )
  : Tool( doc )
{
  m_dragging = false;
}

MoveTool::~MoveTool()
{
}

void MoveTool::mousePress( const KImageShop::MouseEvent& e )
{
  if( !e.leftButton )
    return;

  m_dragging = true;
  m_dragStart.setX( e.posX );
  m_dragStart.setY( e.posY );
} 

void MoveTool::mouseMove( const KImageShop::MouseEvent& e )
{
  if( m_dragging )
  {
    QPoint pos( e.posX, e.posY );

    m_dragPosition = pos - m_dragStart;

    QRect updateRect( m_pDoc->getCurrentLayer()->imageExtents() );
    m_pDoc->moveLayer( m_dragPosition.x(), m_dragPosition.y() );
    updateRect = updateRect.unite( m_pDoc->getCurrentLayer()->imageExtents() );
    m_pDoc->compositeImage( updateRect );

    m_dragStart = pos;

    m_pDoc->slotUpdateViews( updateRect );
  }
}

void MoveTool::mouseRelease( const KImageShop::MouseEvent& e )
{
  if( !e.leftButton )
    return;

  if( m_dragStart != m_dragPosition )
  {
    cout << "add move command" << endl;

    QPoint pos( e.posX, e.posY );

    MoveCommand *moveCommand = new MoveCommand( m_pDoc,
      m_pDoc->getCurrentLayerIndex(), pos - m_dragStart, pos - m_dragPosition );

    m_pDoc->commandHistory()->addCommand( moveCommand );
  }
  m_dragging = false;
}

