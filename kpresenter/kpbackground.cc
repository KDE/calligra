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

#include "KPresenterBackgroundIface.h"

#include "kpbackground.h"
#include "kpresenter_doc.h"
#include "qwmf.h"
#include "kppixmapcollection.h"
#include "kpgradientcollection.h"
#include "kptextobject.h"

#include <qpicture.h>
#include <qpainter.h>
#include <qpen.h>
#include <qbrush.h>
#include <qpixmap.h>
#include <qfileinfo.h>
#include <qcstring.h>

#include <komlParser.h>
#include <komlWriter.h>

#include <kapp.h>

#include <fstream.h>

/******************************************************************/
/* Class: KPBackGround                                            */
/******************************************************************/

/*================================================================*/
KPBackGround::KPBackGround( KPPixmapCollection *_pixmapCollection, KPGradientCollection *_gradientCollection,
                            KPClipartCollection *_clipartCollection, KPresenterDoc *_doc )
    : footerHeight( 0 )
{
    dcop = 0;
    backType = BT_COLOR;
    backView = BV_CENTER;
    backColor1 = Qt::white;
    backColor2 = Qt::white;
    bcType = BCT_PLAIN;
    pageEffect = PEF_NONE;
    unbalanced = false;
    xfactor = 100;
    yfactor = 100;

    pixmapCollection = _pixmapCollection;
    gradientCollection = _gradientCollection;
    clipartCollection = _clipartCollection;
    backPix = 0L;
    gradient = 0L;
    picture = 0L;

    doc = _doc;
}

/*================================================================*/
void KPBackGround::setBackPixmap( const QString &_filename, QDateTime _lastModified )
{
    if ( backType != BT_PICTURE )
        return;

    if ( !_lastModified.isValid() )
    {
        QFileInfo inf( _filename );
        _lastModified = inf.lastModified();
    }

    if ( backPix )
        pixmapCollection->removeRef( key );

    QSize pixSize;
    switch ( backView )
    {
    case BV_ZOOM: pixSize = QSize( ext.width(), ext.height() );
        break;
    case BV_TILED: case BV_CENTER: pixSize = orig_size;
        break;
    }

    key = KPPixmapCollection::Key( KPPixmapDataCollection::Key( _filename, _lastModified ), pixSize );
    backPix = pixmapCollection->findPixmap( key );
}

/*================================================================*/
void KPBackGround::setBackClipFilename( const QString &_filename, QDateTime _lastModified )
{
    if ( backType != BT_CLIPART )
        return;

    if ( !_lastModified.isValid() )
    {
        QFileInfo inf( _filename );
        _lastModified = inf.lastModified();
    }

    if ( picture )
        clipartCollection->removeRef( clipKey );

    clipKey = KPClipartCollection::Key(_filename, _lastModified );
    picture = clipartCollection->findClipart( clipKey );
}

/*================================================================*/
void KPBackGround::draw( QPainter *_painter, QPoint _offset, bool _drawBorders )
{
    _painter->save();
    QRect r = _painter->viewport();

    switch ( backType )
    {
    case BT_COLOR:
    {
        _painter->setViewport( _offset.x(), _offset.y(), r.width(), r.height() );
        drawBackColor( _painter );
    } break;
    case BT_PICTURE:
    {
        _painter->setViewport( _offset.x(), _offset.y(), r.width(), r.height() );
        if ( backView == BV_CENTER ) drawBackColor( _painter );
        drawBackPix( _painter );
    } break;
    case BT_CLIPART:
    {
        _painter->setViewport( _offset.x(), _offset.y(), r.width(), r.height() );
        drawBackColor( _painter );
        _painter->setViewport( r );
        _painter->save();
        _painter->setViewport( _offset.x(), _offset.y(), ext.width(), ext.height() );
        drawBackClip( _painter );
        _painter->restore();
    } break;
    }

    if ( _drawBorders )
    {
        _painter->setViewport( _offset.x(), _offset.y(), r.width(), r.height() );
        drawBorders( _painter );
    }

    drawHeaderFooter( _painter, _offset );

    _painter->setViewport( r );
    _painter->restore();
}

