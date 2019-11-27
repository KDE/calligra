/* This file is part of the KDE project
 * Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2011 Paul Mendez <paulestebanms@gmail.com>
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

#ifndef KPRDELETESLIDESCOMMAND_H
#define KPRDELETESLIDESCOMMAND_H

#include <kundo2command.h>
#include <QMap>

class KPrDocument;
class KoPAPageBase;

#include "stage_export.h"

/**
 * Command for deleting a slide from a document
 * It deletes also a slide from all custom shows
 */
class STAGE_TEST_EXPORT KPrDeleteSlidesCommand : public KUndo2Command
{
public:
    /**
     * Command to delete a single page.
     * @param document the document to delete the page from
     * @param page the page to delete
     * @param parent the parent command used for macro commands
     */
    KPrDeleteSlidesCommand(KPrDocument *document, KoPAPageBase *page, KUndo2Command *parent = 0 );

    /**
     * Command to delete multiple pages.
     * @param document the document to delete the pages from
     * @param pages a list of pages to delete
     * @param parent the parent command used for macro commands
     */
    KPrDeleteSlidesCommand(KPrDocument *document, const QList<KoPAPageBase*> &pages, KUndo2Command *parent = 0);

    ~KPrDeleteSlidesCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KPrDocument *m_document; //< the document to delete the pages from
    QMap<int, KoPAPageBase*> m_pages; //< a map of pages to delete, with document index as key
    QMap<QString, QList<KoPAPageBase*> > m_customSlideShows;
    bool m_deletePages; //< shows if pages should be deleted when deleting the command
};

#endif // KPRDELETESLIDESCOMMAND_H
