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
#include "AbstractRegionCommand.h"

#include <QApplication>

#include <klocale.h>
#include <kpassivepopup.h>

#include <KoCanvasBase.h>

#include "Cell.h"
#include "part/Doc.h" // FIXME detach from part
#include "Map.h"
#include "Sheet.h"

using namespace KSpread;

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

AbstractRegionCommand::AbstractRegionCommand(QUndoCommand* parent)
        : Region(),
        QUndoCommand(parent),
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
        canvas ? canvas->addCommand(this) : m_sheet->doc()->addCommand(this);
    else
        redo();
    return m_success;
}

void AbstractRegionCommand::redo()
{
    if (!m_sheet) {
        kWarning() << "AbstractRegionCommand::redo(): No explicit m_sheet is set. "
        << "Manipulating all sheets of the region." << endl;
    }

    bool successfully = true;
    successfully = preProcessing();
    if (!successfully) {
        m_success = false;
        return;   // do nothing if pre-processing fails
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    m_sheet->setRegionPaintDirty(*this);

    successfully = mainProcessing();
    if (!successfully) {
        m_success = false;
        kWarning() << "AbstractRegionCommand::redo(): processing was not successful!";
    }

    successfully = true;
    successfully = postProcessing();
    if (!successfully) {
        m_success = false;
        kWarning() << "AbstractRegionCommand::redo(): postprocessing was not successful!";
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
    const QList<Element *> elements = cells();
    const int begin = m_reverse ? elements.count() - 1 : 0;
    const int end = m_reverse ? -1 : elements.count();
    for (int i = begin; i != end; m_reverse ? --i : ++i) {
        const QRect range = elements[i]->rect();

        for (int col = range.left(); col <= range.right(); ++col) {
            for (int row = range.top(); row <= range.bottom(); ++row) {
                Cell cell(m_sheet, col, row);
                if (m_sheet->isProtected() && !cell.style().notProtected()) {
                    KPassivePopup::message(i18n("Processing is not possible, "
                                                "because some cells are protected."),
                                           QApplication::activeWindow());
                    return false;
                }

                // check for matrix locks
                if (m_checkLock && cell.isLocked()) {
                    KPassivePopup::message(i18n("Processing is not possible, because some "
                                                "cells are locked as elements of a matrix."),
                                           QApplication::activeWindow());
                    return false;
                }
            }
        }
    }
    return true;
}

bool AbstractRegionCommand::mainProcessing()
{
    bool successfully = true;
    const QList<Element *> elements = cells();
    const int begin = m_reverse ? elements.count() - 1 : 0;
    const int end = m_reverse ? -1 : elements.count();
    for (int i = begin; i != end; m_reverse ? --i : ++i) {
        successfully = successfully && process(elements[i]);
    }
    return successfully;
}
