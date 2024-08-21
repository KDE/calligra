/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_SHEET_VIEW
#define CALLIGRA_SHEETS_SHEET_VIEW

#include <QObject>
#include <QRect>

#include "sheets_ui_export.h"

class QPainter;
class QRectF;
class QSize;
class QSizeF;
class QColor;

class KoViewConverter;

namespace Calligra
{
namespace Sheets
{
class CellView;
class Region;
class Sheet;
class CanvasBase;

/**
 * \ingroup Painting
 * The SheetView controls the painting of the sheets' cells.
 * It caches a set of CellViews.
 */
class CALLIGRA_SHEETS_UI_EXPORT SheetView : public QObject
{
    Q_OBJECT

    friend class CellView;

public:
    /**
     * Constructor.
     */
    explicit SheetView(Sheet *sheet);

    /**
     * Destructor.
     */
    ~SheetView() override;

    /**
     * \return the Sheet
     */
    Sheet *sheet() const;

    /**
     * Sets the KoViewConverter used by this SheetView.
     */
    void setViewConverter(const KoViewConverter *viewConverter);

    /**
     * \return the view in which the Sheet is painted
     */
    const KoViewConverter *viewConverter() const;

    /**
     * Looks up a CellView for the position \p col , \p row in the cache.
     * If no CellView exists yet, one is created and inserted into the cache.
     *
     * \return the CellView for the position
     */
#ifdef CALLIGRA_SHEETS_MT
    CellView cellView(int col, int row);
    CellView cellView(const QPoint &pos);
#else
    const CellView &cellView(int col, int row);
    const CellView &cellView(const QPoint &pos);
#endif

    /**
     * Set the cell range, that should be painted to \p rect .
     * It also adjusts the cache size linear to the size of \p rect .
     */
    void setPaintCellRange(const QRect &rect);

    /**
     * Invalidates all cached CellViews in \p region .
     */
    void invalidateRegion(const Region &region);

    /**
     * Invalidates all CellViews, the cached and the default.
     */
    virtual void invalidate();

    /**
     * Paints the cells.
     */
    virtual void
    paintCells(QPainter &painter, const QRectF &paintRect, const QPointF &topLeft, CanvasBase *canvas = nullptr, const QRect &visibleRect = QRect());

    QPoint obscuringCell(const QPoint &obscuredCell) const;
    QSize obscuredRange(const QPoint &obscuringCell) const;
    QRect obscuredArea(const QPoint &cell) const;
    bool isObscured(const QPoint &cell) const;
    bool obscuresCells(const QPoint &cell) const;
    QSize totalObscuredRange() const;

    /*
     * Highlighting cells.
     */
    bool isHighlighted(const QPoint &cell) const;
    void setHighlighted(const QPoint &cell, bool isHighlighted);
    bool hasHighlightedCells() const;
    void clearHighlightedCells();
    QPoint activeHighlight() const;
    void setActiveHighlight(const QPoint &cell);

    void setHighlightMaskColor(const QColor &color);
    void setHighlightColor(const QColor &color);
    void setActiveHighlightColor(const QColor &color);

    void setViewSize(const QSize &size);
public Q_SLOTS:
    void updateAccessedCellRange(const QPoint &location = QPoint());

Q_SIGNALS:
    void visibleSizeChanged(const QSizeF &);
    void obscuredRangeChanged(const QSize &);

protected:
    virtual CellView *createDefaultCellView();
    virtual CellView *createCellView(int col, int row);
    QRect paintCellRange() const;

protected:
    /**
     * Helper method for invalidateRegion().
     * Invalidates all cached CellViews in \p range .
     * \internal
     */
    virtual void invalidateRange(const QRect &range);

    /**
     * Marks other CellViews in \p range as obscured by the CellView at \p position .
     * Used by CellView.
     */
    void obscureCells(const QPoint &position, int numXCells, int numYCells);

    /**
     * Returns the default CellView.
     * Used by CellView.
     */
#ifdef CALLIGRA_SHEETS_MT
    CellView defaultCellView() const;
#else
    const CellView &defaultCellView() const;
#endif

private:
    Q_DISABLE_COPY(SheetView)

    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_SHEET_VIEW
