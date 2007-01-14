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

KoEnhancedPathShape::KoEnhancedPathShape()
{
    KoShape::resize( QSize(100, 100) );
}

KoEnhancedPathShape::~KoEnhancedPathShape()
{
    qDeleteAll( m_commands );
    qDeleteAll( m_enhancedHandles );
    qDeleteAll( m_formulae );
    qDeleteAll( m_parameters );
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
        if( modifierIndex >= 0 && modifierIndex < m_modifiers.count() )
            m_modifiers[modifierIndex] = value;
    }
}

void KoEnhancedPathShape::resize( const QSizeF &newSize )
{
    KoShape::resize( newSize );
    KoParameterShape::resize( newSize );

    uint handleCount = m_enhancedHandles.size();
    for( uint i = 0; i < handleCount; ++i )
        m_enhancedHandles[i]->setPosition( m_handles[i], this );

    updatePath( newSize );
}

KoEnhancedPathParameter * KoEnhancedPathShape::parameter( const QString & text )
{
    Q_ASSERT( ! text.isEmpty() );

    ParameterStore::const_iterator parameterIt = m_parameters.find( text );
    if( parameterIt != m_parameters.end() )
        return parameterIt.value();
    else
    {
        KoEnhancedPathParameter *parameter = 0;
        QChar c = text[0];
        if( c.toAscii() == '$' || c.toAscii() == '?' )
            parameter = new KoEnhancedPathReferenceParameter( text );
        else
        {
            if( c.isDigit() )
            {
                bool success = false;
                double constant = text.toDouble( &success );
                if( success )
                    parameter = new KoEnhancedPathConstantParameter( constant );
            }
            else
            {
                Identifier identifier = KoEnhancedPathNamedParameter::identifierFromString( text );
                if( identifier != IdentifierUnknown )
                    parameter = new KoEnhancedPathNamedParameter( identifier );
            }
        }

        if( parameter )
            m_parameters[text] = parameter;

        return parameter;
    }
}

void KoEnhancedPathShape::addFormula( const QString &name, const QString &formula )
{
    if( name.isEmpty() || formula.isEmpty() )
        return;

    m_formulae[name] = new KoEnhancedPathFormula( formula );
}

void KoEnhancedPathShape::addHandle( const QString &handle )
{
    if( handle.isEmpty() )
        return;

    QStringList tokens = handle.simplified().split( ' ' );
    int tokenCount = tokens.count();
    if( tokenCount < 2 )
        return;
    KoEnhancedPathHandle *h = new KoEnhancedPathHandle( parameter( tokens[0] ), parameter( tokens[1] ) );
    if( tokenCount >= 4 )
        h->setRangeX( parameter( tokens[2] ), parameter( tokens[3] ) );
    if( tokenCount >= 6 )
        h->setRangeY( parameter( tokens[4] ), parameter( tokens[5] ) );

    m_enhancedHandles.append( h );

    m_handles.clear();
    foreach( KoEnhancedPathHandle *handle, m_enhancedHandles )
        m_handles.append( handle->position( this ) );
}

void KoEnhancedPathShape::addModifiers( const QString &modifiers )
{
    if( modifiers.isEmpty() )
        return;

    QStringList tokens = modifiers.simplified().split( ' ' );
    int tokenCount = tokens.count();
    for( int i = 0; i < tokenCount; ++i )
       m_modifiers.append( tokens[i].toDouble() );
}

void KoEnhancedPathShape::addCommand( const QString &command )
{
    if( command.isEmpty() )
        return;

    QStringList tokens = command.simplified().split( ' ' );
    int tokenCount = tokens.count();
    if( ! tokenCount )
        return;

    if( tokens[0].length() != 1 )
        return;

    KoEnhancedPathCommand * cmd = new KoEnhancedPathCommand( tokens[0][0] );

    for( int i = 1; i < tokenCount; ++i )
        cmd->addParameter( parameter( tokens[i] ) );

    m_commands.append( cmd );

    updatePath( size() );
}
