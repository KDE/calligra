/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KarbonPathRefineCommand.h"
#include <KoPathShape.h>
#include <KoPathPointData.h>
#include <KoPathPointInsertCommand.h>
#include <klocale.h>
#include <kdebug.h>

class KarbonPathRefineCommand::Private
{
public:
    Private( KoPathShape * p, uint insertPointsCount ) 
    : path( p ), insertCount( insertPointsCount ), initialized(false) {}
    ~Private() {}

    KoPathShape * path; ///< the path to refine
    uint insertCount;   ///< the number of points to insert into segments
    bool initialized;   ///< tells whether the subcommands are already created
};

KarbonPathRefineCommand::KarbonPathRefineCommand( KoPathShape * path, uint insertPointsCount, QUndoCommand *parent )
    : QUndoCommand( parent ), d( new Private( path, insertPointsCount ) )
{
    setText( i18n( "Refine path" ) );
}

KarbonPathRefineCommand::~KarbonPathRefineCommand()
{
    delete d;
}

void KarbonPathRefineCommand::redo()
{
    // check if we have to create the insert points commands
    if( ! d->initialized )
    {
        // create insert point commands, one for each point to insert
        // into each segment
        for( uint iteration = 0; iteration < d->insertCount; ++iteration )
        {
            // in each iteration collect the (iteration+1)th point which starts a segments
            // into which we insert the point of this iteration
            QList<KoPathPointData> pointData;
            // calculate the segment position where to insert the point
            qreal insertPosition = 1.0 / (d->insertCount+1-iteration);
            int subpathCount = d->path->subpathCount();
            // iterate over the paths subpaths
            for( int subpathIndex = 0; subpathIndex < subpathCount; ++subpathIndex )
            {
                int pointCount = d->path->subpathPointCount( subpathIndex );
                // iterate over the subpaths points
                for( int pointIndex = 0; pointIndex < pointCount; ++pointIndex )
                {
                    // we only collect the (iteration+1)th point
                    if( (pointIndex+1) % (iteration+1) != 0 )
                        continue;
                    pointData.append( KoPathPointData( d->path, KoPathPointIndex( subpathIndex, pointIndex ) ) );
                }
            }
            // create the command and execute it
            QUndoCommand * cmd = new KoPathPointInsertCommand( pointData, insertPosition, this );
            cmd->redo();
        }
        d->initialized = true;
    }
    else
    {
        QUndoCommand::redo();
    }
    d->path->update();
}

void KarbonPathRefineCommand::undo()
{
    QUndoCommand::undo();
    d->path->update();
}

