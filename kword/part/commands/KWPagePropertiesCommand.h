/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
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

#ifndef KWPAGEPROPERTIESCOMMAND_H
#define KWPAGEPROPERTIESCOMMAND_H

#include <KoPageLayout.h>

#include <QUndoCommand>

class KWDocument;
class KWPage;

/**
 * The undo / redo command for changing the properties of a KWPage
 * When altering the size of a page this command will also reposition all required frames to account for the changes.
 */
class KWPagePropertiesCommand : public QUndoCommand
{
public:
    /**
     * The command to alter the properties of a page.
     * @param document the document the page belongs to.
     * @param page the unchanged page.
     * @param newLayout the new layout properties.
     * @param parent the parent for macro command functionality
     */
    explicit KWPagePropertiesCommand( KWDocument *document, KWPage *page, const KoPageLayout &newLayout, QUndoCommand *parent = 0 );

    /// redo the command
    void redo();
    /// revert the actions done in redo
    void undo();
private:
    void setLayout(const KoPageLayout &layout);

    KWDocument *m_document;
    KWPage *m_page;
    KoPageLayout m_oldLayout, m_newLayout;
};

#endif
