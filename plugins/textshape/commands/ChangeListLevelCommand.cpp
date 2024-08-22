/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Girish Ramakrishnan <girish@forwardbias.in>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ChangeListLevelCommand.h"

#include "TextTool.h"
#include <KLocalizedString>
#include <KoList.h>
#include <KoListLevelProperties.h>
#include <KoParagraphStyle.h>
#include <KoTextBlockData.h>
#include <KoTextDocument.h>
#include <QDebug>

#include <QHash>
#include <QList>
#include <QTextCursor>

ChangeListLevelCommand::ChangeListLevelCommand(const QTextCursor &cursor, ChangeListLevelCommand::CommandType type, int coef, KUndo2Command *parent)
    : KoTextCommandBase(parent)
    , m_type(type)
    , m_coefficient(coef)
    , m_first(true)
{
    setText(kundo2_i18n("Change List Level"));

    int selectionStart = qMin(cursor.anchor(), cursor.position());
    int selectionEnd = qMax(cursor.anchor(), cursor.position());

    QTextBlock block = cursor.block().document()->findBlock(selectionStart);

    bool oneOf = (selectionStart == selectionEnd); // ensures the block containing the cursor is selected in that case

    while (block.isValid() && ((block.position() < selectionEnd) || oneOf)) {
        m_blocks.append(block);
        if (block.textList()) {
            m_lists.insert(m_blocks.size() - 1, KoTextDocument(block.document()).list(block.textList()));
            Q_ASSERT(m_lists.value(m_blocks.size() - 1));
            m_levels.insert(m_blocks.size() - 1, effectiveLevel(m_lists.value(m_blocks.size() - 1)->level(block)));
        }
        oneOf = false;
        block = block.next();
    }
}

ChangeListLevelCommand::~ChangeListLevelCommand() = default;

int ChangeListLevelCommand::effectiveLevel(int level)
{
    int result = -1;
    if (m_type == IncreaseLevel) {
        result = level + m_coefficient;
    } else if (m_type == DecreaseLevel) {
        result = level - m_coefficient;
    } else if (m_type == SetLevel) {
        result = m_coefficient;
    }
    result = qMax(1, qMin(10, result));
    return result;
}

void ChangeListLevelCommand::redo()
{
    if (!m_first) {
        KoTextCommandBase::redo();
        UndoRedoFinalizer finalizer(this);
        for (int i = 0; i < m_blocks.size(); ++i) {
            m_lists.value(i)->updateStoredList(m_blocks.at(i));
            QTextBlock currentBlock(m_blocks.at(i));
            KoTextBlockData userData(currentBlock);
            userData.setCounterWidth(-1.0);
        }
    } else {
        for (int i = 0; i < m_blocks.size() && m_lists.value(i); ++i) {
            if (!m_lists.value(i)->style()->hasLevelProperties(m_levels.value(i))) {
                KoListLevelProperties llp = m_lists.value(i)->style()->levelProperties(m_levels.value(i));
                KoListLevelProperties parentLlp = m_lists.value(i)->style()->levelProperties(m_levels.value(i) - 1);
                if (llp.alignmentMode() == false) {
                    // old list mode, see KoListLevelProperties::alignmentMode() documentation
                    llp.setIndent((m_levels.value(i) - 1) * 20); // TODO make this configurable
                } else {
                    llp.setTabStopPosition(parentLlp.tabStopPosition() + parentLlp.marginIncrease());
                    llp.setMargin(parentLlp.margin() + parentLlp.marginIncrease());
                    llp.setTextIndent(parentLlp.textIndent());
                }
                llp.setDisplayLevel(llp.displayLevel() + m_coefficient);
                llp.setLevel(m_levels.value(i));

                m_lists.value(i)->style()->setLevelProperties(llp);
            }
            m_lists.value(i)->add(m_blocks.at(i), m_levels.value(i));
        }
    }
    m_first = false;
}

void ChangeListLevelCommand::undo()
{
    KoTextCommandBase::undo();
    UndoRedoFinalizer finalizer(this);
    for (int i = 0; i < m_blocks.size(); ++i) {
        if (m_blocks.at(i).textList())
            m_lists.value(i)->updateStoredList(m_blocks.at(i));

        QTextBlock currentBlock(m_blocks.at(i));
        KoTextBlockData userData(currentBlock);
        userData.setCounterWidth(-1.0);
    }
}

bool ChangeListLevelCommand::mergeWith(const KUndo2Command *)
{
    return false;
}
