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

#include <qfont.h>
#include <qfontmetrics.h>

/******************************************************************/
/* Class: KWUserFont						  */
/******************************************************************/

class KWUserFont
{
public:
    KWUserFont( KWordDocument *_doc, QString _name );
    ~KWUserFont();

    QString getFontName() { return fontName; }

    bool operator==( const KWUserFont &_font )
    { return _font.fontName == fontName; }
    bool operator!=( const KWUserFont &_font )
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

    void scaleFont();

    static void scaleAllFonts();

    KWUserFont* getUserFont() { return userFont; }
    unsigned int getPTSize() { return ptSize; }

    unsigned int getPTAscender() { return fm.ascent(); }
    unsigned int getPTDescender() { return fm.descent() + 2; }

    unsigned int getPTWidth( QString _text );
    unsigned int getPTWidth( char &_c );
    unsigned int getPTWidth( QChar &_c );

    void setPTSize( int _size );
    void setWeight( int _weight );
    void setItalic( bool _italic );
    void setUnderline( bool _underline );

protected:
    QFontMetrics fm;

    unsigned int ptSize;

    /**
     * Pointer to the user font family this font belongs to.
     */
    KWUserFont *userFont;
    /**
     * Poiner to the document this font belongs to.
     */
    KWordDocument *document;

};

#endif


