/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPADISPLAYMASTERSHAPESCOMMAND_H
#define KOPADISPLAYMASTERSHAPESCOMMAND_H

#include <kundo2command.h>

#include "kopageapp_export.h"

class KoPAPage;

/**
 * Command to change if master shapes should be displayed
 */
class KOPAGEAPP_EXPORT KoPADisplayMasterShapesCommand : public KUndo2Command
{
public:
    KoPADisplayMasterShapesCommand(KoPAPage *page, bool display);
    ~KoPADisplayMasterShapesCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KoPAPage *m_page;
    bool m_display;
};

#endif /* KOPADISPLAYMASTERSHAPESCOMMAND_H */
