/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWNEWPAGESTYLECOMMAND_H
#define KWNEWPAGESTYLECOMMAND_H

#include "../words_export.h"
#include "../KWPageStyle.h"

#include <kundo2command.h>

class KWPageStylePrivate;
class KWDocument;

/// The undo / redo command for adding a new page style
class WORDS_TEST_EXPORT KWNewPageStyleCommand : public KUndo2Command
{
public:
    /**
     * The constructor for a command to change page style.
     * @param document the document that gets a new styled page.
     * @param newStyle the new style
     * @param parent the parent for command macros
     */
    explicit KWNewPageStyleCommand(KWDocument *document, const KWPageStyle &newStyle, KUndo2Command *parent = 0);

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    const KWPageStyle m_newStyle;
    KWDocument *m_document;
};

#endif
