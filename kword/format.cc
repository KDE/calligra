#include "format.h"
#include "kword_doc.h"
#include "defs.h"
#include "font.h"
#include "kword_utils.h"
#include <koApplication.h>

/******************************************************************/
/* Class: KWFormat						  */
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
QDomElement KWFormat::save( QDomDocument &doc, int id )
{
    QDomElement format = doc.createElement( "FORMAT" );
    if ( id != -1 )
	format.setAttribute( "id", id );
    format.setAttribute( "color", colorToName( color ) );
    format.setAttribute( "font", userFont->getFontName() );
    format.setAttribute( "size", ptFontSize );
    format.setAttribute( "weight", weight );
    format.setAttribute( "italic", italic );
    format.setAttribute( "underline", underline );
    format.setAttribute( "vertalign", vertAlign );

    return format;
}

/*================================================================*/
bool KWFormat::load( const QDomElement& element, KWordDocument* _doc )
{
    doc = _doc;

    color = QColor( element.attribute( "color" ) );
    userFont = _doc->findUserFont( element.attribute( "font" ) );
    ptFontSize = element.attribute( "size" ).toInt();
    weight = element.attribute( "weight" ).toInt();
    italic = (char)element.attribute( "italic" ).toInt();
    vertAlign = (VertAlign)element.attribute( "vertalign" ).toInt();
}

