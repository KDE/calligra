/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPECREATECOMMAND_H
#define KOSHAPECREATECOMMAND_H

#include "flake_export.h"
#include <kundo2command.h>

class KoShape;
class KoShapeBasedDocumentBase;

/// The undo / redo command for creating shapes
class FLAKE_EXPORT KoShapeCreateCommand : public KUndo2Command
{
public:
    /**
     * Command used on creation of new shapes
     * @param controller the controller used to add/remove the shape from
     * @param shape the shape thats just been created.
     * @param parent the parent command used for macro commands
     */
    KoShapeCreateCommand(KoShapeBasedDocumentBase *controller, KoShape *shape, KUndo2Command *parent = nullptr);
    ~KoShapeCreateCommand() override;
    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    class Private;
    Private *const d;
};

#endif
