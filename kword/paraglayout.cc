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
    : format( _doc ), paragFootOffset(), paragHeadOffset(), firstLineLeftIndent(), leftIndent(), lineSpacing()
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

    format.setDefaults( _doc );

    document = _doc;
    if ( _add )
	document->paragLayoutList.append( this );

    tabList.setAutoDelete( false );
    specialTabs = false;
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

    format = _layout.format;

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
    format = _f;
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
  c.setAttribute( "depth", (counter.counterDepth );
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
  name = layout.attribute( "name" );
  followingParagLayout = layout.attribute( "following-parag-layout" );
  flow = (Flow)layout.attribute( "flow" ).toInt();
  paragHeadOffset = layout.attribute( "head-offset" );
  paragFootOffset = layout.attribute( "foot-offset" );
  firstLineLeftIndent = layout.attribute( "first-line-left-indent" );

	// following parag layout
	else if ( _name == "FOLLOWING" )
	{
	    KOMLParser::parseTag( tag.c_str(), _name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "name" )
		    followingParagLayout = correctQString( ( *it ).m_strValue.c_str() );
	    }
	}

	// following parag layout
	else if ( _name == "TABULATOR" )
	{
	    KOMLParser::parseTag( tag.c_str(), _name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    KoTabulator *tab = new KoTabulator;
	    bool noinch = true;
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "mmpos" )
		    tab->mmPos = atof( ( *it ).m_strValue.c_str() );
		if ( ( *it ).m_strName == "ptpos" )
		    tab->ptPos = atoi( ( *it ).m_strValue.c_str() );
		if ( ( *it ).m_strName == "inchpos" )
		{
		    noinch = false;
		    tab->inchPos = atof( ( *it ).m_strValue.c_str() );
		}
		if ( ( *it ).m_strName == "type" )
		    tab->type = static_cast<KoTabulators>( atoi( ( *it ).m_strValue.c_str() ) );
	    }
	    if ( noinch ) tab->inchPos = MM_TO_INCH( tab->mmPos );
	    tabList.append( tab );
	}


	// head offset
	else if ( _name == "OHEAD" )
	{
	    pt = 0;
	    mm = inch = 0.0;
	    KOMLParser::parseTag( tag.c_str(), _name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "pt" )
		    pt = atoi( ( *it ).m_strValue.c_str() );
		if ( ( *it ).m_strName == "mm" )
		    mm = atof( ( *it ).m_strValue.c_str() );
		if ( ( *it ).m_strName == "inch" )
		    inch = atof( ( *it ).m_strValue.c_str() );
	    }
	    paragHeadOffset.setPT_MM_INCH( pt, mm, inch );
	}

	// foot offset
	else if ( _name == "OFOOT" )
	{
	    pt = 0;
	    mm = inch = 0.0;
	    KOMLParser::parseTag( tag.c_str(), _name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "pt" )
		    pt = atoi( ( *it ).m_strValue.c_str() );
		if ( ( *it ).m_strName == "mm" )
		    mm = atof( ( *it ).m_strValue.c_str() );
		if ( ( *it ).m_strName == "inch" )
		    inch = atof( ( *it ).m_strValue.c_str() );
	    }
	    paragFootOffset.setPT_MM_INCH( pt, mm, inch );
	}

	// first left line indent
	else if ( _name == "IFIRST" )
	{
	    pt = 0;
	    mm = inch = 0.0;
	    KOMLParser::parseTag( tag.c_str(), _name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "pt" )
		    pt = atoi( ( *it ).m_strValue.c_str() );
		if ( ( *it ).m_strName == "mm" )
		    mm = atof( ( *it ).m_strValue.c_str() );
		if ( ( *it ).m_strName == "inch" )
		    inch = atof( ( *it ).m_strValue.c_str() );
	    }
	    firstLineLeftIndent.setPT_MM_INCH( pt, mm, inch );
	}

	// left indent
	else if ( _name == "ILEFT" )
	{
	    pt = 0;
	    mm = inch = 0.0;
	    KOMLParser::parseTag( tag.c_str(), _name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "pt" )
		    pt = atoi( ( *it ).m_strValue.c_str() );
		if ( ( *it ).m_strName == "mm" )
		    mm = atof( ( *it ).m_strValue.c_str() );
		if ( ( *it ).m_strName == "inch" )
		    inch = atof( ( *it ).m_strValue.c_str() );
	    }
	    leftIndent.setPT_MM_INCH( pt, mm, inch );
	}

	// linespacing
	else if ( _name == "LINESPACE" )
	{
	    pt = 0;
	    mm = inch = 0.0;
	    KOMLParser::parseTag( tag.c_str(), _name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "pt" )
		    pt = atoi( ( *it ).m_strValue.c_str() );
		if ( ( *it ).m_strName == "mm" )
		    mm = atof( ( *it ).m_strValue.c_str() );
		if ( ( *it ).m_strName == "inch" )
		    inch = atof( ( *it ).m_strValue.c_str() );
	    }
	    lineSpacing.setPT_MM_INCH( pt, mm, inch );
	}

	// offsets ( old but supported for compatibility )
	else if ( _name == "OFFSETS" )
	{
	    KOMLParser::parseTag( tag.c_str(), _name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "head" )
		    paragHeadOffset.setMM( atof( ( *it ).m_strValue.c_str() ) );
		else if ( ( *it ).m_strName == "foot" )
		    paragFootOffset.setMM( atof( ( *it ).m_strValue.c_str() ) );
	    }
	}

	// indents ( old but supported for compatibility )
	else if ( _name == "INDENTS" )
	{
	    KOMLParser::parseTag( tag.c_str(), _name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "first" )
		    firstLineLeftIndent.setMM( atof( ( *it ).m_strValue.c_str() ) );
		else if ( ( *it ).m_strName == "left" )
		    leftIndent.setMM( atof( ( *it ).m_strValue.c_str() ) );
	    }
	}

	// line spacing ( old but supported for compatibility )
	else if ( _name == "LINESPACING" )
	{
	    KOMLParser::parseTag( tag.c_str(), _name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "value" )
		    lineSpacing.setPT( atoi( ( *it ).m_strValue.c_str() ) );
	    }
	}

	// counter
	else if ( _name == "COUNTER" )
	{
	    KOMLParser::parseTag( tag.c_str(), _name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "type" )
		    counter.counterType = static_cast<CounterType>( atoi( ( *it ).m_strValue.c_str() ) );
		else if ( ( *it ).m_strName == "depth" )
		    counter.counterDepth = atoi( ( *it ).m_strValue.c_str() );
		else if ( ( *it ).m_strName == "bullet" )
		    counter.counterBullet = QChar( static_cast<unsigned short>( atoi( ( *it ).m_strValue.c_str() ) ) );
		else if ( ( *it ).m_strName == "lefttext" )
		    counter.counterLeftText = correctQString( ( *it ).m_strValue.c_str() );
		else if ( ( *it ).m_strName == "righttext" )
		    counter.counterRightText = correctQString( ( *it ).m_strValue.c_str() );
		else if ( ( *it ).m_strName == "start" )
		    counter.startCounter = correctQString( ( *it ).m_strValue.c_str() );
		else if ( ( *it ).m_strName == "numberingtype" )
		    counter.numberingType = static_cast<NumType>( atoi( ( *it ).m_strValue.c_str() ) );
		else if ( ( *it ).m_strName == "bulletfont" )
		    counter.bulletFont = correctQString( ( *it ).m_strValue.c_str() );
	    }
	}

	// left border
	else if ( _name == "LEFTBORDER" )
	{
	    unsigned int r = 0, g = 0, b = 0;
	    KOMLParser::parseTag( tag.c_str(), _name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "red" )
		{
		    r = atoi( ( *it ).m_strValue.c_str() );
		    left.color.setRgb( r, g, b );
		}
		else if ( ( *it ).m_strName == "green" )
		{
		    g = atoi( ( *it ).m_strValue.c_str() );
		    left.color.setRgb( r, g, b );
		}
		else if ( ( *it ).m_strName == "blue" )
		{
		    b = atoi( ( *it ).m_strValue.c_str() );
		    left.color.setRgb( r, g, b );
		}
		else if ( ( *it ).m_strName == "style" )
		    left.style = static_cast<BorderStyle>( atoi( ( *it ).m_strValue.c_str() ) );
		else if ( ( *it ).m_strName == "width" )
		    left.ptWidth = atoi( ( *it ).m_strValue.c_str() );
	    }
	}

	// right border
	else if ( _name == "RIGHTBORDER" )
	{
	    unsigned int r = 0, g = 0, b = 0;
	    KOMLParser::parseTag( tag.c_str(), _name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "red" )
		{
		    r = atoi( ( *it ).m_strValue.c_str() );
		    right.color.setRgb( r, g, b );
		}
		else if ( ( *it ).m_strName == "green" )
		{
		    g = atoi( ( *it ).m_strValue.c_str() );
		    right.color.setRgb( r, g, b );
		}
		else if ( ( *it ).m_strName == "blue" )
		{
		    b = atoi( ( *it ).m_strValue.c_str() );
		    right.color.setRgb( r, g, b );
		}
		else if ( ( *it ).m_strName == "style" )
		    right.style = static_cast<BorderStyle>( atoi( ( *it ).m_strValue.c_str() ) );
		else if ( ( *it ).m_strName == "width" )
		    right.ptWidth = atoi( ( *it ).m_strValue.c_str() );
	    }
	}

	// bottom border
	else if ( _name == "BOTTOMBORDER" )
	{
	    unsigned int r = 0, g = 0, b = 0;
	    KOMLParser::parseTag( tag.c_str(), _name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "red" )
		{
		    r = atoi( ( *it ).m_strValue.c_str() );
		    bottom.color.setRgb( r, g, b );
		}
		else if ( ( *it ).m_strName == "green" )
		{
		    g = atoi( ( *it ).m_strValue.c_str() );
		    bottom.color.setRgb( r, g, b );
		}
		else if ( ( *it ).m_strName == "blue" )
		{
		    b = atoi( ( *it ).m_strValue.c_str() );
		    bottom.color.setRgb( r, g, b );
		}
		else if ( ( *it ).m_strName == "style" )
		    bottom.style = static_cast<BorderStyle>( atoi( ( *it ).m_strValue.c_str() ) );
		else if ( ( *it ).m_strName == "width" )
		    bottom.ptWidth = atoi( ( *it ).m_strValue.c_str() );
	    }
	}

	// top border
	else if ( _name == "TOPBORDER" )
	{
	    unsigned int r = 0, g = 0, b = 0;
	    KOMLParser::parseTag( tag.c_str(), _name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "red" )
		{
		    r = atoi( ( *it ).m_strValue.c_str() );
		    top.color.setRgb( r, g, b );
		}
		else if ( ( *it ).m_strName == "green" )
		{
		    g = atoi( ( *it ).m_strValue.c_str() );
		    top.color.setRgb( r, g, b );
		}
		else if ( ( *it ).m_strName == "blue" )
		{
		    b = atoi( ( *it ).m_strValue.c_str() );
		    top.color.setRgb( r, g, b );
		}
		else if ( ( *it ).m_strName == "style" )
		    top.style = static_cast<BorderStyle>( atoi( ( *it ).m_strValue.c_str() ) );
		else if ( ( *it ).m_strName == "width" )
		    top.ptWidth = atoi( ( *it ).m_strValue.c_str() );
	    }
	}

	else if ( _name == "FORMAT" )
	{
	    KOMLParser::parseTag( tag.c_str(), _name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
	    }
	    format.load( parser, lst, document );
	}

	else
	    cerr << "Unknown tag '" << tag << "' in PARAGRAPHLAYOUT" << endl;

	if ( !parser.close( tag ) )
	{
	    cerr << "ERR: Closing Child" << endl;
	    return;
	}
    }
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
