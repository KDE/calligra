/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005, 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "ConditionCommand.h"

// #include <KLocalizedString>

// #include "CellStorage.h"
// #include "ConditionsStorage.h"
// #include "Sheet.h"

using namespace Calligra::Sheets;

ConditionCommand::ConditionCommand()
        : AbstractRegionCommand()
{
}

bool ConditionCommand::process(Element* element)
{
    if (!m_reverse) {
        // create undo
        if (m_firstrun)
            m_undoData += m_sheet->conditionsStorage()->undoData(Region(element->rect()));
        m_sheet->cellStorage()->setConditions(Region(element->rect()), m_conditions);
    }
    return true;
}

bool ConditionCommand::mainProcessing()
{
    if (m_reverse) {
        m_sheet->cellStorage()->setConditions(*this, Conditions());
        for (int i = 0; i < m_undoData.count(); ++i)
            m_sheet->cellStorage()->setConditions(Region(m_undoData[i].first.toRect()), m_undoData[i].second);
    }
    return AbstractRegionCommand::mainProcessing();
}

void ConditionCommand::setConditionList(const QLinkedList<Conditional>& list)
{
    m_conditions.setConditionList(list);
    if (m_conditions.isEmpty())
        setText(kundo2_i18n("Remove Conditional Formatting"));
    else
        setText(kundo2_i18n("Add Conditional Formatting"));
}
