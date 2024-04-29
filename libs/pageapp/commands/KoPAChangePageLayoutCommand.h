/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPACHANGEPAGELAYOUTCOMMAND_H
#define KOPACHANGEPAGELAYOUTCOMMAND_H

#include "KoPageLayout.h"
#include <kundo2command.h>

class KoPADocument;
class KoPAMasterPage;

/**
 * Command to change the master page of a page
 */
class KoPAChangePageLayoutCommand : public KUndo2Command
{
public:
    KoPAChangePageLayoutCommand(KoPADocument *document,
                                KoPAMasterPage *page,
                                const KoPageLayout &newPageLayout,
                                bool applyToDocument,
                                KUndo2Command *parent = nullptr);
    ~KoPAChangePageLayoutCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KoPADocument *m_document;
    KoPageLayout m_newPageLayout;
    QMap<KoPAMasterPage *, KoPageLayout> m_oldLayouts;
};

#endif /* KOPACHANGEPAGELAYOUTCOMMAND_H */
