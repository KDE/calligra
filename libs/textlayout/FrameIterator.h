/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 C. Boemann KO GmbH <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef FRAMEITERATOR_H
#define FRAMEITERATOR_H

#include <QPointer>
#include <QTextFrame>

class TableIterator;
class QTextTableCell;
class QTextTable;

class FrameIterator
{
public:
    explicit FrameIterator(QTextFrame *frame);
    explicit FrameIterator(const QTextTableCell &frame);
    explicit FrameIterator(FrameIterator *other);
    ~FrameIterator();

    bool isValid() const;

    bool operator==(const FrameIterator &other) const;

    TableIterator *tableIterator(QTextTable *);
    FrameIterator *subFrameIterator(QTextFrame *);

    QTextFrame::iterator it;

    QString masterPageName;

    // lineTextStart and fragmentIterator can be seen as the "sub cursor" of text blocks
    int lineTextStart; // a value of -1 indicate block not processed yet
    QTextBlock::Iterator fragmentIterator;

    TableIterator *currentTableIterator; // useful if it is pointing to a table

    FrameIterator *currentSubFrameIterator; // useful if it is pointing to a subFrame

    int endNoteIndex;

private:
    QPointer<QTextFrame> m_frame;
};

#endif
