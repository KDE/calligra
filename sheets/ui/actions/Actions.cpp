/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Actions.h"

#include "InsertSeries.h"
#include "InsertSpecialChar.h"

#include "engine/SheetsDebug.h"

using namespace Calligra::Sheets;

Actions::Actions(CellToolBase *tool)
    : m_tool(tool)
{
    createActions();
}

Actions::~Actions()
{
    qDeleteAll(cellActions);
}

void Actions::createActions()
{
    addAction(new InsertSpecialChar(this));
    addAction(new InsertSeries(this));

    // TODO!

}

void Actions::addAction(CellAction *a)
{
    QString name = a->name();
    if (cellActions.contains(name))
        warnSheets << "Duplicated cell action: " << name;

    cellActions[name] = a;
}


