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

#ifndef USERFONT_H
#define USERFONT_H

class KWUserFont;
class KWDisplayFont;
class KWordDocument;

#include "defs.h"
#include "kword_doc.h"

#include <qfont.h>
#include <qfontmetrics.h>

/******************************************************************/
/* Class: KWUserFont						  */
/******************************************************************/

class KWUserFont
{
public:
    KWUserFont( KWordDocument *_doc, const QString& _name );
    ~KWUserFont();

    QString getFontName() const { return fontName; }

    bool operator==( const KWUserFont &_font ) const
    { return _font.fontName == fontName; }
    bool operator!=( const KWUserFont &_font ) const
    { return _font.fontName != fontName; }

protected:
    QString fontName;

    KWordDocument *document;

};

/******************************************************************/
/* Class: KWDisplayFont						  */
/******************************************************************/

class KWDisplayFont : public QFont
{
public:
    KWDisplayFont( KWordDocument *_doc, KWUserFont *_font, unsigned int _size,
		   int _weight, bool _italic, bool _underline );
    ~KWDisplayFont();

    KWUserFont* getUserFont() const;
    unsigned int getPTSize() const;

    unsigned int getPTAscender() const;
    unsigned int getPTDescender() const;

    unsigned int getPTWidth( QString _text ) const;
    unsigned int getPTWidth( const QChar &_c ) const;

    void setPTSize( int _size );
    void setWeight( int _weight );
    void setItalic( bool _italic );
    void setUnderline( bool _underline );

    void updateZoom();
    
protected:
    QFontMetrics fm;
    unsigned int ptSize;
    int widths[ 65536 ];
    int asc, desc;
    KWUserFont *userFont;
    KWordDocument *document;
    int lastZoom;
    
};

inline void KWDisplayFont::setPTSize( int _size )
{
    ptSize = _size;
    setPointSize( _size );
    fm = QFontMetrics( *this );
    for ( int i = 0; i < 65536; ++i )
	widths[ i ] = 0;
    asc = fm.ascent();
    desc = fm.descent();
}

inline void KWDisplayFont::setWeight( int _weight )
{
    QFont::setWeight( _weight );
    fm = QFontMetrics( *this );
    for ( int i = 0; i < 65536; ++i )
	widths[ i ] = 0;
    asc = fm.ascent();
    desc = fm.descent();
}

inline void KWDisplayFont::setItalic( bool _italic )
{
    QFont::setItalic( _italic );
    fm = QFontMetrics( *this );
    for ( int i = 0; i < 65536; ++i )
	widths[ i ] = 0;
    asc = fm.ascent();
    desc = fm.descent();
}

inline void KWDisplayFont::setUnderline( bool _underline )
{
    QFont::setUnderline( _underline );
    fm = QFontMetrics( *this );
    for ( int i = 0; i < 65536; ++i )
	widths[ i ] = 0;
    asc = fm.ascent();
    desc = fm.descent();
}

inline unsigned int KWDisplayFont::getPTWidth( QString _text ) const
{
    ( (KWDisplayFont*)this )->updateZoom();
    return fm.width( _text );
}

inline unsigned int KWDisplayFont::getPTWidth( const QChar &c ) const
{
    ( (KWDisplayFont*)this )->updateZoom();
    int w = widths[ c.unicode() ];
    if ( w == 0 ) {
	w = fm.width( c );
	( (KWDisplayFont*)this )->widths[ c.unicode() ] = w;
    }
    return w;
}

inline KWUserFont* KWDisplayFont::getUserFont() const
{
    return userFont;
}

inline unsigned int KWDisplayFont::getPTSize() const
{
    return document->zoomIt( ptSize );
}

inline unsigned int KWDisplayFont::getPTAscender() const
{
    return document->zoomIt( asc );
}

inline unsigned int KWDisplayFont::getPTDescender() const
{
    return document->zoomIt( desc + 2 );
}

inline void KWDisplayFont::updateZoom()
{
    if ( lastZoom == document->getZoom() )
	return;
    QFont f( *this );
    f.setPointSize( document->zoomIt( pointSize() ) );
    fm = QFontMetrics( f );
    for ( int i = 0; i < 65536; ++i )
	widths[ i ] = 0;
    lastZoom = document->getZoom();
}

#endif


