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

#ifndef parag_h
#define parag_h

#include "char.h"
#include "searchdia.h"
#include "paraglayout.h"

#include <assert.h>
#include <qtextstream.h>

#include <koStream.h>
#include <koRuler.h>

class KWVariable;
class KWFootNote;
class KWTextFrameSet;
class KWordDocument;

/******************************************************************/
/* Class: KWParag                                                 */
/******************************************************************/

/**
 * This class hold a real paragraph. It tells about the text in this
 * paragraph, its format etc. The complete text is a list of KWParag instances.
 */
class KWParag
{
public:
    enum Info { PI_NONE, PI_FOOTNOTE, PI_CONTENTS };

    /**
     * Creates a new instance of KWParag.
     *
     * @param _doc the document this paragraph is belonging to.
     * @param _prev a pointer to the previous paragraph or 0L if this one is to become
     *              the first one.
     * @param _next a pointer to the next paragraph or 0L if this one is to become
     *              the last one.
     * @param _paragLayout the layout to use in this paragraph.
     *
     * @see KWParagraphLayout
     */
    KWParag( KWTextFrameSet *_frameSet, KWordDocument *_doc, KWParag* _prev,
             KWParag* _next, KWParagLayout* _paragLayout, bool useForFirst = true );
    KWParag( const KWParag& _parag );

    /**
     * Desctructor
     *
     * The destructor does not remove this paragrph from the chain of paragraphs!
     */
    ~KWParag();

    /**
     * @return the paragraph following this one or 0L if this is the last one.
     */
    KWParag* getNext() { return next; }
    /**
     * @return the paragraph before this one or 0L if this is the first one.
     */
    KWParag* getPrev() { return prev; }
    /**
     * @return The page this paragraph starts on. This value is only valid if this paragraph is
     *         in front of the last modified paragraph. The value is zoomed.
     */
    unsigned int getStartPage() { return startPage; }
    unsigned int getEndPage() { return endPage; }
    /**
     * @return The column this paragraph starts in. This value is only valid if this paragraph is
     *         in front of the last modified paragraph. The value is zoomed.
     */
    unsigned int getStartFrame() { return startFrame; }
    /**
     * @return The column this paragraph ends in. This value is only valid if this paragraph is
     *         in front of the last modified paragraph. The value is zoomed.
     */
    unsigned int getEndFrame() { return endFrame; }
    /**
     * @return The y position on the page on which this paragraph starts.
     *         This value is only valid if this paragraph is in front of the last modified paragraph.
     *         The value is zoomed.
     */
    unsigned int getPTYStart() { return ptYStart; }
    /**
     * @return The y position on the page on which this paragraph ends.
     *         This value is only valid if this paragraph is in front of the last modified paragraph.
     *         The value is zoomed.
     */
    unsigned int getPTYEnd() { return ptYEnd; }

    /**
     * @return the size of the text in bytes. This does NOT mean
     *         the amount of characters or whatever. The size needed
     *         to store pointers to @ref #KWFormat is included for example.
     */
    unsigned int getTextLen() { return text.size(); }
    /**
     * @return a pointer to the memory segment, which holds text paragraphs
     *         text.
     */
    KWChar* getText() { return text.data(); }
    KWChar* getChar( unsigned int _pos ) { assert( _pos < text.size() ); return text.data() + _pos; }
    KWString* getKWString() { return &text; }

    /**
     * @return the paragraph Layout of this paragraph.
     */
    KWParagLayout* getParagLayout() { return paragLayout; }

    void setParagLayout( KWParagLayout* _paragLayout ) { *paragLayout = *_paragLayout; }

    /**
     * Fille '_str' with the counters text. If this paragraph has no counter,
     * an empty but non null string is returned.
     *
     * @return a reference to '_str'
     */
    void makeCounterText();
    void makeCounterWidth();
    QString getCounterText() { return counterText; }
    QString getCounterWidth() { return counterWidth; }

    /**
     * Set the paragraph following this one.
     */
    void setNext( KWParag* _p ) { next = _p; }
    /**
     * Set the paragraph before this one.
     */
    void setPrev( KWParag* _p ) { prev = _p; }

    void setStartPage( unsigned int _page ) { startPage = _page; }
    void setEndPage( unsigned int _page ) { endPage = _page; }
    void setStartFrame( unsigned int _frame ) { startFrame = _frame; }
    void setEndFrame( unsigned int _frame ) { endFrame = _frame; }
    void setPTYStart( unsigned int _y ) { ptYStart = _y; }
    void setPTYEnd( unsigned int _y ) { ptYEnd = _y; }

