#ifndef paraglayout_h
#define paraglayout_h

class KWParagLayout;

#include "defs.h"
#include "format.h"

/**
 * A KWParagLayout-Instance provides all information neccessary to format a given
 * parag. This means: Font, TextFlow, Counter ....
 * Every KWParag owns a KWParagLayout instance.
 */
class KWParagLayout
{
public:
    enum Flow { LEFT, RIGHT, CENTER, BLOCK };
    enum CounterFlow { C_LEFT, C_RIGHT };
    
    KWParagLayout( KWordDocument_impl *_doc );
    ~KWParagLayout();
    
    void setFormat( KWFormat &_format );
    void setPTFirstLineLeftIndent( unsigned int _i ) { ptFirstLineLeftIndent = _i; }
    void setptLeftIndent( unsigned int _i ) { ptLeftIndent = _i; }
    void setptParagFootOffset( unsigned int _i) { ptParagFootOffset = _i; }
    void setptParagHeadOffset( unsigned int _i) { ptParagHeadOffset = _i; }
    void setName( const char* _n) { name = _n; }
    void setFlow( Flow _f ) { flow = _f; }
    /**
     * Set the text left to the counter.
     */
    void setCounterLeftText( const char *_t ) { counterLeftText = _t; }
    /**
     * Set the text right to the counter.
     */
    void setCounterRightText( const char *_t ) { counterRightText = _t; }
    /**
     *  Set counters flow
     */
    void setCounterFlow( CounterFlow _f ) { counterFlow = _f; }
    /**
     *  Set Depth of Counter.
     */
    void setCounterDepth( unsigned int _d ) { counterDepth = _d; }
    /**
     * Set Number of Counter.
     */
    void setCounterNr( int _nr ) { counterNr = _nr; }
    /**
     * Set the format in which to print the counter.
     * This encloses font, color, point-size etc.
     *
     * @see KWFormat
     */
    void setCounterFormat( KWFormat& _format );
    /**
     * Set the name of the following parag layout.
     * 
     * @param _paragname must be a valid name of a KWParagLayout
     *                   instance.
     */
    void setFollowingParagLayout( const char *_paragname );
    /**
     * Sets the parag layout that is used for numbering. For example a theorem
     * will be numbered like this: Theorem C.N .....    with C=Chapter Counter
     * and N=Theorem Counter. In this case Chapter is the NumberLikeParag.
     *
     * @param _paragname is the name of a valid KWParagLayout.
     */
    void setNumberLikeParagLayout( const char *_paragname );

    KWParagLayout* getNumberLikeParagLayout() {	return numberLikeParagLayout; }
    
    /**
     * @return a reference to the used format.
     *
     * @see KWFormat
     */
    KWFormat& getFormat() { return format; }
    const char* getName() { return name.data(); }
    unsigned int getPTFirstLineLeftIndent() { return ptFirstLineLeftIndent; }
    unsigned int getPTLeftIndent() { return ptLeftIndent; }
    unsigned int getPTParagFootOffset() { return ptParagFootOffset; }
    unsigned int getPTParagHeadOffset() { return ptParagHeadOffset; }
    Flow getFlow() { return flow; }
    /**
     * @return the flow of the Counter.
     */
    int getCounterFlow() { return counterFlow; }
    /**
     * @return depth of Counter.
     */
    int getCounterDepth() { return counterDepth; }
    /**
     * @return number of Counter.
     */
    int getCounterNr() { return counterNr; }
    /**
     * @return KWFormat for printing counter.
     *
     * @see KWFormat
     */
    KWFormat& getCounterFormat() { return counterFormat; }
    /**
     * @return the text left to the counter
     */
    const char* getCounterLeftText() { return counterLeftText.data(); }
    /**
     * @return the text right to the counter
     */
    const char* getCounterRightText() { return counterRightText.data(); }  
    /**
     * @return the name of the following parag layout.
     */
    KWParagLayout* getFollowingParagLayout() { return followingParagLayout; }  

protected:
    Flow flow;
    unsigned int ptParagFootOffset;
    unsigned int ptParagHeadOffset;
    unsigned int ptFirstLineLeftIndent;
    unsigned int ptLeftIndent;
    /**
     * This instance holds information about the font and color etc. for
     * the parags text.
     *
     * @see KWFormat
     */
    KWFormat format;
    CounterFlow counterFlow;  
    /**
     * A value between 0 and 7.
     */
    unsigned int counterDepth;
    /**
     * A value between 0 and 7 or -1.
     * -1 means that we dont use any counter.
     */
    int counterNr;
    KWFormat counterFormat;
    QString counterLeftText;
    QString counterRightText;    
    /**
     * Parag that is used for numbering. For example a theorem
     * will be numbered like this: Theorem C.N .....    with C=Chapter Counter
     * and N=Theorem Counter. In this case Chapter is the NumberLikeParag.
     * This value may be 0L, if we dont have the above situation or no counter at all.
     *
     * @see KWParagLayout
     */
    KWParagLayout* numberLikeParagLayout;  
    /**
     * Name of the Parag that will be used when you press RETURN in this Parag
     * If this pointer is 0L the following parag layout is still the same.
     *
     * @see KWParagLayout
     */
    KWParagLayout* followingParagLayout;  
    /**
     * This parags name.
     */
    QString name;
    /**
     * The document this parag layout belongs to
     */
    KWordDocument_impl *document;
};

#endif






