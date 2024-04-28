/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Pierre Stirnweiss \pierre.stirnweiss_calligra@gadz.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef CHANGETRACKER_H
#define CHANGETRACKER_H

#include <QObject>

class QTextDocument;
class TextTool;

class ChangeTracker : public QObject
{
    Q_OBJECT
public:
    explicit ChangeTracker(TextTool *parent);

    void setDocument(QTextDocument *document);

    int getChangeId(QString title, int existingChangeId);

    void notifyForUndo();

private Q_SLOTS:
    void contentsChange(int from, int charsRemoves, int charsAdded);

private:
    QTextDocument *m_document;
    TextTool *m_tool;
    bool m_enableSignals, m_reverseUndo;
    int m_changeId;
};

#endif
