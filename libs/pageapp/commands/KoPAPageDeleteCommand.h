/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPAPAGEDELETECOMMAND_H
#define KOPAPAGEDELETECOMMAND_H

#include <QMap>
#include <kundo2command.h>

#include "kopageapp_export.h"

class KoPADocument;
class KoPAPageBase;

/**
 * Command for deleting a page from a document
 */
class KOPAGEAPP_TEST_EXPORT KoPAPageDeleteCommand : public KUndo2Command
{
public:
    /**
     * Command to delete a single page.
     * @param document the document to delete the page from
     * @param page the page to delete
     * @param parent the parent command used for macro commands
     */
    KoPAPageDeleteCommand(KoPADocument *document, KoPAPageBase *page, KUndo2Command *parent = nullptr);

    /**
     * Command to delete multiple pages.
     * @param document the document to delete the pages from
     * @param pages a list of pages to delete
     * @param parent the parent command used for macro commands
     */
    KoPAPageDeleteCommand(KoPADocument *document, const QList<KoPAPageBase *> &pages, KUndo2Command *parent = nullptr);

    ~KoPAPageDeleteCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KoPADocument *m_document; //< the document to delete the pages from
    QMap<int, KoPAPageBase *> m_pages; //< a map of pages to delete, with document index as key
    bool m_deletePages; //< shows if pages should be deleted when deleting the command
};

#endif // KOPAPAGEDELETECOMMAND_H
