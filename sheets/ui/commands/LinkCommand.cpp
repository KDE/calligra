/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2004 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "LinkCommand.h"

#include "engine/Damages.h"
#include "engine/MapBase.h"
#include "engine/SheetBase.h"

using namespace Calligra::Sheets;

LinkCommand::LinkCommand(const Cell& c, const QString& text, const QString& link)
{
    cell = c;
    oldText = cell.userInput();
    oldLink = cell.link();
    newText = text;
    newLink = link;

    setText(newLink.isEmpty() ? kundo2_i18n("Remove Link") : kundo2_i18n("Set Link"));
}

void LinkCommand::redo()
{
    if (!cell) return;

    if (!newText.isEmpty())
        cell.parseUserInput(newText);
    cell.setLink(newLink);

    cell.sheet()->map()->addDamage(new CellDamage(cell, CellDamage::Appearance));
}

void LinkCommand::undo()
{
    if (!cell) return;

    cell.parseUserInput(oldText);
    cell.setLink(oldLink);

    cell.sheet()->map()->addDamage(new CellDamage(cell, CellDamage::Appearance));
}
