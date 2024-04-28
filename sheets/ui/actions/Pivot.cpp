/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Pivot.h"
#include "./dialogs/pivot.h"
#include "Actions.h"

#include <KLocalizedString>
#include <KMessageBox>

using namespace Calligra::Sheets;

Pivot::Pivot(Actions *actions)
    : CellAction(actions, "pivot", i18n("&Pivot Tables..."), QIcon(), i18n("Create Pivot Tables"))
{
}

Pivot::~Pivot()
{
}

void Pivot::execute(Selection *selection, Sheet *, QWidget *canvasWidget)
{
    if ((selection->lastRange().width() < 2) || (selection->lastRange().height() < 2)) {
        KMessageBox::error(canvasWidget, i18n("You must select multiple cells."));
        return;
    }

    PivotDialog *dialog = new PivotDialog(canvasWidget, selection);
    dialog->exec();
    delete dialog;
}
