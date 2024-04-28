/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_GOAL_SEEK
#define CALLIGRA_SHEETS_ACTION_GOAL_SEEK

#include "CellAction.h"

namespace Calligra
{
namespace Sheets
{
class GoalSeekDialog;

class GoalSeek : public CellAction
{
    Q_OBJECT
public:
    GoalSeek(Actions *actions);
    virtual ~GoalSeek();

protected Q_SLOTS:
    void calculate();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;

    Selection *m_selection;
    GoalSeekDialog *m_dlg;
    QWidget *m_canvasWidget;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_GOAL_SEEK
