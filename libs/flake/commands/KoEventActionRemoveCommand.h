/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOEVENTACTIONREMOVECOMMAND_H
#define KOEVENTACTIONREMOVECOMMAND_H

#include "flake_export.h"
#include <kundo2command.h>

class KoShape;
class KoEventAction;
class KoEventActionRemoveCommandPrivate;

/// API docs go here
class FLAKE_EXPORT KoEventActionRemoveCommand : public KUndo2Command
{
public:
    KoEventActionRemoveCommand(KoShape *shape, KoEventAction *eventAction, KUndo2Command *parent = nullptr);
    ~KoEventActionRemoveCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KoEventActionRemoveCommandPrivate *const d;
};

#endif /* KOEVENTACTIONREMOVECOMMAND_H */
