/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "InsertFromDatabase.h"
#include "Actions.h"

#include "dialogs/DatabaseDialog.h"

#include <QAction>
#include <KLocalizedString>
#include <KMessageBox>

#ifndef QT_NO_SQL
// #include <QSqlDatabase>
#endif


using namespace Calligra::Sheets;


InsertFromDatabase::InsertFromDatabase(Actions *actions)
    : CellAction(actions, "insertFromDatabase", i18n("From &Database..."), koIcon("network-server-database"), i18n("Insert data from a SQL database"))
{
    m_closeEditor = true;
}

InsertFromDatabase::~InsertFromDatabase()
{
}

QAction *InsertFromDatabase::createAction() {
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Database"));
    return res;
}

void InsertFromDatabase::execute(Selection *selection, Sheet *, QWidget *canvasWidget)
{
#ifndef QT_NO_SQL
    QStringList str = QSqlDatabase::drivers();
    if (str.isEmpty()) {
        KMessageBox::error(canvasWidget, i18n("No database drivers available. To use this feature you need "
                           "to install the necessary Qt database drivers."));
        return;
    }

    DatabaseDialog *dialog = new DatabaseDialog(canvasWidget, selection);
    dialog->exec();
    delete dialog;
#endif
}


