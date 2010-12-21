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
#include <QPainter>

ComicBoxesTool::ComicBoxesTool(KoCanvasBase *canvas) : KoToolBase(canvas), m_dragging(false)
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
  if(m_dragging)
  {
      painter.setPen(Qt::black);
      painter.drawLine(converter.documentToView(m_currentStartingPoint),
                       converter.documentToView(m_currentPoint) );
  }
}

void ComicBoxesTool::mousePressEvent( KoPointerEvent *event )
{
    m_dragging = true;
    m_currentStartingPoint = event->point;
}

void ComicBoxesTool::mouseMoveEvent( KoPointerEvent *event )
{
    if(m_dragging)
    {
        canvas()->updateCanvas(currentDraggingRect());
        m_currentPoint = event->point;
        if(event->modifiers() & Qt::SHIFT) makeVerticalOrHorizontal(m_currentStartingPoint, m_currentPoint);
        canvas()->updateCanvas(currentDraggingRect());
    } else {
        event->ignore();
    }
}

inline bool near(qreal a, qreal b)
{
    return qAbs(a - b) < 1e-6;
}

bool tryIntersect( const QLineF& _l1, const QLineF& _l2, QPointF& pt )
{
    return _l1.intersect(_l2, &pt) != QLineF::NoIntersection && (near(pt.x(), 0.0) || near(pt.x(), 1.0) || near(pt.y(), 0.0) || near(pt.y(), 1.0)) && QRectF(0,0,1,1).contains(pt);
}

bool tryIntersects( const QLineF& _l, const QLineF& _l0, const QLineF& _l1, const QLineF& _l2, const QLineF& _l3, QPointF& pt1, QPointF& pt2 )
{
    QPointF* pt = &pt1;
    if( tryIntersect(_l, _l0, *pt))
    {
        pt = &pt2;
    }
    if( tryIntersect(_l, _l1, *pt))
    {
        if( pt == &pt2) return true;
        pt = &pt2;
    }
    if( tryIntersect(_l, _l2, *pt))
    {
        if( pt == &pt2) return true;
        pt = &pt2;
    }
    if( tryIntersect(_l, _l3, *pt))
    {
        if( pt == &pt2) return true;
        pt = &pt2;
    }
    return false;
}

void ComicBoxesTool::mouseReleaseEvent( KoPointerEvent *event )
{
    if(m_dragging)
    {
        m_dragging = false;
        
        QPointF p1 = m_currentStartingPoint - m_currentShape->boundingRect().topLeft();
        QPointF p2 = event->point;
        if(event->modifiers() & Qt::SHIFT) makeVerticalOrHorizontal(m_currentStartingPoint, p2);
        p2 -= m_currentShape->boundingRect().topLeft();
        
        p1 = QPointF(p1.x() / m_currentShape->boundingRect().width(), p1.y() / m_currentShape->boundingRect().height() );
        p2 = QPointF(p2.x() / m_currentShape->boundingRect().width(), p2.y() / m_currentShape->boundingRect().height() );
        
        QLineF line(p1, p2);
        
        QLineF line0(QPointF(0,0), QPointF(1,0));
        QLineF line1(QPointF(1,0), QPointF(1,1));
        QLineF line2(QPointF(1,1), QPointF(0,1));
        QLineF line3(QPointF(0,1), QPointF(0,0));
        
        QPointF p1_b, p2_b;
        
        if(tryIntersects(line, line0, line1, line2, line3, p1_b, p2_b) )
        {
            m_currentShape->addLine(QLineF(p1_b, p2_b));
        }
        
        canvas()->updateCanvas(currentDraggingRect().united(m_currentShape->boundingRect()));
        
        
        
    } else {
        event->ignore();
    }
}

QRectF ComicBoxesTool::currentDraggingRect() const
{
    return QRectF(m_currentStartingPoint, m_currentPoint).normalized();
}

QMap<QString, QWidget *> ComicBoxesTool::createOptionWidgets() {
  QMap<QString, QWidget *> widgets;
  return widgets;
}

void ComicBoxesTool::makeVerticalOrHorizontal(const QPointF& origin, QPointF& point)
{
    if( qAbs(origin.x() - point.x()) > qAbs(origin.y() - point.y()))
    {
        point.setY(origin.y());
    } else {
        point.setX(origin.x());        
    }
}

#include "ComicBoxesTool.moc"
