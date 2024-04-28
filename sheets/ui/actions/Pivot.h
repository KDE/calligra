/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_PIVOT
#define CALLIGRA_SHEETS_ACTION_PIVOT

#include "CellAction.h"

namespace Calligra
{
namespace Sheets
{
class Pivot : public CellAction
{
    Q_OBJECT
public:
    Pivot(Actions *actions);
    virtual ~Pivot();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_PIVOT
