char s_FONT_CCId[]="$Id$";

#include "font.h"
#include "kword_doc.h"

#include <qpainter.h>

KWUserFont::KWUserFont( KWordDocument_impl *_doc, const char *_name )
{
    fontName = _name;
    fontName.detach();
    
    document = _doc;
    document->userFontList.append( this );
}

KWUserFont::~KWUserFont()
{
    document->userFontList.removeRef( this );
}

KWDisplayFont::KWDisplayFont( KWordDocument_impl *_doc, QPainter& _painter, KWUserFont *_font, unsigned int _size,
			      int _weight, bool _italic ) :
    QFont( _font->getFontName(), ZOOM(_size), _weight, _italic )
{
    document = _doc;
    document->displayFontList.append( this );

    userFont = _font;
    
    _painter.setFont( *this );
    QFontMetrics fm( _painter.fontMetrics() );
    
    for ( int c = -128; c <= 127; c++ )
    {
	// ptWidth[i] = fm.width( (char)i );
	ptWidthBuffer[ 128 + c ] = fm.width( c );
    }
    ptWidth = ptWidthBuffer + 128;
    
    ptAscender = fm.ascent();
    ptDescender = fm.descent();
    
    ptSize = _size;

    if ( ZOOM(100) != 100 )
	scaleFont();
}

void KWDisplayFont::scaleFont()
{
    setPointSize( ZOOM( ptSize ) );
}

unsigned int KWDisplayFont::getPTWidth( const char *_text )
{
    unsigned int w = 0;
    const char *p = _text;
    while ( *p )
	w += ptWidth[ *p++ ];
	
    return w;
}

KWDisplayFont::~KWDisplayFont()
{
    document->displayFontList.removeRef( this );
}
    
