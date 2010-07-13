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

#ifndef KSPREAD_HEADERS
#define KSPREAD_HEADERS

#include <QGraphicsWidget>

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
class ColumnHeaderItem : public QGraphicsWidget
{
    Q_OBJECT
public:
    ColumnHeaderItem(QGraphicsItem *_parent, CanvasItem *_canvas, View *_view);
    virtual ~ColumnHeaderItem();

    int markerColumn() const {
        return  m_iSelectionAnchor;
    }
    void equalizeColumn(double resize);

    void updateColumns(int from, int to);

private slots:
    void slotAutoScroll(const QPoint& distance);

protected:
    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* _ev);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _ev);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _ev);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* _ev);
    virtual void wheelEvent(QGraphicsSceneWheelEvent*);
    virtual void focusOutEvent(QFocusEvent* ev);
    virtual void resizeEvent(QGraphicsSceneResizeEvent * _ev);
    void paintSizeIndicator(int mouseX);

    void drawText(QPainter *painter, const QFont& font, const QPointF& location, const QString& text, double width) const;

private Q_SLOTS:
    void toolChanged(const QString& toolId);

private:
    CanvasItem *m_pCanvas;
    View *m_pView;

    /**
     * Flag that indicates whether the user wants to mark columns.
     * The user may mark columns by dragging the mouse around in th XBorder widget.
     * If he is doing that right now, this flag is true. Mention that the user may
     * also resize columns by dragging the mouse. This case is not covered by this flag.
     */
    bool m_bSelection;

    /**
     * The column over which the user pressed the mouse button.
     * If the user marks columns in the XBorder widget, then this is the initial
     * column on which he pressed the mouse button.
     */
    int m_iSelectionAnchor;

    /**
     * Flag that indicates whether the user resizes a column
     * The user may resize columns by dragging the mouse around in the ColumnHeader widget.
     * If he is doing that right now, this flag is true.
     */
    bool m_bResize;

    /**
     * The column over which the user pressed the mouse button.
     * The user may resize columns by dragging the mouse around the XBorder widget.
     * This is the column over which he pressed the mouse button. This column is going
     * to be resized.
      */
    int m_iResizedColumn;

    /**
     * Last position of the mouse, when resizing.
     */
    int m_iResizePos;

    /**
     * The label used for showing the current size, when resizing
     */
    QLabel *m_lSize;

    /**
     * True when the mouse button is pressed
     */
    bool m_bMousePressed;

    QRubberBand* m_rubberband;
    bool m_cellToolIsActive;
};



/**
 * The widget left to the cells showing the row headers.
 */
class RowHeaderItem : public QGraphicsWidget
{
    Q_OBJECT
public:
    RowHeaderItem(QGraphicsItem *_parent, CanvasItem *_canvas, View *_view);
    virtual ~RowHeaderItem();

    int markerRow() const {
        return  m_iSelectionAnchor;
    }
    void equalizeRow(double resize);
    void updateRows(int from, int to);

private slots:
    void slotAutoScroll(const QPoint& distance);

protected:
    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* _ev);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _ev);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* _ev);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _ev);
    virtual void wheelEvent(QGraphicsSceneWheelEvent*);
    virtual void focusOutEvent(QFocusEvent* ev);
    void paintSizeIndicator(int mouseY);

    void drawText(QPainter * painter, const QFont& font, const QPointF& location, const QString& text) const;

private Q_SLOTS:
    void toolChanged(const QString& toolId);

private:
    CanvasItem *m_pCanvas;
    View *m_pView;

    bool m_bSelection;
    int m_iSelectionAnchor;
    bool m_bResize;
    int m_iResizedRow;
    int m_iResizePos;
    /**
     * The label used for showing the current size, when resizing
     */
    QLabel *m_lSize;

    /**
     * True when the mouse button is pressed
     */
    bool m_bMousePressed;

    QRubberBand* m_rubberband;
    bool m_cellToolIsActive;
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
