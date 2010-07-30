/* This file is part of the KDE project
   Copyright 2006 - Robert Knight <robertknight@gmail.com>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_HEADERITEMS
#define KSPREAD_HEADERITEMS

#include <QGraphicsWidget>
#include "Headers.h"

#include "kspread_export.h"

class QLabel;
class QRubberBand;

class KoCanvasBase;

namespace KSpread
{
class CanvasItem;
class Selection;
class View;

/**
 * The widget above the cells showing the column headers.
 */
class KSPREAD_EXPORT ColumnHeaderItem : public QGraphicsWidget, public ColumnHeader
{
    Q_OBJECT
public:
    ColumnHeaderItem(QGraphicsItem *_parent, CanvasItem *_canvas);
    virtual ~ColumnHeaderItem();

    void updateColumns(int from, int to);
    virtual void update() { QGraphicsWidget::update(); }

private slots:
    //void slotAutoScroll(const QPoint& distance);

protected:
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* _ev);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _ev);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _ev);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* _ev);
    virtual void wheelEvent(QGraphicsSceneWheelEvent*);
    virtual void focusOutEvent(QFocusEvent* ev);
    virtual void resizeEvent(QGraphicsSceneResizeEvent * _ev);

    virtual void paintSizeIndicator(int mouseX);
    virtual void removeSizeIndicator();

    virtual QSizeF size() const { return QGraphicsWidget::size(); }
    virtual void setCursor(const QCursor& cursor) { QGraphicsWidget::setCursor(cursor); }
    virtual void scroll(qreal x, qreal y) { QGraphicsWidget::scroll(x, y); }
    virtual QPalette palette() const { return QGraphicsWidget::palette(); }
/*private */public Q_SLOTS:
    void toolChanged(const QString& toolId);
};



/**
 * The widget left to the cells showing the row headers.
 */
class KSPREAD_EXPORT RowHeaderItem : public QGraphicsWidget, public RowHeader
{
    Q_OBJECT
public:
    RowHeaderItem(QGraphicsItem *_parent, CanvasItem *_canvas);
    virtual ~RowHeaderItem();

    void updateRows(int from, int to);
    virtual void update() { QGraphicsWidget::update(); }

private slots:
    //void slotAutoScroll(const QPoint& distance);

protected:
    virtual void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* _ev);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _ev);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* _ev);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _ev);
    virtual void wheelEvent(QGraphicsSceneWheelEvent*);
    virtual void focusOutEvent(QFocusEvent* ev);

    virtual void paintSizeIndicator(int mouseY);
    virtual void removeSizeIndicator();

    virtual QSizeF size() const { return QGraphicsWidget::size(); }
    virtual void setCursor(const QCursor& cursor) { QGraphicsWidget::setCursor(cursor); }
    virtual void scroll(qreal x, qreal y) { QGraphicsWidget::scroll(x, y); }
    virtual QPalette palette() const { return QGraphicsWidget::palette(); }

/*private */public Q_SLOTS:
    void toolChanged(const QString& toolId);
};



/**
 * The widget in the top left corner of the canvas,
 * responsible for selecting all cells in a sheet.
 */
class KSPREAD_EXPORT SelectAllButtonItem : public QGraphicsWidget, public SelectAllButton
{
    Q_OBJECT
public:
    explicit SelectAllButtonItem(QGraphicsItem *_parent, CanvasBase* canvasBase);
    virtual ~SelectAllButtonItem();

protected:
    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void wheelEvent(QGraphicsSceneWheelEvent *event);

private Q_SLOTS:
    void toolChanged(const QString& toolId);
};

} // namespace KSpread
#endif
