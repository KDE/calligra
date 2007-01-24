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

KoEnhancedPathShape::KoEnhancedPathShape( const QRectF &viewBox )
: m_viewBox( viewBox ), m_viewBoxOffset( 0.0, 0.0 )
{
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
        handle->setPosition( shapeToViewbox( point ), this );
        evaluateHandles();
    }
}

void KoEnhancedPathShape::updatePath( const QSizeF & )
{
    clear();

    foreach( KoEnhancedPathCommand *cmd, m_commands )
        cmd->execute( this );

    normalize();
}

void KoEnhancedPathShape::resize( const QSizeF &newSize )
{
    QSizeF oldSize = size();

    KoParameterShape::resize( newSize );

    double scaleX = newSize.width() / oldSize.width();
    double scaleY = newSize.height() / oldSize.height();
    m_viewBoxOffset.rx() *= scaleX;
    m_viewBoxOffset.ry() *= scaleY;
    m_viewMatrix.scale( scaleX, scaleY );
}


QPointF KoEnhancedPathShape::normalize()
{
    QPointF offset = KoPathShape::normalize();
    m_viewBoxOffset -= offset;

    return offset;
}

void KoEnhancedPathShape::evaluateHandles()
{
    if( m_handles.size() != m_enhancedHandles.size() )
    {
        m_handles.clear();
        uint handleCount = m_enhancedHandles.size();
        for( uint i = 0; i < handleCount; ++i )
            m_handles.append( viewboxToShape( m_enhancedHandles[i]->position( this ) ) );
    }
    else
    {
        uint handleCount = m_enhancedHandles.size();
        for( uint i = 0; i < handleCount; ++i )
            m_handles[i] = viewboxToShape( m_enhancedHandles[i]->position( this ) );
    }
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

void KoEnhancedPathShape::addHandle( const QMap<QString,QVariant> &handle )
{
    if( handle.isEmpty() )
        return;

    KoEnhancedPathHandle *newHandle = 0;

    if( ! handle.contains( "draw:handle-position" ) )
        return;
    QVariant position = handle.value("draw:handle-position");

    QStringList tokens = position.toString().simplified().split( ' ' );
    if( tokens.count() < 2 )
        return;

    newHandle = new KoEnhancedPathHandle( parameter( tokens[0] ), parameter( tokens[1] ) );

    // check if we have a polar handle
    if( handle.contains( "draw:handle-polar" ) )
    {
        QVariant polar = handle.value( "draw:handle-polar" );
        QStringList tokens = polar.toString().simplified().split( ' ' );
        if( tokens.count() == 2 )
        {
            newHandle->setPolarCenter( parameter( tokens[0] ), parameter( tokens[1] ) );

            QVariant minRadius = handle.value( "draw:handle-radius-range-minimum" );
            QVariant maxRadius = handle.value( "draw:handle-radius-range-maximum" );
            if( minRadius.isValid() && maxRadius.isValid() )
                newHandle->setRadiusRange( parameter( minRadius.toString() ), parameter( maxRadius.toString() ) );
        }
    }
    else
    {
        QVariant minX = handle.value( "draw:handle-range-x-minimum" );
        QVariant maxX = handle.value( "draw:handle-range-x-maximum" );
        if( minX.isValid() && maxX.isValid() )
            newHandle->setRangeX( parameter( minX.toString() ), parameter( maxX.toString() ) );

        QVariant minY = handle.value( "draw:handle-range-y-minimum" );
        QVariant maxY = handle.value( "draw:handle-range-y-maximum" );
        if( minY.isValid() && maxY.isValid() )
            newHandle->setRangeY( parameter( minY.toString() ), parameter( maxY.toString() ) );
    }

    if( ! newHandle )
        return;

    m_enhancedHandles.append( newHandle );

    evaluateHandles();
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

const QRectF & KoEnhancedPathShape::viewBox() const
{
    return m_viewBox;
}

QPointF KoEnhancedPathShape::shapeToViewbox( const QPointF & point ) const
{
    return m_viewMatrix.inverted().map( point-m_viewBoxOffset );
}

QPointF KoEnhancedPathShape::viewboxToShape( const QPointF & point ) const
{
    return m_viewMatrix.map( point ) + m_viewBoxOffset;
}

double KoEnhancedPathShape::shapeToViewbox( double value ) const
{
    return m_viewMatrix.inverted().map( QPointF( value, value ) ).x();
}

double KoEnhancedPathShape::viewboxToShape( double value ) const
{
    return m_viewMatrix.map( QPointF( value, value ) ).x();
}
