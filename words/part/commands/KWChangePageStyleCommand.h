/* This file is part of the KDE project
 * Copyright (C) 2010 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KWCHANGEPAGESTYLECOMMAND_H
#define KWCHANGEPAGESTYLECOMMAND_H

#include "../words_export.h"
#include "../KWPageStyle.h"
#include "../KWPage.h"

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
    explicit KWChangePageStyleCommand(KWDocument *document, KWPage &page, const KWPageStyle &newStyle, KUndo2Command *parent = 0);

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