/*================================================================*/
void KPBackGround::restore()
{
    if ( backType == BT_PICTURE )
        setBackPixmap( key.dataKey.filename, key.dataKey.lastModified );

    if ( backType == BT_CLIPART )
        setBackClipFilename( clipKey.filename, clipKey.lastModified );

    if ( backType != BT_PICTURE && backPix ) {
        pixmapCollection->removeRef( key );
        backPix = 0L;
    }

    if ( backType == BT_COLOR || backType == BT_CLIPART ||
         backType == BT_PICTURE && backView == BV_CENTER ) {
        if ( gradient ) {
            gradientCollection->removeRef( backColor1, backColor2, bcType, ext, unbalanced, xfactor, yfactor );
            gradient = 0;
        }
        gradient = gradientCollection->getGradient( backColor1, backColor2, bcType, ext, unbalanced, xfactor, yfactor );
    }

    if ( backType == BT_PICTURE && backView != BV_CENTER && gradient ) {
        gradientCollection->removeRef( backColor1, backColor2, bcType, ext, unbalanced, xfactor, yfactor );
        gradient = 0;
    }
}

/*================================================================*/
void KPBackGround::save( QTextStream& out )
{
    out << indent << "<BACKTYPE value=\"" << static_cast<int>( backType ) << "\"/>" << endl;
    out << indent << "<BACKVIEW value=\"" << static_cast<int>( backView ) << "\"/>" << endl;
    out << indent << "<BACKCOLOR1 red=\"" << backColor1.red() << "\" green=\""
        << backColor1.green() << "\" blue=\"" << backColor1.blue() << "\"/>" << endl;
    out << indent << "<BACKCOLOR2 red=\"" << backColor2.red() << "\" green=\""
        << backColor2.green() << "\" blue=\"" << backColor2.blue() << "\"/>" << endl;
    out << indent << "<BCTYPE value=\"" << static_cast<int>( bcType ) << "\"/>" << endl;
    out << indent << "<BGRADIENT unbalanced=\"" << static_cast<int>( unbalanced )
        << "\" xfactor=\"" << xfactor << "\" yfactor=\"" << yfactor << "\"/>" << endl;

    if ( backPix && backType == BT_PICTURE )
        out << indent << "<BACKPIXKEY " << key << " />" << endl;

    if ( picture && backType == BT_CLIPART )
        out << indent << "<BACKCLIPKEY " << clipKey << " />" << endl;

    out << indent << "<PGEFFECT value=\"" << static_cast<int>( pageEffect ) << "\"/>" << endl;
}

