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
/* Module: line object                                            */
/******************************************************************/

#include "kplineobject.h"
#include "kpresenter_utils.h"

#include <qpainter.h>
#include <qwmatrix.h>

#include <math.h>

/******************************************************************/
/* Class: KPLineObject                                             */
/******************************************************************/

/*================ default constructor ===========================*/
KPLineObject::KPLineObject()
    : KPObject(), pen()
{
    lineBegin = L_NORMAL;
    lineEnd = L_NORMAL;
    lineType = LT_HORZ;
}

/*================== overloaded constructor ======================*/
KPLineObject::KPLineObject( QPen _pen, LineEnd _lineBegin, LineEnd _lineEnd, LineType _lineType )
    : KPObject(), pen( _pen )
{
    lineBegin = _lineBegin;
    lineEnd = _lineEnd;
    lineType = _lineType;
}

/*========================= save =================================*/
void KPLineObject::save( ostream& out )
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
    out << indent << "<LINETYPE value=\"" << static_cast<int>( lineType ) << "\"/>" << endl;
    out << indent << "<LINEBEGIN value=\"" << static_cast<int>( lineBegin ) << "\"/>" << endl;
    out << indent << "<LINEEND value=\"" << static_cast<int>( lineEnd ) << "\"/>" << endl;
    out << indent << "<PRESNUM value=\"" << presNum << "\"/>" << endl;
    out << indent << "<ANGLE value=\"" << angle << "\"/>" << endl;
    out << indent << "<DISAPPEAR effect=\"" << static_cast<int>( effect3 ) << "\" doit=\"" << static_cast<int>( disappear )
        << "\" num=\"" << disappearNum << "\"/>" << endl;
}

/*========================== load ================================*/
void KPLineObject::load( KOMLParser& parser, vector<KOMLAttrib>& lst )
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

        // lineType
        else if ( name == "LINETYPE" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "value" )
                    lineType = ( LineType )atoi( ( *it ).m_strValue.c_str() );
            }
        }

        // lineBegin
        else if ( name == "LINEBEGIN" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "value" )
                    lineBegin = ( LineEnd )atoi( ( *it ).m_strValue.c_str() );
            }
        }

        // lineEnd
        else if ( name == "LINEEND" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "value" )
                    lineEnd = ( LineEnd )atoi( ( *it ).m_strValue.c_str() );
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
        else
            cerr << "Unknown tag '" << tag << "' in LINE_OBJECT" << endl;

        if ( !parser.close( tag ) )
        {
            cerr << "ERR: Closing Child" << endl;
            return;
        }
    }
}

/*========================= draw =================================*/
void KPLineObject::draw( QPainter *_painter, int _diffx, int _diffy )
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

    _painter->save();

    if ( shadowDistance > 0 )
    {
        QPen tmpPen( pen );
        pen.setColor( shadowColor );
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

            QRect br = QRect( 0, 0, ow, oh );
            int pw = br.width();
            int ph = br.height();
            QRect rr = br;
            int yPos = -rr.y();
            int xPos = -rr.x();
            rr.moveTopLeft( QPoint( -rr.width() / 2, -rr.height() / 2 ) );

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
    }

    _painter->restore();
    _painter->save();

    r = _painter->viewport();
    _painter->setViewport( ox, oy, r.width(), r.height() );

    if ( angle == 0 )
        paint( _painter );
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
        paint( _painter );
    }

    _painter->setViewport( r );

    _painter->restore();

    KPObject::draw( _painter, _diffx, _diffy );
}

/*===================== get angle ================================*/
float KPLineObject::getAngle( QPoint p1, QPoint p2 )
{
    float _angle = 0.0;

    if ( p1.x() == p2.x() )
    {
        if ( p1.y() < p2.y() )
            _angle = 270.0;
        else
            _angle = 90.0;
    }
    else
    {
        float x1, x2, y1, y2;

        if ( p1.x() <= p2.x() )
        {
            x1 = p1.x(); y1 = p1.y();
            x2 = p2.x(); y2 = p2.y();
        }
        else
        {
            x2 = p1.x(); y2 = p1.y();
            x1 = p2.x(); y1 = p2.y();
        }

        float m = -( y2 - y1 ) / ( x2 - x1 );
        _angle = atan( m ) * RAD_FACTOR;

        if ( p1.x() < p2.x() )
            _angle = 180.0 - _angle;
        else
            _angle = -_angle;
    }

    return _angle;
}

