#ifndef USERFONT_H
#define USERFONT_H

class KWUserFont;
class KWDisplayFont;
class KWordDocument;

#include "defs.h"

#include <qfont.h>
#include <qfontmetrics.h>

/******************************************************************/
/* Class: KWUserFont                                              */
/******************************************************************/

class KWUserFont
{
public:
    KWUserFont( KWordDocument *_doc, QString _name );
    ~KWUserFont();

    QString getFontName() { return fontName; }

    bool operator==( KWUserFont &_font )
    { return _font.getFontName() == fontName; }
    bool operator!=( KWUserFont &_font )
    { return _font.getFontName() != fontName; }

protected:
    QString fontName;

    KWordDocument *document;

};

/******************************************************************/
/* Class: KWDisplayFont                                           */
/******************************************************************/

class KWDisplayFont : public QFont
{
public:
    KWDisplayFont( KWordDocument *_doc, KWUserFont *_font, unsigned int _size,
                   int _weight, bool _italic, bool _underline );
    ~KWDisplayFont();

    void scaleFont();

    static void scaleAllFonts();

    KWUserFont* getUserFont() { return userFont; }
    unsigned int getPTSize() { return ptSize; }

    unsigned int getPTAscender() { return fm.ascent(); }
    unsigned int getPTDescender() { return fm.descent() + 2; }

    unsigned int getPTWidth( QString _text );
    unsigned int getPTWidth( char &_c );
    unsigned int getPTWidth( QChar &_c );

    void setPTSize( int _size );
    void setWeight( int _weight );
    void setItalic( bool _italic );
    void setUnderline( bool _underline );

protected:
    QFontMetrics fm;

    unsigned int ptSize;

    /**
     * Pointer to the user font family this font belongs to.
     */
    KWUserFont *userFont;
    /**
     * Poiner to the document this font belongs to.
     */
    KWordDocument *document;

};

#endif


