/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "kpclipartobject.h"
#include "kpgradient.h"

#include <qpainter.h>
#include <qwmatrix.h>
#include <qfileinfo.h>
#include <iostream>
using namespace std;

/******************************************************************/
/* Class: KPClipartObject					 */
/******************************************************************/

/*================ default constructor ===========================*/
KPClipartObject::KPClipartObject( KPClipartCollection *_clipartCollection )
    : KPObject()
{
    brush = Qt::NoBrush;
    gradient = 0;
    fillType = FT_BRUSH;
    gType = BCT_GHORZ;
    redrawPix = false;
    pen = QPen( Qt::black, 1, Qt::NoPen );
    gColor1 = Qt::red;
    gColor2 = Qt::green;
    clipartCollection = _clipartCollection;
    picture = 0L;
    unbalanced = false;
    xfactor = 100;
    yfactor = 100;
}

/*================== overloaded constructor ======================*/
KPClipartObject::KPClipartObject( KPClipartCollection *_clipartCollection, const QString &_filename, QDateTime _lastModified )
    : KPObject()
{
    brush = Qt::NoBrush;
    gradient = 0;
    fillType = FT_BRUSH;
    gType = BCT_GHORZ;
    redrawPix = false;
    pen = QPen( Qt::black, 1, Qt::NoPen );
    gColor1 = Qt::red;
    gColor2 = Qt::green;
    clipartCollection = _clipartCollection;
    unbalanced = false;
    xfactor = 100;
    yfactor = 100;

    if ( !_lastModified.isValid() )
    {
	QFileInfo inf( _filename );
	_lastModified = inf.lastModified();
    }

    picture = 0L;
    setFileName( _filename, _lastModified );
}

/*================================================================*/
KPClipartObject &KPClipartObject::operator=( const KPClipartObject & )
{
    return *this;
}

/*================================================================*/
void KPClipartObject::setFileName( const QString &_filename, QDateTime _lastModified )
{
    if ( !_lastModified.isValid() )
    {
	QFileInfo inf( _filename );
	_lastModified = inf.lastModified();
    }

    if ( picture )
	clipartCollection->removeRef( key );

    key = KPClipartCollection::Key( _filename, _lastModified );
    picture = clipartCollection->findClipart( key );
}

/*================================================================*/
void KPClipartObject::setFillType( FillType _fillType )
{
    fillType = _fillType;

    if ( fillType == FT_BRUSH && gradient )
    {
	delete gradient;
	gradient = 0;
    }
    if ( fillType == FT_GRADIENT && !gradient )
    {
	gradient = new KPGradient( gColor1, gColor2, gType, getSize(), unbalanced, xfactor, yfactor );
	redrawPix = true;
	pix.resize( getSize() );
    }
}

/*================================================================*/
void KPClipartObject::setSize( int _width, int _height )
{
    KPObject::setSize( _width, _height );
    if ( move ) return;

    if ( fillType == FT_GRADIENT && gradient )
    {
	gradient->setSize( getSize() );
	redrawPix = true;
	pix.resize( getSize() );
    }
}

/*================================================================*/
void KPClipartObject::resizeBy( int _dx, int _dy )
{
    KPObject::resizeBy( _dx, _dy );
    if ( move ) return;

    if ( fillType == FT_GRADIENT && gradient )
    {
	gradient->setSize( getSize() );
	redrawPix = true;
	pix.resize( getSize() );
    }
}

/*========================= save =================================*/
void KPClipartObject::save( QTextStream& out )
{
    out << indent << "<ORIG x=\"" << orig.x() << "\" y=\"" << orig.y() << "\"/>" << endl;
    out << indent << "<SIZE width=\"" << ext.width() << "\" height=\"" << ext.height() << "\"/>" << endl;
    out << indent << "<SHADOW distance=\"" << shadowDistance << "\" direction=\""
	<< static_cast<int>( shadowDirection ) << "\" red=\"" << shadowColor.red() << "\" green=\"" << shadowColor.green()
	<< "\" blue=\"" << shadowColor.blue() << "\"/>" << endl;
    out << indent << "<EFFECTS effect=\"" << static_cast<int>( effect ) << "\" effect2=\""
	<< static_cast<int>( effect2 ) << "\"/>" << endl;
    out << indent << "<PRESNUM value=\"" << presNum << "\"/>" << endl;
    out << indent << "<ANGLE value=\"" << angle << "\"/>" << endl;
    out << indent << "<KEY " << key << " />" << endl;
    out << indent << "<FILLTYPE value=\"" << static_cast<int>( fillType ) << "\"/>" << endl;
    out << indent << "<GRADIENT red1=\"" << gColor1.red() << "\" green1=\"" << gColor1.green()
	<< "\" blue1=\"" << gColor1.blue() << "\" red2=\"" << gColor2.red() << "\" green2=\""
	<< gColor2.green() << "\" blue2=\"" << gColor2.blue() << "\" type=\""
	<< static_cast<int>( gType ) << "\" unbalanced=\"" << unbalanced << "\" xfactor=\"" << xfactor
	<< "\" yfactor=\"" << yfactor << "\"/>" << endl;
    out << indent << "<PEN red=\"" << pen.color().red() << "\" green=\"" << pen.color().green()
	<< "\" blue=\"" << pen.color().blue() << "\" width=\"" << pen.width()
	<< "\" style=\"" << static_cast<int>( pen.style() ) << "\"/>" << endl;
    out << indent << "<BRUSH red=\"" << brush.color().red() << "\" green=\"" << brush.color().green()
	<< "\" blue=\"" << brush.color().blue() << "\" style=\"" << static_cast<int>( brush.style() ) << "\"/>" << endl;
    out << indent << "<DISAPPEAR effect=\"" << static_cast<int>( effect3 ) << "\" doit=\"" << static_cast<int>( disappear )
	<< "\" num=\"" << disappearNum << "\"/>" << endl;
}

