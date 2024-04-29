/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPECLIPCOMMAND_H
#define KOSHAPECLIPCOMMAND_H

#include "flake_export.h"
#include <QList>
#include <kundo2command.h>

class KoShape;
class KoPathShape;
class KoShapeBasedDocumentBase;

/// The undo / redo command for setting the shape clip path
class FLAKE_EXPORT KoShapeClipCommand : public KUndo2Command
{
public:
    /**
     * Command to set a new shape clipping path for multiple shape.
     * @param controller the controller to used for deleting.
     * @param shapes a set of all the shapes that should get the clipping path.
     * @param clipPathShapes the path shapes to be used a clipping path
     * @param parent the parent command used for macro commands
     */
    KoShapeClipCommand(KoShapeBasedDocumentBase *controller,
                       const QList<KoShape *> &shapes,
                       const QList<KoPathShape *> &clipPathShapes,
                       KUndo2Command *parent = nullptr);

    /**
     * Command to set a new shape clipping path for a single shape
     * @param controller the controller to used for deleting.
     * @param shape a single shape that should get the new shadow.
     * @param clipPathShapes the path shapes to be used a clipping path
     * @param parent the parent command used for macro commands
     */
    KoShapeClipCommand(KoShapeBasedDocumentBase *controller, KoShape *shape, const QList<KoPathShape *> &clipPathShapes, KUndo2Command *parent = nullptr);

    /// Destroys the command
    ~KoShapeClipCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    class Private;
    Private *const d;
};

#endif // KOSHAPECLIPCOMMAND_H
