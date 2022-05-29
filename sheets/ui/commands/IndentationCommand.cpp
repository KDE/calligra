/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "IndentationCommand.h"

#include "core/CellStorage.h"
#include "core/Sheet.h"
#include "core/Style.h"

using namespace Calligra::Sheets;

IndentationCommand::IndentationCommand()
        : AbstractRegionCommand()
{
    setIndent(0);
}

bool IndentationCommand::performCommands()
{
    Style style;
    style.setIndentation(m_indent);
    m_sheet->fullCellStorage()->setStyle(*this, style);
    return true;
}

void IndentationCommand::setIndent(double val)
{
    m_indent = val;

    if (val >= 0)
        setText(kundo2_i18n("Increase Indentation"));
    else
        setText(kundo2_i18n("Decrease Indentation"));
}

