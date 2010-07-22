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

#include <QWidget>

class QLabel;
class KoPointerEvent;

namespace KSpread
{
class CanvasBase;
class Selection;
class View;

/**
 * Base class for the widget above the cells showing the column headers.
 */
class ColumnHeader
{
public:
    ColumnHeader(CanvasBase *_canvas, View *_view);
    virtual ~ColumnHeader();

    int markerColumn() const {
        return  m_iSelectionAnchor;
    }
    void equalizeColumn(double resize);

    virtual void updateColumns(int from, int to) = 0;
    virtual QSizeF size() const = 0;
    qreal width() const { return size().width(); }
    qreal height() const { return size().height(); }
    virtual void setCursor(const QCursor& cursor) = 0;
    virtual void scroll(qreal dx, qreal dy) = 0;
    virtual QPalette palette() const = 0;
    virtual void update() = 0;
protected:
    void paint(QPainter* painter, const QRectF& painterRect);
    void mousePress(KoPointerEvent* _ev);
    void mouseRelease(KoPointerEvent* _ev);
    void mouseDoubleClick(KoPointerEvent* _ev);
    void mouseMove(KoPointerEvent* _ev);
    void wheel(KoPointerEvent* _ev);
    void focusOut(QFocusEvent* ev);
    void resize(const QSizeF& size, const QSizeF& oldSize);
    virtual void paintSizeIndicator(int mouseX) = 0;
    virtual void removeSizeIndicator() = 0;

    void drawText(QPainter* painter, const QFont& font, const QPointF& location, const QString& text, double width) const;

    
    void doToolChanged(const QString& toolId);

protected:
    CanvasBase *m_pCanvas;
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

    bool m_cellToolIsActive;
};



/**
 * Base class for the widget left to the cells showing the row headers.
 */
class RowHeader
{
public:
    RowHeader(CanvasBase *_canvas, View *_view);
    virtual ~RowHeader();

    int markerRow() const {
        return  m_iSelectionAnchor;
    }
    void equalizeRow(double resize);
    
    virtual void updateRows(int from, int to) = 0;
    virtual QSizeF size() const = 0;
    qreal width() const { return size().width(); }
    qreal height() const { return size().height(); }
    virtual void setCursor(const QCursor& cursor) = 0;
    virtual void scroll(qreal dx, qreal dy) = 0;
    virtual QPalette palette() const = 0;
    virtual void update() = 0;
protected:
    void paint(QPainter* painter, const QRectF& painterRect);
    void mousePress(KoPointerEvent* _ev);
    void mouseRelease(KoPointerEvent* _ev);
    void mouseDoubleClick(KoPointerEvent* _ev);
    void mouseMove(KoPointerEvent* _ev);
    void wheel(KoPointerEvent* _ev);
    void focusOut(QFocusEvent* ev);
    virtual void paintSizeIndicator(int mouseX) = 0;
    virtual void removeSizeIndicator() = 0;

    void drawText(QPainter* painter, const QFont& font, const QPointF& location, const QString& text) const;

    void doToolChanged(const QString& toolId);
protected:
    CanvasBase *m_pCanvas;
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

    bool m_cellToolIsActive;
};



/**
 * Base class for the widget in the top left corner of the canvas,
 * responsible for selecting all cells in a sheet.
 */
class SelectAllButton
{
public:
    explicit SelectAllButton(CanvasBase* canvasBase, Selection* selection);
    virtual ~SelectAllButton();

    virtual QPalette palette() const = 0;
    virtual void update() = 0;
protected:
    virtual void paint(QPainter* painter, const QRectF& painterRect);
    virtual void mousePress(KoPointerEvent* event);
    virtual void mouseRelease(KoPointerEvent* event);
    virtual void wheel(KoPointerEvent* event);
    void doToolChanged(const QString& toolId);

private:
    CanvasBase* m_canvasBase;
    Selection* m_selection;
    bool m_mousePressed;
    bool m_cellToolIsActive;
};

} // namespace KSpread
#endif
