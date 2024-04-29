/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWCHANGEPAGESTYLECOMMAND_H
#define KWCHANGEPAGESTYLECOMMAND_H

#include "../KWPage.h"
#include "../KWPageStyle.h"
#include "../words_export.h"

#include <kundo2command.h>

class KWDocument;

/// The undo / redo command for setting a page style on a page
class WORDS_TEST_EXPORT KWChangePageStyleCommand : public KUndo2Command
{
public:
    /**
     * The constructor for a command to change page style.
     * @param document the document that gets a new styled page.
     * @param page we will change the style for this page
     * @param newStyle the new style
     * @param parent the parent for command macros
     */
    explicit KWChangePageStyleCommand(KWDocument *document, KWPage &page, const KWPageStyle &newStyle, KUndo2Command *parent = nullptr);

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KWDocument *m_document;
    const KWPageStyle m_newStyle;
    const KWPageStyle m_oldStyle;
    KWPage m_page;
};

#endif
