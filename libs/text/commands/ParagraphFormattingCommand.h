/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef PARAGRAPHFORMATTINGCOMMAND_H
#define PARAGRAPHFORMATTINGCOMMAND_H

#include <KoListLevelProperties.h>

#include "kundo2command.h"

class KoTextEditor;
class QTextCharFormat;
class QTextBlockFormat;

/**
 * This command is used to apply paragraph settings
 */
class ParagraphFormattingCommand : public KUndo2Command
{
public:
    ParagraphFormattingCommand(KoTextEditor *editor,
                               const QTextCharFormat &characterFormat,
                               const QTextBlockFormat &blockFormat,
                               const KoListLevelProperties &llp,
                               KUndo2Command *parent = nullptr);

    ~ParagraphFormattingCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    bool m_first;
    KoTextEditor *m_editor;
    QTextCharFormat m_charFormat;
    QTextBlockFormat m_blockFormat;
    KoListLevelProperties m_levelProperties;
};

#endif
