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

#include <kpautoformobject.h>
#include <kpresenter_utils.h>
#include <kpgradient.h>

#include <qpointarray.h>
#include <qlist.h>
#include <qregion.h>
#include <qdom.h>
#include <qpicture.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <kstddirs.h>
#include <kdebug.h>

#include <math.h>
using namespace std;

/******************************************************************/
/* Class: KPAutoformObject                                        */
/******************************************************************/

/*================ default constructor ===========================*/
KPAutoformObject::KPAutoformObject()
    : KP2DObject(), atfInterp( 0, "" )
{
    lineBegin = L_NORMAL;
    lineEnd = L_NORMAL;
    redrawPix = false;
}

/*================== overloaded constructor ======================*/
KPAutoformObject::KPAutoformObject( QPen _pen, QBrush _brush, QString _filename, LineEnd _lineBegin, LineEnd _lineEnd,
                                    FillType _fillType, QColor _gColor1, QColor _gColor2, BCType _gType,
                                    bool _unbalanced, int _xfactor, int _yfactor)
    : KP2DObject( _pen, _brush, _fillType, _gColor1, _gColor2, _gType, _unbalanced, _xfactor, _yfactor ),
      filename( _filename ), atfInterp( 0, "" )
{
    atfInterp.load( filename );
    lineBegin = _lineBegin;
    lineEnd = _lineEnd;
    redrawPix = true;

    if ( fillType == FT_GRADIENT )
    {
        gradient = new KPGradient( gColor1, gColor2, gType, QSize( 1, 1 ), unbalanced, xfactor, yfactor );
        redrawPix = true;
        pix.resize( getSize() );
    }
    else
        gradient = 0;
}

/*================================================================*/
KPAutoformObject &KPAutoformObject::operator=( const KPAutoformObject & )
{
    return *this;
}

/*================================================================*/
void KPAutoformObject::setSize( int _width, int _height )
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
void KPAutoformObject::resizeBy( int _dx, int _dy )
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

/*====================== set filename ============================*/
void KPAutoformObject::setFileName( QString _filename )
{
    filename = _filename;
    atfInterp.load( filename );
}

/*================================================================*/
void KPAutoformObject::setFillType( FillType _fillType )
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

/*========================= save =================================*/
QDomDocumentFragment KPAutoformObject::save( QDomDocument& doc )
{
    QDomDocumentFragment fragment=KP2DObject::save(doc);
    fragment.appendChild(KPObject::createValueElement("LINEBEGIN", static_cast<int>(lineBegin), doc));
    fragment.appendChild(KPObject::createValueElement("LINEEND", static_cast<int>(lineEnd), doc));

    // The filename contains the absolute path to the autoform. This is
    // bad, so we simply remove everything but the last dir and the name.
    // e.g. /my/local/path/to/kpresenter/Arrow/.source/Arrow1.atf -> Arrow/.source/Arrow1.atf
    QStringList afDirs = KPresenterFactory::global()->dirs()->resourceDirs("autoforms");
    QValueList<QString>::ConstIterator it=afDirs.begin();
    QString str;
    for( ; it!=afDirs.end(); ++it) {
        if(filename.startsWith(*it)) {
            str=filename.mid((*it).length());
            break;
        }
    }
    QDomElement elem=doc.createElement("FILENAME");
    elem.setAttribute("value", str);
    fragment.appendChild(elem);
    return fragment;
}

/*========================== load ================================*/
void KPAutoformObject::load(const QDomElement &element)
{
    KP2DObject::load(element);
    QDomElement e=element.namedItem("LINEBEGIN").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        lineBegin=static_cast<LineEnd>(tmp);
    }
    e=element.namedItem("LINEEND").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        lineEnd=static_cast<LineEnd>(tmp);
    }
    e=element.namedItem("FILENAME").toElement();
    if(!e.isNull()) {
        if(e.hasAttribute("value"))
            filename=e.attribute("value");
        // workaround for a bug in the (very) old file format
        if(filename[0]=='/') {
            kdDebug() << "rubbish ahead! cleaning up..." << endl;
            // remove the leading absolute path (i.e. to create Arrow/Arrow1.atf)
            filename=filename.mid(filename.findRev('/', filename.findRev('/')-1)+1);
        }
        // okay... we changed the file format again and now the autoforms
        // are stored in .../kpresenter/autoforms/.source/foo.atf (note: we didn't have .source
        // before. Therefore we have to add this dir if it's not already there to make it
        // work with old files
        if(filename.find(".source")==-1) {
            // okay, old file -- add the .source dir
            filename=filename.insert(filename.find('/'), "/.source");
        }
        filename = locate("autoforms", filename, KPresenterFactory::global());
        atfInterp.load( filename );
    }
}