/*================================================================*/
void KPBackGround::load( KOMLParser& parser, QValueList<KOMLAttrib>& lst )
{
    QString name;
    QString tag;

    while ( parser.open( QString::null, tag ) )
    {
        parser.parseTag( tag, name, lst );

        // backtype
        if ( name == "BACKTYPE" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "value" )
                    setBackType( ( BackType )( *it ).m_strValue.toInt() );
            }
        }

        // pageEffect
        else if ( name == "PGEFFECT" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "value" )
                    setPageEffect( ( PageEffect )( *it ).m_strValue.toInt() );
            }
        }

        // backview
        else if ( name == "BACKVIEW" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "value" )
                    setBackView( ( BackView )( *it ).m_strValue.toInt() );
            }
        }

        // backcolor 1
        else if ( name == "BACKCOLOR1" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "red" )
                    setBackColor1( QColor( ( *it ).m_strValue.toInt(),
                                           backColor1.green(), backColor1.blue() ) );
                if ( ( *it ).m_strName == "green" )
                    setBackColor1( QColor( backColor1.red(),
                                           ( *it ).m_strValue.toInt(), backColor1.blue() ) );
                if ( ( *it ).m_strName == "blue" )
                    setBackColor1( QColor( backColor1.red(), backColor1.green(),
                                           ( *it ).m_strValue.toInt() ) );
            }
        }

        // gradient
        else if ( name == "BGRADIENT" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "unbalanced" )
                    setBackUnbalanced( ( *it ).m_strValue.toInt() );
                if ( ( *it ).m_strName == "xfactor" )
                    setBackXFactor( ( *it ).m_strValue.toInt() );
                if ( ( *it ).m_strName == "yfactor" )
                    setBackYFactor( ( *it ).m_strValue.toInt() );
            }
        }

        // backcolor 2
        else if ( name == "BACKCOLOR2" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "red" )
                    setBackColor2( QColor( ( *it ).m_strValue.toInt(),
                                           backColor2.green(), backColor2.blue() ) );
                if ( ( *it ).m_strName == "green" )
                    setBackColor2( QColor( backColor2.red(),
                                           ( *it ).m_strValue.toInt(), backColor2.blue() ) );
                if ( ( *it ).m_strName == "blue" )
                    setBackColor2( QColor( backColor2.red(), backColor2.green(),
                                           ( *it ).m_strValue.toInt() ) );
            }
        }

        // backColorType
        else if ( name == "BCTYPE" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "value" )
                    setBackColorType( ( BCType )( *it ).m_strValue.toInt() );
            }
        }

        // back pixmap
        else if ( name == "BACKPIXKEY" )
        {
            int year, month, day, hour, minute, second, msec;

            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "filename" )
                    key.dataKey.filename = ( *it ).m_strValue;
                else if ( ( *it ).m_strName == "year" )
                    year = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "month" )
                    month = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "day" )
                    day = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "hour" )
                    hour = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "minute" )
                    minute = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "second" )
                    second = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "msec" )
                    msec = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "width" )
                    key.size.setWidth( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "height" )
                    key.size.setHeight( ( *it ).m_strValue.toInt() );
            }
            key.dataKey.lastModified.setDate( QDate( year, month, day ) );
            key.dataKey.lastModified.setTime( QTime( hour, minute, second, msec ) );
        }

        // backpic
        else if ( name == "BACKPIX" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();

            bool openPic = true;
            QString _data;
            QString _fileName;

            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "data" )
                {
                    _data = ( *it ).m_strValue;
                    if ( _data.isEmpty() )
                        openPic = true;
                    else
                        openPic = false;
                }
                else if ( ( *it ).m_strName == "filename" )
                {
                    _fileName = ( *it ).m_strValue;
                    if ( !_fileName.isEmpty() )
                    {
                        if ( int _envVarB = _fileName.find( '$' ) >= 0 )
                        {
                            int _envVarE = _fileName.find( '/', _envVarB );
                            QString path = getenv( QFile::encodeName(_fileName.mid( _envVarB, _envVarE-_envVarB )) );
                            _fileName.replace( _envVarB-1, _envVarE-_envVarB+1, path );
                        }
                    }
                }
            }

            key.dataKey.filename = _fileName;
            key.dataKey.lastModified.setDate( pixmapCollection->tmpDate() );
            key.dataKey.lastModified.setTime( pixmapCollection->tmpTime() );
            key.size = ext;
            if ( !openPic )
                pixmapCollection->getPixmapDataCollection().setPixmapOldVersion( key.dataKey, _data );
            else
                pixmapCollection->getPixmapDataCollection().setPixmapOldVersion( key.dataKey );
        }

        // back clipart
        else if ( name == "BACKCLIPKEY" )
        {
            int year, month, day, hour, minute, second, msec;

            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "filename" )
                    clipKey.filename = ( *it ).m_strValue;
                else if ( ( *it ).m_strName == "year" )
                    year = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "month" )
                    month = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "day" )
                    day = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "hour" )
                    hour = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "minute" )
                    minute = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "second" )
                    second = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "msec" )
                    msec = ( *it ).m_strValue.toInt();
            }
            clipKey.lastModified.setDate( QDate( year, month, day ) );
            clipKey.lastModified.setTime( QTime( hour, minute, second, msec ) );
        }

        // backclip
        else if ( name == "BACKCLIP" )
        {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it )
            {
                if ( ( *it ).m_strName == "filename" )
                {
                    QString _fileName = ( *it ).m_strValue;
                    if ( !_fileName.isEmpty() )
                    {
                        if ( int _envVarB = _fileName.find( '$' ) >= 0 )
                        {
                            int _envVarE = _fileName.find( '/', _envVarB );
                            QString path = getenv( QFile::encodeName(_fileName.mid( _envVarB, _envVarE-_envVarB )) );
                            _fileName.replace( _envVarB-1, _envVarE-_envVarB+1, path );
                        }
                    }
                    clipKey.filename = _fileName;
                    clipKey.lastModified.setDate( clipartCollection->tmpDate() );
                    clipKey.lastModified.setTime( clipartCollection->tmpTime() );
                }
            }
        }

        else
            cerr << "Unknown tag '" << tag.latin1() << "' in CLPARTOBJECT" << endl;

        if ( !parser.close( tag ) )
        {
            cerr << "ERR: Closing Child" << endl;
            return;
        }
    }
}

/*================================================================*/
void KPBackGround::drawBackColor( QPainter *_painter )
{
    if ( getBackColorType() == BCT_PLAIN || getBackColor1() == getBackColor2() )
        _painter->fillRect( 0, 0, ext.width(), ext.height(), QBrush( getBackColor1() ) );
    else if ( gradient )
        _painter->drawPixmap( 0, 0, *gradient );
}

