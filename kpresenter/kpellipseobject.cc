/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: ellipse object                                         */
/******************************************************************/

#include "kpellipseobject.h"
#include "kpgradient.h"

#include <qregion.h>
#include <qpicture.h>
#include <qpainter.h>
#include <qwmatrix.h>

/******************************************************************/
/* Class: KPEllipseObject                                         */
/******************************************************************/

/*================ default constructor ===========================*/
KPEllipseObject::KPEllipseObject()
	: KPObject(), pen(), brush(), gColor1( Qt::red ), gColor2( Qt::green ), pix()
{
	gradient = 0;
	fillType = FT_BRUSH;
	gType = BCT_GHORZ;
	drawShadow = false;
	redrawPix = false;
}

/*================== overloaded constructor ======================*/
KPEllipseObject::KPEllipseObject( QPen _pen, QBrush _brush, FillType _fillType,
								  QColor _gColor1, QColor _gColor2, BCType _gType )
	: KPObject(), pen( _pen ), brush( _brush ), gColor1( _gColor1 ), gColor2( _gColor2 )
{
	gType = _gType;
	fillType = _fillType;
	redrawPix = false;

	if ( fillType == FT_GRADIENT )
    {
		gradient = new KPGradient( gColor1, gColor2, gType, KSize( 1, 1 ) );
		redrawPix = true;
		pix.resize( getSize() );
    }
	else
		gradient = 0;
	drawShadow = false;
}

/*================================================================*/
void KPEllipseObject::setSize( int _width, int _height )
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
void KPEllipseObject::resizeBy( int _dx, int _dy )
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

/*================================================================*/
void KPEllipseObject::setFillType( FillType _fillType )
{
	fillType = _fillType;

	if ( fillType == FT_BRUSH && gradient )
    {
		delete gradient;
		gradient = 0;
    }
	if ( fillType == FT_GRADIENT && !gradient )
    {
		gradient = new KPGradient( gColor1, gColor2, gType, getSize() );
		redrawPix = true;
		pix.resize( getSize() );
    }
}

/*========================= save =================================*/
void KPEllipseObject::save( ostream& out )
{
	out << indent << "<ORIG x=\"" << orig.x() << "\" y=\"" << orig.y() << "\"/>" << endl;
	out << indent << "<SIZE width=\"" << ext.width() << "\" height=\"" << ext.height() << "\"/>" << endl;
	out << indent << "<SHADOW distance=\"" << shadowDistance << "\" direction=\""
		<< static_cast<int>( shadowDirection ) << "\" red=\"" << shadowColor.red() << "\" green=\"" << shadowColor.green()
		<< "\" blue=\"" << shadowColor.blue() << "\"/>" << endl;
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
		<< static_cast<int>( gType ) << "\"/>" << endl;
}

/*========================== load ================================*/
void KPEllipseObject::load( KOMLParser& parser, vector<KOMLAttrib>& lst )
{
	string tag;
	string name;

	while ( parser.open( 0L, tag ) )
    {
		KOMLParser::parseTag( tag.c_str(), name, lst );
	
		// orig
		if ( name == "ORIG" )
		{
			KOMLParser::parseTag( tag.c_str(), name, lst );
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
			KOMLParser::parseTag( tag.c_str(), name, lst );
			vector<KOMLAttrib>::const_iterator it = lst.begin();
			for( ; it != lst.end(); it++ )
			{
				if ( ( *it ).m_strName == "width" )
					ext.setWidth( atoi( ( *it ).m_strValue.c_str() ) );
				if ( ( *it ).m_strName == "height" )
					ext.setHeight( atoi( ( *it ).m_strValue.c_str() ) );
			}
		}

		// shadow
		else if ( name == "SHADOW" )
		{
			KOMLParser::parseTag( tag.c_str(), name, lst );
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
			}
			setGColor1( gColor1 );
			setGColor2( gColor2 );
			setGType( gType );
		}

		else
			cerr << "Unknown tag '" << tag << "' in ELLIPSE_OBJECT" << endl;

		if ( !parser.close( tag ) )
		{
			cerr << "ERR: Closing Child" << endl;
			return;
		}
    }
}

