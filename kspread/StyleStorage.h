/* This file is part of the KDE project
   Copyright 2006,2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KSPREAD_STYLE_STORAGE
#define KSPREAD_STYLE_STORAGE

#include <QObject>
#include <QPair>
#include <QPoint>
#include <QRect>

#include "kspread_export.h"
#include <Region.h>
#include <Style.h>

namespace KSpread
{
class Map;
class OdfSavingContext;
class Style;
class StyleManager;
class SubStyle;

/**
 * \ingroup Storage
 * \ingroup Style
 * The style storage.
 * Acts mainly as a wrapper around the R-Tree data structure to allow a future
 * replacement of this backend. Decorated with some additional features like
 * garbage collection, caching, used area tracking, etc.
 */
class KSPREAD_EXPORT StyleStorage : public QObject
{
    Q_OBJECT

public:
    explicit StyleStorage(Map* map);
    StyleStorage(const StyleStorage& other);
    virtual ~StyleStorage();

    /**
     * Composes the style for \p point. All substyles intersecting \p point are considered.
     * \return the Style at the position \p point .
     */
    Style contains(const QPoint& point) const;

    /**
     * Composes the style for \p rect. Only substyles which fill out \p rect completely are
     * considered. In contrast to intersects(const QRect&).
     * Especially useful on saving cell styles assigned to columns or rows.
     * \return the Style for the area \p rect .
     * \see intersects
     */
    Style contains(const QRect& rect) const;

    /**
     * Composes the style for \p rect. All substyles which intersect \p rect are considered.
     * In contrast to contains(const QRect&).
     * \return the Style for the area \p rect .
     * \see contains
     */
    Style intersects(const QRect& rect) const;

    /**
     * Collects all substyle/range pairs, that intersect \p rect. With this data one can
     * reconstruct the former state of the storage after modification.
     * \return all substyle/range pairs intersecting \p rect
     */
    QList< QPair<QRectF, SharedSubStyle> > undoData(const Region& rect) const;

    /**
     * Returns the area, which got a style attached.
     * \return the area using styles
     */
    QRect usedArea() const;

    /**
     * \return the OpenDocument column/row default cell styles
     * \ingroup OpenDocument
     */
    void saveOdfCreateDefaultStyles(int& maxCols, int& maxRows, OdfSavingContext& tableContext) const;

    /**
     * Returns the index of the next column-wide cell style after \p column or zero
     * if there's none.
     * \return the index of the next styled column
     */
    int nextColumnStyleIndex(int column) const;

    /**
     * Returns the index of the next row-wide cell style after \p row or zero
     * if there's none.
     * \return the index of the next styled row
     */
    int nextRowStyleIndex(int row) const;

    /**
     * Returns the index of the first cell style in \p row or zero
     * if there's none.
     * \return the index of the next styled column
     */
    int firstColumnIndexInRow(int row) const;

    /**
     * Returns the index of the next cell style in \p row after \p column or zero
     * if there's none.
     * \return the index of the next styled column
     */
    int nextColumnIndexInRow(int column, int row) const;

    /**
     * Assigns \p subStyle to the area \p rect .
     */
    void insert(const QRect& rect, const SharedSubStyle& subStyle);

    /**
     * Assigns the substyles contained in \p style to the area \p region .
     */
    void insert(const Region& region, const Style& style);

    /**
     * Inserts \p number rows at the position \p position .
     * It extends or shifts rectangles, respectively.
     */
    QList< QPair<QRectF, SharedSubStyle> > insertRows(int position, int number = 1);

    /**
     * Inserts \p number columns at the position \p position .
     * It extends or shifts rectangles, respectively.
     */
    QList< QPair<QRectF, SharedSubStyle> > insertColumns(int position, int number = 1);

    /**
     * Deletes \p number rows at the position \p position .
     * It shrinks or shifts rectangles, respectively.
     */
    QList< QPair<QRectF, SharedSubStyle> > removeRows(int position, int number = 1);

    /**
     * Deletes \p number columns at the position \p position .
     * It shrinks or shifts rectangles, respectively.
     */
    QList< QPair<QRectF, SharedSubStyle> > removeColumns(int position, int number = 1);

    /**
     * Shifts the rows right of \p rect to the right by the width of \p rect .
     * It extends or shifts rectangles, respectively.
     */
    QList< QPair<QRectF, SharedSubStyle> > insertShiftRight(const QRect& rect);

    /**
     * Shifts the columns at the bottom of \p rect to the bottom by the height of \p rect .
     * It extends or shifts rectangles, respectively.
     */
    QList< QPair<QRectF, SharedSubStyle> > insertShiftDown(const QRect& rect);

    /**
     * Shifts the rows left of \p rect to the left by the width of \p rect .
     * It shrinks or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    QList< QPair<QRectF, SharedSubStyle> > removeShiftLeft(const QRect& rect);

    /**
     * Shifts the columns on top of \p rect to the top by the height of \p rect .
     * It shrinks or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    QList< QPair<QRectF, SharedSubStyle> > removeShiftUp(const QRect& rect);

    /**
     * Invalidates all cached styles.
     */
    void invalidateCache();

protected Q_SLOTS:
    void garbageCollection();

protected:
    /**
     * Triggers all necessary actions after a change of \p rect .
     * Calls invalidateCache() and adds the substyles in
     * \p rect to the list of possible garbage.
     */
    void regionChanged(const QRect& rect);

    /**
     * Invalidates all cached styles lying in \p rect .
     */
    void invalidateCache(const QRect& rect);

    /**
     * Composes a style of \p substyles .
     * \return the composed style
     */
    Style composeStyle(const QList<SharedSubStyle>& subStyles) const;

    /**
     * Convenience method.
     * \return the StyleManager
     */
    StyleManager* styleManager() const;

private:
    // disable assignment
    void operator=(const StyleStorage& other);

    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_STYLE_STORAGE
