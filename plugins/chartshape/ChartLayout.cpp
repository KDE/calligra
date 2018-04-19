/* This file is part of the KDE project

   Copyright 2017 Dag Andersen <danders@get2net.dk>
   Copyright 2010 Johannes Simon <johannes.simon@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

// Qt
#include <QRectF>
#include <QTransform>

// KoChart
#include "ChartLayout.h"
#include "Legend.h"
#include "ChartDebug.h"
#include "PlotArea.h"
#include "Axis.h"
#include "ScreenConversions.h"

// Calligra
#include <KoShapeContainer.h>

using namespace KoChart;

class ChartLayout::LayoutData
{
public:
    int  itemType;
    Position pos;
    bool clipped;
    bool inheritsTransform;
    int alignment;

    LayoutData(int _itemType, Position _pos)
        : itemType(_itemType)
        , pos(_pos)
        , clipped(true)
        , inheritsTransform(true)
        , alignment(Qt::AlignCenter)
    {}
};

ChartLayout::ChartLayout()
    : m_doingLayout(false)
    , m_relayoutScheduled(false)
    , m_hMargin(5)
    , m_vMargin(5)
    , m_autoLayoutEnabled(true)
{
    m_spacing = QPointF(m_hMargin, m_vMargin);
}

ChartLayout::~ChartLayout()
{
    foreach(LayoutData *data, m_layoutItems.values())
        delete data;
}

void ChartLayout::add(KoShape *shape)
{
    Q_ASSERT(!m_layoutItems.contains(shape));
    m_layoutItems.insert(shape, new LayoutData(GenericItemType, FloatingPosition));
    scheduleRelayout();
}

void ChartLayout::add(KoShape *shape, Position pos, int itemType)
{
    Q_ASSERT(!m_layoutItems.contains(shape));
    m_layoutItems.insert(shape, new LayoutData(itemType, pos));
    scheduleRelayout();
}

void ChartLayout::remove(KoShape *shape)
{
    if (m_layoutItems.contains(shape)) {
        // delete LayoutData
        delete m_layoutItems.value(shape);
        m_layoutItems.remove(shape);
        scheduleRelayout();
    }
}

void ChartLayout::setClipped(const KoShape *shape, bool clipping)
{
    Q_ASSERT(m_layoutItems.contains(const_cast<KoShape*>(shape)));
    m_layoutItems.value(const_cast<KoShape*>(shape))->clipped = clipping;
}

bool ChartLayout::isClipped(const KoShape *shape) const
{
    Q_ASSERT(m_layoutItems.contains(const_cast<KoShape*>(shape)));
    return m_layoutItems.value(const_cast<KoShape*>(shape))->clipped;
}

void ChartLayout::setInheritsTransform(const KoShape *shape, bool inherit)
{
    m_layoutItems.value(const_cast<KoShape*>(shape))->inheritsTransform = inherit;
}

bool ChartLayout::inheritsTransform(const KoShape *shape) const
{
    return m_layoutItems.value(const_cast<KoShape*>(shape))->inheritsTransform;
}

int ChartLayout::count() const
{
    return m_layoutItems.size();
}

QList<KoShape*> ChartLayout::shapes() const
{
    return m_layoutItems.keys();
}

void ChartLayout::setContainerSize(const QSizeF &size)
{
    if (size != m_containerSize) {
        m_containerSize = size;
        scheduleRelayout();
    }
}

void ChartLayout::containerChanged(KoShapeContainer *container, KoShape::ChangeType type)
{
    switch(type) {
    case KoShape::SizeChanged:
        setContainerSize(container->size());
        break;
    default:
        break;
    }
}

bool ChartLayout::isChildLocked(const KoShape *shape) const
{
    return shape->isGeometryProtected();
}

void ChartLayout::setPosition(const KoShape *shape, Position pos, int itemType)
{
    Q_ASSERT(m_layoutItems.contains(const_cast<KoShape*>(shape)));
    LayoutData *data = m_layoutItems.value(const_cast<KoShape*>(shape));
    data->pos = pos;
    data->itemType = itemType;
    scheduleRelayout();
}

void ChartLayout::setAutoLayoutEnabled(bool on)
{
    m_autoLayoutEnabled = on;
}

void ChartLayout::proposeMove(KoShape *child, QPointF &move)
{
    QRectF current = itemRect(child);
    QRectF newRect = current.adjusted(move.x(), move.y(), move.x(), move.y());
    if (newRect.left() < 0.0) {
        move.setX(-current.left());
    } else if (newRect.right() > m_containerSize.width()) {
        move.setX(m_containerSize.width() - current.right());
    }
    if (newRect.top() < 0.0) {
        move.setY(-current.top());
    } else if (newRect.bottom() > m_containerSize.height()) {
        move.setY(m_containerSize.height() - current.bottom());
    }
}

void ChartLayout::childChanged(KoShape *shape, KoShape::ChangeType type)
{
    Q_UNUSED(shape);

    // Do not relayout again if we're currently in the process of a relayout.
    // Repositioning a layout item or resizing it will result in a cull of this method.
    if (m_doingLayout)
        return;

    // This can be fine-tuned, but right now, simply everything will be re-layouted.
    switch (type) {
    case KoShape::PositionChanged:
    case KoShape::SizeChanged:
        scheduleRelayout();
    // FIXME: There's some cases that would require relayouting but that don't make sense
    // for chart items, e.g. ShearChanged. How should these changes be avoided or handled?
    default:
        break;
    }
}

void ChartLayout::scheduleRelayout()
{
    m_relayoutScheduled = true;
}

void ChartLayout::layout()
{
    Q_ASSERT(!m_doingLayout);

    if (!m_relayoutScheduled)
        return;

    m_doingLayout = true;

    QMap<int, KoShape*> top, bottom, start, end;
    KoShape *topStart    = 0;
    KoShape *bottomStart = 0;
    KoShape *topEnd      = 0;
    KoShape *bottomEnd   = 0;
    KoShape *p           = 0;

    if (m_autoLayoutEnabled) {
        QMap<KoShape*, LayoutData*>::const_iterator it;
        for (it = m_layoutItems.constBegin(); it != m_layoutItems.constEnd(); ++it) {
            KoShape *shape = it.key();
            if (!shape->isVisible()) {
                continue; // do nothing
            }
            LayoutData *data = it.value();
            switch (data->pos) {
            case TopPosition:
                top.insert(data->itemType, shape);
                break;
            case BottomPosition:
                bottom.insert(data->itemType, shape);
                break;
            case StartPosition:
                start.insert(data->itemType, shape);
                break;
            case EndPosition:
                end.insert(data->itemType, shape);
                break;
            case TopStartPosition:
                topStart = shape;
                break;
            case BottomStartPosition:
                bottomStart = shape;
                break;
            case TopEndPosition:
                topEnd = shape;
                break;
            case BottomEndPosition:
                bottomEnd = shape;
                break;
            case CenterPosition:
                p = shape;
                break;
            case FloatingPosition:
                // Nothing to do
                break;
            }
        }
    }

    qreal topY = layoutTop(top, p);
    qreal bottomY = layoutBottom(bottom, p);
    qreal startX = layoutStart(start, p);
    qreal endX = layoutEnd(end, p);
    if (p) {
        setItemPosition(p, QPointF(startX, topY));
        p->setSize(QSizeF(endX - startX, bottomY - topY));
    }

    layoutTopStart(topStart, p);
    layoutBottomStart(bottomStart, p);
    layoutTopEnd(topEnd, p);
    layoutBottomEnd(bottomEnd, p);

    m_doingLayout = false;
    m_relayoutScheduled = false;
}



qreal ChartLayout::layoutTop(const QMap<int, KoShape*>& shapes, KoShape *p)
{
    qreal top = m_vMargin;
    qreal pX = m_containerSize.width() / 2.0; // default
    foreach(KoShape *shape, shapes) {
        QRectF itmRect = itemRect(shape);
        switch (m_layoutItems[shape]->itemType) {
            case LegendType:
                if (p) {
                    // default is AlignCenter
                    QRectF pr = diagramArea(p);
                    pX = pr.top() + (pr.width() / 2.0);
                    // Legend may have horizontal alignment specified
                    if (static_cast<Legend*>(shape)->alignment() == Qt::AlignLeft) {
                        pX = pr.left() + (itmRect.width() / 2.0);
                    } else if (static_cast<Legend*>(shape)->alignment() == Qt::AlignRight) {
                        pX = pr.right() - (itmRect.width() / 2.0);
                    }
                }
                break;
            case SecondaryYAxisTitleType:
            case YAxisTitleType:
                // AlignCenter with plot area
                if (p) {
                    QRectF pr = diagramArea(p);
                    pX = pr.left() + (pr.width() / 2.0);
                }
                break;
            default:
                break;
        }
        setItemPosition(shape, QPointF(pX - itmRect.width() / 2.0, top));
        top += itmRect.height() + m_spacing.y();
    }
    return top;
}

qreal ChartLayout::layoutBottom(const QMap<int, KoShape*>& shapes, KoShape *p)
{
    qreal bottom = m_containerSize.height() - m_vMargin;
    qreal pX = m_containerSize.width() / 2.0;
    foreach(KoShape *shape, shapes) {
        QRectF itmRect = itemRect(shape);
        switch (m_layoutItems[shape]->itemType) {
            case LegendType:
                if (p) {
                    // default is AlignCenter
                    QRectF pr = diagramArea(p);
                    pX = pr.left() + (pr.width() / 2.0);
                    // Legend may have horizontal alignment specified
                    if (static_cast<Legend*>(shape)->alignment() == Qt::AlignLeft) {
                        pX = pr.left() + (itmRect.width() / 2.0);
                    } else if (static_cast<Legend*>(shape)->alignment() == Qt::AlignRight) {
                        pX = pr.right() - (itmRect.width() / 2.0);
                    }
                }
                break;
            case SecondaryYAxisTitleType:
            case YAxisTitleType:
                // AlignCenter with plot area
                if (p) {
                    QRectF pr = diagramArea(p);
                    pX = pr.left() + (pr.width() / 2.0);
                }
                break;
            default:
                break;
        }
        bottom -= itmRect.height();
        setItemPosition(shape, QPointF(pX - itmRect.width() / 2.0, bottom));
        bottom -= m_spacing.y();
    }
    return bottom;
}

qreal ChartLayout::layoutStart(const QMap<int, KoShape*>& shapes, KoShape *p)
{
    qreal start = m_hMargin;
    qreal pY = m_containerSize.height() / 2.0;
    foreach(KoShape *shape, shapes) {
        QRectF itmRect = itemRect(shape);
        switch (m_layoutItems[shape]->itemType) {
            case LegendType:
                if (p) {
                    // default is AlignCenter
                    QRectF pr = diagramArea(p);
                    pY = pr.top() + (pr.height() / 2.0);
                    // Legend may have vertical alignment specified
                    if (static_cast<Legend*>(shape)->alignment() == Qt::AlignLeft) {
                        pY = pr.top() + (itmRect.height() / 2.0);
                    } else if (static_cast<Legend*>(shape)->alignment() == Qt::AlignRight) {
                        pY = pr.bottom() - (itmRect.height() / 2.0);
                    }
                }
                break;
            case SecondaryYAxisTitleType:
            case YAxisTitleType:
                // AlignCenter with plot area
                if (p) {
                    QRectF pr = diagramArea(p);
                    pY = pr.top() + (pr.height() / 2.0);
                }
                break;
            default:
                break;
        }
        setItemPosition(shape, QPointF(start, pY - itmRect.height() / 2.0));
        start += itmRect.width() + m_spacing.x();
    }
    return start;
}

qreal ChartLayout::layoutEnd(const QMap<int, KoShape*>& shapes, KoShape *p)
{
    qreal end = m_containerSize.width() - m_hMargin;
    qreal pY = m_containerSize.height() / 2.0;
    foreach(KoShape *shape, shapes) {
        QRectF itmRect = itemRect(shape);
        switch (m_layoutItems[shape]->itemType) {
            case LegendType:
                if (p) {
                    // default is AlignCenter
                    QRectF pr = diagramArea(p);
                    pY = pr.top() + pr.height() / 2.0;
                    // Legend may have vertical alignment specified
                    if (static_cast<Legend*>(shape)->alignment() == Qt::AlignLeft) {
                        pY = pr.top() + (itmRect.height() / 2.0);
                    } else if (static_cast<Legend*>(shape)->alignment() == Qt::AlignRight) {
                        pY = pr.bottom() - (itmRect.height() / 2.0);
                    }
                }
                break;
            case SecondaryYAxisTitleType:
            case YAxisTitleType:
                // AlignCenter with plot area
                if (p) {
                    QRectF pr = diagramArea(p);
                    pY = pr.top() + (pr.height() / 2.0);
                }
                break;
            default:
                break;
        }
        end -= itmRect.width();
        setItemPosition(shape, QPointF(end, pY - itmRect.height() / 2.0));
        end -= m_spacing.x();
    }
    return end;
}

void ChartLayout::layoutTopStart(KoShape *shape, KoShape *p)
{
    // TODO: shape could change size of other areas
    Q_UNUSED(p);

    if (!shape)
        return;
    setItemPosition(shape, QPointF(m_hMargin, m_vMargin));
}

void ChartLayout::layoutBottomStart(KoShape *shape, KoShape *p)
{
    // TODO: shape could change size of other areas
    Q_UNUSED(p);

    if (!shape)
        return;
    setItemPosition(shape, QPointF(0, m_containerSize.height() - itemSize(shape).height()));
}

void ChartLayout::layoutTopEnd(KoShape *shape, KoShape *p)
{
    // TODO: shape could change size of other areas
    Q_UNUSED(p);

    if (!shape)
        return;
    setItemPosition(shape, QPointF(m_containerSize.width() - itemSize(shape).width() - m_hMargin, m_vMargin));
}

void ChartLayout::layoutBottomEnd(KoShape *shape, KoShape *p)
{
    // TODO: shape could change size of other areas
    Q_UNUSED(p);

    if (!shape)
        return;
    const QSizeF size = itemSize(shape);
    setItemPosition(shape, QPointF(m_containerSize.width()  - size.width() - m_hMargin,
                                     m_containerSize.height() - size.height() - m_vMargin));
}

void ChartLayout::setMargins(qreal hMargin, qreal vMargin)
{
    m_vMargin = vMargin;
    m_hMargin = hMargin;
    scheduleRelayout();
}

QPointF ChartLayout::margins() const
{
    return QPointF(m_vMargin, m_hMargin);
}

void ChartLayout::setSpacing(qreal hSpacing, qreal vSpacing)
{
    m_spacing = QPointF(hSpacing, vSpacing);
}

QPointF ChartLayout::spacing() const
{
    return m_spacing;
}

QMap<KoShape*, QRectF> ChartLayout::calculateLayout(const KoShape *shape, bool show) const
{
    Q_ASSERT(m_layoutItems.contains(const_cast<KoShape*>(shape)));

    QMap<KoShape*, QRectF> result;

    KoShape *plotArea = 0;
    for (LayoutData *l : m_layoutItems) {
        if (l->itemType == PlotAreaType) {
            plotArea = m_layoutItems.key(l);
            break;
        }
    }
    Q_ASSERT(plotArea);
    Q_ASSERT(shape != plotArea);

    // find which area the shape is/should be in
    Position area = m_layoutItems[const_cast<KoShape*>(shape)]->pos;
    if (shape->isVisible()) {
        // shape may have been moved into a different area than the default area
        area = itemArea(shape, plotArea);
    }
    debugChartLayout<<(show?"Show:":"Hide:")<<dbg(shape)<<"in area:"<<area;
    switch(area) {
        case StartPosition:
            result = calculateLayoutStart(const_cast<KoShape*>(shape), plotArea, !show);
            break;
        case TopPosition:
            result = calculateLayoutTop(const_cast<KoShape*>(shape), plotArea, !show);
            break;
        case EndPosition:
            result = calculateLayoutEnd(const_cast<KoShape*>(shape), plotArea, !show);
            break;
        case BottomPosition:
            result = calculateLayoutBottom(const_cast<KoShape*>(shape), plotArea, !show);
            break;
        case TopStartPosition:
            result = calculateLayoutTopStart(const_cast<KoShape*>(shape), plotArea, !show);
            break;
        case TopEndPosition:
            result = calculateLayoutTopEnd(const_cast<KoShape*>(shape), plotArea, !show);
            break;
        case BottomStartPosition:
            result = calculateLayoutBottomStart(const_cast<KoShape*>(shape), plotArea, !show);
            break;
        case BottomEndPosition:
            result = calculateLayoutBottomEnd(const_cast<KoShape*>(shape), plotArea, !show);
            break;
        case CenterPosition:
            debugChartLayout<<"Center";
            break; // When shape intersects center area: do nothing
        default:
            break;
    }
    debugChartLayout<<Q_FUNC_INFO<<result;
    return result;
}

QMap<KoShape*, QRectF> ChartLayout::calculateLayoutTop(KoShape *shape, KoShape *plotArea, bool hide) const
{
    debugChartLayout<<Q_FUNC_INFO<<dbg(shape);
    QRectF area = layoutArea(TopPosition, plotArea);
    QRectF plotAreaRect(itemRect(plotArea));
    QMap<KoShape*, QRectF> newlayout;
    QMap<KoShape*, QRectF> oldlayout;
    QMap<KoShape*, LayoutData*>::const_iterator it;
    for (it = m_layoutItems.constBegin(); it != m_layoutItems.constEnd(); ++it) {
        if (it.key()->isVisible() || it.key() == shape) {
            oldlayout.insert(it.key(), itemRect(it.key()));
        }
    }

    if (hide) {
        // This shape shall be hidden.
        // If appropriate, move other shapes into freed space and
        // if the freed space can be utilized: resize the plot area

        // sort visible shapes by type
        QMap<int, KoShape*> sortedShapes;
        QMap<KoShape*, LayoutData*>::const_iterator it;
        for (it = m_layoutItems.constBegin(); it != m_layoutItems.constEnd(); ++it) {
            if (it.key()->isVisible() && it.key() != shape && it.value()->itemType != PlotAreaType) {
                sortedShapes.insert(it.value()->itemType, it.key());
            }
        }
        QRectF itmRect(itemRect(shape));
        debugChartLayout<<"item"<<dbg(shape)<<"at"<<itmRect;
        if (itmRect.intersects(area)) {
            // everything below this item can be moved up
            qreal yoffset = itmRect.height() + m_spacing.y();
            area.setTop(itmRect.bottom());
            for (KoShape *s : sortedShapes) {
                QRectF r(itemRect(s));
                if (r.intersects(area)) {
                    r.moveTop(r.top() - yoffset);
                    newlayout[s] = r;
                }
            }
            area.setBottom(area.bottom() - yoffset);
            debugChartLayout<<"area rect"<<area;
        }
        if (area.bottom() < plotAreaRect.top()) {
            // plot area can be moved/expanded
            plotAreaRect.setTop(area.bottom());
            newlayout[plotArea] = plotAreaRect;
        }
    } else {
        // This shape shall be shown.
        // If neccessary, make space for this shape by moving shapes and resize plotArea shape

        // sort shapes by y position
        QMap<int, KoShape*> sortedShapes;
        QMap<KoShape*, LayoutData*>::const_iterator it;
        for (it = m_layoutItems.constBegin(); it != m_layoutItems.constEnd(); ++it) {
            if (it.key() != shape && it.value()->itemType != PlotAreaType) {
                sortedShapes.insert(itemPosition(it.key()).y(), it.key());
            }
        }
        // First get all shapes in this area
        QList<KoShape*> shapesInArea;
        for (KoShape *s : sortedShapes) {
            if (s->isVisible()) {
                QRectF sr = itemRect(s);
                if (sr.intersects(area)) {
                    shapesInArea << s;
                }
            }
        }
        debugChartLayout<<"in area:"<<area<<dbg(shapesInArea);
        // calculate where to put shape
        QRectF itmRect(itemRect(shape));
        debugChartLayout<<"item"<<dbg(shape)<<"at"<<itmRect;
        itmRect.moveLeft(itemDefaultPosition(shape, itmRect.left(), itmRect.left(), itmRect.right(), area.left(), area.right()));
        itmRect.moveTop(area.top() + m_vMargin);
        for (QMutableListIterator<KoShape*> it(shapesInArea); it.hasNext();) {
            KoShape *s = it.next();
            if (m_layoutItems[s]->itemType < m_layoutItems[shape]->itemType) {
                itmRect.moveTop(itemRect(s).bottom() + m_spacing.y());
                it.remove(); // this shall not be moved
            }
        }
        if (itemRect(shape) != itmRect) {
            newlayout.insert(shape, itmRect);
            debugChartLayout<<"insert"<<dbg(shape)<<itmRect;
        }
        area.setTop(itmRect.bottom() + m_spacing.y());
        for (KoShape *s : shapesInArea) {
            QRectF sr = itemRect(s);
            sr.moveTop(area.top());
            newlayout[s] = sr;
            debugChartLayout<<"shift in area:"<<dbg(s)<<sr;
            area.setTop(sr.bottom() + m_spacing.y());
        }
        debugChartLayout<<"plot area"<<plotAreaRect<<"area"<<area<<area.top()<<area.intersects(plotAreaRect);
        if (area.top() != plotAreaRect.top()) {
            plotAreaRect.setTop(area.top());
            newlayout[plotArea] = plotAreaRect;
        }
        // check legend alignment
        if (m_layoutItems[shape]->itemType == LegendType) {
            QRectF n = diagramArea(plotArea, plotAreaRect);
            qreal xpos = itmRect.left();
            switch (static_cast<Legend*>(shape)->alignment()) {
                case Qt::AlignLeft:
                    xpos = n.left();
                    break;
                case Qt::AlignCenter:
                    xpos = n.left() + ((n.width() - itmRect.width()) * 0.5);
                    break;
                case Qt::AlignRight:
                    xpos = n.right() - itmRect.width();
                    break;
                default:
                    break;
            }
            if (xpos != itmRect.left()) {
                itmRect.moveLeft(xpos);
                newlayout[shape] = itmRect;
                debugChartLayout<<"aligned:"<<dbg(shape)<<itmRect;
            }
        }
    }
    if (newlayout.contains(plotArea)) {
        // Move axis titles/legend relative plot area
        // TODO: check that axis/legend is actually in one of the correct areas
        const QRectF newPARect = newlayout[plotArea];
        const QRectF oldPARect = oldlayout[plotArea];
        QMap<KoShape*, QRectF>::const_iterator i;
        for (i = oldlayout.constBegin(); i != oldlayout.constEnd(); ++i) {
            LayoutData *data = m_layoutItems[i.key()];
            if (data->itemType == LegendType || data->itemType == YAxisTitleType || data->itemType == SecondaryYAxisTitleType) {
                QRectF r = i.value();
                qreal ypos = relativePosition(oldPARect.top(), oldPARect.height(), newPARect.top(), newPARect.height(), r.top(), r.height());
                debugChartLayout<<"check:"<<dbg(i.key())<<r.bottom()<<ypos;
                if (ypos != r.top()) {
                    r.moveTop(ypos);
                    newlayout[i.key()] = r;
                    debugChartLayout<<"moved:"<<dbg(i.key())<<r;
                }
            }
        }
    }
    QMap<KoShape*, QRectF> result;
    QMap<KoShape*, QRectF>::const_iterator i;
    for (i = newlayout.constBegin(); i != newlayout.constEnd(); ++i) {
        if (i.value() != oldlayout[i.key()]) {
            QPointF poffset = i.value().topLeft() - oldlayout[i.key()].topLeft();
            QSizeF soffset = i.value().size() - oldlayout[i.key()].size();
            result.insert(i.key(), QRectF(i.key()->position() + poffset, i.key()->size() + soffset));
        } else qWarning()<<"in newlayout, not moved?"<<dbg(i.key());
    }
    return result;
}

QMap<KoShape*, QRectF> ChartLayout::calculateLayoutBottom(KoShape *shape, KoShape *plotArea, bool hide) const
{
    debugChartLayout<<Q_FUNC_INFO;
    QRectF area = layoutArea(BottomPosition, plotArea);
    QRectF plotAreaRect(itemRect(plotArea));
    QMap<KoShape*, QRectF> newlayout;
    QMap<KoShape*, QRectF> oldlayout;
    QMap<KoShape*, LayoutData*>::const_iterator it;
    for (it = m_layoutItems.constBegin(); it != m_layoutItems.constEnd(); ++it) {
        if (it.key()->isVisible() || it.key() == shape) {
            oldlayout.insert(it.key(), itemRect(it.key()));
        }
    }
    if (hide) {
        // This shape shall be hidden.
        // If appropriate, move other shapes into freed space and
        // if the freed space can be utilized: resize the plot area

        // sort visible shapes by type, descending
        QMap<int, KoShape*> sortedShapes;
        QMap<KoShape*, LayoutData*>::const_iterator it;
        for (it = m_layoutItems.constBegin(); it != m_layoutItems.constEnd(); ++it) {
            if (it.key()->isVisible() && it.key() != shape && it.value()->itemType != PlotAreaType) {
                sortedShapes.insert(-(it.value()->itemType), it.key());
            }
        }
        QRectF itmRect(itemRect(shape));
        debugChartLayout<<"Hide item:"<<itmRect<<"area rect="<<area;
        if (itmRect.intersects(area)) {
            area.setBottom(itmRect.bottom());
            // everything above this item can be moved down
            for (KoShape *s : sortedShapes) {
                QRectF r(itemRect(s));
                if (r.intersects(area)) {
                    r.moveBottom(area.bottom());
                    newlayout[s] = r;
                    area.setBottom(r.top() - m_spacing.y());
                }
            }
            debugChartLayout<<"Hidden: area rect="<<area;
            if (area.bottom() > plotAreaRect.bottom()) {
                // plot area can be expanded
                plotAreaRect.setBottom(area.bottom());
                newlayout[plotArea] = plotAreaRect;
            }
        } else debugChartLayout<<"Shape is not in area rect="<<area<<itmRect;
    } else {
        // This shape shall be shown.
        // If neccessary, make space for this shape by moving shapes and resize plotArea shape

        // sort shapes by y position, descending
        QMap<int, KoShape*> sortedShapes;
        QMap<KoShape*, LayoutData*>::const_iterator it;
        for (it = m_layoutItems.constBegin(); it != m_layoutItems.constEnd(); ++it) {
            if (it.key() != shape && it.value()->itemType != PlotAreaType) {
                sortedShapes.insert(-(itemPosition(it.key()).y()), it.key());
            }
        }
        // First get all shapes in this area
        QList<KoShape*> shapesInArea;
        for (KoShape *s : sortedShapes) {
            if (s->isVisible()) {
                QRectF sr = itemRect(s);
                if (sr.intersects(area)) {
                    shapesInArea << s;
                }
            }
        }
        debugChartLayout<<"in area:"<<area<<shapesInArea;
        // calculate where to put shape
        QRectF itmRect(itemRect(shape));
        debugChartLayout<<"item"<<dbg(shape)<<"at"<<itmRect;
        itmRect.moveLeft(itemDefaultPosition(shape, itmRect.left(), itmRect.left(), itmRect.right(), area.left(), area.right()));
        itmRect.moveBottom(area.bottom() - m_vMargin);
        for (QMutableListIterator<KoShape*> it(shapesInArea); it.hasNext();) {
            KoShape *s = it.next();
            if (m_layoutItems[s]->itemType < m_layoutItems[shape]->itemType) {
                itmRect.moveBottom(itemRect(s).top() - m_spacing.y());
                it.remove(); // this shall not be moved
            }
        }
        if (itemRect(shape) != itmRect) {
            newlayout.insert(shape, itmRect);
            debugChartLayout<<"insert"<<dbg(shape)<<itmRect;
        }
        area.setBottom(itmRect.top() - m_spacing.y());
        for (KoShape *s : shapesInArea) {
            QRectF sr = itemRect(s);
            sr.moveBottom(area.bottom());
            newlayout[s] = sr;
            debugChartLayout<<"shift in area:"<<dbg(s)<<sr;
            area.setBottom(sr.top() - m_spacing.y());
        }
        debugChartLayout<<"plot area"<<plotAreaRect<<"area"<<area;
        if (area.bottom() != plotAreaRect.bottom()) {
            plotAreaRect.setBottom(area.bottom());
            newlayout[plotArea] = plotAreaRect;
        }
        // check legend alignment
        if (m_layoutItems[shape]->itemType == LegendType) {
            QRectF n = diagramArea(plotArea, plotAreaRect);
            qreal xpos = itmRect.left();
            switch (static_cast<Legend*>(shape)->alignment()) {
                case Qt::AlignLeft:
                    xpos = n.left();
                    break;
                case Qt::AlignCenter:
                    xpos = n.left() + ((n.width() - itmRect.width()) * 0.5);
                    break;
                case Qt::AlignRight:
                    xpos = n.right() - itmRect.width();
                    break;
                default:
                    break;
            }
            if (xpos != itmRect.left()) {
                itmRect.moveLeft(xpos);
                newlayout[shape] = itmRect;
                debugChartLayout<<"aligned:"<<dbg(shape)<<itmRect;
            }
        }
    }
    if (newlayout.contains(plotArea)) {
        // Move axis titles/legend relative plot area
        const QRectF newPARect = newlayout[plotArea];
        const QRectF oldPARect = oldlayout[plotArea];
        QMap<KoShape*, QRectF>::const_iterator i;
        for (i = oldlayout.constBegin(); i != oldlayout.constEnd(); ++i) {
            LayoutData *data = m_layoutItems[i.key()];
            if (data->itemType == LegendType || data->itemType == YAxisTitleType || data->itemType == SecondaryYAxisTitleType) {
                QRectF r = oldlayout[i.key()];
                qreal ypos = relativePosition(oldPARect.top(), oldPARect.height(), newPARect.top(), newPARect.height(), r.top(), r.height());
                debugChartLayout<<"check:"<<dbg(i.key())<<r.top()<<ypos;
                if (ypos != r.top()) {
                    r.moveTop(ypos);
                    newlayout[i.key()] = r;
                    debugChartLayout<<"moved:"<<dbg(i.key())<<r;
                }
            }
        }
    }
    QMap<KoShape*, QRectF> result;
    QMap<KoShape*, QRectF>::const_iterator i;
    for (i = newlayout.constBegin(); i != newlayout.constEnd(); ++i) {
        if (i.value() != oldlayout[i.key()]) {
            QPointF poffset = i.value().topLeft() - oldlayout[i.key()].topLeft();
            QSizeF soffset = i.value().size() - oldlayout[i.key()].size();
            result.insert(i.key(), QRectF(i.key()->position() + poffset, i.key()->size() + soffset));
        } else qWarning()<<"in newlayout, not moved?"<<dbg(i.key());
    }
    return result;
}

QMap<KoShape*, QRectF> ChartLayout::calculateLayoutStart(KoShape *shape, KoShape *plotArea, bool hide) const
{
    debugChartLayout<<Q_FUNC_INFO;
    QRectF area = layoutArea(StartPosition, plotArea);
    debugChartLayout<<"plot area="<<itemRect(plotArea)<<"area="<<area;
    QRectF plotAreaRect = itemRect(plotArea);
    QMap<KoShape*, QRectF> newlayout;
    QMap<KoShape*, QRectF> oldlayout;
    {QMap<KoShape*, LayoutData*>::const_iterator it;
    for (it = m_layoutItems.constBegin(); it != m_layoutItems.constEnd(); ++it) {
        if (it.key()->isVisible() || it.key() == shape) {
            oldlayout.insert(it.key(), itemRect(it.key()));
        }
    }}
    if (hide) {
        // shape shall be hidden

        // sort visible shapes by type, ascending
        QMap<int, KoShape*> sortedShapes;
        QMap<KoShape*, LayoutData*>::const_iterator it;
        for (it = m_layoutItems.constBegin(); it != m_layoutItems.constEnd(); ++it) {
            if (it.key()->isVisible() && it.key() != shape && it.value()->itemType != PlotAreaType) {
                sortedShapes.insert(it.value()->itemType, it.key());
            }
        }
        QRectF itmRect(itemRect(shape));
        debugChartLayout<<"Hide item:"<<itmRect<<"area rect="<<area;
        if (itmRect.intersects(area)) {
            area.setLeft(itmRect.left());
            // everything right of this item can be moved left
            for (KoShape *s : sortedShapes) {
                QRectF r(itemRect(s));
                if (r.intersects(area)) {
                    r.moveLeft(area.left());
                    newlayout[s] = r;
                    area.setLeft(r.right() + m_spacing.x());
                }
            }
            debugChartLayout<<"Hidden: area rect="<<area;
            if (area.left() < plotAreaRect.left()) {
                // plot area can be expanded
                plotAreaRect.setLeft(area.left());
                newlayout[plotArea] = plotAreaRect;
            }
        } else debugChartLayout<<"Shape is not in area rect="<<area<<itmRect;
    } else {
        // shape shall be shown
        // If neccessary, make space for this shape by moving shapes and resize plotArea shape

        // sort shapes by x position, asscending
        QMap<int, KoShape*> sortedShapes;
        {QMap<KoShape*, LayoutData*>::const_iterator it;
        for (it = m_layoutItems.constBegin(); it != m_layoutItems.constEnd(); ++it) {
            if (it.key()->isVisible() && it.key() != plotArea) {
                sortedShapes.insert(itemPosition(it.key()).x(), it.key());
            }
        }}
        // First get all shapes in this area
        QList<KoShape*> shapesInArea;
        for (KoShape *s : sortedShapes) {
            if (s->isVisible()) {
                QRectF sr = itemRect(s);
                if (sr.intersects(area)) {
                    shapesInArea << s;
                }
            }
        }
        debugChartLayout<<"in area:"<<shapesInArea;
        // calculate where to put shape
        QRectF itmRect(itemRect(shape));
        itmRect.moveTop(itemDefaultPosition(shape, itmRect.top(), itmRect.top(), itmRect.bottom(), area.top(), area.bottom()));
        itmRect.moveLeft(area.left() + m_hMargin);
        for (QMutableListIterator<KoShape*> it(shapesInArea); it.hasNext();) {
            KoShape *s = it.next();
            if (m_layoutItems[s]->itemType < m_layoutItems[shape]->itemType) {
                itmRect.moveLeft(itemRect(s).right() + m_spacing.x());
                it.remove(); // this shall not be moved
            }
        }
        if (itemRect(shape) != itmRect) {
            newlayout.insert(shape, itmRect);
            debugChartLayout<<"insert"<<dbg(shape)<<itmRect;
        }
        area.setRight(itmRect.right() + m_spacing.x());
        // Move other shapes
        for (KoShape *s : shapesInArea) {
            QRectF sr = itemRect(s);
            sr.moveLeft(area.right());
            newlayout[s] = sr;
            debugChartLayout<<"shift in area:"<<dbg(s)<<sr;
            area.setRight(sr.right() + m_spacing.x());
        }
        debugChartLayout<<"plot area"<<plotAreaRect<<"area"<<area;
        if (area.right() != plotAreaRect.left()) {
            // resize plot area
            plotAreaRect.setLeft(area.right());
            newlayout[plotArea] = plotAreaRect;
        }
        // check legend alignment
        if (m_layoutItems[shape]->itemType == LegendType) {
            QRectF n = diagramArea(plotArea, plotAreaRect);
            qreal ypos = itmRect.top();
            switch (static_cast<Legend*>(shape)->alignment()) {
                case Qt::AlignLeft:
                    ypos = n.top();
                    break;
                case Qt::AlignCenter:
                    ypos = n.top() + ((n.height() - itmRect.height()) * 0.5);
                    break;
                case Qt::AlignRight:
                    ypos = n.bottom() - itmRect.height();
                    break;
                default:
                    break;
            }
            if (ypos != itmRect.top()) {
                itmRect.moveTop(ypos);
                newlayout[shape] = itmRect;
                debugChartLayout<<"aligned:"<<dbg(shape)<<itmRect;
            }
        }
    }
    if (newlayout.contains(plotArea)) {
        // handle axis titles/legend
        debugChartLayout<<"Handle axis titles"<<plotAreaRect;
        const QRectF oldPlotAreaRect = oldlayout[plotArea];
        const QRectF newPlotAreaRect = newlayout[plotArea];
        {QMap<KoShape*, QRectF>::const_iterator it;
        for (it = oldlayout.constBegin(); it != oldlayout.constEnd(); ++it) {
            LayoutData *data = m_layoutItems[it.key()];
            if ((data->itemType == LegendType && !it.key()->isVisible()) || data->itemType == XAxisTitleType || data->itemType == SecondaryXAxisTitleType) {
                QRectF r(it.value());
                debugChartLayout<<"move check:"<<dbg(it.key())<<r;
                qreal xpos = relativePosition(oldPlotAreaRect.left(), oldPlotAreaRect.width(), newPlotAreaRect.left(),  newPlotAreaRect.width(), r.left(), r.width());
                if (xpos != r.left()) {
                    r.moveLeft(xpos);
                    newlayout[it.key()] = r;
                    debugChartLayout<<"moved:"<<dbg(it.key())<<r;
                }
            }
        }}
    }
    QMap<KoShape*, QRectF> result;
    {QMap<KoShape*, QRectF>::const_iterator it;
    for (it = newlayout.constBegin(); it != newlayout.constEnd(); ++it) {
        const QRectF oldrect = oldlayout.value(it.key());
        if (it.value() != oldrect) {
            const QPointF poffset = it.value().topLeft() - oldrect.topLeft();
            const QSizeF soffset = it.value().size() - oldrect.size();
            result.insert(it.key(), QRectF(it.key()->position() + poffset, it.key()->size() + soffset));
        }
    }}
    return result;
}

QMap<KoShape*, QRectF> ChartLayout::calculateLayoutEnd(KoShape *shape, KoShape *plotArea, bool hide) const
{
    debugChartLayout<<Q_FUNC_INFO;
    QRectF area = layoutArea(EndPosition, plotArea);
    debugChartLayout<<"plot area="<<itemRect(plotArea)<<"area="<<area;
    QRectF plotAreaRect = itemRect(plotArea);
    QMap<KoShape*, QRectF> newlayout;
    QMap<KoShape*, QRectF> oldlayout;
    {QMap<KoShape*, LayoutData*>::const_iterator it;
    for (it = m_layoutItems.constBegin(); it != m_layoutItems.constEnd(); ++it) {
        if (it.key()->isVisible() || it.key() == shape) {
            oldlayout.insert(it.key(), itemRect(it.key()));
        }
    }}
    if (hide) {
        // This shape shall be hidden.
        // If appropriate, move other shapes into freed space and
        // if the freed space can be utilized: resize the plot area

        // sort visible shapes by type, descending
        QMap<int, KoShape*> sortedShapes;
        {QMap<KoShape*, LayoutData*>::const_iterator it;
        for (it = m_layoutItems.constBegin(); it != m_layoutItems.constEnd(); ++it) {
            if (it.key()->isVisible() && it.key() != shape && it.key() != plotArea) {
                sortedShapes.insert(-(it.value()->itemType), it.key());
            }
        }}
        QRectF itmRect = itemRect(shape);
        if (itmRect.intersects(area)) {
            area.setRight(itmRect.right());
            // everything left of this item can be moved right
            QRectF itmRect(itemRect(shape));
            debugChartLayout<<"Hide item:"<<itmRect<<"area rect="<<area;
            if (itmRect.intersects(area)) {
                area.setRight(itmRect.right());
                // everything left of this item can be moved right
                for (KoShape *s : sortedShapes) {
                    QRectF r(itemRect(s));
                    if (r.intersects(area)) {
                        r.moveRight(area.right());
                        newlayout[s] = r;
                        debugChartLayout<<"Move right:"<<dbg(s)<<r;
                        area.setRight(r.left() - m_spacing.y());
                    }
                }
            }
            debugChartLayout<<"Hidden: area rect="<<area;
            if (plotAreaRect.right() < area.right()) {
                // plot area can be expanded
                plotAreaRect.setRight(area.right());
                newlayout.insert(plotArea, plotAreaRect);
            }
        } else debugChartLayout<<"Shape is not in area rect="<<area<<itmRect;
    } else {
        // shape shall be shown
        // If neccessary, make space for this shape by moving shapes and resize plotArea shape

        // sort shapes by x position, descending
        QMap<int, KoShape*> sortedShapes;
        {QMap<KoShape*, LayoutData*>::const_iterator it;
        for (it = m_layoutItems.constBegin(); it != m_layoutItems.constEnd(); ++it) {
            if (it.key()->isVisible() && it.key() != plotArea) {
                sortedShapes.insert(-(itemPosition(it.key()).x()), it.key());
            }
        }}
        // First get all shapes in this area
        QList<KoShape*> shapesInArea;
        for (KoShape *s : sortedShapes) {
            if (s->isVisible()) {
                QRectF sr = itemRect(s);
                if (sr.intersects(area)) {
                    shapesInArea << s;
                }
            }
        }
        debugChartLayout<<"in area:"<<shapesInArea;
        // calculate where to put shape
        QRectF itmRect(itemRect(shape));
        itmRect.moveTop(itemDefaultPosition(shape, itmRect.top(), itmRect.top(), itmRect.bottom(), area.top(), area.bottom()));
        itmRect.moveRight(area.right() - m_hMargin);
        for (QMutableListIterator<KoShape*> it(shapesInArea); it.hasNext();) {
            KoShape *s = it.next();
            if (m_layoutItems[s]->itemType < m_layoutItems[shape]->itemType) {
                itmRect.moveRight(itemRect(s).left() - m_spacing.x());
                it.remove(); // this shall not be moved
            }
        }
        if (itemRect(shape) != itmRect) {
            newlayout.insert(shape, itmRect);
            debugChartLayout<<"insert"<<dbg(shape)<<itmRect;
        }
        area.setRight(itmRect.left() - m_spacing.x());
        for (KoShape *s : shapesInArea) {
            QRectF sr = itemRect(s);
            sr.moveLeft(area.right());
            newlayout[s] = sr;
            debugChartLayout<<"shift in area:"<<dbg(s)<<sr;
            area.setRight(sr.left() - m_spacing.x());
        }
        debugChartLayout<<"plot area"<<plotAreaRect<<"area"<<area;
        if (area.right() != plotAreaRect.right()) {
            plotAreaRect.setRight(area.right());
            newlayout[plotArea] = plotAreaRect;
        }
        // check legend alignment
        if (m_layoutItems[shape]->itemType == LegendType) {
            qDebug()<<"check alignment";
            QRectF n = diagramArea(plotArea, plotAreaRect);
            qreal ypos = itmRect.top();
            switch (static_cast<Legend*>(shape)->alignment()) {
                case Qt::AlignLeft:
                    ypos = n.top();
                    break;
                case Qt::AlignCenter:
                    ypos = n.top() + ((n.height() - itmRect.height()) * 0.5);
                    break;
                case Qt::AlignRight:
                    ypos = n.bottom() - itmRect.height();
                    break;
                default:
                    break;
            }
            if (ypos != itmRect.top()) {
                itmRect.moveTop(ypos);
                newlayout[shape] = itmRect;
                debugChartLayout<<"aligned:"<<dbg(shape)<<itmRect;
            }
        }
    }
    if (newlayout.contains(plotArea)) {
        // handle axis titles/legend
        debugChartLayout<<"Handle axis titles"<<plotAreaRect;
        const QRectF oldPlotAreaRect = oldlayout[plotArea];
        const QRectF newPlotAreaRect = newlayout[plotArea];
        {QMap<KoShape*, QRectF>::const_iterator it;
        for (it = oldlayout.constBegin(); it != oldlayout.constEnd(); ++it) {
            if (isShapeToBeMoved(it.key(), EndPosition, plotArea)) {
                QRectF r(it.value());
                debugChartLayout<<"move check:"<<dbg(it.key())<<r;
                qreal xpos = relativePosition(oldPlotAreaRect.left(), oldPlotAreaRect.width(), newPlotAreaRect.left(),  newPlotAreaRect.width(), r.left(), r.width());
                if (xpos != r.left()) {
                    r.moveLeft(xpos);
                    newlayout[it.key()] = r;
                    debugChartLayout<<"moved:"<<dbg(it.key())<<r;
                }
            }
        }}
    }
    QMap<KoShape*, QRectF> result;
    {QMap<KoShape*, QRectF>::const_iterator it;
    for (it = newlayout.constBegin(); it != newlayout.constEnd(); ++it) {
        const QRectF oldrect = oldlayout.value(it.key());
        if (it.value() != oldrect) {
            const QPointF poffset = it.value().topLeft() - oldrect.topLeft();
            const QSizeF soffset = it.value().size() - oldrect.size();
            result.insert(it.key(), QRectF(it.key()->position() + poffset, it.key()->size() + soffset));
        }
    }}
    return result;
}

QMap<KoShape*, QRectF> ChartLayout::calculateLayoutTopStart(KoShape *shape, KoShape *plotArea, bool hide) const
{
    debugChartLayout<<Q_FUNC_INFO<<dbg(shape)<<plotArea<<hide;
    return QMap<KoShape*, QRectF>();
}

QMap<KoShape*, QRectF> ChartLayout::calculateLayoutBottomStart(KoShape *shape, KoShape *plotArea, bool hide) const
{
    debugChartLayout<<Q_FUNC_INFO<<dbg(shape)<<plotArea<<hide;
    return QMap<KoShape*, QRectF>();
}

QMap<KoShape*, QRectF> ChartLayout::calculateLayoutTopEnd(KoShape *shape, KoShape *plotArea, bool hide) const
{
    debugChartLayout<<Q_FUNC_INFO<<dbg(shape)<<'='<<itemRect(shape)<<plotArea<<'='<<itemRect(plotArea)<<hide;
    return QMap<KoShape*, QRectF>();
}

QMap<KoShape*, QRectF> ChartLayout::calculateLayoutBottomEnd(KoShape *shape, KoShape *plotArea, bool hide) const
{
    debugChartLayout<<Q_FUNC_INFO<<dbg(shape)<<plotArea<<hide;
    return QMap<KoShape*, QRectF>();
}

QRectF ChartLayout::layoutArea(Position area, const KoShape *plotArea) const
{
    QRectF center;
    if (plotArea) {
        center = itemRect(plotArea);
    } else {
        center = QRectF(m_containerSize.width() * 0.2, m_containerSize.height() * 0.2, m_containerSize.width() * 0.8, m_containerSize.height() * 0.8);
    }
    switch (area) {
        case StartPosition:
            return QRectF(0, center.left(), center.left(), center.height());
            break;
        case TopPosition:
            return QRectF(center.left(), 0, center.width(), center.top());
            break;
        case EndPosition:
            return QRectF(center.right(), center.top(), m_containerSize.width() - center.right(), center.height());
            break;
        case BottomPosition:
            return QRectF(center.left(), center.bottom(), center.width(), m_containerSize.height() - center.bottom());
            break;
        case TopStartPosition:
            return QRectF(0, 0, center.left(), center.top());
            break;
        case TopEndPosition:
            return QRectF(center.right(), 0, m_containerSize.width() - center.right(), center.top());
            break;
        case BottomStartPosition:
            return QRectF(0, center.bottom(), center.left(), m_containerSize.height() - center.bottom());
            break;
        case BottomEndPosition:
            return QRectF(center.right(), center.bottom(), m_containerSize.width() - center.right(), m_containerSize.height() - center.bottom());
            break;
        case CenterPosition:
            return center;
        default:
            break;
    }
    return QRectF();
}

Position ChartLayout::itemArea(const KoShape *item, const KoShape *plotArea) const
{
    QRectF center(QPointF(m_containerSize.width() * 0.2, m_containerSize.height() * 0.2), m_containerSize * 0.6);
    if (plotArea) {
        center = itemRect(plotArea);
    }
    QRectF r = itemRect(item);
    if (r.intersects(center)) {
        return CenterPosition;
    }
    QRectF area = layoutArea(TopStartPosition, plotArea);
    if (r.intersects(area)) {
        return TopStartPosition;
    }
    area = layoutArea(TopPosition, plotArea);
    if (r.intersects(area)) {
        return TopPosition;
    }
    area = layoutArea(TopEndPosition, plotArea);
    if (r.intersects(area)) {
        return TopEndPosition;
    }
    area = layoutArea(StartPosition, plotArea);
    if (r.intersects(area)) {
        return StartPosition;
    }
    area = layoutArea(EndPosition, plotArea);
    if (r.intersects(area)) {
        return EndPosition;
    }
    area = layoutArea(BottomStartPosition, plotArea);
    if (r.intersects(area)) {
        return BottomStartPosition;
    }
    area = layoutArea(BottomPosition, plotArea);
    if (r.intersects(area)) {
        return BottomPosition;
    }
    area = layoutArea(BottomEndPosition, plotArea);
    if (r.intersects(area)) {
        return BottomEndPosition;
    }
    return m_layoutItems[const_cast<KoShape*>(item)]->pos;
}

qreal ChartLayout::itemDefaultPosition(const KoShape *shape, qreal defaultValue, qreal itemstart, qreal itemend, qreal areastart, qreal areaend) const
{
    qreal result = defaultValue;
    LayoutData *data = m_layoutItems[const_cast<KoShape*>(shape)];
    if (data->pos != FloatingPosition) {
        switch (data->itemType) {
            case LegendType:
                // Legend has alignment
                switch (static_cast<const Legend*>(shape)->alignment()) {
                    case Qt::AlignLeft: result = areastart; break;
                    case Qt::AlignRight: result = areaend - itemRect(shape).height(); break;
                    case Qt::AlignCenter: {
                        qreal w = itemRect(shape).height() * 0.5;
                        if (data->pos == TopPosition || data->pos == BottomPosition) {
                            w = itemRect(shape).width() * 0.5;
                        }
                        result = areastart + (0.5 * (areaend - areastart)) - w; break;
                    }
                    default: break; // default value
                }
                break;
            case XAxisTitleType:
            case YAxisTitleType:
            case SecondaryXAxisTitleType:
            case SecondaryYAxisTitleType:
                if (!shape->isVisible()) {
                    // center
                    result = areastart + (0.5 * (areaend - areastart)) - (0.5 * (itemend - itemstart)); break;
                }
                break;
            case TitleLabelType:
            case SubTitleLabelType:
            case FooterLabelType:
                if (!shape->isVisible()) {
                    switch (data->alignment) {
                        case Qt::AlignLeft:
                            result = areastart;
                            break;
                        case Qt::AlignRight:
                            result = areaend - itemRect(shape).height();
                            break;
                        case Qt::AlignCenter:
                            result = areastart + (0.5 * (areaend - areastart)) - (0.5 * (itemend - itemstart));
                            break;
                        default:
                            break; // default value
                    }
                }
            default:
                break;
        }
    }
    return result;
}

/// Static Helper Methods

/*static*/ qreal ChartLayout::relativePosition(qreal start1, qreal length1, qreal start2, qreal length2, qreal start, qreal length)
{
    qreal pos = start;
    debugChartLayout<<"start1="<<start1<<"length1="<<length1<<"start2="<<start2<<"length2="<<length2<<"start="<<start<<"length="<<length;
    if (start <= start1) {
        qreal diff = start - start1;
        pos = start2 + diff;
        debugChartLayout<<"At start: same distance:"<<diff<<"pos="<<pos;
    } else if (start + length >= start1 + length1) {
        // at the moving end, so stay same distance from new end
        qreal end = start1 + length1;
        qreal diff = start - end;
        pos = start2 + length2 + diff;
        debugChartLayout<<"At end: same distance:"<<diff<<"pos="<<pos;
    } else if (start > start1) {
        // beside the plot area, so move relative change in length
        qreal center1 = start1 + (0.5 * length1);
        qreal center2 = start2 + (0.5 * length2);
        qreal center = start + (0.5 * length);
        qreal diff = center - center1;
        qreal factor = (length2 / length1);
        qreal newcenter = center2 + (diff * factor);
        pos = newcenter - (0.5 * length);
        debugChartLayout<<"Beside: move relative:"<<"f="<<factor<<"d="<<diff<<"c1:"<<center1<<"c2:"<<center2<<"c:"<<center<<"nc:"<<newcenter<<"pos="<<pos;
    }
    return pos;
}

