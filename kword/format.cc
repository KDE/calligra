#include "format.h"
#include "kword_doc.h"
#include "defs.h"
#include "font.h"
#include "kword_utils.h"

#include <komlMime.h>
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
		    VertAlign _vertAlign, char _math, char _direct )
{
    doc = _doc;
    color = _color;
    userFont = _font;
    ptFontSize = _font_size;
    weight = _weight;
    italic = _italic;
    underline = _underline;
    vertAlign = _vertAlign;
    math = _math;
    direct = _direct;
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
KWFormat::KWFormat( const KWFormat& )
{
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
    math = -1;
    direct = -1;
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
    math = -1;
    direct = -1;
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
    math = -1;
    direct = -1;
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

    if ( ref <= 0 && doc )
        doc->getFormatCollection()->removeFormat( this );

    if ( !doc && ref == 0 ) warning( "RefCount of the format == 0, but I couldn't delete it, "
                                     " because I have not a pointer to the document!" );
}

/*================================================================*/
void KWFormat::incRef()
{
    ++ref;
    //QString key = doc->getFormatCollection()->generateKey( this );
    //debug( "inc ref ( %d ): %s", ref, key.data() );
}

/*================================================================*/
void KWFormat::save( ostream &out )
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
void KWFormat::load( KOMLParser& parser, vector<KOMLAttrib>& lst, KWordDocument *_doc )
{
    doc = _doc;
    ref = 0;

    string tag;
    string name;

    while ( parser.open( 0L, tag ) )
    {
        KOMLParser::parseTag( tag.c_str(), name, lst );

        // color
        if ( name == "COLOR" )
        {
            unsigned int r = 0, g = 0, b = 0;
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "red" )
                {
                    r = atoi( ( *it ).m_strValue.c_str() );
                    color.setRgb( r, g, b );
                }
                else if ( ( *it ).m_strName == "green" )
                {
                    g = atoi( ( *it ).m_strValue.c_str() );
                    color.setRgb( r, g, b );
                }
                else if ( ( *it ).m_strName == "blue" )
                {
                    b = atoi( ( *it ).m_strValue.c_str() );
                    color.setRgb( r, g, b );
                }
            }
        }

        // font
        else if ( name == "FONT" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "name" )
                    userFont = _doc->findUserFont( correctQString( ( *it ).m_strValue.c_str() ) );
            }
        }

        // font size
        else if ( name == "SIZE" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "value" )
                    ptFontSize = atoi( ( *it ).m_strValue.c_str() );
            }
        }

        // weight
        else if ( name == "WEIGHT" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "value" )
                    weight = atoi( ( *it ).m_strValue.c_str() );
            }
        }

        // italic
        else if ( name == "ITALIC" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "value" )
                    italic = atoi( ( *it ).m_strValue.c_str() );
            }
        }

        // underline
        else if ( name == "UNDERLINE" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "value" )
                    underline = atoi( ( *it ).m_strValue.c_str() );
            }
        }

        // vertical alignment
        else if ( name == "VERTALIGN" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "value" )
                    vertAlign = static_cast<VertAlign>( atoi( ( *it ).m_strValue.c_str() ) );
            }
        }

        else
            cerr << "Unknown tag '" << tag << "' in FORMAT" << endl;

        if ( !parser.close( tag ) )
        {
            cerr << "ERR: Closing Child" << endl;
            return;
        }
    }
}
