#include "paraglayout.h"
#include "kword_doc.h"
#include "defs.h"
#include "kword_utils.h"

#include <komlMime.h>

#include <strstream>
#include <fstream>

#include <unistd.h>

#include <qfont.h>
#include <qlist.h>

/******************************************************************/
/* Class: KWParagLayout						  */
/******************************************************************/

/*================================================================*/
KWParagLayout::KWParagLayout( KWordDocument *_doc, bool _add, QString _name )
    : format( 0 ), paragFootOffset(), paragHeadOffset(),
      firstLineLeftIndent(), leftIndent(), lineSpacing()
{
    flow = LEFT;
    counter.counterType = CT_NONE;
    counter.counterDepth = 0;
    counter.counterBullet = QChar( '·' );
    counter.counterLeftText = "";
    counter.counterRightText = "";
    followingParagLayout = "Standard";
    name = _name;
    counter.startCounter = "0";
    counter.numberingType = NT_LIST;
    counter.bulletFont = "symbol";

    left.color = Qt::white;
    left.style = SOLID;
    left.ptWidth = 0;
    right.color = Qt::white;
    right.style = SOLID;
    right.ptWidth = 0;
    top.color = Qt::white;
    top.style = SOLID;
    top.ptWidth = 0;
    bottom.color = Qt::white;
    bottom.style = SOLID;
    bottom.ptWidth = 0;

    document = _doc;
    if ( _add )
	document->paragLayoutList.append( this );

    tabList.setAutoDelete( false );
    specialTabs = false;

    KWFormat f( _doc );
    f.setDefaults();
    format = _doc->getFormatCollection()->getFormat( f );
}

/*================================================================*/
KWParagLayout::~KWParagLayout()
{
    document->paragLayoutList.setAutoDelete( true );
    document->paragLayoutList.removeRef( this );
    document->paragLayoutList.setAutoDelete( false );
}

/*================================================================*/
KWParagLayout& KWParagLayout::operator=( const KWParagLayout &_layout )
{
    flow = _layout.flow;
    paragFootOffset = _layout.paragFootOffset;
    paragHeadOffset = _layout.paragHeadOffset;
    firstLineLeftIndent = _layout.firstLineLeftIndent;
    leftIndent = _layout.leftIndent;
    //rightIndent = _layout.rightIndent;
    counter.counterType = _layout.counter.counterType;
    counter.counterDepth = _layout.counter.counterDepth;
    counter.counterBullet = _layout.counter.counterBullet;
    counter.counterLeftText = _layout.counter.counterLeftText;
    counter.counterRightText = _layout.counter.counterRightText;
    followingParagLayout = _layout.followingParagLayout;
    lineSpacing = _layout.lineSpacing;
    counter.startCounter = _layout.counter.startCounter;
    counter.numberingType = _layout.counter.numberingType;
    counter.bulletFont = _layout.counter.bulletFont;
    name = _layout.name;

    left = _layout.left;
    right = _layout.right;
    top = _layout.top;
    bottom = _layout.bottom;

    setFormat( _layout.format );

    tabList.setAutoDelete( true );
    tabList.clear();
    tabList.setAutoDelete( false );
    specialTabs = false;
    const QList<KoTabulator> *_tabList = &_layout.tabList;
    setTabList( _tabList );

    return *this;
}

/*================================================================*/
void KWParagLayout::setFollowingParagLayout( const QString& _name )
{
    followingParagLayout = _name;
}

/*================================================================*/
void KWParagLayout::setFormat( const KWFormat &_f )
{
    if ( format )
	format->decRef();
    format = 0;
    format = doc->getFormatCollection()->getFormat( f );
}