bool ChartLayout::isShapeToBeMoved(const KoShape *shape, Position area, const KoShape *plotArea) const
{
    if (!shape->isVisible()) {
        return false;
    }
    QRectF paRect = itemRect(plotArea);
    LayoutData *data = m_layoutItems[const_cast<KoShape*>(shape)];
    switch (area) {
        case TopPosition:
        case BottomPosition:
            if (data->itemType == LegendType || data->itemType == YAxisTitleType || data->itemType == SecondaryYAxisTitleType) {
                QRectF sr = itemRect(shape);
                QRectF r = QRectF(0, paRect.top(), paRect.left(), paRect.bottom());
                if (sr.intersects(r)) {
                    return true;
                }
                r = QRectF(paRect.right(), paRect.top(), m_containerSize.width(), paRect.bottom());
                return sr.intersects(r);
            }
            break;
        case StartPosition:
        case EndPosition:
            if (data->itemType == LegendType || data->itemType == XAxisTitleType || data->itemType == SecondaryXAxisTitleType) {
                QRectF sr = itemRect(shape);
                QRectF r = QRectF(paRect.left(), 0, paRect.right(), paRect.top());
                if (sr.intersects(r)) {
                    return true;
                }
                r = QRectF(paRect.left(), paRect.bottom(), paRect.right(), m_containerSize.height());
                return sr.intersects(r);
            }
            break;
        default:
            break;
    }
    return false;
}

