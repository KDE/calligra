/*
 *  movetool.cc - part of Krayon
 *
 *  Copyright (c) 1999 Matthias Elter  <me@kde.org>
 *                1999 Michael Koch    <koch@kde.org>
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
#include <iostream>
#include "kis_doc.h"
#include "kis_view.h"
#include "kis_cursor.h"
#include "kis_tool_move.h"

using namespace std;

MoveCommand::MoveCommand( KisDoc *_doc, int _layer, 
    QPoint _oldpos, QPoint _newpos )
  : KisCommand( i18n( "Move layer" ), _doc )
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
  KisImage* img = m_pDoc->current();
  if (!img) return;

  img->setCurrentLayer( m_layer );
  QRect oldRect = img->getCurrentLayer()->imageExtents();
  img->getCurrentLayer()->moveTo( _pos.x(), _pos.y() );
  img->markDirty( img->getCurrentLayer()->imageExtents() );
  img->markDirty( oldRect );
}

MoveTool::MoveTool( KisDoc *doc, KisView *view )
  : KisTool( doc, view )
{
    m_Cursor = KisCursor::moveCursor();
    m_dragging = false;
}

MoveTool::~MoveTool()
{
}

void MoveTool::mousePress( QMouseEvent *e )
{
    KisImage* img = m_pDoc->current();
    if (!img)return;

    if( e->button() != LeftButton )
        return;

    if( !img->getCurrentLayer()->visible() )
        return;

    QPoint pos = e->pos();
    QPoint zoomedPos(zoomed(pos));
    if(!img->getCurrentLayer()->imageExtents().contains(zoomedPos))
        return;

    m_dragging = true;
    m_dragStart.setX(e->x());
    m_dragStart.setY(e->y());
    m_layerStart = img->getCurrentLayer()->imageExtents().topLeft();
    m_layerPosition = m_layerStart;
}

void MoveTool::mouseMove( QMouseEvent *e )
{
    KisImage* img = m_pDoc->current();
    if (!img) return;
  
    if( m_dragging )
    {
        QPoint pos = e->pos();
        QPoint zoomedPos(pos - m_dragStart);
        m_dragPosition = zoomed(zoomedPos);

        QRect oldRect = img->getCurrentLayer()->imageExtents();
        img->getCurrentLayer()->moveBy(m_dragPosition.x(), m_dragPosition.y());
        img->markDirty( img->getCurrentLayer()->imageExtents() );
	    img->markDirty( oldRect );

        m_layerPosition = img->getCurrentLayer()->imageExtents().topLeft();
        m_dragStart = e->pos();
    }
}


void MoveTool::mouseRelease(QMouseEvent *e )
{
    KisImage* img = m_pDoc->current();
    if (!img) return;

    if( e->button() != LeftButton ) return;

    if( !m_dragging ) return;

#if 0
    if( m_layerPosition != m_layerStart )
    {
        MoveCommand *moveCommand = new MoveCommand( m_pDoc,
        img->getCurrentLayerIndex(), m_layerStart, m_layerPosition );

        //m_pDoc->commandHistory()->addCommand( moveCommand ); //jwc
    }
#endif

    m_dragging = false;
}
