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

#include <qpicture.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include "preview.h"
#include "preview.moc"
#include "qwmf.h"
#include <qfileinfo.h>
#include <qscrollview.h>

//#ifndef USE_QFD
/**
 * A preview handler for the KFilePreviewDialag that shows
 * a WMF object

bool wmfPreviewHandler( const KFileInfo* fInfo, const QString fileName, QString&, QPixmap& pixmap )
{
    bool res = false;
    QString ext = fileName.right( 3 ).lower();

    if ( fInfo->isFile() && ( ext == "wmf" ) ) {
	QWinMetaFile wmf;

	if ( wmf.load( fileName.data() ) ) {
	    QPicture pic;
	    wmf.paint( &pic );

	    pixmap = QPixmap( 200, 200 );
	    QPainter p;

	    p.begin( &pixmap );
	    p.setBackgroundColor( Qt::white );
	    pixmap.fill( Qt::white );

	    QRect oldWin = p.window();
	    QRect vPort = p.viewport();
	    p.setViewport( 0, 0, 200, 200 );
	    p.drawPicture( pic );
	    p.setWindow( oldWin );
	    p.setViewport( vPort );
	    p.end();

	    res = true;
	}
    }
    return res;
}


 * A preview handler for the KFilePreviewDialag that shows
 * a Pixmap object.

bool pixmapPreviewHandler( const KFileInfo* fInfo, const QString fileName, QString&, QPixmap& pixmap )
{
    if ( fInfo->isFile() ) {
	pixmap.load( fileName.data() );
	return !pixmap.isNull();
    }
    return false;
}*/
//#endif

class PixmapView : public QScrollView
{
public:
    PixmapView( QWidget *parent )
	: QScrollView( parent ) { viewport()->setBackgroundMode( PaletteBase ); }

    void setPixmap( const QPixmap &pix ) {
	pixmap = pix;
        const QBitmap nullBitmap;
        pixmap.setMask( nullBitmap );  //don't show transparency
	resizeContents( pixmap.size().width(), pixmap.size().height() );
	viewport()->repaint( false );
    }

    void setClipart( const QString &s ) {
	QWinMetaFile wmf;

	if ( wmf.load( s ) ) {
	    QPicture pic;
	    wmf.paint( &pic );

	    pixmap = QPixmap( 200, 200 );
	    QPainter p;

	    p.begin( &pixmap );
	    p.setBackgroundColor( Qt::white );
	    pixmap.fill( Qt::white );

	    QRect oldWin = p.window();
	    QRect vPort = p.viewport();
	    p.setViewport( 0, 0, 200, 200 );
	    p.drawPicture( pic );
	    p.setWindow( oldWin );
	    p.setViewport( vPort );
	    p.end();
	    resizeContents( pixmap.size().width(), pixmap.size().height() );
	    viewport()->repaint( false );
	}
    }

    void drawContents( QPainter *p, int, int, int, int ) {
	p->drawPixmap( 0, 0, pixmap );
    }

private:
    QPixmap pixmap;

};

Preview::Preview( QWidget *parent )
    : QVBox( parent )
{
    pixmap = new PixmapView( this );
}


void Preview::showPreview( const KURL &u )
{
    if ( u.isLocalFile() ) {
	QString path = u.path();
        QFileInfo fi( path );
	if ( fi.extension().lower() == "wmf" )
	    pixmap->setClipart( path );
	else {
	    QPixmap pix( path );
	    pixmap->setPixmap( pix );
	}
    } else {
	pixmap->setPixmap( QPixmap() );
    }
}
