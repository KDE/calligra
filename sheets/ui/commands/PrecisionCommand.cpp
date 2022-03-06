/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "PrecisionCommand.h"

#include <KLocalizedString>

#include "Cell.h"
#include "CellStorage.h"
#include "Sheet.h"
#include "Style.h"

using namespace Calligra::Sheets;

PrecisionCommand::PrecisionCommand()
        : AbstractRegionCommand()
{
    setText(kundo2_i18n("Increase Precision"));
}

bool PrecisionCommand::mainProcessing()
{
    Style style;
    if (!m_reverse) {
        // increase the precision
        style.setPrecision(1);
    } else { // m_reverse
        // decrease the precision
        style.setPrecision(-1);
    }
    m_sheet->cellStorage()->setStyle(*this, style);
    return true;
}

bool PrecisionCommand::postProcessing()
{
    return true;
}

void PrecisionCommand::setReverse(bool reverse)
{
    AbstractRegionCommand::setReverse(reverse);
    if (!m_reverse)
        setText(kundo2_i18n("Increase Precision"));
    else
        setText(kundo2_i18n("Decrease Precision"));
}