/*static*/ QPointF ChartLayout::itemPosition(const KoShape *shape)
{
    const QRectF boundingRect = QRectF(QPointF(0, 0), shape->size());
    return shape->transformation().mapRect(boundingRect).topLeft();
}

/*static*/ QSizeF ChartLayout::itemSize(const KoShape *shape)
{
    const QRectF boundingRect = QRectF(QPointF(0, 0), shape->size());
    return shape->transformation().mapRect(boundingRect).size();
}

/*static*/ QRectF ChartLayout::itemRect(const KoShape *shape)
{
    const QRectF boundingRect = QRectF(itemPosition(shape), itemSize(shape));
    return boundingRect;
}

/*static*/ void ChartLayout::setItemPosition(KoShape *shape, const QPointF& pos)
{
    const QPointF offset =  shape->position() - itemPosition(shape);
    shape->setPosition(pos + offset);
}

QRectF ChartLayout::diagramArea(const KoShape *shape)
{
    return diagramArea(shape, itemRect(shape));
}

// FIXME: Get the actual plot area ex axis labels from KChart
QRectF ChartLayout::diagramArea(const KoShape *shape, const QRectF &rect)
{
    const PlotArea* plotArea = dynamic_cast<const PlotArea*>(shape);
    if (!plotArea) {
        return rect;
    }
    qreal bottom = 0.0;
    qreal left = 0.0;
    qreal top = 0.0;
    qreal right = 0.0;
    // HACK: KChart has some spacing between axis and label
    qreal xspace = ScreenConversions::pxToPtX(6.0) * 2.0;
    qreal yspace = ScreenConversions::pxToPtY(6.0) * 2.0;
    if (plotArea->xAxis() && plotArea->xAxis()->showLabels()) {
        bottom = plotArea->xAxis()->fontSize();
        bottom += yspace;
    }
    if (plotArea->yAxis() && plotArea->yAxis()->showLabels()) {
        left = plotArea->yAxis()->fontSize();
        left += xspace;
    }
    if (plotArea->secondaryXAxis() && plotArea->secondaryXAxis()->showLabels()) {
        top = plotArea->secondaryXAxis()->fontSize();
        top += yspace;
    }
    if (plotArea->secondaryYAxis() && plotArea->secondaryYAxis()->showLabels()) {
        right = plotArea->secondaryYAxis()->fontSize();
        right += xspace;
    }
    return rect.adjusted(left, top, -right, -bottom);
}

