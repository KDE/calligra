#ifndef parag_h
#define parag_h

class KWParag;
class KWordDocument_impl;

#include "defs.h"
#include "paraglayout.h"
#include "fc.h"
#include "char.h"

#include <assert.h>

/**
 * This class hold a real paragraph. It tells abou the text in this
 * paragraph, its format etc. The complete text is a list of KWParag instances.
 */
class KWParag
{
public:    
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
    KWParag( KWordDocument_impl *_doc, KWParag* _prev, KWParag* _next, KWParagLayout* _paragLayout );
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
    /**
     * @return The column this paragraph starts in. This value is only valid if this paragraph is
     *         in front of the last modified paragraph. The value is zoomed.
     */
    unsigned int getStartColumn() { return startColumn; }
    /**
     * @return The column this paragraph ends in. This value is only valid if this paragraph is
     *         in front of the last modified paragraph. The value is zoomed.
     */
    unsigned int getEndColumn() { return endColumn; }
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
    KWChar* getText() { return text.data; }
    KWChar* getChar( unsigned int _pos ) { assert( _pos < text.len ); return text.data + _pos; }
    KWString* getKWString() { return &text; }

    /**
     * @return the paragraph Llyout of this paragraph.
     */
    KWParagLayout* getParagLayout() { return paragLayout; }
    
    /**
     * Fille '_str' with the counters text. If this paragraph has no counter,
     * an empty but non null string is returned.
     *
     * @return a reference to '_str'
     */
    QString& makeCounterText( QString& _str );

    void updateCounters( KWFormatContext* _format );
    
    /**
     * Set the paragraph following this one.
     */
    void setNext( KWParag* _p ) { next = _p; }
    /**
     * Set the paragraph before this one.
     */
    void setPrev( KWParag* _p ) { prev = _p; }

    void setStartPage( unsigned int _page ) { startPage = _page; }
    void setStartColumn( unsigned int _col ) { startColumn = _col; }    
    void setPTYStart( unsigned int _y ) { ptYStart = _y; }

    void insertText( unsigned int _pos, const char *_text );
    void appendText(KWChar *_text,unsigned int _len);
    bool deleteText( unsigned int _pos, unsigned int _len = 1);
    void setFormat( unsigned int _pos, const KWFormat &format );
  
protected:
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
    KWordDocument_impl *document;
    /**
     * The page this paragraph starts on. This value is only valid if this paragraph is
     * in front of the last modified paragraph. The value is zoomed.
     */
    unsigned int startPage;
    /**
     * The column this paragraph starts in. This value is only valid if this paragraph is
     * in front of the last modified paragraph. The value is zoomed.
     */
    unsigned int startColumn;
    /**
     * The column this paragraph ends in. This value is only valid if this paragraph is
     * in front of the last modified paragraph. The value is zoomed.
     */
    unsigned int endColumn;
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

    unsigned short counters[ 10 ];
    unsigned short numberLikeCounters[ 10 ];
};

#endif
