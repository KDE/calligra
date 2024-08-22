/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Copy.h"
#include "Actions.h"

#include "ui/CellEditorBase.h"
#include "ui/CellToolBase.h"
#include "ui/commands/CopyCommand.h"

#include <KLocalizedString>
#include <KStandardAction>

#include <QApplication>
#include <QClipboard>
#include <QMimeData>

using namespace Calligra::Sheets;

Copy::Copy(Actions *actions)
    : CellAction(actions, "copy", QString(), QIcon(), i18n("Copy the cell object to the clipboard"))
{
}

Copy::~Copy() = default;

QAction *Copy::createAction()
{
    QAction *action = KStandardAction::copy(nullptr, nullptr, m_actions->tool());
    connect(action, &QAction::triggered, this, &Copy::triggered);
    action->setToolTip(m_tooltip);
    return action;
}

void Copy::execute(Selection *selection, Sheet *, QWidget *)
{
    CellToolBase *tool = m_actions->tool();
    if (tool->editor()) {
        tool->editor()->copy();
        return;
    }

    QString snippet = CopyCommand::saveAsSnippet(*selection);

    QMimeData *mimeData = new QMimeData();
    mimeData->setText(CopyCommand::saveAsPlainText(*selection));
    mimeData->setData("application/x-calligra-sheets-snippet", snippet.toUtf8());

    QApplication::clipboard()->setMimeData(mimeData);
}

Cut::Cut(Actions *actions)
    : CellAction(actions, "cut", QString(), QIcon(), i18n("Move the cell object to the clipboard"))
{
}

Cut::~Cut() = default;

QAction *Cut::createAction()
{
    QAction *action = KStandardAction::cut(nullptr, nullptr, m_actions->tool());
    connect(action, &QAction::triggered, this, &Cut::triggered);
    action->setToolTip(m_tooltip);
    return action;
}

void Cut::execute(Selection *selection, Sheet *, QWidget *)
{
    CellToolBase *tool = m_actions->tool();
    if (tool->editor()) {
        tool->editor()->cut();
        return;
    }

    QString snippet = CopyCommand::saveAsSnippet(*selection);
    snippet = "CUT\n" + snippet;

    QMimeData *mimeData = new QMimeData();
    mimeData->setText(CopyCommand::saveAsPlainText(*selection));
    mimeData->setData("application/x-calligra-sheets-snippet", snippet.toUtf8());

    QApplication::clipboard()->setMimeData(mimeData);
}
