#ifndef paraglayout_h
#define paraglayout_h

class KWParagLayout;

#include "defs.h"
#include "format.h"

#include <qstring.h>
#include <qcolor.h>

#include <iostream>
#include <koStream.h>

/**
 * A KWParagLayout-Instance provides all information neccessary to format a given
 * parag. This means: Font, TextFlow, Counter ....
 * Every KWParag owns a KWParagLayout instance.
 */
class KWParagLayout
{
public:
    enum Flow { LEFT, RIGHT, CENTER, BLOCK };
  //enum CounterFlow { C_LEFT, C_RIGHT };
    enum CounterType {CT_NONE,CT_NUM = 0,CT_ALPHAB = 1,CT_ROM_NUM = 2,CT_BULLET};
    enum NumType {NT_LIST = 0,NT_CHAPTER = 1};

    enum BorderStyle {SOLID = 0,DASH = 1,DOT = 2,DASH_DOT = 3,DASH_DOT_DOT = 4};
    struct Border
    {
      QColor color;
      BorderStyle style;
      unsigned int ptWidth;
      bool operator==(const Border _brd) {
	return (style == _brd.style && color == _brd.color && ptWidth == _brd.ptWidth);
      }
      bool operator!=(const Border _brd) {
	return (style != _brd.style || color != _brd.color || ptWidth != _brd.ptWidth);
      }
    };

    KWParagLayout( KWordDocument *_doc, bool _add = true );
    ~KWParagLayout();
    
    KWParagLayout& operator=(KWParagLayout &_layout);

    void setFormat( KWFormat &_format );
    void setMMFirstLineLeftIndent( unsigned int _i ) { mmFirstLineLeftIndent = _i; }
    void setMMLeftIndent( unsigned int _i ) { mmLeftIndent = _i; }
    void setMMParagFootOffset( unsigned int _i) { mmParagFootOffset = _i; }
    void setMMParagHeadOffset( unsigned int _i) { mmParagHeadOffset = _i; }
    void setPTLineSpacing( unsigned int _i) { ptLineSpacing = _i; }
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
     *  Set counter type
     */
    void setCounterType(CounterType _t) { counterType = _t; }
    /**
     *  Set counter bullet.
     */
    void setCounterBullet(int _b) { counterBullet = _b; }
    /**
     *  Set Depth of Counter.
     */
    void setCounterDepth( unsigned int _d ) { counterDepth = _d; }
    /**
     * Set Number of Counter.
     */
    //void setCounterNr( int _nr ) { counterNr = _nr; }
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
    //void setNumberLikeParagLayout( const char *_paragname );

    void setLeftBorder(Border _brd) { left = _brd; }
    void setRightBorder(Border _brd) { right = _brd; }
    void setTopBorder(Border _brd) { top = _brd; }
    void setBottomBorder(Border _brd) { bottom = _brd; }

    void setStartCounter(QString _s) { startCounter = _s; }
    QString getStartCounter() { return startCounter; }

    void setNumberingType(NumType _t) { numberingType = _t; }
    NumType getNumberingType() { return numberingType; }

    //KWParagLayout* getNumberLikeParagLayout() {	return numberLikeParagLayout; }
    
    /**
     * @return a reference to the used format.
     *
     * @see KWFormat
     */
    KWFormat& getFormat() { return format; }
    const char* getName() { return name.data(); }
    unsigned int getPTFirstLineLeftIndent() { return MM_TO_POINT(mmFirstLineLeftIndent); }
    unsigned int getPTLeftIndent() { return MM_TO_POINT(mmLeftIndent); }
    unsigned int getPTParagFootOffset() { return MM_TO_POINT(mmParagFootOffset); }
    unsigned int getPTParagHeadOffset() { return MM_TO_POINT(mmParagHeadOffset); }
    unsigned int getMMFirstLineLeftIndent() { return mmFirstLineLeftIndent; }
    unsigned int getMMLeftIndent() { return mmLeftIndent; }
    unsigned int getMMParagFootOffset() { return mmParagFootOffset; }
    unsigned int getMMParagHeadOffset() { return mmParagHeadOffset; }
    unsigned int getPTLineSpacing() { return ptLineSpacing; }
    Flow getFlow() { return flow; }
    Border getLeftBorder() { return left; }
    Border getRightBorder() { return right; }
    Border getTopBorder() { return top; }
    Border getBottomBorder() { return bottom; }
    /**
     * @return the type of the Counter.
     */
    CounterType getCounterType() { return counterType; }
    /**
     * @return the bullet of the Counter.
     */
    int getCounterBullet() { return counterBullet; }
    /**
     * @return depth of Counter.
     */
    int getCounterDepth() { return counterDepth; }
    /**
     * @return number of Counter.
     */
    //int getCounterNr() { return counterNr; }
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

    void save(ostream &out);
    void load(KOMLParser&,vector<KOMLAttrib>&);

protected:
    Flow flow;
    unsigned int mmParagFootOffset;
    unsigned int mmParagHeadOffset;
    unsigned int mmFirstLineLeftIndent;
    unsigned int mmLeftIndent;
    unsigned int ptLineSpacing;
    Border left,right,top,bottom;

    /**
     * This instance holds information about the font and color etc. for
     * the parags text.
     *
     * @see KWFormat
     */
    KWFormat format;
    //CounterFlow counterFlow;  
    /**
     * A value between 0 and 7.
     */
    unsigned int counterDepth;
    /**
     * A value between 0 and 7 or -1.
     * -1 means that we dont use any counter.
     */
    int counterBullet;
    //int counterNr;
    KWFormat counterFormat;
    QString counterLeftText;
    QString counterRightText;    
    CounterType counterType;
    QString startCounter;
    NumType numberingType;
    /**
     * Parag that is used for numbering. For example a theorem
     * will be numbered like this: Theorem C.N .....    with C=Chapter Counter
     * and N=Theorem Counter. In this case Chapter is the NumberLikeParag.
     * This value may be 0L, if we dont have the above situation or no counter at all.
     *
     * @see KWParagLayout
     */
    //KWParagLayout* numberLikeParagLayout;  
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
    KWordDocument *document;
};

#endif






