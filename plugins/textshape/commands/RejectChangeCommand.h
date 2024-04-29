/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Pierre Stirnweiss \pstirnweiss@googlemail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef REJECTCHANGECOMMAND_H
#define REJECTCHANGECOMMAND_H

#include <KoTextCommandBase.h>

#include <QPair>

class KoChangeTracker;
class KoTextDocumentLayout;

class QTextDocument;

class RejectChangeCommand : public QObject, public KoTextCommandBase
{
    Q_OBJECT
public:
    RejectChangeCommand(int changeId, const QList<QPair<int, int>> &changeRanges, QTextDocument *document, KUndo2Command *parent = nullptr);
    ~RejectChangeCommand() override;

    void redo() override;
    void undo() override;

Q_SIGNALS:
    void acceptRejectChange();

private:
    bool m_first;
    int m_changeId;
    QList<QPair<int, int>> m_changeRanges;
    QTextDocument *m_document;
    KoChangeTracker *m_changeTracker;
    KoTextDocumentLayout *m_layout;
};

#endif // REJECTCHANGECOMMAND_H
