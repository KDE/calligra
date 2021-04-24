/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoShapeTraversal.h"

#include <PageAppDebug.h>

#include <KoShape.h>
#include <KoShapeContainer.h>

#include <algorithm>

KoShape * KoShapeTraversal::nextShape( const KoShape * current )
{
    return nextShapeStep( current, 0 );
}

KoShape * KoShapeTraversal::nextShape( const KoShape * current, const QString & shapeId )
{
    KoShape * next = nextShapeStep( current, 0 );

    while ( next != 0 && next->shapeId() != shapeId ) {
        next = nextShapeStep( next, 0 );
    }

    return next;
}

KoShape * KoShapeTraversal::previousShape( const KoShape * current )
{
    return previousShapeStep( current, 0 );
}

KoShape * KoShapeTraversal::previousShape( const KoShape * current, const QString & shapeId )
{
    KoShape * previous = previousShapeStep( current, 0 );

    while ( previous != 0 && previous->shapeId() != shapeId ) {
        previous = previousShapeStep( previous, 0 );
    }

    return previous;
}

KoShape * KoShapeTraversal::last( KoShape * current )
{
    KoShape * last = current;
    while ( const KoShapeContainer * container = dynamic_cast<const KoShapeContainer *>( last ) ) {
        QList<KoShape*> shapes = container->shapes();
        if ( !shapes.isEmpty() ) {
            last = shapes.last();
        }
        else {
            break;
        }
    }
    return last;
}

KoShape * KoShapeTraversal::nextShapeStep( const KoShape * current, const KoShapeContainer * parent )
{
    Q_ASSERT( current );
    if ( !current ) {
        return 0;
    }

    KoShape * next = 0;

    if ( parent ) {
        const QList<KoShape*> shapes = parent->shapes();
        QList<KoShape*>::const_iterator it( std::find( shapes.begin(), shapes.end(), current ) );
        Q_ASSERT( it != shapes.end() );

        if ( it == shapes.end() ) {
            warnPageApp << "the shape is not in the list of children of his parent";
            return 0;
        }

        ++it;
        if ( it != shapes.end() ) {
            next = *it;
        }
        else {
            KoShapeContainer * currentParent = parent->parent();
            next = currentParent ? nextShapeStep( parent, currentParent ) : 0;
        }
    }
    else {
        if ( const KoShapeContainer * container = dynamic_cast<const KoShapeContainer *>( current ) ) {
            QList<KoShape*> shapes = container->shapes();
            if ( !shapes.isEmpty() ) {
                next = shapes[0];
            }
        }

        if ( next == 0 ) {
            KoShapeContainer * currentParent = current->parent();
            next = currentParent ? nextShapeStep( current, currentParent ) : 0;
        }
    }

    return next;
}

KoShape * KoShapeTraversal::previousShapeStep( const KoShape * current, const KoShapeContainer * parent )
{
    Q_ASSERT( current );
    if ( !current ) {
        return 0;
    }

    KoShape * previous = 0;

    if ( parent ) {
        if ( previous == 0 ) {
            const QList<KoShape*> shapes = parent->shapes();
            QList<KoShape*>::const_iterator it( std::find( shapes.begin(), shapes.end(), current ) );
            Q_ASSERT( it != shapes.end() );

            if ( it == shapes.end() ) {
                warnPageApp << "the shape is not in the list of children of his parent";
                return 0;
            }

            if ( it != shapes.begin() ) {
                --it;
                previous = last( *it );
            }
            else {
                previous = current->parent();
            }
        }
    }
    else {
        KoShapeContainer * currentParent = current->parent();
        previous = currentParent ? previousShapeStep( current, currentParent ) : 0;
    }

    return previous;
}
