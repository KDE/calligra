/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Actions.h"

#include "InsertSeries.h"
#include "InsertSpecialChar.h"
#include "Link.h"
#include "Sort.h"
#include "Validity.h"


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
    addAction(new Link(this));
    addAction(new ClearLink(this));
    addAction(new Sort(this));
    addAction(new SortInc(this));
    addAction(new SortDesc(this));
    addAction(new SetValidity(this));
    addAction(new ClearValidity(this));

    // TODO!

}

void Actions::addAction(CellAction *a)
{
    QString name = a->name();
    if (cellActions.contains(name))
        warnSheets << "Duplicated cell action: " << name;

    cellActions[name] = a;
}

void Actions::updateOnChange(bool readWrite, Selection *selection, const Cell &activeCell)
{
    for (CellAction *cellAction : cellActions) {
        bool enabled = cellAction->shouldBeEnabled(readWrite, selection, activeCell);
        cellAction->action()->setEnabled(enabled);
    }
}


