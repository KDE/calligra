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
#include <kcharsets.h>

#include <qpainter.h>
#include <qlist.h>
#include <qwidget.h>
#include <qstring.h>

/******************************************************************/
/* Class: KWString                                                */
/******************************************************************/

/*================================================================*/
KWUserFont::KWUserFont( KWordDocument *_doc, const QString& _name )
{
    fontName = _name;

    document = _doc;
    document->userFontList.append( this );
}

/*================================================================*/
KWUserFont::~KWUserFont()
{
    int index = document->userFontList.findRef(this);
    if (index >= 0)
        document->userFontList.take( index );
}

/******************************************************************/
/* Class: KWString                                                */
/******************************************************************/

/*================================================================*/
KWDisplayFont::KWDisplayFont( KWordDocument *_doc, KWUserFont *_font, unsigned int _size,
                              int _weight, bool _italic, bool _underline )
    : QFont( _font->getFontName(), _size, _weight, _italic ), fm( *this )
{
    KGlobal::charsets()->setQFont(*this, KGlobal::locale()->charset());

    lastZoom = 100;
    setUnderline( _underline );

    document = _doc;
    document->displayFontList.append( this );

    userFont = _font;

    fm = QFontMetrics( *this );

    ptSize = _size;

    for ( int i = 0; i < 65536; ++i )
        widths[ i ] = 0;
    asc = fm.ascent();
    desc = fm.descent();
}

/*================================================================*/
KWDisplayFont::~KWDisplayFont()
{
    int index = document->displayFontList.findRef(this);
    if (index >= 0)
        document->displayFontList.take( index );
}