    void insertText( unsigned int _pos, QString _text );
    void insertText( unsigned int _pos, KWString *_text );
    void insertVariable( unsigned int _pos, KWVariable *_var );
    void insertFootNote( unsigned int _pos, KWFootNote *_fn );
    void insertAnchor( unsigned int _pos, KWCharAnchor *_anchor );
    void insertPictureAsChar( unsigned int _pos, QString _filename );
    void insertTab( unsigned int _pos );
    void appendText( KWChar *_text, unsigned int _len );
    bool deleteText( unsigned int _pos, unsigned int _len = 1 );
    void setFormat( unsigned int _pos, unsigned int _len, const KWFormat &format, int flags = KWFormat::All );

    void save( QTextStream&out );
    void load( KOMLParser&, QValueList<KOMLAttrib>& );

    int *getCounterData() { return counterData; }

    void applyStyle( QString _style );
    void tabListChanged( QList<KoTabulator>* );

    int find( QString _expr, KWSearchDia::KWSearchEntry *_format, int _index, bool _cs, bool _whole );
    int find( QRegExp _regexp, KWSearchDia::KWSearchEntry *_format, int _index, int &_len, bool _cs,
              bool _wildcard = false );
    int findRev( QString _expr, KWSearchDia::KWSearchEntry *_format, int _index, bool _cs, bool _whole );
    int findRev( QRegExp _regexp, KWSearchDia::KWSearchEntry *_format, int _index, int &_len, bool _cs,
                 bool _wildcard = false );
    void replace( int _pos, int _len, QString _text, KWFormat &_format );

    void setHardBreak( bool hb ) { hardBreak = hb; }
    bool hasHardBreak() { return hardBreak; }

    QString getParagName() { return paragName; }
    void setParagName( const QString &name )
    { paragName = name; }

    void setFrameSet( KWTextFrameSet *_fs )
    { frameSet = _fs; }

    void setDocument( KWordDocument *_doc )
    { document = _doc; }

    Info getInfo() { return info; }
    void setInfo( Info _info ) { info = _info; }

    static void correctFormat( KWParag *newParag, KWParag *oldParag );

    KWordDocument* getDocument() { return document; }

protected:
    /**
     * called by makeCounterText(). Creates a counter text using a custom
     * string that defines a counter (syntax a bit similar to LaTeX)
     */
    QString counterTextByCustomDef( const QString& d_ );
    /**
     * called by makeCounterText and by counterTextByCustomDef. Returns counterText.
     */
    QString counterTextByType( KWParagLayout::CounterType ct_ );
    /**
     * Pointer to the previous paragraph or 0L if this is the first one.
     */
    KWParag *prev;
    /**
     * Pointer to the next paragraph or 0L if this is the last one.
     */
    KWParag *next;

    /**
     * Pointer to the paragraph layout used.
     *
     * @see KWParagraphLayout
     */
    KWParagLayout *paragLayout;
    /**
     * Pointer to the text array.
     */
    KWString text;
    /**
     * Length of the array.
     */
    // unsigned int maxTextLen;
    /**
     * Length of the used text array.
     */
    // unsigned int textLen;
    /**
     * The document this paragraph is belonging to.
     */
    KWordDocument *document;
    /**
     * The page this paragraph starts on. This value is only valid if this paragraph is
     * in front of the last modified paragraph. The value is zoomed.
     */
    unsigned int startPage;
    unsigned int endPage;
    /**
     * The column this paragraph starts in. This value is only valid if this paragraph is
     * in front of the last modified paragraph. The value is zoomed.
     */
    unsigned int startFrame;
    /**
     * The column this paragraph ends in. This value is only valid if this paragraph is
     * in front of the last modified paragraph. The value is zoomed.
     */
    unsigned int endFrame;
    /**
     * The y position on the page on which this paragraph starts.
     * This value is only valid if this paragraph is in front of the last modified paragraph.
     * The value is not zoomed.
     */
    unsigned int ptYStart;
    /**
     * The y position on the page on which this paragraph ends.
     * This value is only valid if this paragraph is in front of the last modified paragraph.
     * The value is not zoomed.
     */
    unsigned int ptYEnd;

    KWTextFrameSet *frameSet;
    int counterData[ 16 ];
    QString counterText;
    QString counterWidth;
    bool hardBreak;

    QString paragName;
    Info info;

};

#endif

