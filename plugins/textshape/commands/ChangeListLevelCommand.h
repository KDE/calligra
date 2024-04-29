/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Girish Ramakrishnan <girish@forwardbias.in>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CHANGELISTLEVELCOMMAND
#define CHANGELISTLEVELCOMMAND

#include "KoTextCommandBase.h"

#include <QHash>
#include <QList>
#include <QTextBlock>

class KoList;

/**
 * This command is used the change level of a list-item.
 */
class ChangeListLevelCommand : public KoTextCommandBase
{
public:
    enum CommandType { IncreaseLevel, DecreaseLevel, SetLevel };

    /**
     * Change the list property of 'block'.
     * @param block the paragraph to change the list property of
     * @param coef indicates by how many levels the list item should be displaced
     * @param parent the parent undo command for macro functionality
     */
    ChangeListLevelCommand(const QTextCursor &cursor, CommandType type, int coef, KUndo2Command *parent = nullptr);

    ~ChangeListLevelCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

    /// reimplemnted from KUndo2Command
    int id() const override
    {
        return 58450689;
    }
    /// reimplemnted from KUndo2Command
    bool mergeWith(const KUndo2Command *other) override;

private:
    int effectiveLevel(int level);

    CommandType m_type;
    int m_coefficient;

    QList<QTextBlock> m_blocks;
    QHash<int, KoList *> m_lists;
    QHash<int, int> m_levels;

    bool m_first;
};

#endif
