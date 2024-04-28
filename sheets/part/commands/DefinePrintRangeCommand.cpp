/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2004 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "DefinePrintRangeCommand.h"

#include "core/PrintSettings.h"
#include "core/Sheet.h"

using namespace Calligra::Sheets;

// ----- DefinePrintRangeCommand -----

DefinePrintRangeCommand::DefinePrintRangeCommand()
    : AbstractRegionCommand()
{
    setText(kundo2_i18n("Define Print Range"));
}

bool DefinePrintRangeCommand::performNonCommandActions()
{
    if (m_firstrun) {
        m_oldPrintRegion = m_sheet->printSettings()->printRegion();
    }
    m_sheet->printSettings()->setPrintRegion(*this);
    return true;
}

bool DefinePrintRangeCommand::undoNonCommandActions()
{
    m_sheet->printSettings()->setPrintRegion(m_oldPrintRegion);
    return true;
}
