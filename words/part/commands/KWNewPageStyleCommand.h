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
