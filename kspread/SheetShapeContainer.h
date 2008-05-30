/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KSPREAD_SHEET_SHAPE_CONTAINER
#define KSPREAD_SHEET_SHAPE_CONTAINER

#include <KoShapeLayer.h>

namespace KSpread
{
class Sheet;

/**
 * A container that owns all shapes present in a sheet.
 */
class SheetShapeContainer : public KoShapeLayer
{
public:
    /**
     * Constructor.
     * \param sheet the Sheet this container belongs to
     */
    SheetShapeContainer( Sheet* sheet );

    /**
     * Copy Constructor.
     */
    SheetShapeContainer(const SheetShapeContainer& other, Sheet* sheet);

    /**
     * Destructor.
     */
    virtual ~SheetShapeContainer();

    /**
     * \ingroup OpenDocument
     * Loads table shapes from odf.
     *
     * \param context the KoShapeLoadingContext used for loading
     * \param element element which represents the shape in odf
     *
     * \return false if loading failed
     */
    // reimplemented from KoShapeLayer
    virtual bool loadOdf( const KoXmlElement& element, KoShapeLoadingContext& context );

    /**
     * \ingroup OpenDocument
     * Saves table shapes to odf.
     *
     * This is the method that will be called when saving table shapes as a described in¬
     * OpenDocument 8.3.4 Table Shapes.
     *
     * \param context the KoShapeSavingContext used for saving
     */
    // reimplemented from KoShapeLayer
    virtual void saveOdf( KoShapeSavingContext& context ) const;

    /**
     * Insert \p number columns at \p position .
     * \return the data, that became out of range (shifted over the end)
     */
    void insertColumns(int position, int number);

    /**
     * Removes \p number columns at \p position .
     * \return the removed data
     */
    void removeColumns(int position, int number);

    /**
     * Insert \p number rows at \p position .
     * \return the data, that became out of range (shifted over the end)
     */
    void insertRows(int position, int number);

    /**
     * Removes \p number rows at \p position .
     * \return the removed data
     */
    void removeRows(int position, int number);

    /**
     * Shifts the data right of \p rect to the left by the width of \p rect .
     * The data formerly contained in \p rect becomes overridden.
     */
    void removeShiftLeft(const QRect& rect);

    /**
     * Shifts the data in and right of \p rect to the right by the width of \p rect .
     */
    void insertShiftRight(const QRect& rect);

    /**
     * Shifts the data below \p rect to the top by the height of \p rect .
     * The data formerly contained in \p rect becomes overridden.
     */
    void removeShiftUp(const QRect& rect);

    /**
     * Shifts the data in and below \p rect to the bottom by the height of \p rect .
     */
    void insertShiftDown(const QRect& rect);

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_SHEET_SHAPE_CONTAINER