/*========================== load ================================*/
void KPClipartObject::load( KOMLParser& parser, vector<KOMLAttrib>& lst )
{
    string tag;
    string name;

    while ( parser.open( 0L, tag ) )
    {
	parser.parseTag( tag.c_str(), name, lst );

	// orig
	if ( name == "ORIG" )
	{
	    parser.parseTag( tag.c_str(), name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "x" )
		    orig.setX( atoi( ( *it ).m_strValue.c_str() ) );
		if ( ( *it ).m_strName == "y" )
		    orig.setY( atoi( ( *it ).m_strValue.c_str() ) );
	    }
	}

	// size
	else if ( name == "SIZE" )
	{
	    parser.parseTag( tag.c_str(), name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "width" )
		    ext.setWidth( atoi( ( *it ).m_strValue.c_str() ) );
		if ( ( *it ).m_strName == "height" )
		    ext.setHeight( atoi( ( *it ).m_strValue.c_str() ) );
	    }
	}

	// disappear
	else if ( name == "DISAPPEAR" )
	{
	    parser.parseTag( tag.c_str(), name, lst );
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

	// shadow
	else if ( name == "SHADOW" )
	{
	    parser.parseTag( tag.c_str(), name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "distance" )
		    shadowDistance = atoi( ( *it ).m_strValue.c_str() );
		if ( ( *it ).m_strName == "direction" )
		    shadowDirection = ( ShadowDirection )atoi( ( *it ).m_strValue.c_str() );
		if ( ( *it ).m_strName == "red" )
		    shadowColor.setRgb( atoi( ( *it ).m_strValue.c_str() ),
					shadowColor.green(), shadowColor.blue() );
		if ( ( *it ).m_strName == "green" )
		    shadowColor.setRgb( shadowColor.red(), atoi( ( *it ).m_strValue.c_str() ),
					shadowColor.blue() );
		if ( ( *it ).m_strName == "blue" )
		    shadowColor.setRgb( shadowColor.red(), shadowColor.green(),
					atoi( ( *it ).m_strValue.c_str() ) );
	    }
	}

	// effects
	else if ( name == "EFFECTS" )
	{
	    parser.parseTag( tag.c_str(), name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "effect" )
		    effect = ( Effect )atoi( ( *it ).m_strValue.c_str() );
		if ( ( *it ).m_strName == "effect2" )
		    effect2 = ( Effect2 )atoi( ( *it ).m_strValue.c_str() );
	    }
	}

	// angle
	else if ( name == "ANGLE" )
	{
	    parser.parseTag( tag.c_str(), name, lst );
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
	    parser.parseTag( tag.c_str(), name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "value" )
		    presNum = atoi( ( *it ).m_strValue.c_str() );
	    }
	}

	// filename
	else if ( name == "FILENAME" )
	{
	    parser.parseTag( tag.c_str(), name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "filename" )
		    key.filename = ( *it ).m_strValue.c_str();

	    }

	    key.lastModified.setDate( clipartCollection->tmpDate() );
	    key.lastModified.setTime( clipartCollection->tmpTime() );
	}

	// key
	else if ( name == "KEY" )
	{
	    int year, month, day, hour, minute, second, msec;

	    parser.parseTag( tag.c_str(), name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ )
	    {
		if ( ( *it ).m_strName == "filename" )
		    key.filename = ( *it ).m_strValue.c_str();
		else if ( ( *it ).m_strName == "year" )
		    year = atoi( ( *it ).m_strValue.c_str() );
		else if ( ( *it ).m_strName == "month" )
		    month = atoi( ( *it ).m_strValue.c_str() );
		else if ( ( *it ).m_strName == "day" )
		    day = atoi( ( *it ).m_strValue.c_str() );
		else if ( ( *it ).m_strName == "hour" )
		    hour = atoi( ( *it ).m_strValue.c_str() );
		else if ( ( *it ).m_strName == "minute" )
		    minute = atoi( ( *it ).m_strValue.c_str() );
		else if ( ( *it ).m_strName == "second" )
		    second = atoi( ( *it ).m_strValue.c_str() );
		else if ( ( *it ).m_strName == "msec" )
		    msec = atoi( ( *it ).m_strValue.c_str() );
	    }
	    key.lastModified.setDate( QDate( year, month, day ) );
	    key.lastModified.setTime( QTime( hour, minute, second, msec ) );
	}

	// pen
	else if ( name == "PEN" )
	{
	    parser.parseTag( tag.c_str(), name, lst );
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
	    parser.parseTag( tag.c_str(), name, lst );
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

	// fillType
	else if ( name == "FILLTYPE" )
	{
	    parser.parseTag( tag.c_str(), name, lst );
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
	    parser.parseTag( tag.c_str(), name, lst );
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
	    cerr << "Unknown tag '" << tag << "' in CLIPART_OBJECT" << endl;

	if ( !parser.close( tag ) )
	{
	    cerr << "ERR: Closing Child" << endl;
	    return;
	}
    }
}

