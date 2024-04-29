/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEDELETECOMMAND_H
#define KOSHAPEDELETECOMMAND_H

#include "flake_export.h"
#include <QList>
#include <kundo2command.h>

class KoShape;
class KoShapeBasedDocumentBase;

/// The undo / redo command for deleting shapes
class FLAKE_EXPORT KoShapeDeleteCommand : public KUndo2Command
{
public:
    /**
     * Command to delete a single shape by means of a shape controller.
     * @param controller the controller to used for deleting.
     * @param shape a single shape that should be deleted.
     * @param parent the parent command used for macro commands
     */
    KoShapeDeleteCommand(KoShapeBasedDocumentBase *controller, KoShape *shape, KUndo2Command *parent = nullptr);
    /**
     * Command to delete a set of shapes by means of a shape controller.
     * @param controller the controller to used for deleting.
     * @param shapes a set of all the shapes that should be deleted.
     * @param parent the parent command used for macro commands
     */
    KoShapeDeleteCommand(KoShapeBasedDocumentBase *controller, const QList<KoShape *> &shapes, KUndo2Command *parent = nullptr);
    ~KoShapeDeleteCommand() override;
    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    class Private;
    Private *const d;
};

#endif
