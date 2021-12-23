/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2021 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __CELLBASE_H__
#define __CELLBASE_H__


#include "sheets_engine_export.h"
#include <QString>


namespace Calligra
{
namespace Sheets
{


class CALLIGRA_SHEETS_ENGINE_EXPORT CellBase
{
public:
    explicit CellBase();

    ~CellBase();



    /**
     * Given the column number, this static function returns the corresponding
     * column name, i.e. the first column is "A", the second is "B", and so on.
     */
    static QString columnName(uint column);




private:
    class Private;
    Private * const d;
};


} // namespace Sheets
} // namespace Calligra


#endif
