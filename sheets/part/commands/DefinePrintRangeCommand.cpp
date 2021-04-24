/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2004 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "DefinePrintRangeCommand.h"

#include "Localization.h"
#include "Map.h"
#include "PrintSettings.h"
#include "Sheet.h"
#include "SheetPrint.h"

using namespace Calligra::Sheets;

// ----- DefinePrintRangeCommand -----


DefinePrintRangeCommand::DefinePrintRangeCommand()
        : AbstractRegionCommand()
{
    setText(kundo2_i18n("Define Print Range"));
}

void DefinePrintRangeCommand::redo()
{
    if (m_firstrun) {
        m_oldPrintRegion = m_sheet->printSettings()->printRegion();
    }
    m_sheet->printSettings()->setPrintRegion(*this);
}

void DefinePrintRangeCommand::undo()
{
    m_sheet->printSettings()->setPrintRegion(m_oldPrintRegion);
}
