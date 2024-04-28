/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 - Robert Knight <robertknight@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_HEADERWIDGETS
#define CALLIGRA_SHEETS_HEADERWIDGETS

#include "Headers.h"
#include <QWidget>

class QRubberBand;

namespace Calligra
{
namespace Sheets
{
class Canvas;
class View;

/**
 * The widget above the cells showing the column headers.
 */
class ColumnHeaderWidget : public QWidget, public ColumnHeader
{
    Q_OBJECT
public:
    ColumnHeaderWidget(QWidget *_parent, Canvas *_canvas, View *_view);
    ~ColumnHeaderWidget() override;

    void updateColumns(int from, int to) override;
    QSizeF size() const override
    {
        return QWidget::size();
    }
    void setCursor(const QCursor &cursor) override
    {
        QWidget::setCursor(cursor);
    }
    void scroll(qreal dx, qreal dy) override
    {
        QWidget::scroll(dx, dy);
    }
    QPalette palette() const override
    {
        return QWidget::palette();
    }
    void update() override
    {
        QWidget::update();
    }
private Q_SLOTS:
    void slotAutoScroll(const QPoint &distance);

protected:
    void paintEvent(QPaintEvent *_ev) override;
    void mousePressEvent(QMouseEvent *_ev) override;
    void mouseReleaseEvent(QMouseEvent *_ev) override;
    void mouseDoubleClickEvent(QMouseEvent *_ev) override;
    void mouseMoveEvent(QMouseEvent *_ev) override;
    void wheelEvent(QWheelEvent *) override;
    void focusOutEvent(QFocusEvent *ev) override;
    void resizeEvent(QResizeEvent *_ev) override;

    void paintSizeIndicator(int mouseX) override;
    void removeSizeIndicator() override;

private Q_SLOTS:
    void toolChanged(const QString &toolId);

private:
    QRubberBand *m_rubberband;
};

/**
 * The widget left to the cells showing the row headers.
 */
class RowHeaderWidget : public QWidget, public RowHeader
{
    Q_OBJECT
public:
    RowHeaderWidget(QWidget *_parent, Canvas *_canvas, View *_view);
    ~RowHeaderWidget() override;

    void updateRows(int from, int to) override;
    QSizeF size() const override
    {
        return QWidget::size();
    }
    void setCursor(const QCursor &cursor) override
    {
        QWidget::setCursor(cursor);
    }
    void scroll(qreal dx, qreal dy) override
    {
        QWidget::scroll(dx, dy);
    }
    QPalette palette() const override
    {
        return QWidget::palette();
    }
    void update() override
    {
        QWidget::update();
    }
private Q_SLOTS:
    void slotAutoScroll(const QPoint &distance);

protected:
    void paintEvent(QPaintEvent *_ev) override;
    void mousePressEvent(QMouseEvent *_ev) override;
    void mouseReleaseEvent(QMouseEvent *_ev) override;
    void mouseMoveEvent(QMouseEvent *_ev) override;
    void mouseDoubleClickEvent(QMouseEvent *_ev) override;
    void wheelEvent(QWheelEvent *) override;
    void focusOutEvent(QFocusEvent *ev) override;

    void paintSizeIndicator(int mouseY) override;
    void removeSizeIndicator() override;
private Q_SLOTS:
    void toolChanged(const QString &toolId);

private:
    QRubberBand *m_rubberband;
};

/**
 * The widget in the top left corner of the canvas,
 * responsible for selecting all cells in a sheet.
 */
class SelectAllButtonWidget : public QWidget, public SelectAllButton
{
    Q_OBJECT
public:
    explicit SelectAllButtonWidget(CanvasBase *canvasBase);
    ~SelectAllButtonWidget() override;

    QPalette palette() const override
    {
        return QWidget::palette();
    }
    void update() override
    {
        QWidget::update();
    }

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *) override;

private Q_SLOTS:
    void toolChanged(const QString &toolId);
};

} // namespace Sheets
} // namespace Calligra

#endif
