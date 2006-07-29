/***************************************************************************
*   Copyright (C) 2005 by Adam Treat                                      *
*   treat@kde.org                                                         *
*                                                                         *
*   Copyright (C) 2000 Trolltech AS.  All rights reserved.                *
*   info@trolltech.com                                                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU Library General Public License as       *
*   published by the Free Software Foundation; either version 2 of the    *
*   License, or (at your option) any later version.                       *
*                                                                         *
***************************************************************************/

#include "inputmask.h"

namespace Kugar
{

InputMask::InputMask( QObject *parent, const char *name )
        : QObject( parent, name ),
        m_text( QString::null ),
        m_maxLength( 32767 ),
        m_blank( ' ' ),
        m_mask( QString::null ),
        m_maskData( 0L )
{}

InputMask::~InputMask()
{
    delete [] m_maskData;
}

QString InputMask::mask() const
{
    return ( m_maskData ? m_mask + ';' + m_blank : QString::null );
}

void InputMask::setMask( const QString &mask )
{
    parseInputMask( mask );
}

QString InputMask::formatText( const QString &txt )
{
    return maskString( 0, txt, true );
}

void InputMask::parseInputMask( const QString &maskFields )
{
    if ( maskFields.isEmpty() || maskFields.section( ';', 0, 0 ).isEmpty() )
    {
        if ( m_maskData )
        {
            delete [] m_maskData;
            m_maskData = 0;
            m_maxLength = 32767;
        }
        return ;
    }

    m_mask = maskFields.section( ';', 0, 0 );
    m_blank = maskFields.section( ';', 1, 1 ).at( 0 );
    if ( m_blank.isNull() )
        m_blank = ' ';

    // calculate m_maxLength / m_maskData length
    m_maxLength = 0;
    QChar c = 0;
    uint i;
    for ( i = 0; i < m_mask.length(); i++ )
    {
        c = m_mask.at( i );
        if ( i > 0 && m_mask.at( i - 1 ) == '\\' )
        {
            m_maxLength++;
            continue;
        }
        if ( c != '\\' && c != '!' &&
                c != '<' && c != '>' &&
                c != '{' && c != '}' &&
                c != '[' && c != ']' )
            m_maxLength++;
    }

    delete [] m_maskData;
    m_maskData = new MaskInputData[ m_maxLength ];

    MaskInputData::Casemode m = MaskInputData::NoCaseMode;
    c = 0;
    bool s;
    bool escape = FALSE;
    int index = 0;
    for ( i = 0; i < m_mask.length(); i++ )
    {
        c = m_mask.at( i );
        if ( escape )
        {
            s = TRUE;
            m_maskData[ index ].maskChar = c;
            m_maskData[ index ].separator = s;
            m_maskData[ index ].caseMode = m;
            index++;
            escape = FALSE;
        }
        else if ( c == '<' || c == '>' || c == '!' )
        {
            switch ( c )
            {
            case '<':
                m = MaskInputData::Lower;
                break;
            case '>':
                m = MaskInputData::Upper;
                break;
            case '!':
                m = MaskInputData::NoCaseMode;
                break;
            }
        }
        else if ( c != '{' && c != '}' && c != '[' && c != ']' )
        {
            switch ( c )
            {
            case 'A':
            case 'a':
            case 'N':
            case 'n':
            case 'X':
            case 'x':
            case '9':
            case '0':
            case 'D':
            case 'd':
            case '#':
                s = FALSE;
                break;
            case '\\':
                escape = TRUE;
            default:
                s = TRUE;
                break;
            }

            if ( !escape )
            {
                m_maskData[ index ].maskChar = c;
                m_maskData[ index ].separator = s;
                m_maskData[ index ].caseMode = m;
                index++;
            }
        }
    }
}

bool InputMask::isValidInput( QChar key, QChar mask ) const
{
    switch ( mask )
    {
    case 'A':
        if ( key.isLetter() && key != m_blank )
            return TRUE;
        break;
    case 'a':
        if ( key.isLetter() || key == m_blank )
            return TRUE;
        break;
    case 'N':
        if ( key.isLetterOrNumber() && key != m_blank )
            return TRUE;
        break;
    case 'n':
        if ( key.isLetterOrNumber() || key == m_blank )
            return TRUE;
        break;
    case 'X':
        if ( key.isPrint() && key != m_blank )
            return TRUE;
        break;
    case 'x':
        if ( key.isPrint() || key == m_blank )
            return TRUE;
        break;
    case '9':
        if ( key.isNumber() && key != m_blank )
            return TRUE;
        break;
    case '0':
        if ( key.isNumber() || key == m_blank )
            return TRUE;
        break;
    case 'D':
        if ( key.isNumber() && key.digitValue() > 0 && key != m_blank )
            return TRUE;
        break;
    case 'd':
        if ( ( key.isNumber() && key.digitValue() > 0 ) || key == m_blank )
            return TRUE;
        break;
    case '#':
        if ( key.isNumber() || key == '+' || key == '-' || key == m_blank )
            return TRUE;
        break;
    default:
        break;
    }
    return FALSE;
}

QString InputMask::maskString( uint pos, const QString &str, bool clear ) const
{
    if ( pos >= ( uint ) m_maxLength )
        return QString::fromLatin1( "" );

    QString fill;
    fill = clear ? clearString( 0, m_maxLength ) : m_text;

    uint strIndex = 0;
    QString s = QString::fromLatin1( "" );
    int i = pos;
    while ( i < m_maxLength )
    {
        if ( strIndex < str.length() )
        {
            if ( m_maskData[ i ].separator )
            {
                s += m_maskData[ i ].maskChar;
                if ( str[ ( int ) strIndex ] == m_maskData[ i ].maskChar )
                    strIndex++;
                ++i;
            }
            else
            {
                if ( isValidInput( str[ ( int ) strIndex ], m_maskData[ i ].maskChar ) )
                {
                    switch ( m_maskData[ i ].caseMode )
                    {
                    case MaskInputData::Upper:
                        s += str[ ( int ) strIndex ].upper();
                        break;
                    case MaskInputData::Lower:
                        s += str[ ( int ) strIndex ].lower();
                        break;
                    default:
                        s += str[ ( int ) strIndex ];
                    }
                    ++i;
                }
                else
                {
                    // search for separator first
                    int n = findInMask( i, TRUE, TRUE, str[ ( int ) strIndex ] );
                    if ( n != -1 )
                    {
                        if ( str.length() != 1 || i == 0 || ( i > 0 && ( !m_maskData[ i - 1 ].separator || m_maskData[ i - 1 ].maskChar != str[ ( int ) strIndex ] ) ) )
                        {
                            s += fill.mid( i, n - i + 1 );
                            i = n + 1; // update i to find + 1
                        }
                    }
                    else
                    {
                        // search for valid m_blank if not
                        n = findInMask( i, TRUE, FALSE, str[ ( int ) strIndex ] );
                        if ( n != -1 )
                        {
                            s += fill.mid( i, n - i );
                            switch ( m_maskData[ n ].caseMode )
                            {
                            case MaskInputData::Upper:
                                s += str[ ( int ) strIndex ].upper();
                                break;
                            case MaskInputData::Lower:
                                s += str[ ( int ) strIndex ].lower();
                                break;
                            default:
                                s += str[ ( int ) strIndex ];
                            }
                            i = n + 1; // updates i to find + 1
                        }
                    }
                }
                strIndex++;
            }
        }
        else
            break;
    }

    return s;
}

QString InputMask::clearString( uint pos, uint len ) const
{
    if ( pos >= ( uint ) m_maxLength )
        return QString::null;

    QString s;
    int end = QMIN( ( uint ) m_maxLength, pos + len );
    for ( int i = pos; i < end; i++ )
        if ( m_maskData[ i ].separator )
            s += m_maskData[ i ].maskChar;
        else
            s += m_blank;

    return s;
}

QString InputMask::stripString( const QString &str ) const
{
    if ( !m_maskData )
        return str;

    QString s;
    int end = QMIN( m_maxLength, ( int ) str.length() );
    for ( int i = 0; i < end; i++ )
        if ( m_maskData[ i ].separator )
            s += m_maskData[ i ].maskChar;
        else
            if ( str[ i ] != m_blank )
                s += str[ i ];

    return s;
}

int InputMask::findInMask( int pos, bool forward, bool findSeparator, QChar searchChar ) const
{
    if ( pos >= m_maxLength || pos < 0 )
        return -1;

    int end = forward ? m_maxLength : -1;
    int step = forward ? 1 : -1;
    int i = pos;

    while ( i != end )
    {
        if ( findSeparator )
        {
            if ( m_maskData[ i ].separator && m_maskData[ i ].maskChar == searchChar )
                return i;
        }
        else
        {
            if ( !m_maskData[ i ].separator )
            {
                if ( searchChar.isNull() )
                    return i;
                else if ( isValidInput( searchChar, m_maskData[ i ].maskChar ) )
                    return i;
            }
        }
        i += step;
    }
    return -1;
}

}

#include "inputmask.moc"
