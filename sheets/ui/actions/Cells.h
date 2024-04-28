/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_CELLS
#define CALLIGRA_SHEETS_ACTION_CELLS

#include "CellAction.h"

namespace Calligra
{
namespace Sheets
{
class InsertDialog;

class CellsInsert : public CellAction
{
    Q_OBJECT
public:
    CellsInsert(Actions *actions);
    virtual ~CellsInsert();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    QAction *createAction() override;

    InsertDialog *m_dlg;
};

class CellsRemove : public CellAction
{
    Q_OBJECT
public:
    CellsRemove(Actions *actions);
    virtual ~CellsRemove();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    QAction *createAction() override;

    InsertDialog *m_dlg;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_CELLS
