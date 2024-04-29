/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Girish Ramakrishnan <girish@forwardbias.in>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CHANGELISTCOMMAND
#define CHANGELISTCOMMAND

#include "KoListLevelProperties.h"
#include "KoListStyle.h"
#include "KoTextCommandBase.h"
#include "KoTextEditor.h"

#include <QHash>
#include <QList>
#include <QTextBlock>

class KoList;

/**
 * This command is useful to alter the list-association of a single textBlock.
 */
class ChangeListCommand : public KoTextCommandBase
{
public:
    /**
     * Change the list command.
     * @param cursor text cursor properties.
     * @param levelProperties level properties.
     * @param flags the list flags.
     * @param parent the parent undo command for macro functionality
     */
    ChangeListCommand(const QTextCursor &cursor,
                      const KoListLevelProperties &levelProperties,
                      KoTextEditor::ChangeListFlags flags,
                      KUndo2Command *parent = nullptr);

    /**
     * Change the list command.
     * @param cursor text cursor properties.
     * @param style the style to apply.
     * @param level the level in the list.
     * @param flags the list flags.
     * @param parent the parent undo command for macro functionality
     */
    ChangeListCommand(const QTextCursor &cursor, KoListStyle *style, int level, KoTextEditor::ChangeListFlags flags, KUndo2Command *parent = nullptr);
    ~ChangeListCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

    /// reimplemnted from KUndo2Command
    int id() const override
    {
        return 58450687;
    }
    /// reimplemnted from KUndo2Command
    bool mergeWith(const KUndo2Command *other) override;

private:
    enum CommandAction { CreateNew, ModifyExisting, ReparentList, MergeList, RemoveList };
    bool extractTextBlocks(const QTextCursor &cursor, int level, KoListStyle::LabelType newLabelType = KoListStyle::None);
    int detectLevel(const QTextBlock &block, int givenLevel);
    void initList(KoListStyle *style);
    bool formatsEqual(const KoListLevelProperties &llp, const QTextListFormat &format);

    int m_flags;
    bool m_first;
    bool m_alignmentMode;

    QList<QTextBlock> m_blocks;
    QHash<int, KoListLevelProperties> m_formerProperties;
    QHash<int, KoListLevelProperties> m_newProperties;
    QHash<int, int> m_levels;
    QHash<int, KoList *> m_list;
    QHash<int, KoList *> m_oldList;
    QHash<int, CommandAction> m_actions;
};

#endif
