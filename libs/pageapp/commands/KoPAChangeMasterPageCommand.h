/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPACHANGEMASTERPAGECOMMAND_H
#define KOPACHANGEMASTERPAGECOMMAND_H

#include <kundo2command.h>

class KoPADocument;
class KoPAPage;
class KoPAMasterPage;

/**
 * Command to change the master page of a page
 */
class KoPAChangeMasterPageCommand : public KUndo2Command
{
public:
    KoPAChangeMasterPageCommand(KoPADocument *document, KoPAPage *page, KoPAMasterPage *masterPage);
    ~KoPAChangeMasterPageCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KoPADocument *m_document;
    KoPAPage *m_page;
    KoPAMasterPage *m_oldMasterPage;
    KoPAMasterPage *m_newMasterPage;
};

#endif /* KOPACHANGEMASTERPAGECOMMAND_H */
