/* This file is part of the KDE project
   SPDX-FileSelectAllrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileSelectAllrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_EDITING
#define CALLIGRA_SHEETS_ACTION_EDITING

#include "CellAction.h"

namespace Calligra
{
namespace Sheets
{

class EditCell : public CellAction
{
    Q_OBJECT
public:
    EditCell(Actions *actions);
    virtual ~EditCell();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    QAction *createAction() override;
};

/** Handles the F4 key in editor */
class PermuteFixation : public CellAction
{
    Q_OBJECT
public:
    PermuteFixation(Actions *actions);
    virtual ~PermuteFixation();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    QAction *createAction() override;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_EDITING
