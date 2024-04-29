/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPETRANSFORMCOMMAND_H
#define KOSHAPETRANSFORMCOMMAND_H

#include "flake_export.h"
#include <kundo2command.h>

class KoShape;
class QTransform;

/**
 * A command to transform a selection of shapes with the same transformation.
 */
class FLAKE_EXPORT KoShapeTransformCommand : public KUndo2Command
{
public:
    /**
     * A command to transform a selection of shapes to the new state.
     * Each shape passed has an old state and a new state of transformation passed in.
     * @param shapes all the shapes that should be transformed
     * @param oldState the old shapes transformations
     * @param newState the new shapes transformations
     * @see KoShape::transformation()
     * @see KoShape::setTransformation()
     */
    KoShapeTransformCommand(const QList<KoShape *> &shapes,
                            const QVector<QTransform> &oldState,
                            const QVector<QTransform> &newState,
                            KUndo2Command *parent = nullptr);
    ~KoShapeTransformCommand() override;
    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    class Private;
    Private *const d;
};

#endif // KOSHAPETRANSFORMCOMMAND_H
