/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "PageBreakCommand.h"

#include "core/ColFormatStorage.h"
#include "core/RowFormatStorage.h"
#include "core/Sheet.h"
#include "core/SheetPrint.h"
#include "engine/Damages.h"
#include "engine/MapBase.h"

using namespace Calligra::Sheets;

PageBreakCommand::PageBreakCommand(KUndo2Command *parent)
    : AbstractRegionCommand(parent)
    , m_mode(BreakBeforeColumn)
{
}

PageBreakCommand::~PageBreakCommand() = default;

void PageBreakCommand::setMode(Mode mode)
{
    m_mode = mode;
}

bool PageBreakCommand::setRangeBreak(Sheet *sheet, const QRect range, bool enable)
{
    if (m_mode == BreakBeforeColumn && range.left() > 1) {
        sheet->columnFormats()->setPageBreak(range.left(), range.left(), enable);
        sheet->print()->updateHorizontalPageParameters(range.left() - 1);
    } else if (m_mode == BreakBeforeRow && range.top() > 1) {
        sheet->rowFormats()->setPageBreak(range.top(), range.top(), enable);
        sheet->print()->updateVerticalPageParameters(range.top() - 1);
    }

    if (sheet->isShowPageOutline())
        sheet->map()->addDamage(new SheetDamage(sheet, SheetDamage::ContentChanged));

    return true;
}

bool PageBreakCommand::performNonCommandActions()
{
    const bool enable = m_breaking;
    const QList<Element *> elements = cells();
    for (int i = 0; i < elements.count(); ++i) {
        Element *element = elements[i];
        Sheet *sheet = dynamic_cast<Sheet *>(element->sheet());
        setRangeBreak(sheet, element->rect(), enable);
    }

    return true;
}

bool PageBreakCommand::undoNonCommandActions()
{
    const bool enable = !m_breaking;
    const QList<Element *> elements = cells();
    for (int i = 0; i < elements.count(); ++i) {
        Element *element = elements[i];
        Sheet *sheet = dynamic_cast<Sheet *>(element->sheet());
        setRangeBreak(sheet, element->rect(), enable);
    }

    return true;
}
