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

#include "KoEnhancedPathShape.h"
#include "KoEnhancedPathCommand.h"
#include "KoEnhancedPathParameter.h"
#include "KoEnhancedPathHandle.h"
#include "KoEnhancedPathFormula.h"

#include <kdebug.h>

KoEnhancedPathShape::KoEnhancedPathShape()
{
    KoShape::resize( QSize(100, 100) );

    m_formulae["HalfHeight"] = new KoEnhancedPathFormula( "0.5 * height" );
    m_formulae["LowerCorner"] = new KoEnhancedPathFormula( "height - $1" );

    m_modifiers.append( 60 );
    m_modifiers.append( 35 );

    KoEnhancedPathCommand * cmd = 0;
    cmd = new KoEnhancedPathCommand( 'M' );
    cmd->addParameter( new KoEnhancedPathReferenceParameter( "$0" ) );
    cmd->addParameter( new KoEnhancedPathReferenceParameter( "$1" ) );
    m_commands.append( cmd );

    cmd = new KoEnhancedPathCommand( 'L' );

    cmd->addParameter( new KoEnhancedPathReferenceParameter( "$0" ) );
    cmd->addParameter( new KoEnhancedPathConstantParameter( 0 ) );

    cmd->addParameter( new KoEnhancedPathNamedParameter( IdentifierWidth ) );
    cmd->addParameter( new KoEnhancedPathReferenceParameter( "?HalfHeight" ) );

    cmd->addParameter( new KoEnhancedPathReferenceParameter( "$0" ) );
    cmd->addParameter( new KoEnhancedPathNamedParameter( IdentifierHeight ) );

    cmd->addParameter( new KoEnhancedPathReferenceParameter( "$0" ) );
    cmd->addParameter( new KoEnhancedPathReferenceParameter( "?LowerCorner" ) );

    cmd->addParameter( new KoEnhancedPathConstantParameter( 0 ) );
    cmd->addParameter( new KoEnhancedPathReferenceParameter( "?LowerCorner" ) );

    cmd->addParameter( new KoEnhancedPathConstantParameter( 0 ) );
    cmd->addParameter( new KoEnhancedPathReferenceParameter( "$1" ) );

    m_commands.append( cmd );

    cmd = new KoEnhancedPathCommand( 'Z' );
    m_commands.append( cmd );

    KoEnhancedPathHandle *handle = 0;
    handle = new KoEnhancedPathHandle( new KoEnhancedPathReferenceParameter( "$0" ), new KoEnhancedPathReferenceParameter( "$1" ) );
    handle->setRangeX( new KoEnhancedPathConstantParameter( 0 ), new KoEnhancedPathNamedParameter( IdentifierWidth ) );
    handle->setRangeY( new KoEnhancedPathConstantParameter( 0 ), new KoEnhancedPathReferenceParameter( "?HalfHeight" ) );
    m_enhancedHandles.append( handle );

    foreach( KoEnhancedPathHandle *handle, m_enhancedHandles )
        m_handles.append( handle->position( this ) );

    updatePath( QSize( 100, 100 ) );
}

KoEnhancedPathShape::~KoEnhancedPathShape()
{
    qDeleteAll( m_commands );
    qDeleteAll( m_enhancedHandles );
    qDeleteAll( m_formulae );
}

void KoEnhancedPathShape::moveHandleAction( int handleId, const QPointF & point, Qt::KeyboardModifiers modifiers )
{
    Q_UNUSED( modifiers );
    KoEnhancedPathHandle *handle = m_enhancedHandles[ handleId ];
    if( handle )
    {
        handle->setPosition( point, this );
        m_handles[handleId] = handle->position( this );
    }
}

void KoEnhancedPathShape::updatePath( const QSizeF &size )
{
    KoShape::resize( size );

    clear();
    foreach( KoEnhancedPathCommand *cmd, m_commands )
        cmd->execute( this );

    uint handleCount = m_enhancedHandles.size();
    for( uint i = 0; i < handleCount; ++i )
        m_handles[i] = m_enhancedHandles[i]->position( this );
}

double KoEnhancedPathShape::evaluateReference( const QString &reference )
{
    kDebug() << "evaluating reference " << reference << endl;
    if( reference.isEmpty() )
        return 0.0;

    QChar c = reference[0];

    double res = 0.0;

    switch( c.toAscii() )
    {
        // referenced modifier
        case '$':
        {
            bool success = false;
            int modifierIndex = reference.mid( 1 ).toInt( &success );
            res = m_modifiers[modifierIndex];
        }
        break;
        // referenced formula
        case '?':
        {
            QString fname = reference.mid( 1 );
            FormulaStore::const_iterator formulaIt = m_formulae.find( fname );
            if( formulaIt != m_formulae.end() )
            {
                KoEnhancedPathFormula * formula = formulaIt.value();
                if( formula )
                    res = formula->evaluate( this );
            }
        }
        break;
        // maybe an identifier ?
        default:
            KoEnhancedPathNamedParameter p( reference );
            res = p.evaluate( this );
        break;
    }

    kDebug() << "evaluation result = " << res << endl;
    return res;
}

void KoEnhancedPathShape::modifyReference( const QString &reference, double value )
{
    if( reference.isEmpty() )
        return;

    QChar c = reference[0];

    if( c.toAscii() == '$' )
    {
        bool success = false;
        int modifierIndex = reference.mid( 1 ).toInt( &success );
        m_modifiers[modifierIndex] = value;
    }
}

void KoEnhancedPathShape::resize( const QSizeF &newSize )
{
    KoShape::resize( newSize );
    updatePath( newSize );
}
