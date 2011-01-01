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

#define HANDLE_SIZE 3
#define HANDLE_POINT QPointF(3, 3)

inline bool near(qreal a, qreal b)
{
    return qAbs(a - b) < 1e-6;
}

inline bool near(const QPointF& a, const QPointF& b, qreal d)
{
    return (a - b).manhattanLength() < d;
}

inline qreal norm2_2(const QPointF& pt)
{
    return (pt.x() * pt.x() + pt.y() * pt.y());
}

inline qreal norm2(const QPointF& pt)
{
    return std::sqrt(norm2_2(pt));
}

// See http://local.wasp.uwa.edu.au/~pbourke/geometry/pointline/
inline qreal projection(const QLineF& l, const QPointF& pt_1 )
{
    qreal top = (pt_1.x() - l.p1().x()) * (l.p2().x() - l.p1().x()) + (pt_1.y() - l.p1().y()) * (l.p2().y() - l.p1().y());
    return top / norm2_2(l.p2() - l.p1());
}

ComicBoxesTool::ComicBoxesTool(KoCanvasBase *canvas) : KoToolBase(canvas), m_mode(NOTHING)
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
    switch(m_mode)
    {
    case DRAGING_NEW_LINE:
        painter.setPen(Qt::black);
        painter.drawLine(converter.documentToView(m_currentStartingPoint),
                        converter.documentToView(m_currentPoint) );
        break;
    default:
        break;
    }
    
    // Draw the handles
    if(m_currentShape)
    {
        QTransform t = m_currentShape->lines2ShapeTransform();
        foreach(ComicBoxesLine* line, m_currentShape->lines())
        {
            if(line->isEditable())
            {
                QLineF l = line->line();
                painter.setPen(Qt::black);
                if(line == m_currentLine && m_currentPointOnTheLine == POINT_1)
                {
                    painter.setBrush(Qt::black);
                } else {
                    painter.setBrush(Qt::white);
                }
                painter.drawEllipse(converter.documentToView(t.map(l.p1())), HANDLE_SIZE, HANDLE_SIZE);
                if(line == m_currentLine && m_currentPointOnTheLine == POINT_2)
                {
                    painter.setBrush(Qt::black);
                } else {
                    painter.setBrush(Qt::white);
                }
                painter.drawEllipse(converter.documentToView(t.map(l.p2())), HANDLE_SIZE, HANDLE_SIZE);
            }
        }
    }
  
}

void ComicBoxesTool::mousePressEvent( KoPointerEvent *event )
{
    QPair<ComicBoxesLine*, Point> ptn = pointNear(event->point);
    
    if(ptn.first)
    {        
        m_currentLine = ptn.first;
        m_currentPointOnTheLine = ptn.second;
        Q_ASSERT(m_currentPointOnTheLine != POINT_NONE);
        
        if(event->modifiers() & Qt::ShiftModifier)
        {
            canvas()->updateCanvas(rectForCurrentLine());
            m_currentShape->removeLine(m_currentLine);
            m_currentLine = 0;
            m_currentPointOnTheLine = POINT_NONE;
            m_mode = NOTHING;
        } else {
            m_mode = DRAGING_POINT;
        }        
    } else {
        m_mode = DRAGING_NEW_LINE;
        m_currentStartingPoint = event->point;
    }
}

