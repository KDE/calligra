/* This file is part of the KDE project
   Copyright 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
#include "PrecisionCommand.h"

#include <kdebug.h>
#include <klocale.h>

#include "Cell.h"
#include "CellStorage.h"
#include "Sheet.h"
#include "Style.h"

using namespace KSpread;

PrecisionCommand::PrecisionCommand()
        : AbstractRegionCommand()
{
    setText(i18n("Increase Precision"));
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
        setText(i18n("Increase Precision"));
    else
        setText(i18n("Decrease Precision"));
}
