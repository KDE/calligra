/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHPOINTMERGECOMMAND_H
#define KOPATHPOINTMERGECOMMAND_H

#include <kundo2command.h>

#include "flake_export.h"

class KoPathPointData;

/// The undo / redo command for merging two subpath end points
class FLAKE_EXPORT KoPathPointMergeCommand : public KUndo2Command
{
public:
    /**
     * Command to merge two subpath end points.
     *
     * The points have to be from the same path shape.
     *
     * @param pointData1 the data of the first point to merge
     * @param pointData2 the data of the second point to merge
     * @param parent the parent command used for macro commands
     */
    KoPathPointMergeCommand(const KoPathPointData &pointData1, const KoPathPointData &pointData2, KUndo2Command *parent = nullptr);
    ~KoPathPointMergeCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    class Private;
    Private *const d;
};

#endif // KOPATHPOINTMERGECOMMAND_H
