/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef CALLIGRA_SHEETS_SHAPE_APPLICATION_DATA
#define CALLIGRA_SHEETS_SHAPE_APPLICATION_DATA

#include <KoShapeApplicationData.h>

#include "sheets_odf_export.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Embedding
 * Calligra Sheets specific data attached to each shape.
 */
class CALLIGRA_SHEETS_ODF_EXPORT ShapeApplicationData : public KoShapeApplicationData
{
public:
    /**
     * Constructor.
     */
    ShapeApplicationData();

    /**
     * Destructor.
     */
    ~ShapeApplicationData() override;

    void setAnchoredToCell(bool state);
    bool isAnchoredToCell() const;

private:
    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_SHAPE_APPLICATION_DATA
