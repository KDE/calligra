/*
 *  Copyright (c) 2010 Cyrille Berger <cberger@cberger.net>
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

#include "ComicBoxesTool.h"

#include <KoCanvasBase.h>
#include <KoPointerEvent.h>
#include <KoShapeManager.h>
#include <KoSelection.h>

#include "ComicBoxesShape.h"

ComicBoxesTool::ComicBoxesTool(KoCanvasBase *canvas) : KoToolBase(canvas)
{
}

ComicBoxesTool::~ComicBoxesTool()
{
}

void ComicBoxesTool::activate(ToolActivation /*toolActivation*/, const QSet<KoShape*> &/*shapes*/)
{
  KoSelection *selection = canvas()->shapeManager()->selection();
  foreach( KoShape *shape, selection->selectedShapes() ) 
  {
    m_currentShape = dynamic_cast<ComicBoxesShape*>( shape );
    if(m_currentShape)
      break;
  }
  if( m_currentShape == 0 ) 
  {
    // none found
    emit done();
    return;
  }
  useCursor( QCursor( Qt::ArrowCursor ) );
}
void ComicBoxesTool::paint( QPainter &painter, const KoViewConverter &converter)
{
  Q_UNUSED(painter);
  Q_UNUSED(converter);
}

void ComicBoxesTool::mousePressEvent( KoPointerEvent *event )
{
  event->ignore();
}

void ComicBoxesTool::mouseMoveEvent( KoPointerEvent *event )
{
  event->ignore();
}

void ComicBoxesTool::mouseReleaseEvent( KoPointerEvent *event )
{
  event->ignore();
}

QMap<QString, QWidget *> ComicBoxesTool::createOptionWidgets() {
  QMap<QString, QWidget *> widgets;
  return widgets;
}

#include "ComicBoxesTool.moc"
