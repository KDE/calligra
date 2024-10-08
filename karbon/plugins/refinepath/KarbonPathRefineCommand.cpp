/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KarbonPathRefineCommand.h"
#include <KLocalizedString>
#include <KoPathPointData.h>
#include <KoPathPointInsertCommand.h>
#include <KoPathShape.h>

class KarbonPathRefineCommand::Private
{
public:
    Private(KoPathShape *p, uint insertPointsCount)
        : path(p)
        , insertCount(insertPointsCount)
        , initialized(false)
    {
    }
    ~Private() = default;

    KoPathShape *path; ///< the path to refine
    uint insertCount; ///< the number of points to insert into segments
    bool initialized; ///< tells whether the subcommands are already created
};

KarbonPathRefineCommand::KarbonPathRefineCommand(KoPathShape *path, uint insertPointsCount, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new Private(path, insertPointsCount))
{
    setText(kundo2_i18n("Refine path"));
}

KarbonPathRefineCommand::~KarbonPathRefineCommand()
{
    delete d;
}

void KarbonPathRefineCommand::redo()
{
    // check if we have to create the insert points commands
    if (!d->initialized) {
        // create insert point commands, one for each point to insert
        // into each segment
        for (uint iteration = 0; iteration < d->insertCount; ++iteration) {
            // in each iteration collect the (iteration+1)th point which starts a segments
            // into which we insert the point of this iteration
            QList<KoPathPointData> pointData;
            // calculate the segment position where to insert the point
            qreal insertPosition = 1.0 / (d->insertCount + 1 - iteration);
            int subpathCount = d->path->subpathCount();
            // iterate over the paths subpaths
            for (int subpathIndex = 0; subpathIndex < subpathCount; ++subpathIndex) {
                int pointCount = d->path->subpathPointCount(subpathIndex);
                // iterate over the subpaths points
                for (int pointIndex = 0; pointIndex < pointCount; ++pointIndex) {
                    // we only collect the (iteration+1)th point
                    if ((pointIndex + 1) % (iteration + 1) != 0)
                        continue;
                    pointData.append(KoPathPointData(d->path, KoPathPointIndex(subpathIndex, pointIndex)));
                }
            }
            // create the command and execute it
            KUndo2Command *cmd = new KoPathPointInsertCommand(pointData, insertPosition, this);
            cmd->redo();
        }
        d->initialized = true;
    } else {
        KUndo2Command::redo();
    }
    d->path->update();
}

void KarbonPathRefineCommand::undo()
{
    KUndo2Command::undo();
    d->path->update();
}
