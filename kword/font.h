#ifndef USERFONT_H
#define USERFONT_H

class KWUserFont;
class KWDisplayFont;
class KWordDocument_impl;

#include "defs.h"

#include <qfont.h>
#include <qfontmet.h>
#include <qlist.h>
#include <qwidget.h>
#include <qstring.h>

class KWUserFont
{
 public:
    KWUserFont( KWordDocument_impl *_doc, const char *_name );
    ~KWUserFont();
    
    const char *getFontName() {	return fontName.data(); }
    
protected:
    QString fontName;
    
    KWordDocument_impl *document;
};

class KWDisplayFont : public QFont
{
public:
    KWDisplayFont( KWordDocument_impl *_doc, QPainter& _painter, KWUserFont *_font, unsigned int _size,
		   int _weight, bool _italic );
    ~KWDisplayFont();
    
    void scaleFont();

    static void scaleAllFonts();

    KWUserFont* getUserFont() { return userFont; }
    unsigned int getPTSize() { return ptSize; }

    unsigned int getPTAscender() { return ptAscender; }
    unsigned int getPTDescender() { return ptDescender; }

    unsigned int getPTWidth( const char *_text );
    
    unsigned int* ptWidth;

protected:
    unsigned int ptSize;
    /**
     * The unzoomed ascender points
     */
    unsigned int ptAscender;
    /**
     * The unzoomed descender in points
     */
    unsigned int ptDescender;
    
    /**
     * Pointer to the user font family this font belongs to.
     */
    KWUserFont *userFont;
    /**
     * Poiner to the document this font belongs to.
     */
    KWordDocument_impl *document;

    unsigned int ptWidthBuffer[256]; 
};

#endif


