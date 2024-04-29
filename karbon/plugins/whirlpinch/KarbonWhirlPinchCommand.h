/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KARBONWHIRLPINCHCOMMAND_H
#define KARBONWHIRLPINCHCOMMAND_H

#include <kundo2command.h>

class KoPathShape;

/// The undo / redo command for the whirl pinch effect on a given path
class KarbonWhirlPinchCommand : public KUndo2Command
{
public:
    /**
     * Command for whirl pinch effect on a path.
     *
     * @param path the path to flatten
     * @param angle the whirl angle
     * @param pinch the pinch amount between -1 and 1
     * @param radius the effect radius
     * @param parent the parent command used for macro commands
     */
    KarbonWhirlPinchCommand(KoPathShape *path, qreal angle, qreal pinch, qreal radius, KUndo2Command *parent = nullptr);
    ~KarbonWhirlPinchCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    class Private;
    Private *const d;
};

#endif // KARBONWHIRLPINCHCOMMAND_H
