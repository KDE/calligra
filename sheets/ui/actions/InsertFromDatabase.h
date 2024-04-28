/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_INSERT_FROM_DATABASE
#define CALLIGRA_SHEETS_ACTION_INSERT_FROM_DATABASE

#include "DialogCellAction.h"

namespace Calligra
{
namespace Sheets
{

class InsertFromDatabase : public DialogCellAction
{
    Q_OBJECT
public:
    InsertFromDatabase(Actions *actions);
    virtual ~InsertFromDatabase();

protected:
    virtual ActionDialog *createDialog(QWidget *canvasWidget) override;
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual QAction *createAction() override;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_INSERT_FROM_DATABASE