/*========================= draw =================================*/
void KPClipartObject::draw( QPainter *_painter, int _diffx, int _diffy )
{
    if ( move )
    {
	KPObject::draw( _painter, _diffx, _diffy );
	return;
    }

    if ( !picture )
	return;

    int ox = orig.x() - _diffx;
    int oy = orig.y() - _diffy;
    int ow = ext.width();
    int oh = ext.height();

    _painter->save();
    QRect ov = _painter->viewport();

    _painter->setPen( pen );
    _painter->setBrush( brush );

    int penw = pen.width() / 2;

    _painter->save();
    _painter->setViewport( 0, 0, ov.width(), ov.height() );
    if ( angle == 0 ) {
	_painter->setPen( Qt::NoPen );
	_painter->setBrush( brush );
	if ( fillType == FT_BRUSH || !gradient )
	    _painter->drawRect( ox + penw, oy + penw, ext.width() - 2 * penw, ext.height() - 2 * penw );
	else
	    _painter->drawPixmap( ox + penw, oy + penw, *gradient->getGradient(), 
				  0, 0, ow - 2 * penw, oh - 2 * penw );

	QRect r2 = _painter->viewport();
	_painter->save();
	_painter->setViewport( ox + 1, oy + 1, ext.width() - 2, ext.height() - 2 );
	_painter->drawPicture( *picture );
	_painter->setViewport( r2 );
	_painter->restore();
	
	_painter->setPen( pen );
	_painter->setBrush( Qt::NoBrush );
	_painter->drawRect( ox + penw, oy + penw, ow - 2 * penw, oh - 2 * penw );
    } else {
	QRect r = _painter->viewport();
	_painter->setViewport( ox, oy, r.width(), r.height() );

	QRect br( QPoint( 0, 0 ), ext );
	int pw = br.width();
	int ph = br.height();
	QRect rr = br;
	int pixYPos = -rr.y();
	int pixXPos = -rr.x();
	br.moveTopLeft( QPoint( -br.width() / 2, -br.height() / 2 ) );
	rr.moveTopLeft( QPoint( -rr.width() / 2, -rr.height() / 2 ) );

	QWMatrix m, mtx;
	mtx.rotate( angle );
	m.translate( pw / 2, ph / 2 );
	m = mtx * m;

	QPixmap pm( pw, ph );
	pm.fill( Qt::white );
	QPainter pnt;
	pnt.begin( &pm );
	pnt.drawPicture( *picture );
	pnt.end();

	_painter->setWorldMatrix( m );

	_painter->setPen( Qt::NoPen );
	_painter->setBrush( brush );

	if ( fillType == FT_BRUSH || !gradient )
	    _painter->drawRect( rr.left() + pixXPos + penw, 
				rr.top() + pixYPos + penw, ext.width() - 2 * penw, ext.height() - 2 * penw );
	else
	    _painter->drawPixmap( rr.left() + pixXPos + penw, rr.top() + pixYPos + penw, 
				  *gradient->getGradient(), 0, 0, ow - 2 * penw, oh - 2 * penw );

	_painter->drawPixmap( br.left() + pixXPos, br.top() + pixYPos, pm );

	_painter->setPen( pen );
	_painter->setBrush( Qt::NoBrush );
	_painter->drawRect( rr.left() + pixXPos + penw, rr.top() + pixYPos + penw, ow - 2 * penw, oh - 2 * penw );

	_painter->setViewport( r );
    }

    _painter->setViewport( ov );
    _painter->restore();

    KPObject::draw( _painter, _diffx, _diffy );
}




