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

#ifndef CALLIGRA_SHEETS_CANVASBASE
#define CALLIGRA_SHEETS_CANVASBASE

#include <QList>
#include <QSizeF>
#include <KoCanvasBase.h>

#include "sheets_common_export.h"
#include "../Global.h"

class QFocusEvent;
class QKeyEvent;
class QInputMethodEvent;
class QTabletEvent;
class QMimeData;
class QPainter;
class KoPointerEvent;
class KoZoomHandler;

namespace Calligra
{
namespace Sheets
{
class Canvas;
class ColumnHeader;
class Doc;
class Sheet;
class RowHeader;
class Selection;
class SheetView;

/**
 * The scrollable area showing the cells.
 */
class CALLIGRA_SHEETS_COMMON_EXPORT CanvasBase : public KoCanvasBase
{
    friend class ColumnHeader;
    friend class RowHeader;
    friend class View;
    friend class CellTool;
    friend class Canvas;
public:
    explicit CanvasBase(Doc* doc);
    ~CanvasBase() override;

    Doc* doc() const;

    // KoCanvasBase interface methods.
    /// reimplemented method from KoCanvasBase
    void gridSize(qreal* horizontal, qreal* vertical) const override;
    /// reimplemented method from KoCanvasBase
    bool snapToGrid() const override;
    /// reimplemented method from KoCanvasBase
    void addCommand(KUndo2Command* command) override;
    /// reimplemented method from KoCanvasBase
    KoShapeManager* shapeManager() const override;
    /// reimplemented method from KoCanvasBase
    void updateCanvas(const QRectF& rc) override;
    /// reimplemented method from KoCanvasBase
    KoToolProxy* toolProxy() const override;

    /// reimplemented method from KoCanvasBase
    KoUnit unit() const override;
    /// reimplemented method from KoCanvasBase
    void updateInputMethodInfo() override;

    /**
     * @return the usual selection of cells
     */
    virtual Calligra::Sheets::Selection* selection() const = 0;

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
    virtual Sheet* activeSheet() const = 0;

    /**
     * Validates the selected cell.
     */
    void validateSelection();

public:
    void setDocumentOffset(const QPoint& offset);
    void setDocumentSize(const QSizeF& size);

protected:
    void keyPressed(QKeyEvent* _ev);
    void paint(QPainter* painter, const QRectF& painterRect); // maybe add a QRegion overload
    void mousePressed(KoPointerEvent* _ev);
    void mouseReleased(KoPointerEvent* _ev);
    void mouseMoved(KoPointerEvent* _ev);
    void mouseDoubleClicked(KoPointerEvent* _ev);
    void focusIn(QFocusEvent*);

    bool dragEnter(const QMimeData* mimeData);
    bool dragMove(const QMimeData* mimeData, const QPointF& eventPos, const QObject *source);
    void dragLeave();
    bool drop(const QMimeData* mimeData, const QPointF& eventPos, const QObject *source);

    QVariant inputMethodQuery(Qt::InputMethodQuery query) const;
    void inputMethodEvent(QInputMethodEvent *event);
    void tabletEvent(QTabletEvent *e);

    bool eventFilter(QObject *o, QEvent *e);
    virtual ColumnHeader* columnHeader() const { return 0; };
    virtual RowHeader* rowHeader() const { return 0; };

public:
    virtual void update() = 0;
    virtual void update(const QRectF& rect) = 0;
    virtual void documentSizeChanged(const QSize&) = 0;
    virtual Qt::LayoutDirection layoutDirection() const = 0;
    virtual QRectF rect() const = 0;
    virtual QSizeF size() const = 0;
    qreal width() const { return size().width(); }
    qreal height() const { return size().height(); }
    virtual QPoint mapToGlobal(const QPointF& point) const = 0;
    virtual void updateMicroFocus() = 0;

    virtual KoZoomHandler* zoomHandler() const = 0;
    KoViewConverter* viewConverter() const override;
    virtual bool isViewLoading() const = 0; // not sure if is needed
    virtual SheetView* sheetView(const Sheet* sheet) const = 0;
    virtual void enableAutoScroll() = 0;
    virtual void disableAutoScroll() = 0;

    virtual void showContextMenu(const QPoint& globalPos) = 0;
protected:
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

    // negative values mean max - pos
    virtual void setVertScrollBarPos(qreal pos) = 0;    
    virtual void setHorizScrollBarPos(qreal pos) = 0;
private:
    /**
     * Determines the cell at @p point and shows its tooltip.
     * @param point the position for which a tooltip is requested
     */
    void showToolTip(const QPoint& point);

private:
    Q_DISABLE_COPY(CanvasBase)

    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CANVAS
