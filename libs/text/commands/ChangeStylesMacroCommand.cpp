/*
 *  SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "ChangeStylesMacroCommand.h"

#include "ChangeStylesCommand.h"
#include "KoCharacterStyle.h"
#include "KoParagraphStyle.h"
#include "KoStyleManager.h"
#include "OdfTextTrackStyles.h"

#include <KoTextDocument.h>
#include <KoTextEditor.h>

#include <KLocalizedString>

#include <QTextDocument>

ChangeStylesMacroCommand::ChangeStylesMacroCommand(const QList<QTextDocument *> &documents, KoStyleManager *styleManager)
    : KUndo2Command(kundo2_i18n("Change Styles"))
    , m_documents(documents)
    , m_styleManager(styleManager)
    , m_first(true)
{
}

ChangeStylesMacroCommand::~ChangeStylesMacroCommand() = default;

// on first pass the subcommands are created (where they collect needed info)
//     then styles are changed in the styleManager
//     finally the new styles are applied to the documents through super::redo()
void ChangeStylesMacroCommand::redo()
{
    QList<ChangeStylesCommand *> commands;
    if (m_first) {
        // IMPORTANT: the sub commands needs to be created now so the can collect
        // info before we change the styles
        foreach (QTextDocument *qDoc, m_documents) {
            commands.append(new ChangeStylesCommand(qDoc, m_origCharacterStyles, m_origParagraphStyles, m_changedStyles, this));
        }
    }

    // Okay so now it's safe to change the styles and this should always be done
    foreach (KoCharacterStyle *newStyle, m_changedCharacterStyles) {
        int id = newStyle->styleId();
        m_styleManager->characterStyle(id)->copyProperties(newStyle);
    }

    foreach (KoParagraphStyle *newStyle, m_changedParagraphStyles) {
        int id = newStyle->styleId();
        m_styleManager->paragraphStyle(id)->copyProperties(newStyle);
    }

    if (m_first) {
        int i = 0;
        foreach (QTextDocument *qDoc, m_documents) {
            // add and execute it's redo
            //  ToC documents doesn't have a texteditor so make sure we ignore that
            if (KoTextDocument(qDoc).textEditor()) {
                KoTextDocument(qDoc).textEditor()->addCommand(commands[i]);
            }
            i++;
        }
        m_first = false;
    } else {
        KUndo2Command::redo(); // calls redo on all children
    }
}

void ChangeStylesMacroCommand::undo()
{
    foreach (KoCharacterStyle *oldStyle, m_origCharacterStyles) {
        int id = oldStyle->styleId();
        m_styleManager->characterStyle(id)->copyProperties(oldStyle);
    }

    foreach (KoParagraphStyle *oldStyle, m_origParagraphStyles) {
        int id = oldStyle->styleId();
        m_styleManager->paragraphStyle(id)->copyProperties(oldStyle);
    }

    KUndo2Command::undo(); // calls undo on all children
}
