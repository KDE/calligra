/*
 *  SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef CHANGESTYLESMACROCOMMAND_H
#define CHANGESTYLESMACROCOMMAND_H

#include <kundo2command.h>

#include <QList>
#include <QSet>

class QTextDocument;
class KoCharacterStyle;
class KoParagraphStyle;
class KoStyleManager;

class ChangeStylesMacroCommand : public KUndo2Command
{
public:
    ChangeStylesMacroCommand(const QList<QTextDocument *> &documents, KoStyleManager *styleManager);

    ~ChangeStylesMacroCommand() override;

    /// redo the command
    void redo() override;

    /// revert the actions done in redo
    void undo() override;

    void changedStyle(KoCharacterStyle *s)
    {
        m_changedCharacterStyles.append(s);
    }
    void origStyle(KoCharacterStyle *s)
    {
        m_origCharacterStyles.append(s);
    }
    void changedStyle(KoParagraphStyle *s)
    {
        m_changedParagraphStyles.append(s);
    }
    void origStyle(KoParagraphStyle *s)
    {
        m_origParagraphStyles.append(s);
    }
    void changedStyle(int id)
    {
        m_changedStyles.insert(id);
    }

private:
    QList<QTextDocument *> m_documents;
    QList<KoCharacterStyle *> m_origCharacterStyles;
    QList<KoCharacterStyle *> m_changedCharacterStyles;
    QList<KoParagraphStyle *> m_origParagraphStyles;
    QList<KoParagraphStyle *> m_changedParagraphStyles;
    QSet<int> m_changedStyles;
    KoStyleManager *m_styleManager;
    bool m_first;
};

#endif // CHANGESTYLESMACROCOMMAND_H
