/* This file is part of the KDE project
 * Copyright (C) 2007-2008 Thomas Zander <zander@kde.org>
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

#ifndef KWPAGEREMOVECOMMAND_H
#define KWPAGEREMOVECOMMAND_H

#include "words_export.h"
#include "KWPage.h"

#include <kundo2command.h>

class KWDocument;
class KoShapeMoveCommand;
class KWTextFrameSet;

/// The undo / redo command for removing a page in a words document.
class WORDS_TEST_EXPORT KWPageRemoveCommand : public KUndo2Command
{
public:
    /**
     * The constructor for a command to insert a new page.
     * @param document the document that gets a new page.
     * @param page the page to remove.
     * @param parent the parent for command macros
     */
    explicit KWPageRemoveCommand(KWDocument *document, KWPage page, KUndo2Command *parent = 0);
    ~KWPageRemoveCommand();

    /// (re)do the command
    virtual void redo();
    /// revert the actions done in redo
    virtual void undo();

private:
    KWDocument *m_document;
    KWPage::PageSide m_pageSide;
    KoPageLayout m_pageLayout;
    KoPageFormat::Orientation m_orientation;
    int m_pageNumber;
    QString m_masterPageName;
    KoText::Direction m_direction;

    struct AutoGenFrameSet {
        KWTextFrameSet *frameSet;
        int deleteFromFrame;
    };
    QList<AutoGenFrameSet> m_autoGenFrameSets;
};

#endif
