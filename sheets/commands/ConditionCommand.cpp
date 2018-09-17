/* This file is part of the KDE project
   Copyright 2005,2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Local
#include "ConditionCommand.h"

#include <KLocalizedString>

#include "CellStorage.h"
#include "ConditionsStorage.h"
#include "Sheet.h"

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
