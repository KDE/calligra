/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "StateTool.h"

#include <KoCanvasBase.h>
#include <KoPointerEvent.h>
#include <KoShapeManager.h>
#include <KoSelection.h>

#include "StateShape.h"
#include "StatesRegistry.h"
#include "StateShapeChangeStateCommand.h"

StateTool::StateTool(KoCanvasBase *canvas) : KoTool(canvas), m_tmpShape(0)
{
}

StateTool::~StateTool()
{
}

void StateTool::paint( QPainter &painter, const KoViewConverter &converter)
{
  Q_UNUSED(painter);
  Q_UNUSED(converter);
}

void StateTool::mousePressEvent( KoPointerEvent *event )
{
  StateShape *hit = 0;
  QRectF roi( event->point, QSizeF(1,1) );
  QList<KoShape*> shapes = m_canvas->shapeManager()->shapesAt( roi );
  KoSelection *selection = m_canvas->shapeManager()->selection();
  foreach( KoShape *shape, shapes ) 
  {
    hit = dynamic_cast<StateShape*>( shape );
    if(hit) {
      if(hit == m_currentShape) {
        const State* state = StatesRegistry::instance()->state(m_currentShape->categoryId(), m_currentShape->stateId());
        const State* newState = StatesRegistry::instance()->nextState(state);
        if(newState)
        {
          m_canvas->addCommand(new StateShapeChangeStateCommand(m_currentShape, newState->category()->id(), newState->id()));
        }
      } else {
        selection->deselectAll();
        m_currentShape = hit;
        selection->select( m_currentShape );
      }
    }
  }
}

void StateTool::mouseMoveEvent( KoPointerEvent *event )
{
  m_tmpShape = 0;
  QRectF roi( event->point, QSizeF(1,1) );
  QList<KoShape*> shapes = m_canvas->shapeManager()->shapesAt( roi );
  foreach( KoShape * shape, shapes )
  {
    if( not dynamic_cast<StateShape*>( shape ) )
    {
      m_tmpShape = shape;
      break;
    }
  }
  if( m_tmpShape )
    useCursor( QCursor( Qt::PointingHandCursor ) );
  else if( shapes.empty() ) {
    useCursor( QCursor( Qt::ForbiddenCursor ) );
  } else {
    useCursor( QCursor( Qt::ArrowCursor ) );
  }
}

void StateTool::mouseReleaseEvent( KoPointerEvent *event )
{
  event->ignore();
}
#include "StateTool.moc"
