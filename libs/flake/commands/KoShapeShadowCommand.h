/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPESHADOWCOMMAND_H
#define KOSHAPESHADOWCOMMAND_H

#include "flake_export.h"
#include <QList>
#include <kundo2command.h>

class KoShape;
class KoShapeShadow;

/// The undo / redo command for setting the shape shadow
class FLAKE_EXPORT KoShapeShadowCommand : public KUndo2Command
{
public:
    /**
     * Command to set a new shape shadow.
     * @param shapes a set of all the shapes that should get the new shadow.
     * @param shadow the new shadow, the same for all given shapes
     * @param parent the parent command used for macro commands
     */
    KoShapeShadowCommand(const QList<KoShape *> &shapes, KoShapeShadow *shadow, KUndo2Command *parent = nullptr);

    /**
     * Command to set new shape shadows.
     * @param shapes a set of all the shapes that should get a new shadow.
     * @param shadows the new shadows, one for each shape
     * @param parent the parent command used for macro commands
     */
    KoShapeShadowCommand(const QList<KoShape *> &shapes, const QList<KoShapeShadow *> &shadows, KUndo2Command *parent = nullptr);

    /**
     * Command to set a new shape shadow.
     * @param shape a single shape that should get the new shadow.
     * @param shadow the new shadow
     * @param parent the parent command used for macro commands
     */
    KoShapeShadowCommand(KoShape *shape, KoShapeShadow *shadow, KUndo2Command *parent = nullptr);

    ~KoShapeShadowCommand() override;
    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    class Private;
    Private *const d;
};

#endif // KOSHAPESHADOWCOMMAND_H
