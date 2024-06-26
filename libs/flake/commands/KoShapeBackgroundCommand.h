/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006, 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEBACKGROUNDCOMMAND_H
#define KOSHAPEBACKGROUNDCOMMAND_H

#include "flake_export.h"

#include <QList>
#include <QSharedPointer>
#include <kundo2command.h>

class KoShape;
class KoShapeBackground;

/// The undo / redo command for setting the shape background
class FLAKE_EXPORT KoShapeBackgroundCommand : public KUndo2Command
{
public:
    /**
     * Command to set a new shape background.
     * @param shapes a set of all the shapes that should get the new background.
     * @param fill the new shape background
     * @param parent the parent command used for macro commands
     */
    KoShapeBackgroundCommand(const QList<KoShape *> &shapes, QSharedPointer<KoShapeBackground> fill, KUndo2Command *parent = nullptr);

    /**
     * Command to set a new shape background.
     * @param shape a single shape that should get the new background.
     * @param fill the new shape background
     * @param parent the parent command used for macro commands
     */
    KoShapeBackgroundCommand(KoShape *shape, QSharedPointer<KoShapeBackground> fill, KUndo2Command *parent = nullptr);

    /**
     * Command to set new shape backgrounds.
     * @param shapes a set of all the shapes that should get a new background.
     * @param fills the new backgrounds, one for each shape
     * @param parent the parent command used for macro commands
     */
    KoShapeBackgroundCommand(const QList<KoShape *> &shapes, const QList<QSharedPointer<KoShapeBackground>> &fills, KUndo2Command *parent = nullptr);

    ~KoShapeBackgroundCommand() override;
    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    class Private;
    Private *const d;
};

#endif
