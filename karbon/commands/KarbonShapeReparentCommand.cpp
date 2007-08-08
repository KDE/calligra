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

#include "KarbonShapeReparentCommand.h"
#include "KoShapeContainer.h"
#include <klocale.h>

KarbonShapeReparentCommand::KarbonShapeReparentCommand( QList<KoShape*> shapes, QList<KoShapeContainer*> oldParents,
                        QList<KoShapeContainer*> newParents, QUndoCommand *parent )
    : QUndoCommand( parent ), m_shapes(shapes), m_oldParents(oldParents), m_newParents(newParents)
{
    Q_ASSERT( m_shapes.count() == m_oldParents.count() );
    Q_ASSERT( m_shapes.count() == m_newParents.count() );

    setText( i18n("Reparent shapes") );
}

KarbonShapeReparentCommand::~KarbonShapeReparentCommand()
{
}

void KarbonShapeReparentCommand::redo()
{
    uint shapesCount = m_shapes.count();
    for( uint i = 0; i < shapesCount; ++i )
    {
        KoShape * child = m_shapes[i];
        if( m_oldParents[i] )
            m_oldParents[i]->removeChild( child );
        if( m_newParents[i] )
            m_newParents[i]->addChild( child );
    }
    QUndoCommand::redo();
}

void KarbonShapeReparentCommand::undo()
{
    uint shapesCount = m_shapes.count();
    for( uint i = 0; i < shapesCount; ++i )
    {
        KoShape * child = m_shapes[i];
        if( m_newParents[i] )
            m_newParents[i]->removeChild( child );
        if( m_oldParents[i] )
            m_oldParents[i]->addChild( child );
    }
    QUndoCommand::undo();
}