QString ChartLayout::dbg(const KoShape *shape) const
{
    QString s;
    LayoutData *data = m_layoutItems[const_cast<KoShape*>(shape)];
    switch(data->itemType) {
        case GenericItemType: s = "KoShape[Generic:"+ shape->shapeId() + "]"; break;
        case TitleLabelType: s = "KoShape[ChartTitle]"; break;
        case SubTitleLabelType: s = "KoShape[ChartSubTitle]"; break;
        case FooterLabelType: s = "KoShape[ChartFooter]"; break;
        case PlotAreaType: s = "KoShape[PlotArea]"; break;
        case LegendType:
            s = "KoShape[Legend";
            switch(static_cast<const Legend*>(shape)->alignment()) {
                case Qt::AlignLeft: s += ":Start"; break;
                case Qt::AlignCenter: s += ":Center"; break;
                case Qt::AlignRight: s += ":End"; break;
                default: s += ":Float"; break;
            }
            s += ']';
            break;
        case XAxisTitleType: s = "KoShape[XAxisTitle]"; break;
        case YAxisTitleType: s = "KoShape[YAxisTitle]"; break;
        case SecondaryXAxisTitleType: s = "KoShape[SXAxisTitle]"; break;
        case SecondaryYAxisTitleType: s = "KoShape[SYAxisTitle]"; break;
        default: s = "KoShape[Unknown]"; break;
    }
    return s;
}

QString ChartLayout::dbg(const QList<KoShape*> &shapes) const
{
    QString s = "(";
    for (int i = 0; i < shapes.count(); ++i) {
        if (i > 0) s += ',';
        s += dbg(shapes.at(i));
    }
    s += ')';
    return s;
}