/*================================================================*/
QDOM::Element KWParagLayout::save( const QDOM::Document& doc )
{
    QDOM::Element layout = doc.createElement( "PARAGLAYOUT" );
    layout.setAttribute( "name", name );
    layout.setAttribute( "following-parag-layout", followingParagLayout );
    layout.setAttribute( "flow", (int)flow );
    layout.setAttribute( "head-offset", paragHeadOffset );
    layout.setAttribute( "foot-offset", paragFootOffset );
    layout.setAttribute( "first-line-left-indent", firstLineLeftIndent );
    layout.setAttribute( "left-indent", leftIndent );
    layout.setAttribute( "line-spacing", lineSpacing );

    QDOM::Element c = doc.createElement( "COUNTER" );
    layout.appendChild( c );
    c.setAttribute( "type", (int)counter.counterType );
    c.setAttribute( "depth", (counter.counterDepth ) );
    c.setAttribute( "bullet", (int)counter.counterBullet.unicode() );
    c.setAttribute( "start", counter.startCounter );
    c.setAttribute( "numbering-type", (int)counter.numberingType );
    c.setAttribute( "left-text", counter.counterLeftText );
    c.setAttribute( "right-text", counter.counterRightText );
    c.setAttribute( "bullet-font", counter.bulletFont );

    QDOM::Element b = doc.createElement( "LEFTBORDER", left.color() )
		      b.setAttribute( "width", left.ptWidth );
    b.setAttribute( "style", left.style );
    layout.appendChild( b );

    QDOM::Element b = doc.createElement( "RIGHTBORDER", right.color() )
		      b.setAttribute( "width", right.ptWidth );
    b.setAttribute( "style", right.style );
    layout.appendChild( b );

    QDOM::Element b = doc.createElement( "TOPBORDER", top.color() )
		      b.setAttribute( "width", top.ptWidth );
    b.setAttribute( "style", top.style );
    layout.appendChild( b );

    QDOM::Element b = doc.createElement( "BOTTOMBORDER", bottom.color() )
		      b.setAttribute( "width", bottom.ptWidth );
    b.setAttribute( "style", bottom.style );
    layout.appendChild( b );

    // TOOD: Use only the id of the format
    QDOM::Element f = format.save( doc );
    if ( f.isNull() )
	return f;
    layout.appendChild( f );

    for ( unsigned int i = 0; i < tabList.count(); i++ )
    {
	QDOM::Element tab = doc.createElement( "TABULATOR" );
	if ( tab.isNull() )
	    return tab;
	layout.appendChild( tab );
	tab.setAttribute( "mmpos", tabList.at( i )->mmPos );
	tab.setAttribute( "ptpos", tabList.at( i )->ptPos );
	tab.setAttribute( "inchpos", tabList.at( i )->inchPos );
	tab.setAttribute( "type", (int)tabList.at( i )->type );
    }

    return layout;
}

/*================================================================*/
bool KWParagLayout::load( QDOM::Element& layout )
{
}

/*================================================================*/
void KWParagLayout::setTabList( const QList<KoTabulator> *_tabList )
{
    tabList.setAutoDelete( true );
    tabList.clear();
    tabList.setAutoDelete( false );
    specialTabs = false;

    QListIterator<KoTabulator> it(*_tabList);
    for ( it.toFirst(); it.current(); ++it )
    {
	KoTabulator *t = new KoTabulator;
	t->type = it.current()->type;
	t->mmPos = it.current()->mmPos;
	t->ptPos = it.current()->ptPos;
	t->inchPos = it.current()->inchPos;
	tabList.append( t );
	if ( t->type != T_LEFT ) specialTabs = true;
    }
}

/*================================================================*/
bool KWParagLayout::getNextTab( unsigned int _ptPos, unsigned int _lBorder, unsigned int _rBorder, unsigned int &_tabPos, KoTabulators &_tabType )
{
    _tabPos = 0;
    _tabType = T_LEFT;

    if ( tabList.isEmpty() ) return false;

    int _mostLeft = -1, _best = -1;
    unsigned int ptPos = 0;

    for ( unsigned int i = 0; i < tabList.count(); i++ )
    {
	ptPos = tabList.at( i )->ptPos + _lBorder;
	if ( ptPos > _ptPos && ptPos < _rBorder && ( _best == -1 || ptPos < static_cast<unsigned int>( tabList.at( _best )->ptPos ) ) )
	    _best = i;
	if ( ptPos <= _ptPos && ptPos > _lBorder && ( _mostLeft == -1 || ptPos < static_cast<unsigned int>( tabList.at( _mostLeft )->ptPos ) ) )
	    _mostLeft = i;
    }

    if ( _best != -1 )
    {
	_tabPos = tabList.at( _best )->ptPos + _lBorder;
	_tabType = tabList.at( _best )->type;
	return true;
    }

    if ( _mostLeft != -1 )
    {
	_tabPos = tabList.at( _mostLeft )->ptPos + _lBorder;
	_tabType = tabList.at( _mostLeft )->type;
	return true;
    }

    return false;
}
