// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005-2006 Thorsten Zachmann <zachmann@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrAutoformObject.h"
#include "KPrAutoFormObjectIface.h"
#include "KPrUtils.h"
#include "KPrGradient.h"

#include <qbitmap.h>
#include <q3pointarray.h>
#include <q3ptrlist.h>
#include <qregion.h>
#include <qdom.h>
#include <qpainter.h>
#include <qmatrix.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <KoTextZoomHandler.h>
#include <math.h>
#include <kinstance.h>
using namespace std;

KPrAutoformObject::KPrAutoformObject()
: KPr2DObject()
, KPrStartEndLine( L_NORMAL, L_NORMAL )
, atfInterp()
{
}

KPrAutoformObject::KPrAutoformObject( const KoPen & _pen, const QBrush &_brush, const QString & _filename,
                                    LineEnd _lineBegin, LineEnd _lineEnd,
                                    FillType _fillType, const QColor &_gColor1,
                                    const QColor &_gColor2, BCType _gType,
                                    bool _unbalanced, int _xfactor, int _yfactor)
: KPr2DObject( _pen, _brush, _fillType, _gColor1, _gColor2, _gType, _unbalanced, _xfactor, _yfactor )
, KPrStartEndLine( _lineBegin, _lineEnd )
, filename( _filename ), atfInterp()
{
    atfInterp.load( filename );
}

KPrAutoformObject &KPrAutoformObject::operator=( const KPrAutoformObject & )
{
    return *this;
}

DCOPObject* KPrAutoformObject::dcopObject()
{
    if ( !dcop )
        dcop = new KPrAutoFormObjectIface( this );
    return dcop;
}

void KPrAutoformObject::setFileName( const QString & _filename )
{
    filename = _filename;
    atfInterp.load( filename );
}


bool KPrAutoformObject::saveOasisObjectAttributes( KPOasisSaveContext &sc ) const
{
    kDebug(33001) << "bool KPrAutoformObject::saveOasisObjectAttributes()" << endl;
    QSize size( int( ext.width() * 100 ), int( ext.height() * 100 )  );

    sc.xmlWriter.addAttribute( "svg:viewBox", QString( "0 0 %1 %2" ).arg( size.width() )
                                                                    .arg( size.height() ) );

    Q3PointArray points = const_cast<ATFInterpreter &>( atfInterp ).getPointArray( size.width(), size.height() );

    unsigned int pointCount = points.size();
    unsigned int pos = 0;
    bool closed = points.at( 0 ) == points.at( pointCount - 1 );

    if ( closed )
        --pointCount;

    QString d;
    d += QString( "M%1 %2" ).arg( points.at(pos).x() )
                            .arg( points.at(pos).y() );
    ++pos;

    while ( pos < pointCount )
    {
        d += QString( "L%1 %2" ).arg( points.at( pos ).x() )
                                .arg( points.at( pos ).y() );
        ++pos;
    }

    if ( closed )
        d += "Z";

    sc.xmlWriter.addAttribute( "svg:d", d );

    return true;
}

void KPrAutoformObject::fillStyle( KoGenStyle& styleObjectAuto, KoGenStyles& mainStyles ) const
{
    kDebug(33001) << "KPr2DObject::fillStyle" << endl;
    KPrShadowObject::fillStyle( styleObjectAuto, mainStyles );

    Q3PointArray points = const_cast<ATFInterpreter &>( atfInterp ).getPointArray( int( ext.width() * 100 ),
                                                                                  int( ext.height() * 100 ) );

    // if it is a closed object save the background
    if ( points.at( 0 ) == points.at( points.size() - 1 ) )
    {
        m_brush.saveOasisFillStyle( styleObjectAuto, mainStyles );
    }
    else
    {
        saveOasisMarkerElement( mainStyles, styleObjectAuto );
    }
}

const char * KPrAutoformObject::getOasisElementName() const
{
    return "draw:path";
}

QDomDocumentFragment KPrAutoformObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment=KPr2DObject::save(doc, offset);
    KPrStartEndLine::save( fragment, doc );

    // The filename contains the absolute path to the autoform. This is
    // bad, so we simply remove everything but the last dir and the name.
    // e.g. /my/local/path/to/kpresenter/Arrow/.source/Arrow1.atf -> Arrow/.source/Arrow1.atf
    QStringList afDirs = KPrFactory::global()->dirs()->resourceDirs("autoforms");
    QStringList::ConstIterator it=afDirs.begin();
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

