/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2021 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __CELLBASE_H__
#define __CELLBASE_H__


#include "sheets_engine_export.h"
#include "Value.h"

#include <QString>
#include <QPoint>
#include <QSharedDataPointer>

namespace Calligra
{
namespace Sheets
{

class SheetBase;

class CALLIGRA_SHEETS_ENGINE_EXPORT CellBase
{
public:

    /**
     * Constructor.
     * Creates the null cell.
     * \note Accessing many methods will fail as the sheet is not set.
     */
    CellBase();

    /**
     * Constructor.
     * Creates a CellBase for accessing the data in \p sheet at position \p col , \p row.
     */
    CellBase(SheetBase* sheet, unsigned int column, unsigned int row);

    /**
     * Constructor.
     * Creates a Cell for accessing the data in \p sheet at position \p pos .
     */
    CellBase(SheetBase* sheet, const QPoint& pos);

    /**
     * Copy constructor.
     */
    CellBase(const CellBase& other);

    /**
     * Destructor.
     */
    virtual ~CellBase();

    /**
     * Returns true if this cell is the null cell.
     */
    bool isNull() const;

    /**
     * \return the sheet this cell belongs to
     */
    SheetBase* sheet() const;

    /**
     * Returns the cell's column.
     */
    int column() const;

    /**
     * Returns the cell's row.
     */
    int row() const;


    /**
     * Returns the value that this cell holds. It could be from the user
     * (i.e. when s/he enters a value) or a result of formula.
     */
    const Value value() const;

    /**
     * Sets the value for this cell.
     * It also clears all errors, if the value itself is not an error.
     * In addition to this, it calculates the outstring and sets the dirty
     * flags so that a redraw is forced.
     * \param value the new value
     *
     * \see setUserInput, parseUserInput
     */
    void setValue(const Value& value);





    /**
     * Given the column number, this static function returns the corresponding
     * column name, i.e. the first column is "A", the second is "B", and so on.
     */
    static QString columnName(unsigned int column);




private:
    class Private;
    QSharedDataPointer<Private> d;
};


} // namespace Sheets
} // namespace Calligra


#endif
