/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "NamedAreaCommand.h"

#include "KLocalizedString"

#include "Damages.h"
#include "FormulaStorage.h"
#include "calligra_sheets_limits.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "Sheet.h"

using namespace Calligra::Sheets;

NamedAreaCommand::NamedAreaCommand(KUndo2Command* parent)
        : AbstractRegionCommand(parent)
{
    setText(kundo2_i18n("Add Named Area"));
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
        setText(kundo2_i18n("Add Named Area"));
    else
        setText(kundo2_i18n("Remove Named Area"));
}

bool NamedAreaCommand::preProcessing()
{
    if (!m_firstrun)
        return true;
    if (m_reverse)
        return true;

    const Region namedArea = m_sheet->map()->namedAreaManager()->namedArea(m_areaName);
    if (!namedArea.isEmpty()) {
        if (namedArea == *this)
            return false;
        m_oldArea = namedArea;
    }
    // no protection or matrix lock check needed
    return isContiguous();
}

bool NamedAreaCommand::mainProcessing()
{
    debugSheets ;
    if (!m_reverse) {
        if (!m_oldArea.isEmpty())
            m_sheet->map()->namedAreaManager()->remove(m_areaName);
        m_sheet->map()->namedAreaManager()->insert(*this, m_areaName);
    } else {
        m_sheet->map()->namedAreaManager()->remove(m_areaName);
        if (!m_oldArea.isEmpty())
            m_sheet->map()->namedAreaManager()->insert(m_oldArea, m_areaName);
    }
    return true;
}

bool NamedAreaCommand::postProcessing()
{
    // update formulas containing either the new or the old name
    Map* const map = m_sheet->map();
    foreach(Sheet* sheet, map->sheetList()) {
        const QString tmp = '\'' + m_areaName + '\'';
        const FormulaStorage* const storage = sheet->formulaStorage();
        for (int c = 0; c < storage->count(); ++c) {
            if (storage->data(c).expression().contains(tmp)) {
                Cell cell(sheet, storage->col(c), storage->row(c));
                if (cell.isFormula()) {
                    // recalculate cells
                    map->addDamage(new CellDamage(cell, CellDamage::Appearance | CellDamage::Binding |
                                                  CellDamage::Value));
                }
            }
        }
    }
    return AbstractRegionCommand::postProcessing();
}
