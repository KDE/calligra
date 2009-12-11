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
#include "IndentationCommand.h"

#include <kdebug.h>
#include <klocale.h>

#include "ApplicationSettings.h"
#include "Cell.h"
#include "CellStorage.h"
#include "Map.h"
#include "Sheet.h"
#include "Style.h"

using namespace KSpread;

IndentationCommand::IndentationCommand()
        : AbstractRegionCommand()
{
    setText(i18n("Increase Indentation"));
}

bool IndentationCommand::mainProcessing()
{
    Style style;
    if (!m_reverse) {
        // increase the indentation
        style.setIndentation(m_sheet->map()->settings()->indentValue());
    } else { // m_reverse
        // decrease the indentation
        style.setIndentation(-m_sheet->map()->settings()->indentValue());
    }
    m_sheet->cellStorage()->setStyle(*this, style);
    return true;
}

bool IndentationCommand::postProcessing()
{
    return true;
}

void IndentationCommand::setReverse(bool reverse)
{
    AbstractRegionCommand::setReverse(reverse);
    if (!m_reverse)
        setText(i18n("Increase Indentation"));
    else
        setText(i18n("Decrease Indentation"));
}
