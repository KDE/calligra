/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 C. Boemann KO GmbH <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "FrameIterator.h"

#include "TableIterator.h"

#include <QTextFrame>
#include <QTextTableCell>

FrameIterator::FrameIterator(QTextFrame *frame)
{
    it = frame->begin();
    m_frame = it.parentFrame();
    currentTableIterator = nullptr;
    currentSubFrameIterator = nullptr;
    lineTextStart = -1;
    endNoteIndex = 0;
}

FrameIterator::FrameIterator(const QTextTableCell &cell)
{
    Q_ASSERT(cell.isValid());
    it = cell.begin();
    m_frame = it.parentFrame();
    currentTableIterator = nullptr;
    currentSubFrameIterator = nullptr;
    lineTextStart = -1;
    endNoteIndex = 0;
}

FrameIterator::FrameIterator(FrameIterator *other)
{
    it = other->it;
    m_frame = it.parentFrame();
    masterPageName = other->masterPageName;
    lineTextStart = other->lineTextStart;
    fragmentIterator = other->fragmentIterator;
    endNoteIndex = other->endNoteIndex;
    if (other->currentTableIterator)
        currentTableIterator = new TableIterator(other->currentTableIterator);
    else
        currentTableIterator = nullptr;

    if (other->currentSubFrameIterator)
        currentSubFrameIterator = new FrameIterator(other->currentSubFrameIterator);
    else
        currentSubFrameIterator = nullptr;
}

FrameIterator::~FrameIterator()
{
    delete currentTableIterator;
    delete currentSubFrameIterator;
}

bool FrameIterator::isValid() const
{
    return m_frame;
}

bool FrameIterator::operator==(const FrameIterator &other) const
{
    if (it != other.it)
        return false;
    if (endNoteIndex != other.endNoteIndex)
        return false;
    if (currentTableIterator || other.currentTableIterator) {
        if (!currentTableIterator || !other.currentTableIterator)
            return false;
        return *currentTableIterator == *(other.currentTableIterator);
    } else if (currentSubFrameIterator || other.currentSubFrameIterator) {
        if (!currentSubFrameIterator || !other.currentSubFrameIterator)
            return false;
        return *currentSubFrameIterator == *(other.currentSubFrameIterator);
    } else {
        return lineTextStart == other.lineTextStart;
    }
}

TableIterator *FrameIterator::tableIterator(QTextTable *table)
{
    if (table == nullptr) {
        delete currentTableIterator;
        currentTableIterator = nullptr;
    } else if (currentTableIterator == nullptr) {
        currentTableIterator = new TableIterator(table);
        currentTableIterator->masterPageName = masterPageName;
    }
    return currentTableIterator;
}

FrameIterator *FrameIterator::subFrameIterator(QTextFrame *subFrame)
{
    if (subFrame == nullptr) {
        delete currentSubFrameIterator;
        currentSubFrameIterator = nullptr;
    } else if (currentSubFrameIterator == nullptr) {
        currentSubFrameIterator = new FrameIterator(subFrame);
        currentSubFrameIterator->masterPageName = masterPageName;
    }
    return currentSubFrameIterator;
}
