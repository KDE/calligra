#include "format.h"
#include "kword_doc.h"

KWFormat::KWFormat( const QColor& _color, KWUserFont *_font = 0L, int _font_size = -1, int _weight = -1,
		    char _italic = -1, char _underline = -1, char _math = -1, char _direct = -1 )
{
    color = _color;
    userFont = _font;
    ptFontSize = _font_size;
    weight = _weight;
    italic = _italic;
    underline = _underline;
    math = _math;
    direct = _direct;
}

KWFormat::KWFormat()
{
    userFont = 0L;
    ptFontSize = -1;
    weight = -1;
    italic = -1;
    underline = -1;
    math = -1;
    direct = -1;
}

KWFormat::KWFormat( const KWFormat &_format )
{
    userFont = _format.getUserFont();
    ptFontSize = _format.getPTFontSize();
    weight = _format.getWeight();
    italic = _format.getItalic();
    underline = _format.getUnderline();
    color = _format.getColor();
    math = -1;
    direct = -1;
}

KWFormat& KWFormat::operator=( const KWFormat& _format )
{
    userFont = _format.getUserFont();
    ptFontSize = _format.getPTFontSize();
    weight = _format.getWeight();
    italic = _format.getItalic();
    underline = _format.getUnderline();
    color = _format.getColor();
    math = -1;
    direct = -1;

    return *this;
}

bool KWFormat::operator==(const KWFormat & _format)
{
  return (*userFont == *_format.getUserFont() &&
	  ptFontSize == _format.getPTFontSize() &&
	  weight == _format.getWeight() &&
	  italic == _format.getItalic() &&
	  underline == _format.getUnderline() &&
	  color == _format.getColor());
}

bool KWFormat::operator!=(const KWFormat & _format)
{
  return (*userFont != *_format.getUserFont() ||
	  ptFontSize != _format.getPTFontSize() ||
	  weight != _format.getWeight() ||
	  italic != _format.getItalic() ||
	  underline != _format.getUnderline() ||
	  color != _format.getColor());
}

void KWFormat::setDefaults( KWordDocument_impl *_doc )
{
    userFont = _doc->getDefaultUserFont();
    ptFontSize = 12;
    weight = QFont::Normal;
    italic = 0;
    underline = 0;
    color = black;
    math = -1;
    direct = -1;
}

KWDisplayFont* KWFormat::loadFont( KWordDocument_impl *_doc )
{
    KWDisplayFont *font = _doc->findDisplayFont( userFont, ptFontSize, weight, italic, underline );
    if ( font )
	return font;

    KWUserFont* uf = userFont;
    if ( !uf )
      uf = _doc->getDefaultUserFont(); 
    assert( uf );
    
    font = new KWDisplayFont( _doc, uf, ptFontSize, weight, italic, underline );
    return font;
}

void KWFormat::apply( KWFormat &_format )
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
}
