// This file is part of the KDE project
// SPDX-FileCopyrightText: 2010 Pierre Stirnweiss \pstirnweiss@googlemail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef ACCEPTCHANGECOMMAND_H
#define ACCEPTCHANGECOMMAND_H

#include <KoTextCommandBase.h>

#include <QPair>

class KoChangeTracker;

class QTextDocument;

class AcceptChangeCommand : public QObject, public KoTextCommandBase
{
    Q_OBJECT
public:
    AcceptChangeCommand(int changeId, const QList<QPair<int, int>> &changeRanges, QTextDocument *document, KUndo2Command *parent = nullptr);
    ~AcceptChangeCommand() override;

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
};

#endif // ACCEPTCHANGECOMMAND_H
