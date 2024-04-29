/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006, 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEGROUPCOMMAND_H
#define KOSHAPEGROUPCOMMAND_H

#include "flake_export.h"

#include <QList>
#include <kundo2command.h>

class KoShape;
class KoShapeGroup;
class KoShapeContainer;
class KoShapeGroupCommandPrivate;

/// The undo / redo command for grouping shapes
class FLAKE_EXPORT KoShapeGroupCommand : public KUndo2Command
{
public:
    /**
     * Create command to group a set of shapes into a predefined container.
     * This uses the KoShapeGroupCommand(KoShapeGroup *container, const QList<KoShape *> &shapes, KUndo2Command *parent = nullptr);
     * constructor.
     * The createCommand will make sure that the group will have the z-index and the parent of the top most shape in the group.
     *
     * @param container the group to group the shapes under.
     * @param parent the parent command if the resulting command is a compound undo command.
     * @param shapes a list of all the shapes that should be grouped.
     */
    static KoShapeGroupCommand *createCommand(KoShapeGroup *container, const QList<KoShape *> &shapes, KUndo2Command *parent = nullptr);

    /**
     * Command to group a set of shapes into a predefined container.
     * @param container the container to group the shapes under.
     * @param shapes a list of all the shapes that should be grouped.
     * @param clipped a list of the same length as the shapes list with one bool for each shape.
     *      See KoShapeContainer::isClipped()
     * @param inheritTransform a list of the same length as the shapes list with one bool for each shape.
     *      See KoShapeContainer::inheritsTransform()
     * @param parent the parent command used for macro commands
     */
    KoShapeGroupCommand(KoShapeContainer *container,
                        const QList<KoShape *> &shapes,
                        const QList<bool> &clipped,
                        const QList<bool> &inheritTransform,
                        KUndo2Command *parent = nullptr);
    /**
     * Command to group a set of shapes into a predefined container.
     * Convenience constructor since KoShapeGroup does not allow clipping.
     * @param container the group to group the shapes under.
     * @param parent the parent command if the resulting command is a compound undo command.
     * @param shapes a list of all the shapes that should be grouped.
     */
    KoShapeGroupCommand(KoShapeGroup *container, const QList<KoShape *> &shapes, KUndo2Command *parent = nullptr);
    ~KoShapeGroupCommand() override;
    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

protected:
    KoShapeGroupCommandPrivate *d;
    KoShapeGroupCommand(KoShapeGroupCommandPrivate &, KUndo2Command *parent);
};

#endif
