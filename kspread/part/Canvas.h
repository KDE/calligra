/* This file is part of the KDE project
   Copyright 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 1999-2002,2004 Laurent Montel <montel@kde.org>
   Copyright 2002-2005 Ariya Hidayat <ariya@kde.org>
   Copyright 1999-2001,2003 David Faure <faure@kde.org>
   Copyright 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2000-2001 Werner Trobin <trobin@kde.org>
   Copyright 2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 1999-2000 Torben Weis <weis@kde.org>
   Copyright 2000 Wilco Greven <greven@kde.org>
   Copyright 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   Copyright 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef KSPREAD_CANVAS
#define KSPREAD_CANVAS

#include <QList>
#include <QWidget>

#include <KoCanvasBase.h>

#include "kspread_export.h"

#include "Global.h"

// Width of row header and height of column headers.  These are not
// part of the styles.
// FIXME: Rename to ROWHEADER_WIDTH and COLHEADER_HEIGHT?
#define YBORDER_WIDTH  35
#define XBORDER_HEIGHT 20

class QDragLeaveEvent;
class QDragMoveEvent;
class QDropEvent;
class QEvent;
class QFocusEvent;
class QKeyEvent;
class QMouseEvent;
class QPainter;
class QPaintEvent;
class QPen;
class QResizeEvent;
class QScrollBar;

namespace KSpread
{
class Cell;
class CellEditor;
class Canvas;
class ColumnHeader;
class Doc;
class Sheet;
class RowHeader;
class Selection;
class View;


/**
 * The scrollable area showing the cells.
 *
 * Especially most of the user interface logic is implemented here.
 * That means that this class knows what to do when a key is pressed
 * or if the mouse button was clicked.
 */
class KSPREAD_EXPORT Canvas : public QWidget, public KoCanvasBase
{
    friend class ColumnHeader;
    friend class RowHeader;
    friend class View;
    friend class CellTool;

    Q_OBJECT

public:
    /**
     * The current action associated with the mouse.
     * Default is 'NoAction'.
     */
    enum MouseActions {
        NoAction,       /**< No mouse action (default) */
        Mark,           /**< Marking action */
        ResizeCell,     /**< Merging cell */
        AutoFill,       /**< Autofilling */
        ResizeSelection /**< Resizing the selection */
    };

    explicit Canvas(View* view);
    ~Canvas();

    View* view() const;
    Doc* doc() const;

    // KoCanvasBase interface methods.
    /// reimplemented method from KoCanvasBase
    virtual void gridSize(qreal* horizontal, qreal* vertical) const;
    /// reimplemented method from KoCanvasBase
    virtual bool snapToGrid() const;
    /// reimplemented method from KoCanvasBase
    virtual void addCommand(QUndoCommand* command);
    /// reimplemented method from KoCanvasBase
    virtual KoShapeManager* shapeManager() const;
    /// reimplemented method from KoCanvasBase
    virtual void updateCanvas(const QRectF& rc);
    /// reimplemented method from KoCanvasBase
    virtual KoToolProxy* toolProxy() const;
    /// reimplemented method from KoCanvasBase
    virtual const KoViewConverter* viewConverter() const;
    /// reimplemented method from KoCanvasBase
    virtual QWidget* canvasWidget() {
        return this;
    }
    virtual const QWidget* canvasWidget() const {
        return this;
    }
    /// reimplemented method from KoCanvasBase
    virtual KoUnit unit() const;
    /// reimplemented method from KoCanvasBase
    virtual void updateInputMethodInfo();

    /**
     * @return the usual selection of cells
     */
    KSpread::Selection* selection() const;

    QPointF offset() const;

    /**
     * @return the width of the columns before the current screen
     */
    double xOffset() const;

    /**
     * @return the height of the rows before the current screen
     */
    double yOffset() const;

    /**
     * @return a rect indicating which cell range is currently visible onscreen
     */
    QRect visibleCells() const;

    /**
     * @return a pointer to the active sheet
     */
    Sheet* activeSheet() const;

    /**
     * Validates the selected cell.
     */
    void validateSelection();

public Q_SLOTS:
    void setDocumentOffset(const QPoint& offset);
    void setDocumentSize(const QSizeF& size);

Q_SIGNALS:
    void documentSizeChanged(const QSize&);

protected:
    virtual void keyPressEvent(QKeyEvent* _ev);
    virtual void paintEvent(QPaintEvent* _ev);
    virtual void mousePressEvent(QMouseEvent* _ev);
    virtual void mouseReleaseEvent(QMouseEvent* _ev);
    virtual void mouseMoveEvent(QMouseEvent* _ev);
    virtual void mouseDoubleClickEvent(QMouseEvent*);
    virtual void focusInEvent(QFocusEvent*);
    virtual void focusOutEvent(QFocusEvent*);
    virtual void dragEnterEvent(QDragEnterEvent*);
    virtual void dragMoveEvent(QDragMoveEvent*);
    virtual void dragLeaveEvent(QDragLeaveEvent*);
    virtual void dropEvent(QDropEvent*);
    /// reimplemented method from superclass
    virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const;
    /// reimplemented method from superclass
    virtual void inputMethodEvent(QInputMethodEvent *event);
    /// reimplemented method from superclass
    virtual void tabletEvent(QTabletEvent *e);

    /**
     * Checks to see if there is a size grip for a highlight range at a given position.
     * Note that both X and Y coordinates are UNZOOMED.  To translate from a zoomed coordinate (eg. position of a mouse event) to
     * an unzoomed coordinate, use Doc::unzoomItX and Doc::unzoomItY.  The document object
     * can be accessed via view()->doc()
     * @param x Unzoomed x coordinate to check
     * @param y Unzoomed y coordinate to check
     * @return @c true if there is a size grip at the specified position, @c false otherwise.
     */
    bool highlightRangeSizeGripAt(double x, double y);

private:
    virtual bool eventFilter(QObject *o, QEvent *e);

    ColumnHeader* columnHeader() const;
    RowHeader* rowHeader() const;
    QScrollBar* horzScrollBar() const;
    QScrollBar* vertScrollBar() const;

    /**
     * Returns the range of cells which appear in the specified area of the Canvas widget
     * For example, viewToCellCoordinates( QRect(0,0,width(),height()) ) returns a range containing all visible cells
     *
     * @param area The area (in pixels) on the Canvas widget
     */
    QRect viewToCellCoordinates(const QRectF& area) const;

    /**
     * Calculates the region in view coordinates occupied by a range of cells on
     * the currently active sheet. Respects the scrolling offset and the layout
     * direction
     *
     * \param cellRange The range of cells on the current sheet.
     */
    QRectF cellCoordinatesToView(const QRect& cellRange) const;

private:
    void startTheDrag();

    /**
     * Determines the cell at @p point and shows its tooltip.
     * @param point the position for which a tooltip is requested
     */
    void showToolTip(const QPoint& point);

private:
    Q_DISABLE_COPY(Canvas)

    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_CANVAS
