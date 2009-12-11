/* This file is part of the KDE project
   Copyright 2005,2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
#include "ValidityCommand.h"


#include <klocale.h>

#include "CellStorage.h"
#include "Sheet.h"
#include "ValidityStorage.h"

using namespace KSpread;

ValidityCommand::ValidityCommand()
        : AbstractRegionCommand()
{
}

bool ValidityCommand::process(Element* element)
{
    if (!m_reverse) {
        // create undo
        if (m_firstrun)
            m_undoData += m_sheet->validityStorage()->undoData(Region(element->rect()));
        m_sheet->cellStorage()->setValidity(Region(element->rect()), m_validity);
    }
    return true;
}

bool ValidityCommand::mainProcessing()
{
    if (m_reverse) {
        m_sheet->cellStorage()->setValidity(*this, Validity());
        for (int i = 0; i < m_undoData.count(); ++i)
            m_sheet->cellStorage()->setValidity(Region(m_undoData[i].first.toRect()), m_undoData[i].second);
    }
    return AbstractRegionCommand::mainProcessing();
}

void ValidityCommand::setValidity(Validity validity)
{
    m_validity = validity;
    if (m_validity.isEmpty())
        setText(i18n("Remove Validity Check"));
    else
        setText(i18n("Add Validity Check"));
}
