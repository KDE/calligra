/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPETRANSPARENCYCOMMAND_H
#define KOSHAPETRANSPARENCYCOMMAND_H

#include "flake_export.h"

#include <QList>
#include <kundo2command.h>

class KoShape;

/// The undo / redo command for setting the shape transparency
class FLAKE_EXPORT KoShapeTransparencyCommand : public KUndo2Command
{
public:
    /**
     * Command to set a new shape transparency.
     * @param shapes a set of all the shapes that should get the new background.
     * @param transparency the new shape transparency
     * @param parent the parent command used for macro commands
     */
    KoShapeTransparencyCommand(const QList<KoShape *> &shapes, qreal transparency, KUndo2Command *parent = nullptr);

    /**
     * Command to set a new shape transparency.
     * @param shape a single shape that should get the new transparency.
     * @param transparency the new shape transparency
     * @param parent the parent command used for macro commands
     */
    KoShapeTransparencyCommand(KoShape *shape, qreal transparency, KUndo2Command *parent = nullptr);

    /**
     * Command to set new shape transparencies.
     * @param shapes a set of all the shapes that should get a new transparency.
     * @param fills the new transparencies, one for each shape
     * @param parent the parent command used for macro commands
     */
    KoShapeTransparencyCommand(const QList<KoShape *> &shapes, const QList<qreal> &transparencies, KUndo2Command *parent = nullptr);

    ~KoShapeTransparencyCommand() override;
    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    class Private;
    Private *const d;
};

#endif // KOSHAPETRANSPARENCYCOMMAND_H
