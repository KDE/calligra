/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998		  */
/* Version: 0.1.0						  */
/* Author: Reginald Stadlbauer					  */
/* E-Mail: reggie@kde.org					  */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs			  */
/* needs c++ library Qt (http://www.troll.no)			  */
/* written for KDE (http://www.kde.org)				  */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)	  */
/* needs OpenParts and Kom (weis@kde.org)			  */
/* License: GNU GPL						  */
/******************************************************************/
/* Module: Part Object						  */
/******************************************************************/

#include "kppartobject.h"
#include "kpresenter_doc.h"
#include "kpresenter_view.h"
#include "kpgradient.h"

#include <qpicture.h>
#include <qwidget.h>

/******************************************************************/
/* Class: KPPartObject						  */
/******************************************************************/

/*======================== constructor ===========================*/
KPPartObject::KPPartObject( KPresenterChild *_child )
    : KPObject()
{
    child = _child;
    frame = 0L;
    brush = Qt::NoBrush;
    gradient = 0;
    fillType = FT_BRUSH;
    gType = BCT_GHORZ;
    pen = QPen( Qt::black, 1, Qt::NoPen );
    gColor1 = Qt::red;
    gColor2 = Qt::green;
    _enableDrawing = true;
    getNewPic = false;
    unbalanced = FALSE;
    xfactor = 100;
    yfactor = 100;
}

/*================================================================*/
KPPartObject::~KPPartObject()
{
    if ( frame )
    {
	frame->detach();
	delete frame;
    }
}

/*================================================================*/
KPPartObject &KPPartObject::operator=( const KPPartObject & )
{
    return *this;
}

/*======================== draw ==================================*/
void KPPartObject::draw( QPainter *_painter, int _diffx, int _diffy )
{
    if ( move )
    {
	KPObject::draw( _painter, _diffx, _diffy );
	return;
    }

    int ox = orig.x() - _diffx;
    int oy = orig.y() - _diffy;
    int ow = ext.width();
    int oh = ext.height();
    QRect r;

    int penw = pen.width();

    _painter->save();

    r = _painter->viewport();
    _painter->setViewport( ox, oy, r.width(), r.height() );

    if ( angle == 0 )
    {
	_painter->setPen( Qt::NoPen );
	_painter->setBrush( brush );
	if ( fillType == FT_BRUSH || !gradient )
	    _painter->drawRect( penw, penw, ext.width() - 2 * penw, ext.height() - 2 * penw );
	else
	    _painter->drawPixmap( penw, penw, *gradient->getGradient(), 0, 0, ow - 2 * penw, oh - 2 * penw );

	_painter->setPen( pen );
	_painter->setBrush( Qt::NoBrush );
	_painter->drawRect( penw, penw, ow - 2 * penw, oh - 2 * penw );

	paint( _painter );
    }
    else
    {
	QRect br = QRect( 0, 0, ow, oh );
	int pw = br.width();
	int ph = br.height();
	QRect rr = br;
	int yPos = -rr.y();
	int xPos = -rr.x();
	rr.moveTopLeft( QPoint( -rr.width() / 2, -rr.height() / 2 ) );

	QWMatrix m, mtx, m2;
	mtx.rotate( angle );
	m.translate( pw / 2, ph / 2 );
	m2.translate( rr.left() + xPos, rr.top() + yPos );
	m = m2 * mtx * m;

	_painter->setWorldMatrix( m );

	_painter->setPen( Qt::NoPen );
	_painter->setBrush( brush );

	if ( fillType == FT_BRUSH || !gradient )
	    _painter->drawRect(penw, penw, ext.width() - 2 * penw, ext.height() - 2 * penw );
	else
	    _painter->drawPixmap( penw, penw, *gradient->getGradient(), 0, 0, ow - 2 * penw, oh - 2 * penw );

	_painter->setPen( pen );
	_painter->setBrush( Qt::NoBrush );
	_painter->drawRect( penw, penw, ow - 2 * penw, oh - 2 * penw );

	paint( _painter );
    }

    _painter->setViewport( r );

    _painter->restore();

    KPObject::draw( _painter, _diffx, _diffy );
}

/*================================================================*/
void KPPartObject::paint( QPainter *_painter )
{
    if ( !_enableDrawing ) return;

    QPicture* pic;
    pic = child->draw( ( zoomed ? presFakt : 1.0 ), ( zoomed ? true : getNewPic ) );
    getNewPic = false;

    _painter->setPen( pen );
    _painter->setBrush( brush );

    _painter->drawPicture( *pic );
}

/*================================================================*/
void KPPartObject::activate( QWidget *_widget, int diffx, int diffy )
{
    if ( !frame ) {
	frame = new KPresenterFrame( dynamic_cast<KPresenterView*>( _widget ), child );
	frame->attachView( view );
	frame->show();
    }
    frame->setGeometry( orig.x() - diffx + 20, orig.y() - diffy + 20, ext.width(), ext.height() );
    frame->view()->mainWindow()->setActivePart( frame->view()->id() );
    frame->setFocus();
    
    parentID = dynamic_cast<KPresenterView*>( _widget )->getID();
}

