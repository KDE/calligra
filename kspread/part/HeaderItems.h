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
class ColumnHeaderItem : public QGraphicsWidget, public ColumnHeader
{
    Q_OBJECT
public:
    ColumnHeaderItem(QGraphicsItem *_parent, CanvasItem *_canvas);
    virtual ~ColumnHeaderItem();

    void updateColumns(int from, int to);

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

private Q_SLOTS:
    void toolChanged(const QString& toolId);
};



/**
 * The widget left to the cells showing the row headers.
 */
class RowHeaderItem : public QGraphicsWidget, public RowHeader
{
    Q_OBJECT
public:
    RowHeaderItem(QGraphicsItem *_parent, CanvasItem *_canvas);
    virtual ~RowHeaderItem();

    void updateRows(int from, int to);

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

private Q_SLOTS:
    void toolChanged(const QString& toolId);
};



/**
 * The widget in the top left corner of the canvas,
 * responsible for selecting all cells in a sheet.
 */
class SelectAllButtonItem : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit SelectAllButtonItem(QGraphicsItem *_parent, KoCanvasBase* canvasBase, Selection* selection);
    virtual ~SelectAllButtonItem();

protected:
    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void wheelEvent(QGraphicsSceneWheelEvent *event);

private Q_SLOTS:
    void toolChanged(const QString& toolId);

private:
    KoCanvasBase* m_canvasBase;
    Selection* m_selection;
    bool m_mousePressed;
    bool m_cellToolIsActive;
};

} // namespace KSpread
#endif
