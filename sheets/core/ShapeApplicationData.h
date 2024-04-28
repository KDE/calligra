/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_SHAPE_APPLICATION_DATA
#define CALLIGRA_SHEETS_SHAPE_APPLICATION_DATA

#include <KoShapeApplicationData.h>

#include "sheets_core_export.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Embedding
 * Calligra Sheets specific data attached to each shape.
 */
class CALLIGRA_SHEETS_CORE_EXPORT ShapeApplicationData : public KoShapeApplicationData
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
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_SHAPE_APPLICATION_DATA
