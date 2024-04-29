/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPAPAGEMOVECOMMAND_H
#define KOPAPAGEMOVECOMMAND_H

#include <QMap>
#include <kundo2command.h>

#include "kopageapp_export.h"

class KoPADocument;
class KoPAPageBase;

/**
 * Command for moving a page in a document
 */
class KOPAGEAPP_TEST_EXPORT KoPAPageMoveCommand : public KUndo2Command
{
public:
    KoPAPageMoveCommand(KoPADocument *document, KoPAPageBase *page, KoPAPageBase *after, KUndo2Command *parent = nullptr);
    KoPAPageMoveCommand(KoPADocument *document, const QList<KoPAPageBase *> &pages, KoPAPageBase *after, KUndo2Command *parent = nullptr);
    void init(const QList<KoPAPageBase *> &pages);
    ~KoPAPageMoveCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KoPADocument *m_document;
    QMap<int, KoPAPageBase *> m_pageIndices;
    KoPAPageBase *m_after;
};

#endif // KOPAPAGEMOVECOMMAND_H
