#include "format.h"
#include "kword_doc.h"

#include <koIMR.h>
#include <komlMime.h>
#include <koStream.h>
#include <strstream>
#include <fstream>
#include <unistd.h>

KWFormat::KWFormat( KWordDocument_impl *_doc, const QColor& _color, KWUserFont *_font = 0L, int _font_size = -1, int _weight = -1,
		    char _italic = -1, char _underline = -1, VertAlign _vertAlign = VA_NORMAL, char _math = -1, char _direct = -1 )
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

KWFormat::KWFormat(KWordDocument_impl *_doc)
{
    doc = _doc;
    setDefaults(_doc);
    ref = 0;
}

KWFormat::KWFormat( KWordDocument_impl *_doc,const KWFormat &_format )
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
    if (!doc) doc = const_cast<KWFormat>(_format).getDocument();

    return *this;
}

bool KWFormat::operator==(const KWFormat & _format)
{
  return (*userFont == *_format.getUserFont() &&
	  ptFontSize == _format.getPTFontSize() &&
	  weight == _format.getWeight() &&
	  italic == _format.getItalic() &&
	  underline == _format.getUnderline() &&
	  color == _format.getColor() &&
	  vertAlign == _format.getVertAlign());
}

bool KWFormat::operator!=(const KWFormat & _format)
{
  return (*userFont != *_format.getUserFont() ||
	  ptFontSize != _format.getPTFontSize() ||
	  weight != _format.getWeight() ||
	  italic != _format.getItalic() ||
	  underline != _format.getUnderline() ||
	  color != _format.getColor() ||
	  vertAlign != _format.getVertAlign());
}

void KWFormat::setDefaults( KWordDocument_impl *_doc )
{
    userFont = _doc->getDefaultUserFont();
    ptFontSize = 12;
    weight = QFont::Normal;
    italic = 0;
    underline = 0;
    color = black;
    vertAlign = VA_NORMAL;
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
    
    vertAlign = _format.getVertAlign();
}

void KWFormat::decRef()
{ 
  --ref;
  QString key = doc->getFormatCollection()->generateKey(this);
  //debug("dec ref (%d): %s",ref,key.data());

  if (ref <= 0 && doc) 
    doc->getFormatCollection()->removeFormat(this); 

  if (!doc && ref == 0) warning("RefCount of the format == 0, but I couldn't delete it,"
				" because I have not a pointer to the document!");
}

void KWFormat::incRef()
{ 
  ++ref;
  QString key = doc->getFormatCollection()->generateKey(this);
  //debug("inc ref (%d): %s",ref,key.data());
}

void KWFormat::save(ostream &out)
{
  out << indent << "<COLOR red=\"" << color.red() << "\" green=\"" << color.green() << "\" blue=\"" << color.blue() << "\"/>" << endl;
  out << indent << "<FONT name=\"" << userFont->getFontName() << "\"/>" << endl;
  out << indent << "<SIZE value=\"" << ptFontSize << "\"/>" << endl;
  out << indent << "<WEIGHT value=\"" << weight << "\"/>" << endl;
  out << indent << "<ITALIC value=\"" << static_cast<int>(italic) << "\"/>" << endl;
  out << indent << "<UNDERLINE value=\"" << static_cast<int>(underline) << "\"/>" << endl;
  out << indent << "<VERT_ALIGN value=\"" << static_cast<int>(vertAlign) << "\"/>" << endl;
}
