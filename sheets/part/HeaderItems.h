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

#ifndef CALLIGRA_SHEETS_HEADERITEMS
#define CALLIGRA_SHEETS_HEADERITEMS

#include <QGraphicsWidget>
#include "Headers.h"

#include "sheets_common_export.h"



namespace Calligra
{
namespace Sheets
{
class CanvasItem;

/**
 * The widget above the cells showing the column headers.
 */
class CALLIGRA_SHEETS_COMMON_EXPORT ColumnHeaderItem : public QGraphicsWidget, public ColumnHeader
{
    Q_OBJECT
public:
    ColumnHeaderItem(QGraphicsItem *_parent, CanvasItem *_canvas);
    ~ColumnHeaderItem() override;

    void updateColumns(int from, int to) override;
    void update() override { QGraphicsWidget::update(); }

private Q_SLOTS:
    //void slotAutoScroll(const QPoint& distance);

protected:
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* _ev) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* _ev) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _ev) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* _ev) override;
    void wheelEvent(QGraphicsSceneWheelEvent*) override;
    void focusOutEvent(QFocusEvent* ev) override;
    void resizeEvent(QGraphicsSceneResizeEvent * _ev) override;

    void paintSizeIndicator(int mouseX) override;
    void removeSizeIndicator() override;

    QSizeF size() const override { return QGraphicsWidget::size(); }
    void setCursor(const QCursor& cursor) override { QGraphicsWidget::setCursor(cursor); }
    void scroll(qreal x, qreal y) override;
    QPalette palette() const override { return QGraphicsWidget::palette(); }
/*private */public Q_SLOTS:
    void toolChanged(const QString& toolId);
};



/**
 * The widget left to the cells showing the row headers.
 */
class CALLIGRA_SHEETS_COMMON_EXPORT RowHeaderItem : public QGraphicsWidget, public RowHeader
{
    Q_OBJECT
public:
    RowHeaderItem(QGraphicsItem *_parent, CanvasItem *_canvas);
    ~RowHeaderItem() override;

    void updateRows(int from, int to) override;
    void update() override { QGraphicsWidget::update(); }

private Q_SLOTS:
    //void slotAutoScroll(const QPoint& distance);

protected:
    void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* _ev) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* _ev) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* _ev) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _ev) override;
    void wheelEvent(QGraphicsSceneWheelEvent*) override;
    void focusOutEvent(QFocusEvent* ev) override;

    void paintSizeIndicator(int mouseY) override;
    void removeSizeIndicator() override;

    QSizeF size() const override { return QGraphicsWidget::size(); }
    void setCursor(const QCursor& cursor) override { QGraphicsWidget::setCursor(cursor); }
    void scroll(qreal x, qreal y) override { QGraphicsWidget::scroll(x, y); }
    QPalette palette() const override { return QGraphicsWidget::palette(); }

/*private */public Q_SLOTS:
    void toolChanged(const QString& toolId);
};



/**
 * The widget in the top left corner of the canvas,
 * responsible for selecting all cells in a sheet.
 */
class CALLIGRA_SHEETS_COMMON_EXPORT SelectAllButtonItem : public QGraphicsWidget, public SelectAllButton
{
    Q_OBJECT
public:
    explicit SelectAllButtonItem(QGraphicsItem *_parent, CanvasBase* canvasBase);
    ~SelectAllButtonItem() override;

protected:
    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void wheelEvent(QGraphicsSceneWheelEvent *event) override;

private Q_SLOTS:
    void toolChanged(const QString& toolId);
};

} // namespace Sheets
} // namespace Calligra

#endif
