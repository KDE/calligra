char s_FONT_CCId[]="$Id$";

#include "font.h"
#include "kword_doc.h"

#include <qpainter.h>
#include <qfontmetrics.h>

KWUserFont::KWUserFont(KWordDocument *_doc,QString _name)
{
    fontName = _name;
    fontName.detach();
    
    document = _doc;
    document->userFontList.append( this );
}

KWUserFont::~KWUserFont()
{
    document->userFontList.setAutoDelete(true);
    document->userFontList.removeRef( this );
    document->userFontList.setAutoDelete(false);
}

KWDisplayFont::KWDisplayFont( KWordDocument *_doc, KWUserFont *_font, unsigned int _size,
			      int _weight, bool _italic, bool _underline ) :
    QFont( _font->getFontName(), ZOOM(_size), _weight, _italic ), fm(*this)
{
  setUnderline(_underline);

  document = _doc;
  document->displayFontList.append( this );

  userFont = _font;
    
  fm = QFontMetrics(*this);
    
  ptSize = _size;

  if ( ZOOM(100) != 100 )
    scaleFont();
}

void KWDisplayFont::setPTSize(int _size)
{
  ptSize = _size;
  setPointSize(_size);
  fm = QFontMetrics(*this);
}

void KWDisplayFont::setWeight(int _weight)
{
  QFont::setWeight(_weight);
  fm = QFontMetrics(*this);
}

void KWDisplayFont::setItalic(bool _italic)
{
  QFont::setItalic(_italic);
  fm = QFontMetrics(*this);
}

void KWDisplayFont::setUnderline(bool _underline)
{
  QFont::setUnderline(_underline);
  fm = QFontMetrics(*this);
}

void KWDisplayFont::scaleFont()
{
  setPointSize( ZOOM( ptSize ) );
  fm = QFontMetrics(*this);
}

unsigned int KWDisplayFont::getPTWidth(QString _text)
{
  return fm.width(_text);
}

unsigned int KWDisplayFont::getPTWidth( char &_c )
{
  return fm.width(_c);
}

KWDisplayFont::~KWDisplayFont()
{
  document->displayFontList.setAutoDelete(true);
  document->displayFontList.removeRef( this );
  document->displayFontList.setAutoDelete(false);
}
    
