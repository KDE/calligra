/* vi: ts=8 sts=4 sw=4
 *
 * This file is part of the KDE project
 * Copyright (C) 2001 Martin R. Jones <mjones@kde.org>
 *
 * You can Freely distribute this program under the GNU General Public
 * License. See the file "COPYING" for the exact licensing terms.
 */

#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>
#include <qtimer.h>

#include <kapp.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kdebug.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kimageio.h>

#include "preview.h"

/**** KImageFilePreview ****/

KImageFilePreview::KImageFilePreview( QWidget *parent )
    : QWidget( parent )
{
    autoMode = FALSE;

    QVBoxLayout *vb = new QVBoxLayout( this, KDialog::marginHint() );

    imageLabel = new QLabel( this );
    imageLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    imageLabel->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    imageLabel->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    vb->addWidget( imageLabel, 1 );

    infoLabel = new QLabel( this );
    vb->addWidget( infoLabel );

    QHBoxLayout *hb = new QHBoxLayout( vb );

    autoPreview = new QCheckBox( i18n("&Automatic preview"), this );
    hb->addWidget( autoPreview );
    connect( autoPreview, SIGNAL(toggled(bool)), SLOT(toggleAuto(bool)) );

    previewButton = new QPushButton( i18n("&Preview"), this );
    hb->addWidget( previewButton );
    connect( previewButton, SIGNAL(clicked()), SLOT(updatePreview()) );

    timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()), SLOT(showImage()) );
}

void KImageFilePreview::showPreview( const KURL &url )
{
    if ( url != currentURL )
    {
	currentURL = url;
	currentImage = QImage();
	if ( autoMode )
	{
	    if ( currentURL.isLocalFile() )
		currentImage.load( currentURL.path() );
	}
	    
	timer->start( 20, true );
    }
}

void KImageFilePreview::updatePreview()
{
    if ( currentURL.isLocalFile() )
	currentImage.load( currentURL.path() );
    timer->start( 0, true );
}

void KImageFilePreview::toggleAuto( bool a )
{
    autoMode = a;
    if ( autoMode )
    {
	updatePreview();
    }
}

void KImageFilePreview::resizeEvent( QResizeEvent * )
{
    timer->start( 100, true );
}

QSize KImageFilePreview::sizeHint() const
{
    return QSize( 20, 200 ); // otherwise it ends up huge???
}

void KImageFilePreview::showImage()
{
    QPixmap pm;
    if ( !currentImage.isNull() ) {
	infoLabel->setText( i18n( "%1bpp image, %2 x %3 pixels" )
	    .arg(currentImage.depth()).arg(currentImage.width())
	    .arg(currentImage.height()) );
	double hscale = double(imageLabel->contentsRect().width()-4) /
			currentImage.width();
	double vscale = double(imageLabel->contentsRect().height()-4) /
			currentImage.height();
	if ( hscale < 1.0 || vscale < 1.0 )
	{
	    double scale = QMIN( hscale, vscale );
	    int w = int(currentImage.width() * scale);
	    int h = int(currentImage.height() * scale);
	    pm.convertFromImage( currentImage.smoothScale( w, h ) );
	}
	else
	    pm.convertFromImage( currentImage );
    }
    if ( pm.isNull() )
	infoLabel->clear();
    imageLabel->setPixmap( pm );
}


