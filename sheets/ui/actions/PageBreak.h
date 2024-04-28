/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_PAGEBREAK
#define CALLIGRA_SHEETS_ACTION_PAGEBREAK

#include "CellAction.h"

namespace Calligra
{
namespace Sheets
{

class PageBreakColumn : public ToggleableCellAction
{
    Q_OBJECT
public:
    PageBreakColumn(Actions *actions);
    virtual ~PageBreakColumn();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
    virtual bool enabledForSelection(Selection *selection, const Cell &activeCell) override;
};

class PageBreakRow : public ToggleableCellAction
{
    Q_OBJECT
public:
    PageBreakRow(Actions *actions);
    virtual ~PageBreakRow();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
    virtual bool enabledForSelection(Selection *selection, const Cell &activeCell) override;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_PAGEBREAK
