/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPAPAGEINSERTCOMMAND_H
#define KOPAPAGEINSERTCOMMAND_H

#include <kundo2command.h>

#include "kopageapp_export.h"

class KoPADocument;
class KoPAPageBase;

/**
 * Command for inserting a page into a Document
 */
class KOPAGEAPP_TEST_EXPORT KoPAPageInsertCommand : public KUndo2Command
{
public:
    KoPAPageInsertCommand(KoPADocument *document, KoPAPageBase *page, KoPAPageBase *after, KUndo2Command *parent = nullptr);
    ~KoPAPageInsertCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KoPADocument *m_document;
    KoPAPageBase *m_page;
    KoPAPageBase *m_after;
    bool m_deletePage;
};

#endif // KOPAPAGEINSERTCOMMAND_H
