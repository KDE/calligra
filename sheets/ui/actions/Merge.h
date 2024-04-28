/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_MERGE
#define CALLIGRA_SHEETS_ACTION_MERGE

#include "CellAction.h"

#include "ui/commands/AbstractRegionCommand.h"

namespace Calligra
{
namespace Sheets
{

class Merge : public CellAction
{
    Q_OBJECT
public:
    Merge(Actions *actions, bool horizontal, bool vertical);
    virtual ~Merge();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool enabledForSelection(Selection *selection, const Cell &activeCell) override;
    virtual QAction *createAction() override;

    bool m_horizontal, m_vertical;
};

class RemoveMerge : public CellAction
{
    Q_OBJECT
public:
    RemoveMerge(Actions *actions);
    virtual ~RemoveMerge();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_MERGE
