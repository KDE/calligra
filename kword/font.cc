/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>, Torben Weis <weis@kde.org>

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

char s_FONT_CCId[]="$Id$";

#include "font.h"
#include "kword_doc.h"

#include <qpainter.h>
#include <qlist.h>
#include <qwidget.h>
#include <qstring.h>

/******************************************************************/
/* Class: KWString                                                */
/******************************************************************/

/*================================================================*/
KWUserFont::KWUserFont( KWordDocument *_doc, QString _name )
{
    fontName = qstrdup( _name );

    document = _doc;
    document->userFontList.append( this );
}

/*================================================================*/
KWUserFont::~KWUserFont()
{
    document->userFontList.setAutoDelete( true );
    document->userFontList.removeRef( this );
    document->userFontList.setAutoDelete( false );
}

/******************************************************************/
/* Class: KWString                                                */
/******************************************************************/

/*================================================================*/
KWDisplayFont::KWDisplayFont( KWordDocument *_doc, KWUserFont *_font, unsigned int _size,
                              int _weight, bool _italic, bool _underline )
    : QFont( _font->getFontName(), ZOOM( _size ), _weight, _italic ), fm( *this )
{
    setUnderline( _underline );

    document = _doc;
    document->displayFontList.append( this );

    userFont = _font;

    fm = QFontMetrics( *this );

    ptSize = _size;

    if ( ZOOM( 100 ) != 100 )
        scaleFont();
}

/*================================================================*/
void KWDisplayFont::setPTSize( int _size )
{
    ptSize = _size;
    setPointSize( _size );
    fm = QFontMetrics( *this );
}

/*================================================================*/
void KWDisplayFont::setWeight( int _weight )
{
    QFont::setWeight( _weight );
    fm = QFontMetrics( *this );
}

/*================================================================*/
void KWDisplayFont::setItalic( bool _italic )
{
    QFont::setItalic( _italic );
    fm = QFontMetrics( *this );
}

/*================================================================*/
void KWDisplayFont::setUnderline( bool _underline )
{
    QFont::setUnderline( _underline );
    fm = QFontMetrics( *this );
}

/*================================================================*/
void KWDisplayFont::scaleFont()
{
    setPointSize( ZOOM( ptSize ) );
    fm = QFontMetrics( *this );
}

/*================================================================*/
unsigned int KWDisplayFont::getPTWidth( QString _text )
{
    return fm.width( _text );
}

/*================================================================*/
unsigned int KWDisplayFont::getPTWidth( char &_c )
{
    return fm.width( _c );
}

/*================================================================*/
unsigned int KWDisplayFont::getPTWidth( QChar &_c )
{
    return fm.width( _c );
}

/*================================================================*/
KWDisplayFont::~KWDisplayFont()
{
    document->displayFontList.setAutoDelete( true );
    document->displayFontList.removeRef( this );
    document->displayFontList.setAutoDelete( false );
}

