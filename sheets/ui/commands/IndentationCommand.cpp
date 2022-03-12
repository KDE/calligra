/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "IndentationCommand.h"

// #include <KLocalizedString>

// #include "ApplicationSettings.h"
// #include "Cell.h"
// #include "CellStorage.h"
// #include "Map.h"
// #include "Sheet.h"
// #include "Style.h"

using namespace Calligra::Sheets;

IndentationCommand::IndentationCommand()
        : AbstractRegionCommand()
{
    setText(kundo2_i18n("Increase Indentation"));
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
        setText(kundo2_i18n("Increase Indentation"));
    else
        setText(kundo2_i18n("Decrease Indentation"));
}
