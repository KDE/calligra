/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005, 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "ValidityCommand.h"

#include "core/CellStorage.h"
#include "core/Sheet.h"

using namespace Calligra::Sheets;

ValidityCommand::ValidityCommand()
        : AbstractRegionCommand()
{
}

bool ValidityCommand::preProcessing()
{
    if (!m_firstrun)
        return true;
    m_sheet->fullCellStorage()->startUndoRecording();
    return AbstractRegionCommand::preProcessing();
}

bool ValidityCommand::process(Element* element)
{
    if (!m_reverse) {
        m_sheet->cellStorage()->setValidity(Region(element->rect()), m_validity);
    }
    return true;
}

bool ValidityCommand::postProcessing()
{
    if (m_firstrun)
        m_sheet->fullCellStorage()->stopUndoRecording(this);
    return true;
}

void ValidityCommand::setValidity(Validity validity)
{
    m_validity = validity;
    if (m_validity.isEmpty())
        setText(kundo2_i18n("Remove Validity Check"));
    else
        setText(kundo2_i18n("Add Validity Check"));
}
