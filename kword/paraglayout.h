#ifndef paraglayout_h
#define paraglayout_h

class KWParagLayout;

#include "defs.h"
#include "format.h"

#include <qstring.h>
#include <qcolor.h>
#include <qfont.h>

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
    enum CounterType {CT_NONE = 0,CT_NUM = 1,CT_ALPHAB_L = 2,CT_ALPHAB_U = 3,CT_ROM_NUM_L = 4,CT_ROM_NUM_U = 5,CT_BULLET = 6};
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

    struct Counter
    {
      unsigned int counterDepth;
      int counterBullet;
      QString counterLeftText;
      QString counterRightText;    
      CounterType counterType;
      QString startCounter;
      NumType numberingType;
      QString bulletFont;
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
    void setName(QString _n) { name = _n; }
    void setFlow( Flow _f ) { flow = _f; }
    /**
     * Set the text left to the counter.
     */
    void setCounterLeftText(QString _t) { counter.counterLeftText = _t; }
    /**
     * Set the text right to the counter.
     */
    void setCounterRightText(QString _t) { counter.counterRightText = _t; }
    /**
     *  Set counter type
     */
    void setCounterType(CounterType _t) { counter.counterType = _t; }
    /**
     *  Set counter bullet.
     */
    void setCounterBullet(int _b) { counter.counterBullet = _b; }
    /**
     *  Set Depth of Counter.
     */
    void setCounterDepth(unsigned int _d) { counter.counterDepth = _d; }
    /**
     * Set the format in which to print the counter.
     * This encloses font, color, point-size etc.
     *
     * @see KWFormat
     */
    void setFollowingParagLayout(QString _paragname);

    void setLeftBorder(Border _brd) { left = _brd; }
    void setRightBorder(Border _brd) { right = _brd; }
    void setTopBorder(Border _brd) { top = _brd; }
    void setBottomBorder(Border _brd) { bottom = _brd; }

    void setStartCounter(QString _s) { counter.startCounter = _s; }
    QString getStartCounter() { return counter.startCounter; }

    void setNumberingType(NumType _t) { counter.numberingType = _t; }
    NumType getNumberingType() { return counter.numberingType; }

    void setBulletFont(QString _f) { counter.bulletFont = _f; }
    QString getBulletFont() { return counter.bulletFont; }

    KWFormat& getFormat() { return format; }
    QString getName() { return name; }
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
    CounterType getCounterType() { return counter.counterType; }
    /**
     * @return the bullet of the Counter.
     */
    int getCounterBullet() { return counter.counterBullet; }
    /**
     * @return depth of Counter.
     */
    int getCounterDepth() { return counter.counterDepth; }
    /**
     * @return the text left to the counter
     */
    QString getCounterLeftText() { return counter.counterLeftText; }
    /**
     * @return the text right to the counter
     */
    QString getCounterRightText() { return counter.counterRightText.data(); }  
    /**
     * @return the name of the following parag layout.
     */
    QString getFollowingParagLayout() { return followingParagLayout; }  

    void save(ostream &out);
    void load(KOMLParser&,vector<KOMLAttrib>&);

    Counter getCounter() { return counter; }
    void setCounter(Counter _counter) { counter = _counter; }

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
    Counter counter;
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
    QString followingParagLayout;  
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






