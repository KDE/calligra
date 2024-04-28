/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_SHOW_TABLE_VIEW
#define CALLIGRA_SHEETS_ACTION_SHOW_TABLE_VIEW

#include "CellAction.h"

namespace Calligra
{
namespace Sheets
{

class ShowTableView : public CellAction
{
    Q_OBJECT
public:
    ShowTableView(Actions *actions);
    virtual ~ShowTableView();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual QAction *createAction() override;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_SHOW_TABLE_VIEW
