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

#ifndef CALLIGRA_SHEETS_HEADERWIDGETS
#define CALLIGRA_SHEETS_HEADERWIDGETS

#include <QWidget>
#include "Headers.h"

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
    QSizeF size() const override { return QWidget::size(); }
    void setCursor(const QCursor& cursor) override { QWidget::setCursor(cursor); }
    void scroll(qreal dx, qreal dy) override { QWidget::scroll(dx, dy); }
    QPalette palette() const override { return QWidget::palette(); }
    void update() override { QWidget::update(); }
private Q_SLOTS:
    void slotAutoScroll(const QPoint& distance);

protected:
    void paintEvent(QPaintEvent* _ev) override;
    void mousePressEvent(QMouseEvent* _ev) override;
    void mouseReleaseEvent(QMouseEvent* _ev) override;
    void mouseDoubleClickEvent(QMouseEvent* _ev) override;
    void mouseMoveEvent(QMouseEvent* _ev) override;
    void wheelEvent(QWheelEvent*) override;
    void focusOutEvent(QFocusEvent* ev) override;
    void resizeEvent(QResizeEvent * _ev) override;

    void paintSizeIndicator(int mouseX) override;
    void removeSizeIndicator() override;
    
private Q_SLOTS:
    void toolChanged(const QString& toolId);

private:
    QRubberBand* m_rubberband;
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
    QSizeF size() const override { return QWidget::size(); }
    void setCursor(const QCursor& cursor) override { QWidget::setCursor(cursor); }
    void scroll(qreal dx, qreal dy) override { QWidget::scroll(dx, dy); }
    QPalette palette() const override { return QWidget::palette(); }
    void update() override { QWidget::update(); }
private Q_SLOTS:
    void slotAutoScroll(const QPoint& distance);

protected:
    void paintEvent(QPaintEvent* _ev) override;
    void mousePressEvent(QMouseEvent* _ev) override;
    void mouseReleaseEvent(QMouseEvent* _ev) override;
    void mouseMoveEvent(QMouseEvent* _ev) override;
    void mouseDoubleClickEvent(QMouseEvent* _ev) override;
    void wheelEvent(QWheelEvent*) override;
    void focusOutEvent(QFocusEvent* ev) override;
    
    void paintSizeIndicator(int mouseY) override;
    void removeSizeIndicator() override;
private Q_SLOTS:
    void toolChanged(const QString& toolId);

private:
    QRubberBand* m_rubberband;
};

/**
 * The widget in the top left corner of the canvas,
 * responsible for selecting all cells in a sheet.
 */
class SelectAllButtonWidget : public QWidget, public SelectAllButton
{
    Q_OBJECT
public:
    explicit SelectAllButtonWidget(CanvasBase* canvasBase);
    ~SelectAllButtonWidget() override;

    QPalette palette() const override { return QWidget::palette(); }
    void update() override { QWidget::update(); }
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent*) override;

private Q_SLOTS:
    void toolChanged(const QString& toolId);
};

} // namespace Sheets
} // namespace Calligra

#endif
