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

#include "WebTool.h"

#include <KoCanvasBase.h>
#include <KoPointerEvent.h>
#include <KoShapeManager.h>
#include <KoSelection.h>

#include "WebShape.h"
#include <klocalizedstring.h>
#include "WebToolWidget.h"

WebTool::WebTool(KoCanvasBase *canvas) : KoTool(canvas), m_tmpShape(0), m_dragMode(NO_DRAG)
{
}

WebTool::~WebTool()
{
}

void WebTool::activate( bool v )
{
  Q_ASSERT(m_dragMode == NO_DRAG);
  KoSelection *selection = m_canvas->shapeManager()->selection();
  foreach( KoShape *shape, selection->selectedShapes() ) 
  {
    m_currentShape = dynamic_cast<WebShape*>( shape );
    if(m_currentShape)
      break;
  }
  if( m_currentShape == 0 ) 
  {
    // none found
    emit done();
    return;
  }
  KoTool::activate(v);
}

void WebTool::paint( QPainter &painter, const KoViewConverter &converter)
{
  Q_UNUSED(painter);
  Q_UNUSED(converter);
}

void WebTool::mousePressEvent( KoPointerEvent *event )
{
  WebShape *hit = 0;
  QRectF roi( event->point, QSizeF(1,1) );
  QList<KoShape*> shapes = m_canvas->shapeManager()->shapesAt( roi );
  KoSelection *selection = m_canvas->shapeManager()->selection();
  foreach( KoShape *shape, shapes ) 
  {
    hit = dynamic_cast<WebShape*>( shape );
    if(hit) {
      if(hit == m_currentShape) {
          m_scrollPoint = event->point;
          Q_ASSERT(m_dragMode == NO_DRAG);
          m_dragMode = SCROLL_DRAG;
      } else {
        selection->deselectAll();
        m_currentShape = hit;
        selection->select( m_currentShape );
        emit(shapeChanged(hit));
      }
    }
  }
}

void WebTool::mouseMoveEvent( KoPointerEvent *event )
{
  switch(m_dragMode) {
    case NO_DRAG:
      break;
    case SCROLL_DRAG:
    {
      m_currentShape->scrollOf(m_scrollPoint - event->point);
      m_scrollPoint = event->point;
      m_currentShape->update();
      break;
    }
  }
}

void WebTool::mouseReleaseEvent( KoPointerEvent *event )
{
  m_dragMode = NO_DRAG;
}

QMap<QString, QWidget *> WebTool::createOptionWidgets() {
  QMap<QString, QWidget *> widgets;
  widgets[i18n("Web tool options")] = new WebToolWidget(this);
  return widgets;
}
