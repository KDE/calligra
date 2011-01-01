/*
 *  Copyright (c) 2010,2011 Cyrille Berger <cberger@cberger.net>
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

#include "ComicBoxesLine.h"
#include <limits>
#include <cmath>

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
    
    // Draw the handles
    if(m_currentShape)
    {
        QTransform t;
        t.translate(m_currentShape->position().x(), m_currentShape->position().y());
        t.scale(m_currentShape->size().width(), m_currentShape->size().height());
        foreach(ComicBoxesLine* line, m_currentShape->lines())
        {
            if(line->isEditable())
            {
                QLineF l = line->line();
                painter.setPen(Qt::black);
                painter.setBrush(Qt::white);
                painter.drawEllipse(converter.documentToView(t.map(l.p1())), 3, 3);
                painter.drawEllipse(converter.documentToView(t.map(l.p2())), 3, 3);
            }
        }
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

// See http://local.wasp.uwa.edu.au/~pbourke/geometry/pointline/

inline qreal norm2_2(const QPointF& pt)
{
    return (pt.x() * pt.x() + pt.y() * pt.y());
}

inline qreal norm2(const QPointF& pt)
{
    return std::sqrt(norm2_2(pt));
}

inline qreal projection(const QLineF& l, const QPointF& pt_1 )
{
    qreal top = (pt_1.x() - l.p1().x()) * (l.p2().x() - l.p1().x()) + (pt_1.y() - l.p1().y()) * (l.p2().y() - l.p1().y());
    return top / norm2_2(l.p2() - l.p1());
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
        
        ComicBoxesLine* line1 = 0;
        ComicBoxesLine* line2 = 0;
        qreal c1, c2;
        qreal dist1 = std::numeric_limits<qreal>::max();
        qreal dist2 = std::numeric_limits<qreal>::max();
        
        foreach(ComicBoxesLine* line, m_currentShape->lines())
        {
            QLineF l = line->line();
            qreal u1 = projection(l, p1);
            double d1 = norm2(l.pointAt(u1) - p1);
            if( d1 < dist1 && u1 >= 0 && u1 <= 1 )
            {
                line1 = line;
                c1 = u1;
                dist1 = d1;
            }
            qreal u2 = projection(l, p2);
            double d2 = norm2(l.pointAt(u2) - p2);
            if( d2 < dist2 && u2 >= 0 && u2 <= 1 )
            {
                line2 = line;
                c2 = u2;
                dist2 = d2;
            }
        }
        
        if(line1 && line2 && line1 != line2)
        {
            m_currentShape->addLine(new ComicBoxesLine( line1, c1, line2, c2));
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
