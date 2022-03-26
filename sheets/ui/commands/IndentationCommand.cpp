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
    m_indent = 0;
    setText(kundo2_i18n("Increase Indentation"));
}

bool IndentationCommand::mainProcessing()
{
    Style style;
    if (!m_reverse) {
        // increase the indentation
        style.setIndentation(m_indent);
    } else { // m_reverse
        // decrease the indentation
        style.setIndentation(-1 * m_indent);
    }
    m_sheet->fullCellStorage()->setStyle(*this, style);
    return true;
}

bool IndentationCommand::postProcessing()
{
    return true;
}

void IndentationCommand::setIndent(double val)
{
    m_indent = val;
}

void IndentationCommand::setReverse(bool reverse)
{
    AbstractRegionCommand::setReverse(reverse);
    if (!m_reverse)
        setText(kundo2_i18n("Increase Indentation"));
    else
        setText(kundo2_i18n("Decrease Indentation"));
}

