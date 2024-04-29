/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Girish Ramakrishnan <girish@forwardbias.in>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef LISTITEMNUMBERINGCOMMAND
#define LISTITEMNUMBERINGCOMMAND

#include "KoTextCommandBase.h"

#include <QTextBlock>

/**
 * This command is useful to mark a block as numbered or unnumbered list-item.
 */
class ListItemNumberingCommand : public KoTextCommandBase
{
public:
    /**
     * Change the list property of 'block'.
     * @param block the paragraph to change the list property of
     * @param numbered indicates if the block is an numbered list item
     * @param parent the parent undo command for macro functionality
     */
    ListItemNumberingCommand(const QTextBlock &block, bool numbered, KUndo2Command *parent = nullptr);

    ~ListItemNumberingCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

    /// reimplemnted from KUndo2Command
    int id() const override
    {
        return 58450688;
    }
    /// reimplemnted from KUndo2Command
    bool mergeWith(const KUndo2Command *other) override;

private:
    void setNumbered(bool numbered);

    QTextBlock m_block;
    bool m_numbered;
    bool m_wasNumbered;
    bool m_first;
};

#endif
