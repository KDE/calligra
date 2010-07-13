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
#include <QGraphicsWidget>

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
class CanvasItem;
class ColumnHeaderItem;
class Doc;
class Sheet;
class RowHeaderItem;
class Selection;
class View;


/**
 * The scrollable area showing the cells.
 */
class KSPREAD_EXPORT CanvasItem : public QGraphicsWidget, public KoCanvasBase
{
    friend class ColumnHeaderItem;
    friend class RowHeaderItem;
    friend class View;
    friend class CellTool;

    Q_OBJECT

public:
    explicit CanvasItem(View* view);
    ~CanvasItem();

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
        return 0;
    }
    virtual const QWidget* canvasWidget() const {
        return 0;
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
    virtual void paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* _ev);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _ev);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* _ev);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*);
    virtual void focusInEvent(QFocusEvent*);
    virtual void focusOutEvent(QFocusEvent*);
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent*);
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent*);
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent*);
    virtual void dropEvent(QGraphicsSceneDragDropEvent*);
    /// reimplemented method from superclass
    virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const;
    /// reimplemented method from superclass
    virtual void inputMethodEvent(QInputMethodEvent *event);
    /// reimplemented method from superclass
    virtual void tabletEvent(QTabletEvent *e);

private:
    virtual bool eventFilter(QObject *o, QEvent *e);

    ColumnHeaderItem* columnHeader() const;
    RowHeaderItem* rowHeader() const;
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
    /**
     * Determines the cell at @p point and shows its tooltip.
     * @param point the position for which a tooltip is requested
     */
    void showToolTip(const QPoint& point);

private:
    Q_DISABLE_COPY(CanvasItem)

    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_CANVAS
