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

#include "format.h"
#include "kword_doc.h"
#include "defs.h"
#include "font.h"
#include "kword_utils.h"

#include <kdebug.h>
#include <strstream>
#include <fstream>
#include <unistd.h>

/******************************************************************/
/* Class: KWFormat                                                */
/******************************************************************/

/*================================================================*/
KWFormat::KWFormat( KWordDocument *_doc, const QColor& _color,
                    KWUserFont *_font, int _font_size, int _weight,
                    char _italic, char _underline,
                    VertAlign _vertAlign )
{
    doc = _doc;
    color = _color;
    userFont = _font;
    ptFontSize = _font_size;
    weight = _weight;
    italic = _italic;
    underline = _underline;
    vertAlign = _vertAlign;
    ref = 0;
}

/*================================================================*/
KWFormat::KWFormat( KWordDocument *_doc )
{
    doc = _doc;
    setDefaults( _doc );
    ref = 0;
}

/*================================================================*/
KWFormat::KWFormat( KWordDocument *_doc, const KWFormat &_format )
{
    userFont = _format.getUserFont();
    ptFontSize = _format.getPTFontSize();
    weight = _format.getWeight();
    italic = _format.getItalic();
    underline = _format.getUnderline();
    vertAlign = _format.getVertAlign();
    color = _format.getColor();
    ref = 0;
    doc = _doc;
}

/*================================================================*/
KWFormat& KWFormat::operator=( const KWFormat& _format )
{
    userFont = _format.getUserFont();
    ptFontSize = _format.getPTFontSize();
    weight = _format.getWeight();
    italic = _format.getItalic();
    underline = _format.getUnderline();
    vertAlign = _format.getVertAlign();
    color = _format.getColor();
    ref = 0;
    if ( !doc )
        doc =  _format.doc;

    return *this;
}

/*================================================================*/
bool KWFormat::operator==( const KWFormat & _format ) const
{
    return ( *userFont == *_format.getUserFont() &&
             ptFontSize == _format.getPTFontSize() &&
             weight == _format.getWeight() &&
             italic == _format.getItalic() &&
             underline == _format.getUnderline() &&
             color == _format.getColor() &&
             vertAlign == _format.getVertAlign() );
}

/*================================================================*/
bool KWFormat::operator!=( const KWFormat & _format ) const
{
    return ( *userFont != *_format.getUserFont() ||
             ptFontSize != _format.getPTFontSize() ||
             weight != _format.getWeight() ||
             italic != _format.getItalic() ||
             underline != _format.getUnderline() ||
             color != _format.getColor() ||
             vertAlign != _format.getVertAlign() );
}

/*================================================================*/
void KWFormat::setDefaults( KWordDocument *_doc )
{
    userFont = _doc->getDefaultUserFont();
    ptFontSize = 12;
    weight = QFont::Normal;
    italic = 0;
    underline = 0;
    color = Qt::black;
    vertAlign = VA_NORMAL;
}

/*================================================================*/
KWDisplayFont* KWFormat::loadFont( KWordDocument *_doc )
{
    KWDisplayFont *font = _doc->findDisplayFont( userFont, ptFontSize, weight, italic, underline );
    return font;
}

/*================================================================*/
void KWFormat::apply( const KWFormat &_format )
{
    if ( _format.getUserFont() )
        userFont = _format.getUserFont();

    if ( _format.getPTFontSize() != -1 )
        ptFontSize = _format.getPTFontSize();

    if ( _format.getWeight() != -1 )
        weight = _format.getWeight();

    if ( _format.getItalic() != -1 )
        italic = _format.getItalic();

    if ( _format.getUnderline() != -1 )
        underline = _format.getUnderline();

    if ( _format.getColor().isValid() )
        color = _format.getColor();

    vertAlign = _format.getVertAlign();
}

/*================================================================*/
void KWFormat::decRef()
{
    --ref;
    //QString key = doc->getFormatCollection()->generateKey( this );
    //debug( "dec ref ( %d ): %s", ref, key.data() );

    if ( ref <= 0 && doc ) {
        doc->getFormatCollection()->removeFormat( this );
        return;
    }

    if ( !doc && ref == 0 ) kdWarning() << "RefCount of the format == 0, but I couldn't delete it, "
                                " because I have not a pointer to the document!" << endl;
}

/*================================================================*/
void KWFormat::incRef()
{
    ++ref;
    //QString key = doc->getFormatCollection()->generateKey( this );
    //debug( "inc ref ( %d ): %s", ref, key.data() );
}

/*================================================================*/
void KWFormat::save( QTextStream&out )
{
    out << indent << "<COLOR red=\"" << color.red() << "\" green=\"" << color.green() << "\" blue=\"" << color.blue() << "\"/>" << endl;
    out << indent << "<FONT name=\"" << correctQString( userFont->getFontName() ).latin1() << "\"/>" << endl;
    out << indent << "<SIZE value=\"" << ptFontSize << "\"/>" << endl;
    out << indent << "<WEIGHT value=\"" << weight << "\"/>" << endl;
    out << indent << "<ITALIC value=\"" << static_cast<int>( italic ) << "\"/>" << endl;
    out << indent << "<UNDERLINE value=\"" << static_cast<int>( underline ) << "\"/>" << endl;
    out << indent << "<VERTALIGN value=\"" << static_cast<int>( vertAlign ) << "\"/>" << endl;
}

/*================================================================*/
void KWFormat::load( KOMLParser& parser, QValueList<KOMLAttrib>& lst, KWordDocument *_doc )
{
    doc = _doc;
    ref = 0;

    QString tag;
    QString name;

    while ( parser.open( QString::null, tag ) )
    {
        parser.parseTag( tag, name, lst );

        // color
        if ( name == "COLOR" )
        {
            unsigned int r = 0, g = 0, b = 0;
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "red" )
                {
                    r = ( *it ).m_strValue.toInt();
                    color.setRgb( r, g, b );
                }
                else if ( ( *it ).m_strName == "green" )
                {
                    g = ( *it ).m_strValue.toInt();
                    color.setRgb( r, g, b );
                }
                else if ( ( *it ).m_strName == "blue" )
                {
                    b = ( *it ).m_strValue.toInt();
                    color.setRgb( r, g, b );
                }
            }
        }

        // font
        else if ( name == "FONT" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "name" )
                    userFont = _doc->findUserFont( correctQString( ( *it ).m_strValue ) );
            }
        }

        // font size
        else if ( name == "SIZE" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "value" )
                    ptFontSize = ( *it ).m_strValue.toInt();
            }
        }

        // weight
        else if ( name == "WEIGHT" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "value" )
                    weight = ( *it ).m_strValue.toInt();
            }
        }

        // italic
        else if ( name == "ITALIC" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "value" )
                    italic = ( *it ).m_strValue.toInt();
            }
        }

        // underline
        else if ( name == "UNDERLINE" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "value" )
                    underline = ( *it ).m_strValue.toInt();
            }
        }

        // vertical alignment
        else if ( name == "VERTALIGN" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "value" )
                    vertAlign = static_cast<VertAlign>( ( *it ).m_strValue.toInt() );
            }
        }

        else
            kdError(32001) << "Unknown tag '" << tag <<
                        "' in FORMAT" << endl;

        if ( !parser.close( tag ) )
        {
                kdError(32001) << "Closing " << tag << endl;
            return;
        }
    }
}