/*======================== paint =================================*/
void KPLineObject::paint( QPainter* _painter )
{
    int ow = ext.width();
    int oh = ext.height();

    switch ( lineType )
    {
    case LT_HORZ:
    {
        QSize diff1( 0, 0 ), diff2( 0, 0 );
        int _w = pen.width();

        if ( lineBegin != L_NORMAL )
            diff1 = getBoundingSize( lineBegin, _w );

        if ( lineEnd != L_NORMAL )
            diff2 = getBoundingSize( lineEnd, _w );

        if ( lineBegin != L_NORMAL )
            drawFigure( lineBegin, _painter, QPoint( diff1.width() / 2, oh / 2 ), pen.color(), _w, 180.0 );

        if ( lineEnd != L_NORMAL )
            drawFigure( lineEnd, _painter, QPoint( ow - diff2.width() / 2, oh / 2 ), pen.color(), _w, 0.0 );

        _painter->setPen( pen );
        _painter->drawLine( diff1.width() / 2, oh / 2, ow - diff2.width() / 2, oh / 2 );
    } break;
    case LT_VERT:
    {
        QSize diff1( 0, 0 ), diff2( 0, 0 );
        int _w = pen.width();

        if ( lineBegin != L_NORMAL )
            diff1 = getBoundingSize( lineBegin, _w );

        if ( lineEnd != L_NORMAL )
            diff2 = getBoundingSize( lineEnd, _w );

        if ( lineBegin != L_NORMAL )
            drawFigure( lineBegin, _painter, QPoint( ow / 2, diff1.width() / 2 ), pen.color(), _w, 270.0 );

        if ( lineEnd != L_NORMAL )
            drawFigure( lineEnd, _painter, QPoint( ow / 2, oh - diff2.width() / 2 ), pen.color(), _w, 90.0 );

        _painter->setPen( pen );
        _painter->drawLine( ow / 2, diff1.width() / 2, ow / 2, oh - diff2.width() / 2 );
    } break;
    case LT_LU_RD:
    {
        QSize diff1( 0, 0 ), diff2( 0, 0 );
        int _w = pen.width();

        if ( lineBegin != L_NORMAL )
            diff1 = getBoundingSize( lineBegin, _w );

        if ( lineEnd != L_NORMAL )
            diff2 = getBoundingSize( lineEnd, _w );

        QPoint pnt1( diff1.height() / 2 + _w / 2, diff1.width() / 2 + _w / 2 );
        QPoint pnt2( ow - diff2.height() / 2 - _w / 2, oh - diff2.width() / 2 - _w / 2 );
        float _angle;

        _angle = getAngle( pnt1, pnt2 );

        if ( lineBegin != L_NORMAL )
        {
            _painter->save();
            _painter->translate( diff1.height() / 2, diff1.width() / 2 );
            drawFigure( lineBegin, _painter, QPoint( 0, 0 ), pen.color(), _w, _angle );
            _painter->restore();
        }
        if ( lineEnd != L_NORMAL )
        {
            _painter->save();
            _painter->translate( ow - diff2.height() / 2, oh - diff2.width() / 2 );
            drawFigure( lineEnd, _painter, QPoint( 0, 0 ), pen.color(), _w, _angle - 180 );
            _painter->restore();
        }

        _painter->setPen( pen );
        _painter->drawLine( diff1.height() / 2 + _w / 2, diff1.width() / 2 + _w / 2,
                            ow - diff2.height() / 2 - _w / 2, oh - diff2.width() / 2 - _w / 2 );
    } break;
    case LT_LD_RU:
    {
        QSize diff1( 0, 0 ), diff2( 0, 0 );
        int _w = pen.width();

        if ( lineBegin != L_NORMAL )
            diff1 = getBoundingSize( lineBegin, _w );

        if ( lineEnd != L_NORMAL )
            diff2 = getBoundingSize( lineEnd, _w );

        QPoint pnt1( diff1.height() / 2 + _w / 2, oh - diff1.width() / 2 - _w / 2 );
        QPoint pnt2( ow - diff2.height() / 2 - _w / 2, diff2.width() / 2 + _w / 2 );
        float _angle;

        _angle = getAngle( pnt1, pnt2 );

        if ( lineBegin != L_NORMAL )
        {
            _painter->save();
            _painter->translate( diff1.height() / 2, oh - diff1.width() / 2 );
            drawFigure( lineBegin, _painter, QPoint( 0, 0 ), pen.color(), _w, _angle );
            _painter->restore();
        }
        if ( lineEnd != L_NORMAL )
        {
            _painter->save();
            _painter->translate( ow - diff2.height() / 2, diff2.width() / 2 );
            drawFigure( lineEnd, _painter, QPoint( 0, 0 ), pen.color(), _w, _angle - 180 );
            _painter->restore();
        }

        _painter->setPen( pen );
        _painter->drawLine( diff1.height() / 2 + _w / 2, oh - diff1.width() / 2 - _w / 2,
                            ow - diff2.height() / 2 - _w / 2, diff2.width() / 2 + _w / 2 );
    } break;
    }
}