double KPrAutoformObject::load(const QDomElement &element)
{
    double offset=KPr2DObject::load(element);
    KPrStartEndLine::load( element );
    QDomElement e=element.namedItem("FILENAME").toElement();
    if(!e.isNull()) {
        if(e.hasAttribute("value"))
            filename=e.attribute("value");
        // don't crash on invalid files, better insert something
        if(filename.isEmpty())
            filename="Connections/.source/Connection1.atf";
        // workaround for a bug in the (very) old file format
        if(filename[0]=='/') {
            kDebug(33001) << "rubbish ahead! cleaning up..." << endl;
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
        filename = locate("autoforms", filename, KPrFactory::global());
        atfInterp.load( filename );
    }
    return offset;
}

void KPrAutoformObject::paint( QPainter* _painter, KoTextZoomHandler *_zoomHandler,
                              int /* pageNum */, bool drawingShadow, bool drawContour )
{
    unsigned int pw = 0, pwOrig = 0, px, py;
    QPen pen2;
    QSize size( _zoomHandler->zoomSize( ext ) );

    if ( drawContour )
        pen2 = QPen( Qt::black, 1, Qt::DotLine );
    else {
        pen2 = pen.zoomedPen( _zoomHandler );
    }

    _painter->setPen( pen2 );
    pwOrig = ( pen2.style() == Qt::NoPen ) ? 1 : pen2.width();
    if ( !drawContour )
        _painter->setBrush( getBrush() );

    Q3PointArray pntArray = atfInterp.getPointArray( _zoomHandler->zoomItXOld( ext.width()),
                                                    _zoomHandler->zoomItYOld( ext.height() ) );
    Q3PtrList<ATFInterpreter::AttribList> atrLs = atfInterp.getAttribList();
    Q3PointArray pntArray2( pntArray.size() );
    int ex = _zoomHandler->zoomItXOld(ext.width());
    int ey = _zoomHandler->zoomItYOld(ext.height());
    for ( unsigned int i = 0; i < pntArray.size(); i++ )
    {
        px = pntArray.at( i ).x();
        py = pntArray.at( i ).y();
        if ( atrLs.at( i )->pwDiv > 0 )
        {
            pw = pwOrig / atrLs.at( i )->pwDiv;
            px = (int)((double)(ex - pw) / (double)ex * px + pw / 2);
            py = (int)((double)(ey - pw) / (double)ey * py + pw / 2);
        }
        pntArray2.setPoint( i, px, py );
    }

    if ( pntArray2.size() > 0 )
    {
        if ( pntArray2.at( 0 ) == pntArray2.at( pntArray2.size() - 1 ) )
        {
            if ( drawContour || (drawingShadow || getFillType() == FT_BRUSH || !gradient) )
                _painter->drawPolygon( pntArray2 );
            else
            {
                if ( angle == 0 || angle==360 )
                {
                    //int ox = _painter->viewport().x() + static_cast<int>( _painter->worldMatrix().dx() );
                    //int oy = _painter->viewport().y() + static_cast<int>( _painter->worldMatrix().dy() );

                    Q3PointArray pntArray3 = pntArray2.copy();
                    _painter->save();

                    QRegion clipregion( pntArray3 );

                    // Intersect with current clipregion (whereas setupClipRegion unites)
                    if ( _painter->hasClipping() )
						{
                        //clipregion = _painter->clipRegion(QPainter::CoordPainter).intersect( clipregion );
						}

                    //_painter->setClipRegion( clipregion, QPainter::CoordPainter );

                    gradient->setSize( size );
                    _painter->drawPixmap( 0, 0, gradient->pixmap() );

                    _painter->restore();
                }
                else
                {
                    if ( m_redrawGradientPix || gradient->size() != size )
                    {
                        kDebug(33001) << "KPrAutoformObject::draw redrawPix" << endl;
                        gradient->setSize( size );
                        m_redrawGradientPix = false;
                        QRegion clipregion( pntArray2 );
                        m_gradientPix.resize ( _zoomHandler->zoomItXOld(ext.width()),_zoomHandler->zoomItYOld(ext.height()) );
                        m_gradientPix.fill( Qt::white );


                        QPainter p;
                        p.begin( &m_gradientPix );
#warning "kde4: port it"						
                        //p.setClipRegion( clipregion , QPainter::CoordPainter);
                        p.drawPixmap( 0, 0, gradient->pixmap() );
                        p.end();

                        m_gradientPix.setMask( m_gradientPix.createHeuristicMask() );
                    }

                    _painter->drawPixmap( 0, 0, m_gradientPix );
                }

                _painter->setPen( pen2 );
                _painter->setBrush( Qt::NoBrush );
                _painter->drawPolygon( pntArray2 );
            }
        }
        else
        {
            KoSize diff1( 0, 0 ), diff2( 0, 0 );
            int _w = int( pen.pointWidth() );

            if ( lineBegin != L_NORMAL )
                diff1 = getBoundingSize( lineBegin, _w, _zoomHandler );

            if ( lineEnd != L_NORMAL )
                diff2 = getBoundingSize( lineEnd, _w, _zoomHandler );

            if ( pntArray.size() > 1 )
            {
                if ( lineBegin != L_NORMAL && !drawContour )
                {
                    QPoint pnt1( pntArray2.at( 0 ) ), pnt2( pntArray2.at( 1 ) );
                    QPoint pnt3, pnt4( pntArray.at( 0 ) );
                    float _angle = KoPoint::getAngle( KoPoint( pnt1 ), KoPoint( pnt2 ) );

                    switch ( static_cast<int>( _angle ) )
                    {
                    case 0:
                    {
                        pnt3.setX( pnt4.x() - (int)diff1.width() / 2 );
                        pnt3.setY( pnt1.y() );
                    } break;
                    case 180:
                    {
                        pnt3.setX( pnt4.x() + (int)diff1.width() / 2 );
                        pnt3.setY( pnt1.y() );
                    } break;
                    case 90:
                    {
                        pnt3.setX( pnt1.x() );
                        pnt3.setY( pnt4.y() - (int)diff1.width() / 2 );
                    } break;
                    case 270:
                    {
                        pnt3.setX( pnt1.x() );
                        pnt3.setY( pnt4.y() + (int)diff1.width() / 2 );
                    } break;
                    default:
                        pnt3 = pnt1;
                        break;
                    }

                    drawFigure( lineBegin, _painter, _zoomHandler->unzoomPointOld( pnt3 ), pen2.color(), _w, _angle, _zoomHandler );
                }

                if ( lineEnd != L_NORMAL && !drawContour )
                {
                    QPoint pnt1( pntArray2.at( pntArray2.size() - 1 ) ), pnt2( pntArray2.at( pntArray2.size() - 2 ) );
                    QPoint  pnt3, pnt4( pntArray.at( pntArray.size() - 1 ) );
                    float _angle = KoPoint::getAngle( KoPoint( pnt1 ), KoPoint( pnt2 ) );

                    switch ( ( int )_angle )
                    {
                    case 0:
                    {
                        pnt3.setX( pnt4.x() - (int)diff2.width() / 2 );
                        pnt3.setY( pnt1.y() );
                    } break;
                    case 180:
                    {
                        pnt3.setX( pnt4.x() + (int)diff2.width() / 2 );
                        pnt3.setY( pnt1.y() );
                    } break;
                    case 90:
                    {
                        pnt3.setX( pnt1.x() );
                        pnt3.setY( pnt4.y() - (int)diff2.width() / 2 );
                    } break;
                    case 270:
                    {
                        pnt3.setX( pnt1.x() );
                        pnt3.setY( pnt4.y() + (int)diff2.width() / 2 );
                    } break;
                    default:
                        pnt3 = pnt1;
                        break;
                    }

                    drawFigure( lineEnd, _painter, _zoomHandler->unzoomPointOld( pnt3 ), pen2.color(), _w, _angle,_zoomHandler );
                }
            }

            _painter->setPen( pen2 );
            _painter->drawPolyline( pntArray2 );
        }
    }
}
