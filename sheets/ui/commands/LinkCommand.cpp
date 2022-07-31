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
#include "core/Cell.h"

using namespace Calligra::Sheets;

LinkCommand::LinkCommand(const QString& text, const QString& link)
{
    newText = text;
    newLink = link;

    setText(newLink.isEmpty() ? kundo2_i18n("Remove Link") : kundo2_i18n("Set Link"));
}

bool LinkCommand::process(Element* element)
{
    QRect range = element->rect();
    for (int col = range.left(); col <= range.right(); ++col)
        for (int row = range.top(); row <= range.bottom(); ++row) {
            Cell cell = Cell(m_sheet, col, row);
            if (cell.isPartOfMerged()) cell = cell.masterCell();

            cell.parseUserInput(newText);
            cell.setLink(newLink);
        }
    return true;
}


