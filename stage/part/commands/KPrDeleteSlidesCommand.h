/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2011 Paul Mendez <paulestebanms@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRDELETESLIDESCOMMAND_H
#define KPRDELETESLIDESCOMMAND_H

#include <QMap>
#include <kundo2command.h>

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
    KPrDeleteSlidesCommand(KPrDocument *document, KoPAPageBase *page, KUndo2Command *parent = nullptr);

    /**
     * Command to delete multiple pages.
     * @param document the document to delete the pages from
     * @param pages a list of pages to delete
     * @param parent the parent command used for macro commands
     */
    KPrDeleteSlidesCommand(KPrDocument *document, const QList<KoPAPageBase *> &pages, KUndo2Command *parent = nullptr);

    ~KPrDeleteSlidesCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KPrDocument *m_document; //< the document to delete the pages from
    QMap<int, KoPAPageBase *> m_pages; //< a map of pages to delete, with document index as key
    QMap<QString, QList<KoPAPageBase *>> m_customSlideShows;
    bool m_deletePages; //< shows if pages should be deleted when deleting the command
};

#endif // KPRDELETESLIDESCOMMAND_H
