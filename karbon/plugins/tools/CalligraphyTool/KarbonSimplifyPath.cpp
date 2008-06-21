/* This file is part of the KDE project
   Copyright (C) 2008 Fela Winkelmolen <fela.kde@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KarbonSimplifyPath.h"

#include <KarbonCurveFit.h>
#include <KoPathShape.h>
#include <KoPathPoint.h>
#include <KDebug>

/*
the algorithm proceeds as following:

1. divide the paths wherever it's not smooth
2. for each of the resulting paths that has at least three points, add points
   recursively wherever the path is too "complicated"(*). TODO
3. apply bezierFit on the resulting points
4. remerge the paths

(*) TODO: write definition of too complicated here


FIXME: bezier fit seems to crash when getting to many points in input,
       if there are to many point in one of the subpaths, split it
*/

namespace KarbonSimplifyPath {
    // the points remain owned by path, not by the returning subpaths
    QList<KoSubpath *> split( const KoPathShape &path );

    // after this call the points _are_ owned by the subpaths
    void simplifySubpaths( QList<KoSubpath *> *subpaths, double error );

    // auxiliary function for the above
    void simplifySubpath( KoSubpath *subpath, double error );

    // put the result into path
    void mergeSubpaths( QList<KoSubpath *> subpaths, KoPathShape *path );
}

using namespace KarbonSimplifyPath;

void karbonSimplifyPath( KoPathShape *path, double error )
{
    if ( path->pointCount() == 0 )
        return;

    QList<KoSubpath *> subpaths = split( *path );
    // TODO: step 2.
    simplifySubpaths( &subpaths, error );
    mergeSubpaths( subpaths, path );


    while ( ! subpaths.isEmpty() )
    {
        KoSubpath *subpath = subpaths.takeLast();
        while ( ! subpath->isEmpty() )
            delete subpath->takeLast();
    }
}

QList<KoSubpath *> KarbonSimplifyPath::split( const KoPathShape &path )
{
    QList<KoSubpath *> res;
    KoSubpath *subpath = new KoSubpath;
    res.append( subpath );

    for ( int i = 0; i < path.pointCount(); ++i )
    {
        KoPathPoint *p = path.pointByIndex( KoPathPointIndex(0, i) );
        // if the path separates two subpaths
        // (if it isn't smooth nor the first or last point)
        if ( i != 0  &&  i != path.pointCount()-1 )
        {
            KoPathPoint *prev = path.pointByIndex( KoPathPointIndex(0, i-1) );
            KoPathPoint *next = path.pointByIndex( KoPathPointIndex(0, i+1) );
            if ( ! p->isSmooth(prev, next) )
            {
                // create a new subpath
                subpath->append( p );
                kDebug() << p->point();
                subpath = new KoSubpath;
                res.append( subpath );
            }
        }
        subpath->append( p );
        kDebug() << p->point();
    }

    kDebug() << res.size();
    return res;
}


void KarbonSimplifyPath::simplifySubpaths( QList<KoSubpath *> *subpaths,
                                           double error )
{
    foreach ( KoSubpath *subpath, *subpaths )
    {
        if ( subpath->size() > 2 )
        {
            simplifySubpath( subpath, error );
        }
        else
        {
            // make a copy of the pathpoints, to make them owned by the subpath
            for ( int i = 0; i < subpath->size(); ++i )
            {
                (*subpath)[i] = new KoPathPoint( *(*subpath)[i] );
            }
        }
    }
}

void KarbonSimplifyPath::simplifySubpath( KoSubpath *subpath, double error )
{
    QList<QPointF> points;

    for ( int i = 0; i < subpath->size(); ++i )
    {
        points.append( (*subpath)[i]->point() );
    }

    KoPathShape *simplified = bezierFit( points, error );

    subpath->clear();

    for ( int i = 0; i < simplified->pointCount(); ++i )
    {
        KoPathPointIndex index(0, i);
        subpath->append( new KoPathPoint( *simplified->pointByIndex(index) ) );
    }
    //res->setPosition( position() );
    delete simplified;
}

void KarbonSimplifyPath::mergeSubpaths( QList<KoSubpath *> subpaths,
                                        KoPathShape *path )
{
    path->clear();
    path->moveTo( subpaths.first()->first()->point() );


    // TODO: to make the code more readable use foreach and explicit
    //       counters with full name
    // si: subpath index, pi: point index
    for ( int si = 0; si < subpaths.size(); ++si )
    {
        for ( int pi = 1; pi < subpaths[si]->size(); ++pi )
        {
            KoPathPoint *point = (*subpaths[si])[pi];
            path->lineTo( point->point() );
            
            // if the point isn't the last thus has control points
            if ( pi != subpaths[si]->size()-1 )
            {
                // set the first control point
                KoPathPointIndex index( 0, path->pointCount()-1 );
                KoPathPoint *p = path->pointByIndex( index );
                p->setControlPoint1( point->controlPoint1() );
            }

            // if the point isn't the second
            // and thus the previous has control points
            if ( pi != 1 )
            {
                // set the second control point of the previous point
                KoPathPointIndex index( 0, path->pointCount()-2 );
                KoPathPoint *p = path->pointByIndex( index );
                KoPathPoint *prev = (*subpaths[si])[pi-1];
                p->setControlPoint2( prev->controlPoint2() );
            }
        }
    }
}
