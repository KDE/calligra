/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef _KARBONPATHFLATTENCOMMAND_H_
#define _KARBONPATHFLATTENCOMMAND_H_

#include <kundo2command.h>

class KoPathShape;

/// The undo / redo command for flatten a given path
class KarbonPathFlattenCommand : public KUndo2Command
{
public:
    /**
     * Command to flatten a path.
     *
     * The flatten works by inserting a specified number of points into
     * each path segment to get the given amount of flatness.
     *
     * @param path the path to flatten
     * @param flatness the desired flatness
     * @param parent the parent command used for macro commands
     */
    KarbonPathFlattenCommand(KoPathShape *path, qreal flatness, KUndo2Command *parent = nullptr);
    ~KarbonPathFlattenCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    class Private;
    Private *const d;
};

#endif // _KARBONPATHFLATTENCOMMAND_H_