/*================================================================*/
void KPPartObject::deactivate()
{
    if ( frame ) {
	frame->view()->mainWindow()->setActivePart( parentID );
	// HACK!
	frame->setGeometry( -10, -10, 1, 1 );
    }

    getNewPic = true;
}

/*================================================================*/
void KPPartObject::setSize( int _width, int _height )
{
    KPObject::setSize( _width, _height );

    if ( !zoomed )
	child->setGeometry( QRect( orig.x(), orig.y(), ext.width(), ext.height() ) );

    if ( move ) return;

    if ( fillType == FT_GRADIENT && gradient )
	gradient->setSize( getSize() );

    getNewPic = true;
}

/*================================================================*/
void KPPartObject::resizeBy( int _dx, int _dy )
{
    KPObject::resizeBy( _dx, _dy );

    if ( !zoomed )
	child->setGeometry( QRect( orig.x(), orig.y(), ext.width(), ext.height() ) );

    if ( move ) return;

    if ( fillType == FT_GRADIENT && gradient )
	gradient->setSize( getSize() );

    getNewPic = true;
}

/*================================================================*/
void KPPartObject::setOrig( int _x, int _y )
{
    KPObject::setOrig( _x, _y );
    child->setGeometry( QRect( orig.x(), orig.y(), ext.width(), ext.height() ) );
}

/*================================================================*/
void KPPartObject::moveBy( int _dx, int _dy )
{
    KPObject::moveBy( _dx, _dy );
    child->setGeometry( QRect( orig.x(), orig.y(), ext.width(), ext.height() ) );
}

/*================================================================*/
void KPPartObject::setFillType( FillType _fillType )
{
    fillType = _fillType;

    if ( fillType == FT_BRUSH && gradient )
    {
	delete gradient;
	gradient = 0;
    }
    if ( fillType == FT_GRADIENT && !gradient )
	gradient = new KPGradient( gColor1, gColor2, gType, getSize(), unbalanced, xfactor, yfactor );
}

/*================================================================*/
void KPPartObject::save( ostream& out )
{
    out << indent << "<EFFECTS effect=\"" << static_cast<int>( effect ) << "\" effect2=\""
	<< static_cast<int>( effect2 ) << "\"/>" << endl;
    out << indent << "<PEN red=\"" << pen.color().red() << "\" green=\"" << pen.color().green()
	<< "\" blue=\"" << pen.color().blue() << "\" width=\"" << pen.width()
	<< "\" style=\"" << static_cast<int>( pen.style() ) << "\"/>" << endl;
    out << indent << "<BRUSH red=\"" << brush.color().red() << "\" green=\"" << brush.color().green()
	<< "\" blue=\"" << brush.color().blue() << "\" style=\"" << static_cast<int>( brush.style() ) << "\"/>" << endl;
    out << indent << "<PRESNUM value=\"" << presNum << "\"/>" << endl;
    out << indent << "<ANGLE value=\"" << angle << "\"/>" << endl;
    out << indent << "<FILLTYPE value=\"" << static_cast<int>( fillType ) << "\"/>" << endl;
    out << indent << "<GRADIENT red1=\"" << gColor1.red() << "\" green1=\"" << gColor1.green()
	<< "\" blue1=\"" << gColor1.blue() << "\" red2=\"" << gColor2.red() << "\" green2=\""
	<< gColor2.green() << "\" blue2=\"" << gColor2.blue() << "\" type=\""
	<< static_cast<int>( gType ) << "\" unbalanced=\"" << unbalanced << "\" xfactor=\"" << xfactor
	<< "\" yfactor=\"" << yfactor << "\"/>" << endl;
    out << indent << "<DISAPPEAR effect=\"" << static_cast<int>( effect3 ) << "\" doit=\"" << static_cast<int>( disappear )
	<< "\" num=\"" << disappearNum << "\"/>" << endl;
}

