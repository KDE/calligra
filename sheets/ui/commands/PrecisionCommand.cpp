/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "PrecisionCommand.h"

#include "core/CellStorage.h"
#include "core/Sheet.h"
#include "core/Style.h"

using namespace Calligra::Sheets;

PrecisionCommand::PrecisionCommand()
        : AbstractRegionCommand()
{
    setDecrease(false);
}

bool PrecisionCommand::performCommands()
{
    Style style;
    style.setPrecision(m_decrease ? -1 : 1);
    m_sheet->fullCellStorage()->setStyle(*this, style);
    return true;
}

void PrecisionCommand::setDecrease(bool decrease)
{
    m_decrease = decrease;
    if (!m_decrease)
        setText(kundo2_i18n("Increase Precision"));
    else
        setText(kundo2_i18n("Decrease Precision"));
}