void ComicBoxesTool::mouseMoveEvent( KoPointerEvent *event )
{
    switch(m_mode)
    {
    case DRAGING_NEW_LINE:
        canvas()->updateCanvas(currentDraggingRect());
        m_currentPoint = event->point;
        if(event->modifiers() & Qt::SHIFT) makeVerticalOrHorizontal(m_currentStartingPoint, m_currentPoint);
        canvas()->updateCanvas(currentDraggingRect());
        break;
    case DRAGING_POINT:
    {
        QTransform t = m_currentShape->lines2ShapeTransform();
        QLineF line = m_currentLine->line();
        canvas()->updateCanvas(QRectF(t.map(line.p1()), t.map(line.p2())).normalized().adjusted(-HANDLE_SIZE, -HANDLE_SIZE, HANDLE_SIZE, HANDLE_SIZE));
        
        switch(m_currentPointOnTheLine)
        {
        case POINT_1:
            m_currentLine->setC1(qBound<qreal>(0, projection(m_currentLine->line1()->line(), t.inverted().map(event->point)), 1 ) );
            break;
        case POINT_2:
            m_currentLine->setC2(qBound<qreal>(0, projection(m_currentLine->line2()->line(), t.inverted().map(event->point)), 1) );
            break;
        case POINT_NONE:
            qFatal("Impossible");
        }
        m_currentShape->recreatePath();
        line = m_currentLine->line();
        canvas()->updateCanvas(QRectF(t.map(line.p1()), t.map(line.p2())).normalized().adjusted(-HANDLE_SIZE, -HANDLE_SIZE, HANDLE_SIZE, HANDLE_SIZE));
        break;
    }
    default:
    {
        QPair<ComicBoxesLine*, Point> lp = pointNear(event->point);
        m_currentLine = lp.first;
        m_currentPointOnTheLine = lp.second;
        canvas()->updateCanvas(QRectF(event->point - 2 * HANDLE_POINT, event->point + 2 * HANDLE_POINT) );
        break;
    }
    }
}

void ComicBoxesTool::mouseReleaseEvent( KoPointerEvent *event )
{
    switch(m_mode)
    {
    case DRAGING_NEW_LINE:
    {
        m_mode = NOTHING;
        
        QPointF p1 = m_currentStartingPoint - m_currentShape->boundingRect().topLeft();
        QPointF p2 = event->point;
        if(event->modifiers() & Qt::SHIFT) makeVerticalOrHorizontal(m_currentStartingPoint, p2);
        p2 -= m_currentShape->boundingRect().topLeft();
        
        p1 = QPointF(p1.x() / m_currentShape->boundingRect().width(), p1.y() / m_currentShape->boundingRect().height() );
        p2 = QPointF(p2.x() / m_currentShape->boundingRect().width(), p2.y() / m_currentShape->boundingRect().height() );
        
        ComicBoxesLine* line1 = 0;
        ComicBoxesLine* line2 = 0;
        qreal c1 = 0, c2 = 0;
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
        
        
        break;
    }
    case DRAGING_POINT:
    {
        m_mode = NOTHING;
        QLineF l = m_currentLine->line();
        QTransform t = m_currentShape->lines2ShapeTransform();
        QPointF pt = (m_currentPointOnTheLine == POINT_1) ? t.map(l.p1()) : t.map(l.p2());
        canvas()->updateCanvas(QRectF(pt - HANDLE_POINT, pt + HANDLE_POINT) );
        m_currentLine = 0;
        m_currentPointOnTheLine = POINT_NONE;
    }
        break;
    default:
        break;
    }
}

QPair<ComicBoxesLine*, ComicBoxesTool::Point> ComicBoxesTool::pointNear(const QPointF& point)
{
    QTransform t = m_currentShape->lines2ShapeTransform();
    
    foreach(ComicBoxesLine* line, m_currentShape->lines())
    {
        QLineF l = line->line();
        if(near(point, t.map(l.p1()), HANDLE_SIZE))
        {
            return QPair<ComicBoxesLine*, Point>(line, POINT_1);
        }
        if(near(point, t.map(l.p2()), HANDLE_SIZE))
        {
            return QPair<ComicBoxesLine*, Point>(line, POINT_2);
        }
    }
    
    return QPair<ComicBoxesLine*, Point>(0, POINT_NONE);
}

QRectF ComicBoxesTool::currentDraggingRect() const
{
    return QRectF(m_currentStartingPoint, m_currentPoint).normalized();
}

QMap<QString, QWidget *> ComicBoxesTool::createOptionWidgets() {
  QMap<QString, QWidget *> widgets;
  return widgets;
}

QRectF ComicBoxesTool::rectForCurrentLine() const
{
    QTransform t = m_currentShape->lines2ShapeTransform();
    QLineF line = m_currentLine->line();
    return QRectF(t.map(line.p1()), t.map(line.p2())).normalized();
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
