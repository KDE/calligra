// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

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
#include "KPGradient.h"

#include <qbitmap.h>
#include <qpointarray.h>
#include <qptrlist.h>
#include <qregion.h>
#include <qdom.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kotextzoomhandler.h>
#include <math.h>
using namespace std;

KPAutoformObject::KPAutoformObject()
    : KP2DObject(), atfInterp()
{
    lineBegin = L_NORMAL;
    lineEnd = L_NORMAL;
}

KPAutoformObject::KPAutoformObject( const KPPen & _pen, const QBrush &_brush, const QString & _filename,
                                    LineEnd _lineBegin, LineEnd _lineEnd,
                                    FillType _fillType, const QColor &_gColor1,
                                    const QColor &_gColor2, BCType _gType,
                                    bool _unbalanced, int _xfactor, int _yfactor)
    : KP2DObject( _pen, _brush, _fillType, _gColor1, _gColor2, _gType, _unbalanced, _xfactor, _yfactor ),
      filename( _filename ), atfInterp()
{
    atfInterp.load( filename );
    lineBegin = _lineBegin;
    lineEnd = _lineEnd;
}

KPAutoformObject &KPAutoformObject::operator=( const KPAutoformObject & )
{
    return *this;
}

DCOPObject* KPAutoformObject::dcopObject()
{
    if ( !dcop )
        dcop = new KPAutoFormObjectIface( this );
    return dcop;
}

void KPAutoformObject::setFileName( const QString & _filename )
{
    filename = _filename;
    atfInterp.load( filename );
}


bool KPAutoformObject::saveOasisObjectAttributes( KPOasisSaveContext &sc ) const
{
    kdDebug()<<"bool KPAutoformObject::saveOasisObjectAttributes( KPOasisSaveContext &sc ) not implemented\n";
    return true;
}

const char * KPAutoformObject::getOasisElementName() const
{
    return "draw:custom-shape";
}

QDomDocumentFragment KPAutoformObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment=KP2DObject::save(doc, offset);
    if (lineBegin!=L_NORMAL)
        fragment.appendChild(KPObject::createValueElement("LINEBEGIN", static_cast<int>(lineBegin), doc));
    if (lineEnd!=L_NORMAL)
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

double KPAutoformObject::load(const QDomElement &element)
{
    double offset=KP2DObject::load(element);
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
        // don't crash on invalid files, better insert something
        if(filename.isEmpty())
            filename="Connections/.source/Connection1.atf";
        // workaround for a bug in the (very) old file format
        if(filename[0]=='/') {
            kdDebug(33001) << "rubbish ahead! cleaning up..." << endl;
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
    return offset;
}

void KPAutoformObject::paint( QPainter* _painter, KoTextZoomHandler *_zoomHandler,
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

    QPointArray pntArray = atfInterp.getPointArray( _zoomHandler->zoomItX( ext.width()),
                                                    _zoomHandler->zoomItY( ext.height() ) );
    QPtrList<ATFInterpreter::AttribList> atrLs = atfInterp.getAttribList();
    QPointArray pntArray2( pntArray.size() );
    int ex = _zoomHandler->zoomItX(ext.width());
    int ey = _zoomHandler->zoomItY(ext.height());
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

                    QPointArray pntArray3 = pntArray2.copy();
                    _painter->save();

                    QRegion clipregion( pntArray3 );

                    // Intersect with current clipregion (whereas setupClipRegion unites)
                    if ( _painter->hasClipping() )
                        clipregion = _painter->clipRegion(QPainter::CoordPainter).intersect( clipregion );

                    _painter->setClipRegion( clipregion, QPainter::CoordPainter );

                    gradient->setSize( size );
                    _painter->drawPixmap( 0, 0, gradient->pixmap() );

                    _painter->restore();
                }
                else
                {
                    if ( m_redrawGradientPix || gradient->size() != size )
                    {
                        kdDebug(33001) << "KPAutoformObject::draw redrawPix" << endl;
                        gradient->setSize( size );
                        m_redrawGradientPix = false;
                        QRegion clipregion( pntArray2 );
                        m_gradientPix.resize ( _zoomHandler->zoomItX(ext.width()),_zoomHandler->zoomItY(ext.height()) );
                        m_gradientPix.fill( Qt::white );


                        QPainter p;
                        p.begin( &m_gradientPix );
                        p.setClipRegion( clipregion , QPainter::CoordPainter);
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
            int _w = pen.pointWidth();

            if ( lineBegin != L_NORMAL )
                diff1 = getBoundingSize( lineBegin, _w, _zoomHandler );

            if ( lineEnd != L_NORMAL )
                diff2 = getBoundingSize( lineEnd, _w, _zoomHandler );

            if ( pntArray.size() > 1 )
            {
                if ( lineBegin != L_NORMAL && !drawContour )
                {
                    QPoint pnt1( pntArray2.at( 0 ) ), pnt2( pntArray2.at( 1 ) ), pnt3, pnt4( pntArray.at( 0 ) );
                    float _angle = KoPoint::getAngle( pnt1, pnt2 );

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

                    drawFigure( lineBegin, _painter, _zoomHandler->unzoomPoint( pnt3 ), pen2.color(), _w, _angle, _zoomHandler );
                }

                if ( lineEnd != L_NORMAL && !drawContour )
                {
                    QPoint pnt1( pntArray2.at( pntArray2.size() - 1 ) ), pnt2( pntArray2.at( pntArray2.size() - 2 ) );
                    QPoint  pnt3, pnt4( pntArray.at( pntArray.size() - 1 ) );
                    float _angle = KoPoint::getAngle( pnt1, pnt2 );

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

                    drawFigure( lineEnd, _painter, _zoomHandler->unzoomPoint( pnt3 ), pen2.color(), _w, _angle,_zoomHandler );
                }
            }

            _painter->setPen( pen2 );
            _painter->drawPolyline( pntArray2 );
        }
    }
}
