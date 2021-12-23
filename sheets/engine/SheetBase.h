/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2021 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __SHEETBASE_H__
#define __SHEETBASE_H__




#include "sheets_engine_export.h"



namespace Calligra
{
namespace Sheets
{


class CALLIGRA_SHEETS_ENGINE_EXPORT SheetBase
{
public:
    explicit SheetBase();

    ~SheetBase();






private:
    // disable copying
    SheetBase(const SheetBase &) = delete;
    SheetBase &operator=(const SheetBase &) = delete;

    class Private;
    Private * const d;
};


} // namespace Sheets
} // namespace Calligra


#endif
