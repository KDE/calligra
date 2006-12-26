/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include <koffice_export.h>

namespace KSpread
{

class Sheet;
class Style;
class StyleManager;
class StyleManipulator;
class SubStyle;

/**
 * The style storage.
 * Acts mainly as a wrapper around the R-Tree data structure to allow a future
 * replacement of this backend. Decorated with some additional features like
 * garbage collection, caching, used area tracking, etc.
 */
class KSPREAD_EXPORT StyleStorage : public QObject
{
    Q_OBJECT

public:
    explicit StyleStorage( Sheet* sheet );
    virtual ~StyleStorage();

    /**
     * \return the Style at the position \p point .
     */
    Style contains(const QPoint& point) const;

    /**
     * \return the Style for the area \p rect .
     */
    Style contains(const QRect& rect) const;

    /**
     * \return the Style for the area \p rect .
     */
    Style intersects(const QRect& rect) const;

    /**
     *
     */
    QList< QPair<QRectF,SharedSubStyle> > undoData(const QRect& rect) const;

    /**
     * Returns the area, which got a style attached.
     * \return the area using styles
     */
    QRect usedArea() const;

    /**
     * Returns the index of the next column cell style after \p column or zero
     * if there's none.
     * \return the index of the next styled column
     */
    int nextColumn( int column ) const;

    /**
     * Returns the index of the next row cell style after \p row or zero
     * if there's none.
     * \return the index of the next styled row
     */
    int nextRow( int row ) const;

    /**
     * Returns the index of the next cell style in \p row after \p column or zero
     * if there's none.
     * \return the index of the next styled column
     */
    int nextStyleRight( int column, int row ) const;

    /**
     * Assigns \p subStyle to the area \p rect .
     */
    void insert(const QRect& rect, const SharedSubStyle& subStyle);

    /**
     * Assigns the substyles contained in \p style to the area \p region .
     */
    void insert(const Region& region, const Style& style );

    /**
     * Inserts \p number rows at the position \p position .
     * It extends or shifts rectangles, respectively.
     */
    QList< QPair<QRectF,SharedSubStyle> > insertRows(int position, int number = 1);

    /**
     * Inserts \p number columns at the position \p position .
     * It extends or shifts rectangles, respectively.
     */
    QList< QPair<QRectF,SharedSubStyle> > insertColumns(int position, int number = 1);

    /**
     * Deletes \p number rows at the position \p position .
     * It shrinks or shifts rectangles, respectively.
     */
    QList< QPair<QRectF,SharedSubStyle> > deleteRows(int position, int number = 1);

    /**
     * Deletes \p number columns at the position \p position .
     * It shrinks or shifts rectangles, respectively.
     */
    QList< QPair<QRectF,SharedSubStyle> > deleteColumns(int position, int number = 1);

    /**
     * Shifts the rows right of \p rect to the right by the width of \p rect .
     * It extends or shifts rectangles, respectively.
     */
    QList< QPair<QRectF,SharedSubStyle> > shiftRows(const QRect& rect);

    /**
     * Shifts the columns at the bottom of \p rect to the bottom by the height of \p rect .
     * It extends or shifts rectangles, respectively.
     */
    QList< QPair<QRectF,SharedSubStyle> > shiftColumns(const QRect& rect);

    /**
     * Shifts the rows left of \p rect to the left by the width of \p rect .
     * It shrinks or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    QList< QPair<QRectF,SharedSubStyle> > unshiftRows(const QRect& rect);

    /**
     * Shifts the columns on top of \p rect to the top by the height of \p rect .
     * It shrinks or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    QList< QPair<QRectF,SharedSubStyle> > unshiftColumns(const QRect& rect);

protected Q_SLOTS:
    void garbageCollectionInitialization();
    void garbageCollection();

protected:
    /**
     * Invalidates all cached styles lying in \p rect .
     */
    void invalidateCache( const QRect& rect );

    /**
     * Composes a style of \p substyles .
     * \return the composed style
     */
    Style composeStyle( const QList<SharedSubStyle>& subStyles ) const;

    /**
     * Convenience method.
     * \return the StyleManager
     */
    StyleManager* styleManager() const;

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_STYLE_STORAGE
