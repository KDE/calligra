/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006, 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_STYLE_STORAGE
#define CALLIGRA_SHEETS_STYLE_STORAGE

#include <QObject>
#include <QPair>
#include <QVector>

#include "engine/StorageBase.h"
#include "engine/Region.h"

#include "sheets_core_export.h"
#include "Style.h"

namespace Calligra
{
namespace Sheets
{
class Map;
class StyleManager;
class SubStyle;
class StyleStorageLoaderJob;

/**
 * \ingroup Storage
 * \ingroup Style
 * The style storage.
 * Acts mainly as a wrapper around the R-Tree data structure to allow a future
 * replacement of this backend. Decorated with some additional features like
 * garbage collection, caching, used area tracking, etc.
 */
class CALLIGRA_SHEETS_CORE_EXPORT StyleStorage : public QObject, public StorageBase
{
    Q_OBJECT

public:
    explicit StyleStorage(Map* map);
    StyleStorage(const StyleStorage& other);
    ~StyleStorage() override;

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
    QVector< QPair<QRectF, SharedSubStyle> > currentData(const Region& rect) const;

    /**
     * Returns the area, which got a style attached.
     * \return the area using styles
     */
    QRect usedArea() const;

    /**
     * \return the OpenDocument column/row default cell styles
     * \ingroup OpenDocument
     */
   void saveCreateDefaultStyles(int& maxCols, int& maxRows, QMap<int, Style> &columnDefaultStyles, QMap<int, Style> &rowDefaultStyles) const;

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
    void insert(const QRect& rect, const SharedSubStyle& subStyle, bool markRegionChanged = true);

    /**
     * Assigns the substyles contained in \p style to the area \p region .
     */
    void insert(const Region& region, const Style& style);

    /**
     * Replaces the current styles with those in \p styles
     */
    void load(const QList<QPair<Region, Style> >& styles);

    /**
     * Inserts \p number rows at the position \p position .
     * It extends or shifts rectangles, respectively.
     */
    void insertRows(int position, int number) override;

    /**
     * Inserts \p number columns at the position \p position .
     * It extends or shifts rectangles, respectively.
     */
    void insertColumns(int position, int number) override;

    /**
     * Deletes \p number rows at the position \p position .
     * It shrinks or shifts rectangles, respectively.
     */
    void removeRows(int position, int number) override;

    /**
     * Deletes \p number columns at the position \p position .
     * It shrinks or shifts rectangles, respectively.
     */
    void removeColumns(int position, int number) override;

    /**
     * Shifts the rows right of \p rect to the right by the width of \p rect .
     * It extends or shifts rectangles, respectively.
     */
    void insertShiftRight(const QRect& rect) override;

    /**
     * Shifts the columns at the bottom of \p rect to the bottom by the height of \p rect .
     * It extends or shifts rectangles, respectively.
     */
    void insertShiftDown(const QRect& rect) override;

    /**
     * Shifts the rows left of \p rect to the left by the width of \p rect .
     * It shrinks or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    void removeShiftLeft(const QRect& rect) override;

    /**
     * Shifts the columns on top of \p rect to the top by the height of \p rect .
     * It shrinks or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    void removeShiftUp(const QRect& rect) override;

    const QVector< QPair<QRectF, SharedSubStyle> > &undoData() const;

    void resetUndo() override;


protected Q_SLOTS:
    void garbageCollection();

protected:
    /**
     * Triggers all necessary actions after a change of \p rect .
     * Adds the substyles in \p rect to the list of possible garbage.
     */
    void regionChanged(const QRect& rect);

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
    friend class StyleStorageLoaderJob;
    // disable assignment
    void operator=(const StyleStorage& other);

    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_STYLE_STORAGE
