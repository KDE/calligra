/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ShowTableView.h"
#include "Actions.h"

#include <KLocalizedString>
#include <QAction>

#ifndef NDEBUG
#include "core/SheetModel.h"
#include <KoDialog.h>
#include <QTableView>
#endif

using namespace Calligra::Sheets;

ShowTableView::ShowTableView(Actions *actions)
    : CellAction(actions, "qTableView", i18n("Show QTableView..."), koIcon("table"), QString())
{
}

ShowTableView::~ShowTableView() = default;

QAction *ShowTableView::createAction()
{
    QAction *res = CellAction::createAction();
    res->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_T));
    return res;
}

void ShowTableView::execute(Selection *, Sheet *sheet, QWidget *canvasWidget)
{
#ifndef NDEBUG
    KoDialog *dialog = new KoDialog(canvasWidget);
    QTableView *const view = new QTableView(dialog);
    SheetModel *const model = new SheetModel(sheet);
    view->setModel(model);
    dialog->setCaption("Read{Only,Write}TableModel Test");
    dialog->setMainWidget(view);
    dialog->exec();
    delete dialog;
    delete model;
#endif
}