/*========================= draw =================================*/
void KPEllipseObject::draw( QPainter *_painter, int _diffx, int _diffy )
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
	KRect r;

	_painter->save();

	if ( shadowDistance > 0 )
    {
		drawShadow = true;
		QPen tmpPen( pen );
		pen.setColor( shadowColor );
		QBrush tmpBrush( brush );
		brush.setColor( shadowColor );
		r = _painter->viewport();

		if ( angle == 0 )
		{
			int sx = ox;
			int sy = oy;
			getShadowCoords( sx, sy, shadowDirection, shadowDistance );
			
			_painter->setViewport( sx, sy, r.width(), r.height() );
			paint( _painter );
		}
		else
		{
			_painter->setViewport( ox, oy, r.width(), r.height() );

			KRect br = KRect( 0, 0, ow, oh );
			int pw = br.width();
			int ph = br.height();
			KRect rr = br;
			int yPos = -rr.y();
			int xPos = -rr.x();
			rr.moveTopLeft( KPoint( -rr.width() / 2, -rr.height() / 2 ) );
	
			int sx = 0;
			int sy = 0;
			getShadowCoords( sx, sy, shadowDirection, shadowDistance );

			QWMatrix m, mtx, m2;
			mtx.rotate( angle );
			m.translate( pw / 2, ph / 2 );
			m2.translate( rr.left() + xPos + sx, rr.top() + yPos + sy );
			m = m2 * mtx * m;
			
			_painter->setWorldMatrix( m );
			paint( _painter );
		}

		_painter->setViewport( r );
		pen = tmpPen;
		brush = tmpBrush;
    }

	_painter->restore();
	_painter->save();

	r = _painter->viewport();
	_painter->setViewport( ox, oy, r.width(), r.height() );

	drawShadow = false;

	if ( angle == 0 )
		paint( _painter );
	else
    {
		KRect br = KRect( 0, 0, ow, oh );
		int pw = br.width();
		int ph = br.height();
		KRect rr = br;
		int yPos = -rr.y();
		int xPos = -rr.x();
		rr.moveTopLeft( KPoint( -rr.width() / 2, -rr.height() / 2 ) );

		QWMatrix m, mtx, m2;
		mtx.rotate( angle );
		m.translate( pw / 2, ph / 2 );
		m2.translate( rr.left() + xPos, rr.top() + yPos );
		m = m2 * mtx * m;

		_painter->setWorldMatrix( m );
		paint( _painter );
    }

	_painter->setViewport( r );

	_painter->restore();

	KPObject::draw( _painter, _diffx, _diffy );
}

/*======================== paint =================================*/
void KPEllipseObject::paint( QPainter* _painter )
{
	if ( drawShadow || fillType == FT_BRUSH || !gradient )
    {
		int ow = ext.width();
		int oh = ext.height();

		_painter->setPen( pen );
		int pw = pen.width();
		_painter->setBrush( brush );
		_painter->drawEllipse( pw, pw, ow - 2 * pw, oh - 2 * pw );
    }
	else
    {
		int ow = ext.width();
		int oh = ext.height();
		int pw = pen.width();

		int ox = _painter->viewport().x() + static_cast<int>( _painter->worldMatrix().dx() ) + pw;
		int oy = _painter->viewport().y() + static_cast<int>( _painter->worldMatrix().dy() ) + pw;

		if ( angle == 0 )
		{
			_painter->save();

			QRegion clipregion( ox, oy, ow - 2 * pw, oh - 2 * pw, QRegion::Ellipse );

			if ( _painter->hasClipping() )
				clipregion = _painter->clipRegion().intersect( clipregion );

			_painter->setClipRegion( clipregion );

			_painter->drawPixmap( pw, pw, *gradient->getGradient() );

			_painter->restore();
		}
		else
		{
			if ( redrawPix )
			{
				redrawPix = false;
				QRegion clipregion( 0, 0, ow - 2 * pw, oh - 2 * pw, QRegion::Ellipse );
				QPicture pic;
				QPainter p;
	
				p.begin( &pic );
				p.setClipRegion( clipregion );
				p.drawPixmap( 0, 0, *gradient->getGradient() );
				p.end();
	
				pix.fill( Qt::white );
				QPainter p2;
				p2.begin( &pix );
				p2.drawPicture( pic );
				p2.end();
			}

			_painter->drawPixmap( pw, pw, pix, 0, 0, ow - 2 * pw, oh - 2 * pw );
		}

		_painter->setPen( pen );
		_painter->setBrush( Qt::NoBrush );
		_painter->drawEllipse( pw, pw, ow - 2 * pw, oh - 2 * pw );
    }
}




