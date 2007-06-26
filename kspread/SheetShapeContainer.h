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

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_SHEET_SHAPE_CONTAINER
