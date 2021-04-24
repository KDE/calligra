/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOEVENTACTIONADDCOMMAND_H
#define KOEVENTACTIONADDCOMMAND_H

#include <kundo2command.h>
#include "flake_export.h"

class KoShape;
class KoEventAction;
class KoEventActionAddCommandPrivate;

/// API docs go here
class FLAKE_EXPORT KoEventActionAddCommand : public KUndo2Command
{
public:
    KoEventActionAddCommand(KoShape *shape, KoEventAction *eventAction, KUndo2Command *parent = 0);
    ~KoEventActionAddCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KoEventActionAddCommandPrivate * const d;
};

#endif /* KOEVENTACTIONADDCOMMAND_H */
