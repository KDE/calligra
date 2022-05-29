/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005, 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "ConditionCommand.h"

#include "core/CellStorage.h"
#include "core/Sheet.h"

using namespace Calligra::Sheets;

ConditionCommand::ConditionCommand()
        : AbstractRegionCommand()
{
}

bool ConditionCommand::process(Element* element)
{
    m_sheet->fullCellStorage()->setConditions(Region(element->rect()), m_conditions);
    return true;
}

void ConditionCommand::setConditionList(const QLinkedList<Conditional>& list)
{
    m_conditions.setConditionList(list);
    if (m_conditions.isEmpty())
        setText(kundo2_i18n("Remove Conditional Formatting"));
    else
        setText(kundo2_i18n("Add Conditional Formatting"));
}
