/*   
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
 * Boston, MA 02110-1301, USA.
*/
#ifndef BORDER_H
#define BORDER_H

#include <QWidget>

class QLabel;
class QRubberBand;

namespace KSpread
{

class Canvas;
class View;

/**
 * The widget above the cells showing the column headers.
 */
class HBorder : public QWidget
{
    Q_OBJECT
public:
    HBorder( QWidget *_parent, Canvas *_canvas, View *_view  );
    virtual ~HBorder();

    int markerColumn() const { return  m_iSelectionAnchor; }
    void equalizeColumn( double resize );

    void updateColumns( int from, int to );

    QSize sizeHint() const;

private slots:
    void slotAutoScroll(const QPoint& distance);

protected:
    virtual void paintEvent ( QPaintEvent* _ev );
    virtual void mousePressEvent( QMouseEvent* _ev );
    virtual void mouseReleaseEvent( QMouseEvent* _ev );
    virtual void mouseDoubleClickEvent( QMouseEvent* _ev );
    virtual void mouseMoveEvent( QMouseEvent* _ev );
    virtual void wheelEvent( QWheelEvent* );
    virtual void focusOutEvent( QFocusEvent* ev );
    virtual void resizeEvent( QResizeEvent * _ev );
    void paintSizeIndicator( int mouseX );

private:
    Canvas *m_pCanvas;
    View *m_pView;
//     QTimer * m_scrollTimer;

    /**
     * Flag that inidicates whether the user wants to mark columns.
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
     * The user may resize columns by dragging the mouse around in the HBorder widget.
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

private:
};



/**
 * The widget left to the cells showing the row headers.
 */
class VBorder : public QWidget
{
    Q_OBJECT
public:
    VBorder( QWidget *_parent, Canvas *_canvas, View *_view );
    virtual ~VBorder();

    int markerRow() const { return  m_iSelectionAnchor; }
    void equalizeRow( double resize );
    void updateRows( int from, int to );

    QSize sizeHint() const;

private slots:
    void slotAutoScroll(const QPoint& distance);

protected:
    virtual void paintEvent ( QPaintEvent* _ev );
    virtual void mousePressEvent( QMouseEvent* _ev );
    virtual void mouseReleaseEvent( QMouseEvent* _ev );
    virtual void mouseMoveEvent( QMouseEvent* _ev );
    virtual void mouseDoubleClickEvent( QMouseEvent* _ev );
    virtual void wheelEvent( QWheelEvent* );
    virtual void focusOutEvent( QFocusEvent* ev );
    void paintSizeIndicator( int mouseY );

private:
    Canvas *m_pCanvas;
    View *m_pView;
//     QTimer * m_scrollTimer;

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
};

} // namespace KSpread
#endif