/*========================== load ================================*/
void KPPartObject::load( KOMLParser& parser, vector<KOMLAttrib>& lst )
{
    string tag;
    string name;

    while ( parser.open( 0L, tag ) )
    {
	KOMLParser::parseTag( tag.c_str(), name, lst );

	// effects
	if ( name == "EFFECTS" )
	{
	    KOMLParser::parseTag( tag.c_str(), name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "effect" )
		    effect = ( Effect )atoi( ( *it ).m_strValue.c_str() );
		if ( ( *it ).m_strName == "effect2" )
		    effect2 = ( Effect2 )atoi( ( *it ).m_strValue.c_str() );
	    }
	}

	// pen
	else if ( name == "PEN" )
	{
	    KOMLParser::parseTag( tag.c_str(), name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "red" )
		    pen.setColor( QColor( atoi( ( *it ).m_strValue.c_str() ), pen.color().green(), pen.color().blue() ) );
		if ( ( *it ).m_strName == "green" )
		    pen.setColor( QColor( pen.color().red(), atoi( ( *it ).m_strValue.c_str() ), pen.color().blue() ) );
		if ( ( *it ).m_strName == "blue" )
		    pen.setColor( QColor( pen.color().red(), pen.color().green(), atoi( ( *it ).m_strValue.c_str() ) ) );
		if ( ( *it ).m_strName == "width" )
		    pen.setWidth( atoi( ( *it ).m_strValue.c_str() ) );
		if ( ( *it ).m_strName == "style" )
		    pen.setStyle( ( Qt::PenStyle )atoi( ( *it ).m_strValue.c_str() ) );
	    }
	    setPen( pen );
	}

	// brush
	else if ( name == "BRUSH" )
	{
	    KOMLParser::parseTag( tag.c_str(), name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "red" )
		    brush.setColor( QColor( atoi( ( *it ).m_strValue.c_str() ), brush.color().green(), brush.color().blue() ) );
		if ( ( *it ).m_strName == "green" )
		    brush.setColor( QColor( brush.color().red(), atoi( ( *it ).m_strValue.c_str() ), brush.color().blue() ) );
		if ( ( *it ).m_strName == "blue" )
		    brush.setColor( QColor( brush.color().red(), brush.color().green(), atoi( ( *it ).m_strValue.c_str() ) ) );
		if ( ( *it ).m_strName == "style" )
		    brush.setStyle( ( Qt::BrushStyle )atoi( ( *it ).m_strValue.c_str() ) );
	    }
	    setBrush( brush );
	}

	// disappear
	else if ( name == "DISAPPEAR" )
	{
	    KOMLParser::parseTag( tag.c_str(), name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "effect" )
		    effect3 = ( Effect3 )atoi( ( *it ).m_strValue.c_str() );
		if ( ( *it ).m_strName == "doit" )
		    disappear = ( bool )atoi( ( *it ).m_strValue.c_str() );
		if ( ( *it ).m_strName == "num" )
		    disappearNum = atoi( ( *it ).m_strValue.c_str() );
	    }
	}

	// angle
	else if ( name == "ANGLE" )
	{
	    KOMLParser::parseTag( tag.c_str(), name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "value" )
		    angle = atof( ( *it ).m_strValue.c_str() );
	    }
	}

	// presNum
	else if ( name == "PRESNUM" )
	{
	    KOMLParser::parseTag( tag.c_str(), name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "value" )
		    presNum = atoi( ( *it ).m_strValue.c_str() );
	    }
	}

	// fillType
	else if ( name == "FILLTYPE" )
	{
	    KOMLParser::parseTag( tag.c_str(), name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "value" )
		    fillType = static_cast<FillType>( atoi( ( *it ).m_strValue.c_str() ) );
	    }
	    setFillType( fillType );
	}

	// gradient
	else if ( name == "GRADIENT" )
	{
	    KOMLParser::parseTag( tag.c_str(), name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "red1" )
		    gColor1 = QColor( atoi( ( *it ).m_strValue.c_str() ), gColor1.green(), gColor1.blue() );
		if ( ( *it ).m_strName == "green1" )
		    gColor1 = QColor( gColor1.red(), atoi( ( *it ).m_strValue.c_str() ), gColor1.blue() );
		if ( ( *it ).m_strName == "blue1" )
		    gColor1 = QColor( gColor1.red(), gColor1.green(), atoi( ( *it ).m_strValue.c_str() ) );
		if ( ( *it ).m_strName == "red2" )
		    gColor2 = QColor( atoi( ( *it ).m_strValue.c_str() ), gColor2.green(), gColor2.blue() );
		if ( ( *it ).m_strName == "green2" )
		    gColor2 = QColor( gColor2.red(), atoi( ( *it ).m_strValue.c_str() ), gColor2.blue() );
		if ( ( *it ).m_strName == "blue2" )
		    gColor2 = QColor( gColor2.red(), gColor2.green(), atoi( ( *it ).m_strValue.c_str() ) );
		if ( ( *it ).m_strName == "type" )
		    gType = static_cast<BCType>( atoi( ( *it ).m_strValue.c_str() ) );
		if ( ( *it ).m_strName == "unbalanced" )
		    unbalanced = static_cast<bool>( atoi( ( *it ).m_strValue.c_str() ) );
		if ( ( *it ).m_strName == "xfactor" )
		    xfactor = atoi( ( *it ).m_strValue.c_str() );
		if ( ( *it ).m_strName == "yfactor" )
		    yfactor = atoi( ( *it ).m_strValue.c_str() );
	    }
	    setGColor1( gColor1 );
	    setGColor2( gColor2 );
	    setGType( gType );
	    setGUnbalanced( unbalanced );
	    setGXFactor( xfactor );
	    setGYFactor( yfactor );
	}

	else
	    cerr << "Unknown tag '" << tag << "' in PART_OBJECT" << endl;

	if ( !parser.close( tag ) )
	{
	    cerr << "ERR: Closing Child" << endl;
	    return;
	}
    }
}
