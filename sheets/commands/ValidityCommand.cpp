/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005, 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "ValidityCommand.h"


#include <KLocalizedString>

#include "CellStorage.h"
#include "Sheet.h"
#include "ValidityStorage.h"

using namespace Calligra::Sheets;

ValidityCommand::ValidityCommand()
        : AbstractRegionCommand()
{
}

bool ValidityCommand::process(Element* element)
{
    if (!m_reverse) {
        // create undo
        if (m_firstrun)
            m_undoData += m_sheet->validityStorage()->undoData(Region(element->rect()));
        m_sheet->cellStorage()->setValidity(Region(element->rect()), m_validity);
    }
    return true;
}

bool ValidityCommand::mainProcessing()
{
    if (m_reverse) {
        m_sheet->cellStorage()->setValidity(*this, Validity());
        for (int i = 0; i < m_undoData.count(); ++i)
            m_sheet->cellStorage()->setValidity(Region(m_undoData[i].first.toRect()), m_undoData[i].second);
    }
    return AbstractRegionCommand::mainProcessing();
}

void ValidityCommand::setValidity(Validity validity)
{
    m_validity = validity;
    if (m_validity.isEmpty())
        setText(kundo2_i18n("Remove Validity Check"));
    else
        setText(kundo2_i18n("Add Validity Check"));
}
