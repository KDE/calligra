/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "PrecisionCommand.h"

#include "engine/CalculationSettings.h"
#include "engine/MapBase.h"
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
    Style curStyle = m_sheet->fullCellStorage()->style(boundingRect().left(), boundingRect().top());
    int prec = curStyle.precision();

    // TODO base this on the cell's contents ...
    if (prec < 0) prec = m_sheet->map()->calculationSettings()->defaultDecimalPrecision();
    if (prec < 0) prec = 8;
    prec += (m_decrease ? -1 : 1);
    if (prec > 10) prec = 10;
    if (prec < 0) prec = 0;

    Style style;
    style.setPrecision(prec);
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

