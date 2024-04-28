/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 *
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPADISPLAYMASTERBACKGROUNDCOMMAND_H
#define KOPADISPLAYMASTERBACKGROUNDCOMMAND_H

#include <kundo2command.h>

#include "kopageapp_export.h"

class KoPAPage;

/**
 * Command to change if master shapes should be displayed
 */
class KOPAGEAPP_EXPORT KoPADisplayMasterBackgroundCommand : public KUndo2Command
{
public:
    KoPADisplayMasterBackgroundCommand(KoPAPage *page, bool display);
    ~KoPADisplayMasterBackgroundCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KoPAPage *m_page;
    bool m_display;
};

#endif // KOPADISPLAYMASTERBACKGROUNDCOMMAND_H
