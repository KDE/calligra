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
/* Module: Background                                             */
/******************************************************************/

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
#include <komlStreamFeed.h>
#include <komlWriter.h>

#include <kapp.h>

#include <fstream.h>
#include <string.h>

/******************************************************************/
/* Class: KPBackGround                                            */
/******************************************************************/

/*================================================================*/
KPBackGround::KPBackGround( KPPixmapCollection *_pixmapCollection, KPGradientCollection *_gradientCollection,
                            KPClipartCollection *_clipartCollection, KPresenterDoc *_doc )
    : footerHeight( 0 )
{
    backType = BT_COLOR;
    backView = BV_CENTER;
    backColor1 = Qt::white;
    backColor2 = Qt::white;
    bcType = BCT_PLAIN;
    pageEffect = PEF_NONE;

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

    if ( backType != BT_PICTURE && backPix )
    {
        pixmapCollection->removeRef( key );
        backPix = 0L;
    }

    if ( backType == BT_COLOR || backType == BT_CLIPART || backType == BT_PICTURE && backView == BV_CENTER )
    {
        if ( gradient )
        {
            gradientCollection->removeRef( backColor1, backColor2, bcType, ext );
            gradient = 0;
        }
        gradient = gradientCollection->getGradient( backColor1, backColor2, bcType, ext );
    }

    if ( backType == BT_PICTURE && backView != BV_CENTER && gradient )
    {
        gradientCollection->removeRef( backColor1, backColor2, bcType, ext );
        gradient = 0;
    }
}

/*================================================================*/
void KPBackGround::save( ostream& out )
{
    out << indent << "<BACKTYPE value=\"" << static_cast<int>( backType ) << "\"/>" << endl;
    out << indent << "<BACKVIEW value=\"" << static_cast<int>( backView ) << "\"/>" << endl;
    out << indent << "<BACKCOLOR1 red=\"" << backColor1.red() << "\" green=\""
        << backColor1.green() << "\" blue=\"" << backColor1.blue() << "\"/>" << endl;
    out << indent << "<BACKCOLOR2 red=\"" << backColor2.red() << "\" green=\""
        << backColor2.green() << "\" blue=\"" << backColor2.blue() << "\"/>" << endl;
    out << indent << "<BCTYPE value=\"" << static_cast<int>( bcType ) << "\"/>" << endl;

    if ( backPix && backType == BT_PICTURE )
        out << indent << "<BACKPIXKEY " << key << " />" << endl;

    if ( picture && backType == BT_CLIPART )
        out << indent << "<BACKCLIPKEY " << clipKey << " />" << endl;

    out << indent << "<PGEFFECT value=\"" << static_cast<int>( pageEffect ) << "\"/>" << endl;
}

