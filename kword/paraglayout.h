#ifndef paraglayout_h
#define paraglayout_h

#include "format.h"
#include "defs.h"

#include <qstring.h>
#include <qcolor.h>
#include <qlist.h>
#include <qdom.h>

#include <koRuler.h>

class KWParagLayout;

/******************************************************************/
/* Class: KWParagLayout						  */
/******************************************************************/

/**
 * A KWParagLayout-Instance provides all information neccessary to format a given
 * parag. This means: Font, TextFlow, Counter ....
 * Every KWParag owns a KWParagLayout instance.
 */
const QString CounterPlaceHolder[] = {"", "99", "w", "W", "iiix", "IIIX", "W "};

class KWParagLayout
{
public:
    enum Flow {LEFT, RIGHT, CENTER, BLOCK};
    //enum CounterFlow { C_LEFT, C_RIGHT };
    enum CounterType {CT_NONE = 0, CT_NUM = 1, CT_ALPHAB_L = 2, CT_ALPHAB_U = 3, CT_ROM_NUM_L = 4, CT_ROM_NUM_U = 5, CT_BULLET = 6};
    enum NumType {NT_LIST = 0, NT_CHAPTER = 1};

    enum BorderStyle {SOLID = 0, DASH = 1, DOT = 2, DASH_DOT = 3, DASH_DOT_DOT = 4};
    struct Border
    {
	QColor color;
	BorderStyle style;
	unsigned int ptWidth;
	bool operator==( const Border _brd ) const {
	    return ( style == _brd.style && color == _brd.color && ptWidth == _brd.ptWidth );
	}
	bool operator!=( const Border _brd ) const {
	    return ( style != _brd.style || color != _brd.color || ptWidth != _brd.ptWidth );
	}
    };

    struct Counter
    {
	unsigned int counterDepth;
	QChar counterBullet;
	QString counterLeftText;
	QString counterRightText;
	CounterType counterType;
	QString startCounter;
	NumType numberingType;
	QString bulletFont;
    };

    KWParagLayout( KWordDocument *_doc, bool _add = true,
		   QString _name = "Standard" );
    ~KWParagLayout();

    KWParagLayout& operator=( const KWParagLayout &_layout );

    void setFormat( const KWFormat &_format );
    void setFirstLineLeftIndent( KWUnit _i ) { firstLineLeftIndent = _i; }
    void setLeftIndent( KWUnit _i ) { leftIndent = _i; }
    void setParagFootOffset( KWUnit _i ) { paragFootOffset = _i; }
    void setParagHeadOffset( KWUnit _i ) { paragHeadOffset = _i; }
    void setLineSpacing( KWUnit _i ) { lineSpacing = _i; }
    void setName( const QString& _n ) { name = _n; }
    void setFlow( Flow _f ) { flow = _f; }
    void setCounterLeftText( const QString& _t )
    { counter.counterLeftText = _t; }
    void setCounterRightText( const QString& _t )
    { counter.counterRightText = _t; }
    void setCounterType( CounterType _t ) { counter.counterType = _t; }
    void setCounterBullet( QChar _b ) { counter.counterBullet = _b; }
    void setCounterDepth( unsigned int _d ) { counter.counterDepth = _d; }
    void setFollowingParagLayout( const QString& _paragname );

    void setLeftBorder( Border _brd ) { left = _brd; }
    void setRightBorder( Border _brd ) { right = _brd; }
    void setTopBorder( Border _brd ) { top = _brd; }
    void setBottomBorder( Border _brd ) { bottom = _brd; }

    void setStartCounter( const QString& _s ) { counter.startCounter = _s; }
    QString getStartCounter() const { return counter.startCounter; }

    void setNumberingType( NumType _t ) { counter.numberingType = _t; }
    NumType getNumberingType() const { return counter.numberingType; }

    void setBulletFont( const QString& _f ) { counter.bulletFont = _f; }
    QString getBulletFont() const { return counter.bulletFont; }

    const KWFormat* getFormat() { return format; }
    QString getName() const { return name; }
    KWUnit getFirstLineLeftIndent() const { return firstLineLeftIndent; }
    KWUnit getLeftIndent() const { return leftIndent; }
    KWUnit getParagFootOffset() const { return paragFootOffset; }
    KWUnit getParagHeadOffset() const { return paragHeadOffset; }
    KWUnit getLineSpacing() const { return lineSpacing; }
    Flow getFlow() const { return flow; }
    Border getLeftBorder() const { return left; }
    Border getRightBorder() const { return right; }
    Border getTopBorder() const { return top; }
    Border getBottomBorder() const { return bottom; }
    CounterType getCounterType() const { return counter.counterType; }
    QChar getCounterBullet() const { return counter.counterBullet; }
    int getCounterDepth() const { return counter.counterDepth; }
    QString getCounterLeftText() const { return counter.counterLeftText; }
    QString getCounterRightText() const { return counter.counterRightText.data(); }

    QString getFollowingParagLayout() { return followingParagLayout; }

    QDomElement save( QDOM::Document& );
    bool load( const QDomElement& );

    Counter getCounter() const { return counter; }
    void setCounter( Counter _counter ) { counter = _counter; }

    const QList<KoTabulator> *getTabList() const { return &tabList; }
    void setTabList( const QList<KoTabulator> *tabList );

    bool getNextTab( unsigned int _ptPos, unsigned int _lBorder, unsigned int _rBorder, unsigned int &_tabPos, KoTabulators &_tabType );
    bool hasSpecialTabs() const { return specialTabs; }

protected:
    KWFormat *format;

    Flow flow;
    KWUnit paragFootOffset;
    KWUnit paragHeadOffset;
    KWUnit firstLineLeftIndent;
    KWUnit leftIndent;
    KWUnit lineSpacing;
    Border left, right, top, bottom;

    Counter counter;
    QString followingParagLayout;
    QString name;
    KWordDocument *document;
    QList<KoTabulator> tabList;
    bool specialTabs;

};

#endif






