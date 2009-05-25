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

StateTool::StateTool(KoCanvasBase *canvas) : KoTool(canvas)
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
      } else {
        selection->deselectAll();
        m_currentShape = hit;
        selection->select( m_currentShape );
      }
    }
  }
}

#include "StateTool.moc"
