/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_LIST_CHOOSE
#define CALLIGRA_SHEETS_ACTION_LIST_CHOOSE

#include "CellAction.h"

class QMenu;

namespace Calligra
{
namespace Sheets
{
class Cell;
class Selection;

class ListChoose : public CellAction
{
    Q_OBJECT
public:
    ListChoose(Actions *actions);
    virtual ~ListChoose();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;

    virtual bool enabledForSelection(Selection *selection, const Cell &) override;

    QStringList items(Selection *selection, const Cell &cursorCell, int limit = 0);

protected Q_SLOTS:
    void itemSelected(QAction *action);

private:
    QMenu *popupListChoose;
    Selection *m_selection;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_LIST_CHOOSE
