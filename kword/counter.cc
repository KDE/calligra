/* This file is part of the KDE project
   Copyright (C) 2001 Shaheed Haque <srhaque@iee.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "counter.h"
#include "kwtextparag.h"
#include "kwutils.h"
#include <kdebug.h>
#include <qdom.h>

static KWTextParag * const INVALID_PARAG = (KWTextParag *)-1;

Counter::Counter()
{
    m_numbering = NUM_NONE;
    m_style = STYLE_NONE;
    m_depth = 0;
    m_startNumber = 1;
    m_prefix = QString::null;
    m_suffix = '.';
    m_customBullet.character = QChar( '-' );
    m_customBullet.font = QString::null;
    invalidate();
}

bool Counter::operator==( const Counter & c2 ) const
{
    // ## This is kinda wrong. Unused fields (depending on the counter style) shouldn't be compared.
    return (m_numbering==c2.m_numbering &&
            m_style==c2.m_style &&
            m_depth==c2.m_depth &&
            m_startNumber==c2.m_startNumber &&
            m_prefix==c2.m_prefix &&
            m_suffix==c2.m_suffix &&
            m_customBullet.character==c2.m_customBullet.character &&
            m_customBullet.font==c2.m_customBullet.font &&
            m_custom==c2.m_custom);

}

QString Counter::custom() const
{
    return m_custom;
}

QChar Counter::customBulletCharacter() const
{
    return m_customBullet.character;
}

QString Counter::customBulletFont() const
{
    return m_customBullet.font;
}

unsigned int Counter::depth() const
{
    return m_depth;
}

void Counter::invalidate()
{
    m_cache.number = -1;
    m_cache.text = QString::null;
    m_cache.width = -1;
    m_cache.parent = INVALID_PARAG;
    m_cache.counterFormat = 0;
}

bool Counter::isBullet() const
{
    switch ( style() )
    {
    case STYLE_DISCBULLET:
    case STYLE_SQUAREBULLET:
    case STYLE_CIRCLEBULLET:
    case STYLE_CUSTOMBULLET:
        return true;
    default:
        return false;
    }
}

void Counter::load( QDomElement & element )
{
    m_numbering = static_cast<Numbering>( element.attribute("numberingtype", "2").toInt() );
    m_style = static_cast<Style>( element.attribute("type").toInt() );
    // Old docs have this:
    if ( m_numbering == NUM_LIST && m_style == STYLE_NONE )
        m_numbering = NUM_NONE;
    m_depth = element.attribute("depth").toInt();
    m_customBullet.character = QChar( element.attribute("bullet").toInt() );
    m_prefix = correctQString( element.attribute("lefttext") );
    m_suffix = correctQString( element.attribute("righttext") );
    QString s = element.attribute("start");
    if ( s.isEmpty() )
        m_startNumber = 1;
    else if ( s[0].isDigit() )
        m_startNumber = s.toInt();
    else // support for very-old files
        m_startNumber = s.lower()[0].latin1() - 'a' + 1;
    m_customBullet.font = correctQString( element.attribute("bulletfont") );
    m_custom = correctQString( element.attribute("customdef") );
    invalidate();
}

int Counter::number( const KWTextParag *paragraph )
{
    // Return cached value if possible.
    if ( m_cache.number != -1 )
        return m_cache.number;

    // Go looking for another paragraph at the same level or higher level.
    KWTextParag *otherParagraph = static_cast<KWTextParag *>( paragraph->prev() );
    Counter *otherCounter;

    switch ( m_numbering )
    {
    case NUM_NONE:
        // This should not occur!
        m_cache.number = 0;
        break;
    case NUM_CHAPTER:
        m_cache.number = m_startNumber;
        // Go upwards while...
        while ( otherParagraph )
        {
            otherCounter = otherParagraph->counter();
            if ( otherCounter &&                                        // ...numbered paragraphs.
                ( otherCounter->m_numbering == NUM_CHAPTER ) &&         // ...same number type.
                ( otherCounter->m_depth <= m_depth ) )        // ...same or higher level.
            {
                if ( ( otherCounter->m_depth == m_depth ) &&
                   ( otherCounter->m_style == m_style ) )
                {
                    // Found a preceeding paragraph of exactly our type!
                    m_cache.number = otherCounter->number( otherParagraph ) + 1;
                }
                else
                {
                    // Found a preceeding paragraph of higher level!
                    m_cache.number = m_startNumber;
                }
                break;
            }
            otherParagraph = static_cast<KWTextParag *>( otherParagraph->prev() );
        }
        break;
    case NUM_LIST:
        m_cache.number = m_startNumber;
        // Go upwards while...
        while ( otherParagraph )
        {
            otherCounter = otherParagraph->counter();
            if ( otherCounter )                                         // ...numbered paragraphs.
            {
                if ( ( otherCounter->m_numbering == NUM_LIST ) &&       // ...same number type.
                    ( otherCounter->m_depth <= m_depth ) )    // ...same or higher level.
                {
                    if ( ( otherCounter->m_depth == m_depth ) &&
                       ( otherCounter->m_style == m_style ) )
                    {
                        // Found a preceeding paragraph of exactly our type!
                        m_cache.number = otherCounter->number( otherParagraph ) + 1;
                    }
                    else
                    {
                        // Found a preceeding paragraph of higher level!
                        m_cache.number = m_startNumber;
                    }
                    break;
                }
                else
                if ( otherCounter->m_numbering == NUM_CHAPTER )        // ...heading number type.
                {
                    m_cache.number = m_startNumber;
                    break;
                }
            }
            else
            {
                // There is no counter at all.
                m_cache.number = m_startNumber;
                break;
            }
            otherParagraph = static_cast<KWTextParag *>( otherParagraph->prev() );
        }
        break;
    }
    return m_cache.number;
}

Counter::Numbering Counter::numbering() const
{
    return m_numbering;
}

// Go looking for another paragraph at a higher level.
KWTextParag *Counter::parent( const KWTextParag *paragraph )
{
    // Return cached value if possible.
    if ( m_cache.parent != INVALID_PARAG )
        return m_cache.parent;

    KWTextParag *otherParagraph = static_cast<KWTextParag *>( paragraph->prev() );
    Counter *otherCounter;

    switch ( m_numbering )
    {
    case NUM_NONE:
        // This should not occur!
        otherParagraph = 0L;
        break;
    case NUM_CHAPTER:
        // Go upwards while...
        while ( otherParagraph )
        {
            otherCounter = otherParagraph->counter();
            if ( otherCounter &&                                        // ...numbered paragraphs.
                ( otherCounter->m_numbering == NUM_CHAPTER ) &&         // ...same number type.
                ( otherCounter->m_depth < m_depth ) )         // ...higher level.
            {
                break;
            }
            otherParagraph = static_cast<KWTextParag *>( otherParagraph->prev() );
        }
        break;
    case NUM_LIST:
        // Go upwards while...
        while ( otherParagraph )
        {
            otherCounter = otherParagraph->counter();
            if ( otherCounter )                                         // ...numbered paragraphs.
            {
                if ( ( otherCounter->m_numbering == NUM_LIST ) &&       // ...same number type.
                    ( otherCounter->m_depth < m_depth ) )     // ...higher level.
                {
                    break;
                }
                else
                if ( otherCounter->m_numbering == NUM_CHAPTER )         // ...heading number type.
                {
                    otherParagraph = 0L;
                    break;
                }
            }
            otherParagraph = static_cast<KWTextParag *>( otherParagraph->prev() );
        }
        break;
    }
    m_cache.parent = otherParagraph;
    return m_cache.parent;
}

QString Counter::prefix() const
{
    return m_prefix;
}

void Counter::save( QDomElement & element )
{
    element.setAttribute( "type", static_cast<int>( m_style ) );
    element.setAttribute( "depth", m_depth );
    if ( m_style == STYLE_CUSTOMBULLET )
    {
        element.setAttribute( "bullet", m_customBullet.character.unicode() );
        if ( !m_customBullet.font.isEmpty() )
            element.setAttribute( "bulletfont", m_customBullet.font );
    }
    if ( !m_prefix.isEmpty() )
        element.setAttribute( "lefttext", m_prefix );
    if ( !m_suffix.isEmpty() )
        element.setAttribute( "righttext", m_suffix );
    if ( !m_startNumber == 1 )
        element.setAttribute( "start", m_startNumber );
    if ( m_numbering != NUM_NONE )
        element.setAttribute( "numberingtype", static_cast<int>( m_numbering ) );
    if ( !m_custom.isEmpty() )
        element.setAttribute( "customdef", m_custom );
}

void Counter::setCustom( QString c )
{
    m_custom = c;
    invalidate();
}

void Counter::setCustomBulletCharacter( QChar c )
{
    m_customBullet.character = c;
    invalidate();
}

void Counter::setCustomBulletFont( QString f )
{
    m_customBullet.font = f;
    invalidate();
}

void Counter::setDepth( unsigned int d )
{
    m_depth = d;
    invalidate();
}

void Counter::setNumbering( Numbering n )
{
    m_numbering = n;
    invalidate();
}

void Counter::setPrefix( QString p )
{
    m_prefix = p;
    invalidate();
}
void Counter::setStartNumber( int s )
{
    m_startNumber = s;
    invalidate();
}

void Counter::setStyle( Style s )
{
    m_style = s;
    invalidate();
}

void Counter::setSuffix( QString s )
{
    m_suffix = s;
    invalidate();
}

int Counter::startNumber() const
{
    return m_startNumber;
}

Counter::Style Counter::style() const
{
    switch ( m_style )
    {
    case STYLE_DISCBULLET:
    case STYLE_SQUAREBULLET:
    case STYLE_CIRCLEBULLET:
    case STYLE_CUSTOMBULLET:
        if ( m_numbering == NUM_CHAPTER )
        {
            // Shome mishtake surely!
            return STYLE_NUM;
        }
        break;
    default:
        break;
    }
    return m_style;
}

QString Counter::suffix() const
{
    return m_suffix;
}

QString Counter::text( const KWTextParag *paragraph )
{
    // Return cached value if possible.
    if ( !m_cache.text.isNull() )
        return m_cache.text;

    // Recurse to find the text of the preceeding level.
    if ( parent( paragraph ) )
    {
        m_cache.text = m_cache.parent->counter()->text( m_cache.parent );

        // If the preceeding level is a bullet, replace it with blanks.
        if ( m_cache.parent->counter()->isBullet() )
            for ( unsigned i = 0; i < m_cache.text.length(); i++ )
                m_cache.text.at( i ) = ' ';
    }

    // Ensure paragraph number is valid.
    number( paragraph );

    // Now convert to text.
    QString tmp;
    switch ( style() )
    {
    case STYLE_NONE:
        if ( m_numbering == NUM_LIST )
            tmp = ' ';
        break;
    case STYLE_NUM:
        tmp.setNum( m_cache.number );
        break;
    case STYLE_ALPHAB_L:
        tmp=makeAlphaLowerNumber( m_cache.number );
        break;
    case STYLE_ALPHAB_U:
        tmp=makeAlphaUpperNumber( m_cache.number );
        break;
    case STYLE_ROM_NUM_L:
        tmp = makeRomanNumber( m_cache.number ).lower();
        break;
    case STYLE_ROM_NUM_U:
        tmp = makeRomanNumber( m_cache.number ).upper();
        break;
    case STYLE_CUSTOM:
        ////// TODO
        tmp.setNum( m_cache.number );
        break;
    case Counter::STYLE_DISCBULLET:
    case Counter::STYLE_SQUAREBULLET:
    case Counter::STYLE_CIRCLEBULLET:
    case Counter::STYLE_CUSTOMBULLET:
        // Allow space for bullet!
        tmp = ' ';
        break;
    }
    tmp.prepend( prefix() );
    tmp.append( suffix() );

    // Find the number of missing parents, and add dummy text for them.
    int missingParents;
    if ( parent( paragraph ) )
    {
        missingParents = m_depth - m_cache.parent->counter()->m_depth - 1;
    }
    else
    {
        missingParents = m_depth;
    }
    while ( missingParents > 0 )
    {
        // Each missing level adds a "0." prefix.
        tmp.prepend( "0." );
        missingParents--;
    }
    m_cache.text.append( tmp );
    return m_cache.text;
}

int Counter::width( const KWTextParag *paragraph )
{
    // Return cached value if possible.
    if ( m_cache.width != -1 && paragraph->at( 0 )->format() == m_cache.counterFormat )
        return m_cache.width;

    // Ensure paragraph text is valid.
    if ( m_cache.text.isNull() )
        text( paragraph );

    // Now calculate width.
    m_cache.counterFormat = paragraph->at( 0 )->format();/*paragraph->paragFormat()*/;
    m_cache.width = 0;
    QString text = m_cache.text;
    if ( !text.isEmpty() )
        text.append( ' ' ); // append a trailing space, see KWTextParag::drawLabel
    for ( unsigned int i = 0; i < text.length(); i++ )
        m_cache.width += m_cache.counterFormat->width( text, i );
    //kdDebug() << "Counter::width recalculated parag=" << paragraph << " text='" << text << "' width=" << m_cache.width << endl;
    return m_cache.width;
}