/*===================== get angle ================================*/
float KPAutoformObject::getAngle( QPoint p1, QPoint p2 )
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
void KPAutoformObject::paint( QPainter* _painter )
{
    unsigned int pw = 0, pwOrig = 0, px, py;

    _painter->setPen( pen );
    pwOrig = pen.width() + 3;
    _painter->setBrush( brush );

    QPointArray pntArray = atfInterp.getPointArray( ext.width(), ext.height() );
    QList<ATFInterpreter::AttribList> atrLs = atfInterp.getAttribList();
    QPointArray pntArray2( pntArray.size() );
    for ( unsigned int i = 0; i < pntArray.size(); i++ )
    {
        px = pntArray.at( i ).x();
        py = pntArray.at( i ).y();
        if ( atrLs.at( i )->pwDiv > 0 )
        {
            pw = pwOrig / atrLs.at( i )->pwDiv;
            if ( px < static_cast<unsigned int>( ext.width() ) / 2 ) px += pw;
            if ( py < static_cast<unsigned int>( ext.height() ) / 2 ) py += pw;
            if ( px > static_cast<unsigned int>( ext.width() ) / 2 ) px -= pw;
            if ( py > static_cast<unsigned int>( ext.height() ) / 2 ) py -= pw;
        }
        pntArray2.setPoint( i, px, py );
    }

    if ( pntArray2.size() > 0 )
    {
        if ( pntArray2.at( 0 ) == pntArray2.at( pntArray2.size() - 1 ) )
        {
            if ( drawShadow || fillType == FT_BRUSH || !gradient )
                _painter->drawPolygon( pntArray2 );
            else
            {
                if ( angle == 0 || angle==360 )
                {
                    int ox = _painter->viewport().x() + static_cast<int>( _painter->worldMatrix().dx() );
                    int oy = _painter->viewport().y() + static_cast<int>( _painter->worldMatrix().dy() );

                    QPointArray pntArray3 = pntArray2.copy();
                    pntArray3.translate( ox, oy );
                    _painter->save();

                    QRegion clipregion( pntArray3 );

                    if ( _painter->hasClipping() )
                        clipregion = _painter->clipRegion().intersect( clipregion );

                    _painter->setClipRegion( clipregion );
                    setupClipRegion( _painter, clipregion );

                    _painter->drawPixmap( 0, 0, *gradient->getGradient() );

                    _painter->restore();
                }
                else  //lukas: FIXME rotated autoforms && gradient bg
                {
                    if ( redrawPix )
                    {
                        redrawPix = false;
                        QRegion clipregion( pntArray2 );
                        QPicture pic;
                        QPainter p;

                        p.begin( &pic );
                        p.setClipRegion( clipregion );
                        setupClipRegion( &p, clipregion );
                        p.drawPixmap( 0, 0, *gradient->getGradient() );
                        p.end();

                        pix.fill( Qt::white );
                        QPainter p2;
                        p2.begin( &pix );
                        p2.drawPicture( pic );
                        p2.end();

                    }

                    _painter->drawPixmap( 0, 0, pix );
                }

                _painter->setPen( pen );
                _painter->setBrush( Qt::NoBrush );
                _painter->drawPolygon( pntArray2 );
            }
        }
        else
        {
            QSize diff1( 0, 0 ), diff2( 0, 0 );
            int _w = pen.width();

            if ( lineBegin != L_NORMAL )
                diff1 = getBoundingSize( lineBegin, _w );

            if ( lineEnd != L_NORMAL )
                diff2 = getBoundingSize( lineEnd, _w );

            if ( pntArray.size() > 1 )
            {
                if ( lineBegin != L_NORMAL )
                {
                    QPoint pnt1( pntArray2.at( 0 ) ), pnt2( pntArray2.at( 1 ) ), pnt3, pnt4( pntArray.at( 0 ) );
                    float _angle = getAngle( pnt1, pnt2 );

                    switch ( static_cast<int>( _angle ) )
                    {
                    case 0:
                    {
                        pnt3.setX( pnt4.x() - diff1.width() / 2 );
                        pnt3.setY( pnt1.y() );
                    } break;
                    case 180:
                    {
                        pnt3.setX( pnt4.x() + diff1.width() / 2 );
                        pnt3.setY( pnt1.y() );
                    } break;
                    case 90:
                    {
                        pnt3.setX( pnt1.x() );
                        pnt3.setY( pnt4.y() - diff1.width() / 2 );
                    } break;
                    case 270:
                    {
                        pnt3.setX( pnt1.x() );
                        pnt3.setY( pnt4.y() + diff1.width() / 2 );
                    } break;
                    default:
                        pnt3 = pnt1;
                        break;
                    }

                    drawFigure( lineBegin, _painter, pnt3, pen.color(), _w, _angle );
                }

                if ( lineEnd != L_NORMAL )
                {
                    QPoint pnt1( pntArray2.at( pntArray2.size() - 1 ) ), pnt2( pntArray2.at( pntArray2.size() - 2 ) );
                    QPoint  pnt3, pnt4( pntArray.at( pntArray.size() - 1 ) );
                    float _angle = getAngle( pnt1, pnt2 );

                    switch ( ( int )_angle )
                    {
                    case 0:
                    {
                        pnt3.setX( pnt4.x() - diff2.width() / 2 );
                        pnt3.setY( pnt1.y() );
                    } break;
                    case 180:
                    {
                        pnt3.setX( pnt4.x() + diff2.width() / 2 );
                        pnt3.setY( pnt1.y() );
                    } break;
                    case 90:
                    {
                        pnt3.setX( pnt1.x() );
                        pnt3.setY( pnt4.y() - diff2.width() / 2 );
                    } break;
                    case 270:
                    {
                        pnt3.setX( pnt1.x() );
                        pnt3.setY( pnt4.y() + diff2.width() / 2 );
                    } break;
                    default:
                        pnt3 = pnt1;
                        break;
                    }

                    drawFigure( lineEnd, _painter, pnt3, pen.color(), _w, _angle );
                }
            }

            _painter->setPen( pen );
            _painter->drawPolyline( pntArray2 );
        }

    }
}
