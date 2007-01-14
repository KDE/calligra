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

#include "KoEnhancedPathCommand.h"
#include "KoEnhancedPathParameter.h"
#include "KoEnhancedPathShape.h"

KoEnhancedPathCommand::KoEnhancedPathCommand( const QChar & command )
    : m_command( command )
{
}

KoEnhancedPathCommand::~KoEnhancedPathCommand()
{
}

bool KoEnhancedPathCommand::execute( KoEnhancedPathShape * path )
{
    QList<QPointF> points = pointsFromParameters( path );
    uint pointsCount = points.size();

    switch( m_command.toAscii() )
    {
        case 'M':
            if( ! pointsCount )
                return false;
            path->moveTo( points[0] );
        break;
        case 'L':
            foreach( QPointF point, points )
                path->lineTo( point );
        break;
        case 'C':
            for( uint i = 0; i < pointsCount; i+=3 )
                path->curveTo( points[i], points[i+1], points[i+2] );
        break;
        case 'Z':
            path->close();
        break;
    }
    return true;
}

QList<QPointF> KoEnhancedPathCommand::pointsFromParameters( KoEnhancedPathShape *path )
{
    QList<QPointF> points;
    QPointF p;
    uint paramCount = m_parameters.count();
    for( uint i = 0; i < paramCount; ++i )
    {
        if( i % 2 == 0 )
            p.setX( m_parameters[i]->evaluate( path ) );
        else
        {
            p.setY( m_parameters[i]->evaluate( path ) );
            points.append( p );
        }
    }
    return points;
}

void KoEnhancedPathCommand::addParameter( KoEnhancedPathParameter *parameter )
{
    if( parameter )
        m_parameters.append( parameter );
}
