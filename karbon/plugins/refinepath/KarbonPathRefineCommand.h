/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef _KARBONPATHREFINECOMMAND_H_
#define _KARBONPATHREFINECOMMAND_H_

#include <kundo2command.h>

class KoPathShape;

/// The undo / redo command for refining a given path
class KarbonPathRefineCommand : public KUndo2Command
{
public:
    /**
     * Command to refine a path.
     *
     * The refining works by inserting a specified number of points into
     * each path segment.
     *
     * @param path the path to refine
     * @param insertPointsCount number of point to insert into each segment
     * @param parent the parent command used for macro commands
     */
    KarbonPathRefineCommand(KoPathShape *path, uint insertPointsCount, KUndo2Command *parent = nullptr);
    ~KarbonPathRefineCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    class Private;
    Private *const d;
};

#endif // _KARBONPATHREFINECOMMAND_H_
