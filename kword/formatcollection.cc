/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "formatcollection.h"
#include "kword_doc.h"
#include "font.h"

#include <qcolor.h>
#include <qfont.h>

/******************************************************************/
/* Class: KWFormatCollection                                      */
/******************************************************************/

/*================================================================*/
KWFormatCollection::KWFormatCollection( KWordDocument *_doc )
    : formats( 1999, true )
{
    formats.setAutoDelete( true );
    doc = _doc;
}

/*================================================================*/
KWFormatCollection::~KWFormatCollection()
{
    formats.clear();
}

/*================================================================*/
KWFormat *KWFormatCollection::getFormat( const KWFormat &_format )
{
    QString key = generateKey( _format );

    KWFormat *format = findFormat( key );
    if ( format )
    {
        format->incRef();
        return format;
    }
    else
        return insertFormat( key, _format );
}

/*================================================================*/
void KWFormatCollection::removeFormat( KWFormat *_format )
{
    QString key = generateKey( *_format );

    formats.remove( key );
}

/*================================================================*/
QString KWFormatCollection::generateKey( const KWFormat &_format )
{
    QString key;

    // Key: BIU-Fontname-Fontsize-red-gree-blue
    // e.g. B**-Times-12-255-40-32
    key.sprintf( "%c%c%c-%s-%d-%d-%d-%d-%d",
                 ( _format.getWeight() == QFont::Bold ? 'B' : '*' ),
                 ( _format.getItalic() == 1 ? 'I' : '*' ),
                 ( _format.getUnderline() == 1 ? 'U' : '*' ),
                 _format.getUserFont()->getFontName().data(),
                 _format.getPTFontSize(), _format.getColor().red(),
                 _format.getColor().green(), _format.getColor().blue(),
                 _format.getVertAlign() );

    return key;
}

/*================================================================*/
KWFormat *KWFormatCollection::findFormat( QString _key )
{
    return formats.find( _key.data() );
}

/*================================================================*/
KWFormat *KWFormatCollection::insertFormat( QString _key, const KWFormat &_format )
{
    KWFormat *format = new KWFormat( doc, _format );

    formats.insert( _key.data(), format );
    format->incRef();

    return format;
}