/*================================================================*/
void KPBackGround::drawBackPix( QPainter *_painter )
{
    if ( backPix )
    {
        switch ( backView )
        {
        case BV_ZOOM:
            if ( backPix && !backPix->isNull() )
                _painter->drawPixmap( 0, 0, *backPix );
            break;
        case BV_TILED:
            if ( backPix && !backPix->isNull() )
                _painter->drawTiledPixmap( 0, 0, ext.width(), ext.height(), *backPix );
            break;
        case BV_CENTER:
        {
            QPixmap *pix = new QPixmap( ext.width(), ext.height() );
            bool delPix = true;
            int _x = 0, _y = 0;

            if ( backPix->width() > pix->width() && backPix->height() > pix->height() )
                bitBlt( pix, 0, 0, backPix, backPix->width() - pix->width(), backPix->height() - pix->height(),
                        pix->width(), pix->height() );
            else if ( backPix->width() > pix->width() )
            {
                bitBlt( pix, 0, 0, backPix, backPix->width() - pix->width(), 0,
                        pix->width(), backPix->height() );
                _y = ( pix->height() - backPix->height() ) / 2;
            }
            else if ( backPix->height() > pix->height() )
            {
                bitBlt( pix, 0, 0, backPix, 0, backPix->height() - pix->height(),
                        backPix->width(), pix->height() );
                _x = ( pix->width() - backPix->width() ) / 2;
            }
            else
            {
                _x = ( pix->width() - backPix->width() ) / 2;
                _y = ( pix->height() - backPix->height() ) / 2;
                delPix = false;
                delete pix;
                pix = backPix;
            }

            if ( pix && !pix->isNull() )
                _painter->drawPixmap( _x, _y, *pix );
            if ( delPix ) delete pix;
        }
        break;
        }
    }
}

/*================================================================*/
void KPBackGround::drawHeaderFooter( QPainter *_painter, const QPoint &_offset )
{
    if ( doc->hasHeader() ) {

        QSize s( doc->header()->getKTextObject()->size() );
        QPoint pnt( doc->header()->getKTextObject()->x(), doc->header()->getKTextObject()->y() );

        // #### Reggie: not very efficient but ok for now
        if ( true /*doc->header()->getKTextObject()->isModified()*/ ) {
            doc->header()->setSize( ext.width(), 10 );
            //qDebug( "resize h" );
        }
        doc->header()->setOrig( _offset.x(), _offset.y() );

        int pgnum = doc->backgroundList()->findRef( this );
        if ( pgnum == -1 )
            pgnum = 0;
#if 0
        doc->header()->getKTextObject()->setPageNum( ++pgnum );
#endif

        doc->header()->setSize( ext.width(), doc->header()->getKTextObject()->document()->lastParag()->rect().bottom() + 1 );

        doc->header()->draw( _painter, 0, 0 );

        if ( doc->header()->getKTextObject()->isModified() )
            doc->header()->getKTextObject()->resize( s );
        doc->header()->getKTextObject()->move( pnt.x(), pnt.y() );
    }

    if ( doc->hasFooter() ) {
        QSize s( doc->footer()->getKTextObject()->size() );
        QPoint pnt( doc->footer()->getKTextObject()->x(), doc->footer()->getKTextObject()->y() );

        // #### Reggie: not very efficient but ok for now
        if ( true ) { //doc->footer()->getKTextObject()->isModified() || footerHeight <= 0 )
            doc->footer()->setSize( ext.width(), 10 );

            footerHeight = doc->footer()->getKTextObject()->document()->lastParag()->rect().bottom() + 1;
            doc->footer()->setSize( ext.width(), footerHeight );
        }

        doc->footer()->setOrig( _offset.x(), _offset.y() + ext.height() - footerHeight );

        int pgnum = doc->backgroundList()->findRef( this );
        if ( pgnum == -1 )
            pgnum = 0;
#if 0
        doc->footer()->getKTextObject()->setPageNum( ++pgnum );
#endif
        doc->footer()->draw( _painter, 0, 0 );

        if ( doc->footer()->getKTextObject()->isModified() )
            doc->footer()->getKTextObject()->resize( s.width(), s.height() );

        doc->footer()->getKTextObject()->move( pnt.x(), pnt.y() );
    }
}

/*================================================================*/
void KPBackGround::drawBackClip( QPainter *_painter )
{
    if ( picture )
        _painter->drawPicture( *picture );
}

/*================================================================*/
void KPBackGround::drawBorders( QPainter *_painter )
{
    QPen pen( Qt::red, 1 );
    QBrush brush( Qt::NoBrush );

    _painter->setPen( pen );
    _painter->setBrush( brush );
    _painter->drawRect( 0, 0, ext.width() + 1, ext.height() + 1 );
}

/*================================================================*/
void KPBackGround::removeGradient()
{
    if ( gradient ) {
        gradientCollection->removeRef( backColor1, backColor2, bcType, ext,
                                       unbalanced, xfactor, yfactor);
        gradient = 0;
    }
}

/*=============================================================*/
DCOPObject* KPBackGround::dcopObject()
{
    if ( !dcop )
        dcop = new KPresenterBackgroundIface( this );

    return dcop;
}
