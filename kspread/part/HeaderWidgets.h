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

#ifndef KSPREAD_HEADERWIDGETS
#define KSPREAD_HEADERWIDGETS

#include <QWidget>
#include "Headers.h"

class QLabel;
class QRubberBand;

class KoCanvasBase;

namespace KSpread
{
class Canvas;
class Selection;
class View;

/**
 * The widget above the cells showing the column headers.
 */
class ColumnHeaderWidget : public QWidget, public ColumnHeader
{
    Q_OBJECT
public:
    ColumnHeaderWidget(QWidget *_parent, Canvas *_canvas, View *_view);
    virtual ~ColumnHeaderWidget();

    virtual void updateColumns(int from, int to);
    virtual QSizeF size() const { return QWidget::size(); }
    virtual void setCursor(const QCursor& cursor) { QWidget::setCursor(cursor); }
    virtual void scroll(qreal dx, qreal dy) { QWidget::scroll(dx, dy); }
    virtual QPalette palette() const { return QWidget::palette(); }
    virtual void update() { QWidget::update(); }
private slots:
    void slotAutoScroll(const QPoint& distance);

protected:
    virtual void paintEvent(QPaintEvent* _ev);
    virtual void mousePressEvent(QMouseEvent* _ev);
    virtual void mouseReleaseEvent(QMouseEvent* _ev);
    virtual void mouseDoubleClickEvent(QMouseEvent* _ev);
    virtual void mouseMoveEvent(QMouseEvent* _ev);
    virtual void wheelEvent(QWheelEvent*);
    virtual void focusOutEvent(QFocusEvent* ev);
    virtual void resizeEvent(QResizeEvent * _ev);

    virtual void paintSizeIndicator(int mouseX);
    virtual void removeSizeIndicator();
    
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
    virtual ~RowHeaderWidget();

    void updateRows(int from, int to);
    virtual QSizeF size() const { return QWidget::size(); }
    virtual void setCursor(const QCursor& cursor) { QWidget::setCursor(cursor); }
    virtual void scroll(qreal dx, qreal dy) { QWidget::scroll(dx, dy); }
    virtual QPalette palette() const { return QWidget::palette(); }
    virtual void update() { QWidget::update(); }
private slots:
    void slotAutoScroll(const QPoint& distance);

protected:
    virtual void paintEvent(QPaintEvent* _ev);
    virtual void mousePressEvent(QMouseEvent* _ev);
    virtual void mouseReleaseEvent(QMouseEvent* _ev);
    virtual void mouseMoveEvent(QMouseEvent* _ev);
    virtual void mouseDoubleClickEvent(QMouseEvent* _ev);
    virtual void wheelEvent(QWheelEvent*);
    virtual void focusOutEvent(QFocusEvent* ev);
    
    virtual void paintSizeIndicator(int mouseY);
    virtual void removeSizeIndicator();
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
    virtual ~SelectAllButtonWidget();

    virtual QPalette palette() const { return QWidget::palette(); }
    virtual void update() { QWidget::update(); }
protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent*);

private Q_SLOTS:
    void toolChanged(const QString& toolId);
};

} // namespace KSpread
#endif