/*================================================================*/
void KPBackGround::load( KOMLParser& parser, vector<KOMLAttrib>& lst )
{
    string name;
    string tag;


    while ( parser.open( 0L, tag ) )
    {
        KOMLParser::parseTag( tag.c_str(), name, lst );

        // backtype
        if ( name == "BACKTYPE" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "value" )
                    setBackType( ( BackType )atoi( ( *it ).m_strValue.c_str() ) );
            }
        }

        // pageEffect
        else if ( name == "PGEFFECT" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "value" )
                    setPageEffect( ( PageEffect )atoi( ( *it ).m_strValue.c_str() ) );
            }
        }

        // backview
        else if ( name == "BACKVIEW" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "value" )
                    setBackView( ( BackView )atoi( ( *it ).m_strValue.c_str() ) );
            }
        }

        // backcolor 1
        else if ( name == "BACKCOLOR1" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "red" )
                    setBackColor1( QColor( atoi( ( *it ).m_strValue.c_str() ),
                                           backColor1.green(), backColor1.blue() ) );
                if ( ( *it ).m_strName == "green" )
                    setBackColor1( QColor( backColor1.red(),
                                           atoi( ( *it ).m_strValue.c_str() ), backColor1.blue() ) );
                if ( ( *it ).m_strName == "blue" )
                    setBackColor1( QColor( backColor1.red(), backColor1.green(),
                                           atoi( ( *it ).m_strValue.c_str() ) ) );
            }
        }

        // backcolor 2
        else if ( name == "BACKCOLOR2" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "red" )
                    setBackColor2( QColor( atoi( ( *it ).m_strValue.c_str() ),
                                           backColor2.green(), backColor2.blue() ) );
                if ( ( *it ).m_strName == "green" )
                    setBackColor2( QColor( backColor2.red(),
                                           atoi( ( *it ).m_strValue.c_str() ), backColor2.blue() ) );
                if ( ( *it ).m_strName == "blue" )
                    setBackColor2( QColor( backColor2.red(), backColor2.green(),
                                           atoi( ( *it ).m_strValue.c_str() ) ) );
            }
        }

        // backColorType
        else if ( name == "BCTYPE" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "value" )
                    setBackColorType( ( BCType )atoi( ( *it ).m_strValue.c_str() ) );
            }
        }

        // back pixmap
        else if ( name == "BACKPIXKEY" )
        {
            int year, month, day, hour, minute, second, msec;

            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "filename" )
                    key.dataKey.filename = ( *it ).m_strValue.c_str();
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
                else if ( ( *it ).m_strName == "width" )
                    key.size.setWidth( atoi( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "height" )
                    key.size.setHeight( atoi( ( *it ).m_strValue.c_str() ) );
            }
            key.dataKey.lastModified.setDate( QDate( year, month, day ) );
            key.dataKey.lastModified.setTime( QTime( hour, minute, second, msec ) );
        }

        // backpic
        else if ( name == "BACKPIX" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();

            bool openPic = true;
            QCString _data;
            QString _fileName;

            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "data" )
                {
                    _data = ( *it ).m_strValue.c_str();
                    if ( _data.isEmpty() )
                        openPic = true;
                    else
                        openPic = false;
                }
                else if ( ( *it ).m_strName == "filename" )
                {
                    _fileName = ( *it ).m_strValue.c_str();
                    if ( !_fileName.isEmpty() )
                    {
                        if ( int _envVarB = _fileName.find( '$' ) >= 0 )
                        {
                            int _envVarE = _fileName.find( '/', _envVarB );
                            QString path = getenv( _fileName.mid( _envVarB, _envVarE-_envVarB ).data() );
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

            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "filename" )
                    clipKey.filename = ( *it ).m_strValue.c_str();
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
            clipKey.lastModified.setDate( QDate( year, month, day ) );
            clipKey.lastModified.setTime( QTime( hour, minute, second, msec ) );
        }

        // backclip
        else if ( name == "BACKCLIP" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "filename" )
                {
                    QString _fileName = ( *it ).m_strValue.c_str();
                    if ( !_fileName.isEmpty() )
                    {
                        if ( int _envVarB = _fileName.find( '$' ) >= 0 )
                        {
                            int _envVarE = _fileName.find( '/', _envVarB );
                            QString path = getenv( _fileName.mid( _envVarB, _envVarE-_envVarB ).data() );
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
            cerr << "Unknown tag '" << tag << "' in CLPARTOBJECT" << endl;

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
            _painter->drawPixmap( 0, 0, *backPix );
            break;
        case BV_TILED:
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
    if ( doc->hasHeader() )
    {
        QSize s( doc->header()->getKTextObject()->size() );
        QPoint pnt( doc->header()->getKTextObject()->x(), doc->header()->getKTextObject()->y() );

        if ( doc->header()->getKTextObject()->isModified() )
            doc->header()->setSize( ext.width(), 10 );

        doc->header()->setOrig( _offset.x(), _offset.y() );

        int h = 0;
        if ( doc->header()->getKTextObject()->isModified() )
        {
            for ( unsigned int i = 0; i < doc->header()->getKTextObject()->paragraphs(); i++ )
                h += doc->header()->getKTextObject()->paragraphAt( i )->height();
            h += 2;
            doc->header()->setSize( ext.width(), h );
        }

        doc->header()->draw( _painter, 0, 0 );

        if ( doc->header()->getKTextObject()->isModified() )
            doc->header()->getKTextObject()->resize( s.width(), s.height() );

        doc->header()->getKTextObject()->move( pnt.x(), pnt.y() );

        doc->header()->getKTextObject()->toggleModified( false );
    }

    if ( doc->hasFooter() )
    {
        QSize s( doc->footer()->getKTextObject()->size() );
        QPoint pnt( doc->footer()->getKTextObject()->x(), doc->footer()->getKTextObject()->y() );

        if ( doc->footer()->getKTextObject()->isModified() || footerHeight <= 0 )
        {
            doc->footer()->setSize( ext.width(), 10 );

            int h = 0;
            for ( unsigned int i = 0; i < doc->footer()->getKTextObject()->paragraphs(); i++ )
                h += doc->footer()->getKTextObject()->paragraphAt( i )->height();
            h += 2;
            doc->footer()->setSize( ext.width(), h );
            footerHeight = h;
        }

        doc->footer()->setOrig( _offset.x(), _offset.y() + ext.height() - footerHeight );

        doc->footer()->draw( _painter, 0, 0 );

        if ( doc->footer()->getKTextObject()->isModified() )
            doc->footer()->getKTextObject()->resize( s.width(), s.height() );

        doc->footer()->getKTextObject()->move( pnt.x(), pnt.y() );

        doc->footer()->getKTextObject()->toggleModified( false );
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
    if ( gradient )
    {
        gradientCollection->removeRef( backColor1, backColor2, bcType, ext );
        gradient = 0;
    }
}
