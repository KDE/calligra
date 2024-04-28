/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 - Robert Knight <robertknight@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_HEADERS
#define CALLIGRA_SHEETS_HEADERS

#include <QWidget>

class QLabel;
class QScrollBar;
class QRubberBand;

class KoPointerEvent;
class KoCanvasBase;

namespace Calligra
{
namespace Sheets
{
class Selection;
class Sheet;

namespace Tool
{

class ColumnHeader
{
public:
    explicit ColumnHeader();
    virtual ~ColumnHeader();

    int markerColumn() const
    {
        return m_iSelectionAnchor;
    }
    void equalizeColumn(double resize);

    void setSheet(Sheet *sheet)
    {
        m_sheet = sheet;
    }

    /// The geometry in document coordinates
    void setGeometry(const QRectF &geometry);
    QRectF geometry() const;
    QPointF position() const;
    QSizeF size() const;
    qreal width() const;
    qreal height() const;

    QPalette palette() const;

    void setHeaderFont(const QFont &font);
    QFont headerFont() const;

    void paint(QPainter *painter, const QRectF &painterRect);

    void mousePress(KoPointerEvent *_ev);
    void mouseRelease(KoPointerEvent *_ev);
    void mouseDoubleClick(KoPointerEvent *_ev);
    void mouseMove(KoPointerEvent *_ev);
    void resize(const QSizeF &size, const QSizeF &oldSize);
    void paintSizeIndicator(KoPointerEvent *event);
    void removeSizeIndicator();

    void setSelection(Selection *selection);
    void setScrollBars(QScrollBar *hbar, QScrollBar *vbar);

    void setCanvas(KoCanvasBase *canvas);
    void setSheetHeight(qreal height);

protected:
    qreal toUnitUserValue(qreal value);
    QString unitSymbol();
    qreal resizeAreaSize() const;
    qreal scrollOffset() const;
    /// returns lowest column or 0 if not between columns
    int betweenColumns(qreal pos) const;

protected:
    KoCanvasBase *m_pCanvas;
    Sheet *m_sheet;
    QRectF m_geometry;
    Selection *m_selection;
    QScrollBar *m_HScrollBar;
    QScrollBar *m_VScrollBar;
    /**
     * Flag that indicates whether the user wants to mark columns.
     * The user may mark columns by dragging the mouse around in th XBorder widget.
     * If the user is doing that right now, this flag is true. Mention that the user may
     * also resize columns by dragging the mouse. This case is not covered by this flag.
     */
    bool m_bSelection;

    /**
     * The column over which the user pressed the mouse button.
     * If the user marks columns in the XBorder widget, then this is the initial
     * column on which the user pressed the mouse button.
     */
    int m_iSelectionAnchor;

    /**
     * Flag that indicates whether the user resizes a column
     * The user may resize columns by dragging the mouse around in the ColumnHeader widget.
     * If the user is doing that right now, this flag is true.
     */
    bool m_bResize;

    /**
     * The column over which the user pressed the mouse button.
     * The user may resize columns by dragging the mouse around the XBorder widget.
     * This is the column over which the user pressed the mouse button. This column is going
     * to be resized.
     */
    int m_iResizedColumn;

    /**
     * Last position of the mouse, when resizing.
     */
    int m_iResizePos;
    qreal m_lastResizePos;
    QPointF m_mousePressedPoint;
    /**
     * The label used for showing the current size, when resizing
     */
    QLabel *m_lSize;

    /**
     * True when the mouse button is pressed
     */
    bool m_bMousePressed;

    QFont m_font;
    QRubberBand *m_rubberband;
    qreal m_sheetHeight;
};

class RowHeader
{
public:
    explicit RowHeader();
    virtual ~RowHeader();

    int markerRow() const
    {
        return m_iSelectionAnchor;
    }
    void equalizeRow(double resize);

    void setSheet(Sheet *sheet)
    {
        m_sheet = sheet;
    }

    /// The geometry in document coordinates
    void setGeometry(const QRectF &geometry);
    QRectF geometry() const;
    QPointF position() const;
    QSizeF size() const;
    qreal width() const;
    qreal height() const;

    QPalette palette() const;

    void setHeaderFont(const QFont &font);
    QFont headerFont() const;

    void paint(QPainter *painter, const QRectF &painterRect);

    void mousePress(KoPointerEvent *_ev);
    void mouseRelease(KoPointerEvent *_ev);
    void mouseDoubleClick(KoPointerEvent *_ev);
    void mouseMove(KoPointerEvent *_ev);
    void paintSizeIndicator(KoPointerEvent *event);
    void removeSizeIndicator();

    void doToolChanged(const QString &toolId);

    void setSelection(Selection *selection);
    void setScrollBars(QScrollBar *hbar, QScrollBar *vbar);

    void setCanvas(KoCanvasBase *canvas);
    void setSheetWidth(qreal width);

protected:
    qreal toUnitUserValue(qreal value);
    QString unitSymbol();
    qreal resizeAreaSize() const;
    qreal scrollOffset() const;
    /// returns lowest row or 0 if not between rows
    int betweenRows(qreal pos) const;

protected:
    KoCanvasBase *m_pCanvas;
    Sheet *m_sheet;
    QRectF m_geometry;
    Selection *m_selection;
    QScrollBar *m_HScrollBar;
    QScrollBar *m_VScrollBar;

    bool m_bSelection;
    int m_iSelectionAnchor;
    bool m_bResize;
    int m_iResizedRow;
    int m_iResizePos;
    int m_lastResizePos;

    QPointF m_mousePressedPoint;
    /**
     * The label used for showing the current size, when resizing
     */
    QLabel *m_lSize;

    /**
     * True when the mouse button is pressed
     */
    bool m_bMousePressed;

    QFont m_font;
    QRubberBand *m_rubberband;
    qreal m_sheetWidth;
};

} // namespace Tool
} // namespace Sheets
} // namespace Calligra

#endif
