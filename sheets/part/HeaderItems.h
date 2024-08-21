/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 - Robert Knight <robertknight@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_HEADERITEMS
#define CALLIGRA_SHEETS_HEADERITEMS

#include "Headers.h"
#include <QGraphicsWidget>

#include "sheets_part_export.h"

namespace Calligra
{
namespace Sheets
{
class CanvasItem;

/**
 * The widget above the cells showing the column headers.
 */
class CALLIGRA_SHEETS_PART_EXPORT ColumnHeaderItem : public QGraphicsWidget, public ColumnHeader
{
    Q_OBJECT
public:
    ColumnHeaderItem(QGraphicsItem *_parent, CanvasItem *_canvas);
    ~ColumnHeaderItem() override;

    void updateColumns(int from, int to) override;
    void update() override
    {
        QGraphicsWidget::update();
    }

private Q_SLOTS:
    // void slotAutoScroll(const QPoint& distance);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *_ev) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *_ev) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *_ev) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *_ev) override;
    void wheelEvent(QGraphicsSceneWheelEvent *) override;
    void focusOutEvent(QFocusEvent *ev) override;
    void resizeEvent(QGraphicsSceneResizeEvent *_ev) override;

    void paintSizeIndicator(int mouseX) override;
    void removeSizeIndicator() override;

    QSizeF size() const override
    {
        return QGraphicsWidget::size();
    }
    void setCursor(const QCursor &cursor) override
    {
        QGraphicsWidget::setCursor(cursor);
    }
    void scroll(qreal x, qreal y) override;
    QPalette palette() const override
    {
        return QGraphicsWidget::palette();
    }
    /*private */ public Q_SLOTS:
    void toolChanged(const QString &toolId);
};

/**
 * The widget left to the cells showing the row headers.
 */
class CALLIGRA_SHEETS_PART_EXPORT RowHeaderItem : public QGraphicsWidget, public RowHeader
{
    Q_OBJECT
public:
    RowHeaderItem(QGraphicsItem *_parent, CanvasItem *_canvas);
    ~RowHeaderItem() override;

    void updateRows(int from, int to) override;
    void update() override
    {
        QGraphicsWidget::update();
    }

private Q_SLOTS:
    // void slotAutoScroll(const QPoint& distance);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *_ev) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *_ev) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *_ev) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *_ev) override;
    void wheelEvent(QGraphicsSceneWheelEvent *) override;
    void focusOutEvent(QFocusEvent *ev) override;

    void paintSizeIndicator(int mouseY) override;
    void removeSizeIndicator() override;

    QSizeF size() const override
    {
        return QGraphicsWidget::size();
    }
    void setCursor(const QCursor &cursor) override
    {
        QGraphicsWidget::setCursor(cursor);
    }
    void scroll(qreal x, qreal y) override
    {
        QGraphicsWidget::scroll(x, y);
    }
    QPalette palette() const override
    {
        return QGraphicsWidget::palette();
    }

    /*private */ public Q_SLOTS:
    void toolChanged(const QString &toolId);
};

/**
 * The widget in the top left corner of the canvas,
 * responsible for selecting all cells in a sheet.
 */
class CALLIGRA_SHEETS_PART_EXPORT SelectAllButtonItem : public QGraphicsWidget, public SelectAllButton
{
    Q_OBJECT
public:
    explicit SelectAllButtonItem(QGraphicsItem *_parent, CanvasBase *canvasBase);
    ~SelectAllButtonItem() override;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void wheelEvent(QGraphicsSceneWheelEvent *event) override;

private Q_SLOTS:
    void toolChanged(const QString &toolId);
};

} // namespace Sheets
} // namespace Calligra

#endif
