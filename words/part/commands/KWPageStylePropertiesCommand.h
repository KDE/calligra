/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWPAGESTYLEPROPERTIESCOMMAND_H
#define KWPAGESTYLEPROPERTIESCOMMAND_H

#include "../KWPage.h"
#include "../words_export.h"

#include <KoPageLayout.h>
#include <KoText.h>

#include <kundo2command.h>

class KWDocument;
class KWPage;

/**
 * The undo / redo command for changing the properties of a KWPage
 * When altering the size of a page this command will also reposition all required
 * frames to account for the changes.
 */
class WORDS_TEST_EXPORT KWPageStylePropertiesCommand : public KUndo2Command
{
public:
    /**
     * The command to alter the style of a page.
     * @param document the document the page belongs to.
     * @param styleBefore the unchanged style.
     * @param styleAfter the new style.
     * @param parent the parent for macro command functionality
     */
    explicit KWPageStylePropertiesCommand(KWDocument *document, const KWPageStyle &styleBefore, const KWPageStyle &styleAfter, KUndo2Command *parent = nullptr);

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KWDocument *m_document;
    KWPageStyle m_style; // the user style we change
    KWPageStyle m_styleBefore; // one detached set of properties
    KWPageStyle m_styleAfter; // another detached set of properties
};

#endif
