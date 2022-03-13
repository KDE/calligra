/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2005, 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "AbstractRegionCommand.h"

#include <QApplication>

#include <KLocalizedString>
#include <KPassivePopup>

#include <KoCanvasBase.h>

#include "engine/Damages.h"
#include "core/CellStorage.h"
#include "core/Map.h"
#include "core/Sheet.h"

using namespace Calligra::Sheets;

//BEGIN NOTE Stefan: some words on operations
//
// 1. SubTotal
// a) Makes no sense to extend to non-contiguous selections (NCS) as
//    it refers to a change in one column.
// b) No special undo command available yet.
//
// 2. AutoSum
// a) should insert cell at the end of the selection, if the last
//    is not empty
// b) opens an editor, if the user's intention is fuzzy -> hard to
//    convert to NCS
//END

/***************************************************************************
  class AbstractRegionCommand
****************************************************************************/

AbstractRegionCommand::AbstractRegionCommand(KUndo2Command* parent)
        : Region(),
        KUndo2Command(parent),
        m_sheet(0),
        m_reverse(false),
        m_firstrun(true),
        m_register(true),
        m_success(true),
        m_checkLock(false)
{
}

AbstractRegionCommand::~AbstractRegionCommand()
{
}

bool AbstractRegionCommand::execute(KoCanvasBase* canvas)
{
    if (!m_firstrun)
        return false;
    if (!isApproved())
        return false;
    // registering in undo history?
    if (m_register)
        canvas ? canvas->addCommand(this) : m_sheet->fullMap()->addCommand(this);
    else
        redo();
    return m_success;
}

void AbstractRegionCommand::redo()
{
    //sebsauer; following conditions and warning makes no sense cause we would crash
    //later on cause m_sheet is direct accessed without NULL-check. So, let's add
    //some asserts to check for the m_sheet=NULL case to be able to fix it if we
    //can reproduce the situation.
#if 0
    if (!m_sheet) {
        warnSheets << "AbstractRegionCommand::redo(): No explicit m_sheet is set. "
        << "Manipulating all sheets of the region." << endl;
    }
#else
    Q_ASSERT(m_sheet);
    if (!m_sheet) { m_success = false; return; }
#endif

    m_success = true;
    bool successfully = true;
    successfully = preProcessing();
    if (!successfully) {
        m_success = false;
        return;   // do nothing if pre-processing fails
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    // FIXME Stefan: Does every derived command damage the visual cache? No!
    m_sheet->map()->addDamage(new CellDamage(m_sheet, *this, CellDamage::Appearance));

    successfully = mainProcessing();
    if (!successfully) {
        m_success = false;
        warnSheets << "AbstractRegionCommand::redo(): processing was not successful!";
    }

    successfully = true;
    successfully = postProcessing();
    if (!successfully) {
        m_success = false;
        warnSheets << "AbstractRegionCommand::redo(): postprocessing was not successful!";
    }

    QApplication::restoreOverrideCursor();

    m_firstrun = false;
}

void AbstractRegionCommand::undo()
{
    m_reverse = !m_reverse;
    redo();
    m_reverse = !m_reverse;
}

bool AbstractRegionCommand::isApproved() const
{
    //sebsauer; same as in AbstractRegionCommand::redo
    Q_ASSERT(m_sheet);
    if (!m_sheet) return false;

    const QList<Element *> elements = cells();
    const int begin = m_reverse ? elements.count() - 1 : 0;
    const int end = m_reverse ? -1 : elements.count();
    if (m_checkLock && m_sheet->fullCellStorage()->hasLockedCells(*this)) {
        KPassivePopup::message(i18n("Processing is not possible, because some "
                                    "cells are locked as elements of a matrix."),
                               QApplication::activeWindow());
        return false;
    }
    if (m_sheet->isProtected()) {
        for (int i = begin; i != end; m_reverse ? --i : ++i) {
            const QRect range = elements[i]->rect();

            for (int col = range.left(); col <= range.right(); ++col) {
                for (int row = range.top(); row <= range.bottom(); ++row) {
                    Cell cell(m_sheet, col, row);
                    if (!cell.style().notProtected()) {
                        KPassivePopup::message(i18n("Processing is not possible, "
                                                    "because some cells are protected."),
                                               QApplication::activeWindow());
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

bool AbstractRegionCommand::mainProcessing()
{
    //sebsauer; same as in AbstractRegionCommand::redo
    Q_ASSERT(m_sheet);
    if (!m_sheet) return false;

    bool successfully = true;
    const QList<Element *> elements = cells();
    const int begin = m_reverse ? elements.count() - 1 : 0;
    const int end = m_reverse ? -1 : elements.count();
    for (int i = begin; i != end; m_reverse ? --i : ++i) {
        successfully = successfully && process(elements[i]);
    }
    return successfully;
}
