/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "NamedAreaCommand.h"

#include "klocale.h"

#include "Doc.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "Sheet.h"

using namespace KSpread;

NamedAreaCommand::NamedAreaCommand()
{
    setText(i18n("Add Named Area"));
}

NamedAreaCommand::~NamedAreaCommand()
{
}

void NamedAreaCommand::setAreaName(const QString& name)
{
    m_areaName = name;
}

void NamedAreaCommand::setReverse(bool reverse)
{
    AbstractRegionCommand::setReverse(reverse);
    if (!m_reverse)
        setText(i18n("Add Named Area"));
    else
        setText(i18n("Remove Named Area"));
}

bool NamedAreaCommand::preProcessing()
{
    if (!m_firstrun)
        return true;
    if (m_reverse)
        return true;

    const Region namedArea = m_sheet->doc()->namedAreaManager()->namedArea(m_areaName);
    if (!namedArea.isEmpty())
    {
        if (namedArea == *this)
            return false;
        m_oldArea = namedArea;
    }
    // no protection or matrix lock check needed
    return isContiguous();
}

bool NamedAreaCommand::mainProcessing()
{
    kDebug() << k_funcinfo << endl;
    if (!m_reverse)
    {
        if (!m_oldArea.isEmpty())
            m_sheet->doc()->namedAreaManager()->remove(m_areaName);
        m_sheet->doc()->namedAreaManager()->insert(m_sheet, firstRange(), m_areaName);
    }
    else
    {
        m_sheet->doc()->namedAreaManager()->remove(m_areaName);
        if (!m_oldArea.isEmpty())
            m_sheet->doc()->namedAreaManager()->insert(m_oldArea.firstSheet(),
                                                       m_oldArea.firstRange(), m_areaName);
    }
    return true;
}

bool NamedAreaCommand::postProcessing()
{
    // update formulas containing either the new or the old name
    foreach (Sheet* sheet, m_sheet->map()->sheetList())
        sheet->refreshChangeAreaName(m_areaName);
    return AbstractRegionCommand::postProcessing();
}
