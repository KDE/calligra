/* This file is part of the KDE project

   Copyright (C) 2002 Toshitaka Fujioka <fujioka@kde.org>

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

#include <confpicturedia.h>

#include <qapplication.h>
#include <qbuttongroup.h>
#include <qvbuttongroup.h>
#include <qgroupbox.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qimage.h>

#include <knuminput.h>
#include <klocale.h>
#include <kbuttonbox.h>

#include <stdlib.h>
#include <math.h>

/******************************************************************/
/* class PicturePreview                                           */
/******************************************************************/

/*==================== constructor ===============================*/
PicturePreview::PicturePreview( QWidget* parent, const char* name, PictureMirrorType _mirrorType,
                                int _depth, bool _swapRGB, int _bright, QPixmap _origPixmap )
    : QFrame( parent, name )
{
    setFrameStyle( WinPanel | Sunken );
    mirrorType = _mirrorType;
    depth = _depth;
    swapRGB = _swapRGB;
    bright = _bright;
    origPixmap = _origPixmap;

    setMinimumSize( 300, 200 );
}

/*====================== draw contents ===========================*/
void PicturePreview::drawContents( QPainter *painter )
{
    QSize ext = contentsRect().size();

    QPixmap _pix = origPixmap;
    QImage img( _pix.convertToImage().smoothScale( ext.width(),ext.height(), QImage::ScaleMin ) );

    bool _horizontal = false;
    bool _vertical = false;
    if ( mirrorType == PM_HORIZONTAL )
        _horizontal = true;
    else if ( mirrorType == PM_VERTICAL )
        _vertical = true;
    else if ( mirrorType == PM_HORIZONTALANDVERTICAL ) {
        _horizontal = true;
        _vertical = true;
    }

    img = img.mirror( _horizontal, _vertical );

    if ( depth != 0 ) {
        QImage tmpImg = img.convertDepth( depth );
        if ( !tmpImg.isNull() )
            img = tmpImg;
    }

    if ( swapRGB )
        img = img.swapRGB();


    if ( bright != 0 ) {
        if ( depth == 1 || depth == 8 ) {
            for ( int i = 0; i < img.numColors(); ++i ) {
                QRgb rgb = img.color( i );
                QColor c( rgb );

                if ( bright > 0 )
                    rgb = c.light( 100 + bright ).rgb();
                else
                    rgb = c.dark( 100 + abs( bright ) ).rgb();

                img.setColor( i, rgb );
            }
        }
        else {
            int _width = img.width();
            int _height = img.height();
            int _x = 0;
            int _y = 0;

            for ( _x = 0; _x < _width; ++_x ) {
                for ( _y = 0; _y < _height; ++_y ) {
                    if ( img.valid( _x, _y ) ) {
                        QRgb rgb = img.pixel( _x, _y );
                        QColor c( rgb );

                        if ( bright > 0 )
                            rgb = c.light( 100 + bright ).rgb();
                        else
                            rgb = c.dark( 100 + abs( bright ) ).rgb();

                        img.setPixel( _x, _y, rgb );
                    }
                }
            }
        }
    }

    _pix.convertFromImage( img );

    QPixmap tmpPix( _pix.size() );
    tmpPix.fill( Qt::white );

    QPainter _p;
    _p.begin( &tmpPix );
    _p.drawPixmap( 0, 0, _pix );
    _p.end();

    QSize _pixSize = _pix.size();
    int _x = 0, _y = 0;
    int w = _pixSize.width(), h = _pixSize.height();
    _x = ( ext.width() - w ) / 2;
    _y = ( ext.height() - h ) / 2;

    painter->drawPixmap( _x, _y, tmpPix );
}

void PicturePreview::slotNormalPicture()
{
    mirrorType = PM_NORMAL;
    repaint();
}

void PicturePreview::slotHorizontalMirrorPicture()
{
    mirrorType = PM_HORIZONTAL;
    repaint();
}

void PicturePreview::slotVerticalMirrorPicture()
{
    mirrorType = PM_VERTICAL;
    repaint();
}

void PicturePreview::slotHorizontalAndVerticalMirrorPicture()
{
    mirrorType = PM_HORIZONTALANDVERTICAL;
    repaint();
}

void PicturePreview::slotPictureDepth0()
{
    depth = 0;
    repaint();
}

void PicturePreview::slotPictureDepth1()
{
    depth = 1;
    repaint();
}

void PicturePreview::slotPictureDepth8()
{
    depth = 8;
    repaint();
}

void PicturePreview::slotPictureDepth16()
{
    depth = 16;
    repaint();
}

void PicturePreview::slotPictureDepth32()
{
    depth = 32;
    repaint();
}

void PicturePreview::slotSwapRGBPicture( bool _on )
{
    swapRGB = _on;
    repaint();
}

void PicturePreview::slotBrightValue( int _value )
{
    bright = _value;
    repaint();
}

/******************************************************************/
/* class ConfPictureDia                                           */
/******************************************************************/

/*==================== constructor ===============================*/
ConfPictureDia::ConfPictureDia( QWidget *parent, const char *name, PictureMirrorType _mirrorType,
                                int _depth, bool _swapRGB, int _bright, QPixmap _origPixmap )
    : KDialogBase( parent, name, true )
{
    mirrorType = _mirrorType;
    depth = _depth;
    swapRGB = _swapRGB;
    bright = _bright;
    origPixmap = _origPixmap;

    // ------------------------ layout
    QWidget *page = new QWidget( this );
    setMainWidget( page );
    QVBoxLayout *layout = new QVBoxLayout( page, 0, spacingHint() );
    QHBoxLayout *hbox = new QHBoxLayout( layout );
    hbox->setSpacing( 5 );

    // ------------------------ settings
    gSettings = new QGroupBox( 1, Qt::Horizontal, i18n( "Settings" ), page );


    QButtonGroup *mirrorGroup = new QVButtonGroup( i18n( "Mirror" ), gSettings );

    m_normalPicture = new QRadioButton( i18n( "Normal" ), mirrorGroup );
    connect( m_normalPicture, SIGNAL( clicked() ), this, SLOT( slotNormalPicture() ) );

    m_horizontalMirrorPicture = new QRadioButton( i18n( "Horizontal Mirror" ), mirrorGroup );
    connect( m_horizontalMirrorPicture, SIGNAL( clicked() ), this, SLOT( slotHorizontalMirrorPicture() ) );

    m_verticalMirrorPicture = new QRadioButton( i18n( "Vertical Mirror" ), mirrorGroup );
    connect( m_verticalMirrorPicture, SIGNAL( clicked() ), this, SLOT( slotVerticalMirrorPicture() ) );

    m_horizontalAndVerticalMirrorPicture = new QRadioButton( i18n( "Horizontal And Vertical Mirror" ), mirrorGroup );
    connect( m_horizontalAndVerticalMirrorPicture, SIGNAL( clicked() ), this, SLOT( slotHorizontalAndVerticalMirrorPicture() ) );


    if ( mirrorType == PM_NORMAL ) {
        m_normalPicture->setChecked( true );
        m_horizontalMirrorPicture->setChecked( false );
        m_verticalMirrorPicture->setChecked( false );
        m_horizontalAndVerticalMirrorPicture->setChecked( false );
    }
    else if ( mirrorType == PM_HORIZONTAL ) {
        m_normalPicture->setChecked( false );
        m_horizontalMirrorPicture->setChecked( true );
        m_verticalMirrorPicture->setChecked( false );
        m_horizontalAndVerticalMirrorPicture->setChecked( false );
    }
    else if ( mirrorType == PM_VERTICAL ) {
        m_normalPicture->setChecked( false );
        m_horizontalMirrorPicture->setChecked( false );
        m_verticalMirrorPicture->setChecked( true );
        m_horizontalAndVerticalMirrorPicture->setChecked( false );
    }
    else if ( mirrorType == PM_HORIZONTALANDVERTICAL ) {
        m_normalPicture->setChecked( false );
        m_horizontalMirrorPicture->setChecked( false );
        m_verticalMirrorPicture->setChecked( false );
        m_horizontalAndVerticalMirrorPicture->setChecked( true );
    }


    QButtonGroup *depthGroup = new QVButtonGroup( i18n( "Depth" ), gSettings );

    m_depth0 = new QRadioButton( i18n( "Default Color Mode" ), depthGroup );
    connect( m_depth0, SIGNAL( clicked() ), this, SLOT( slotPictureDepth0() ) );

    m_depth1 = new QRadioButton( i18n( "Grayscal Color Mode" ), depthGroup );
    connect( m_depth1, SIGNAL( clicked() ), this, SLOT( slotPictureDepth1() ) );

    m_depth8 = new QRadioButton( i18n( "8 Bit Color Mode" ), depthGroup );
    connect( m_depth8, SIGNAL( clicked() ), this, SLOT( slotPictureDepth8() ) );

    m_depth16 = new QRadioButton( i18n( "16 Bit Color Mode" ), depthGroup );
    connect( m_depth16, SIGNAL( clicked() ), this, SLOT( slotPictureDepth16() ) );

    m_depth32 = new QRadioButton( i18n( "32 Bit Color Mode" ), depthGroup );
    connect( m_depth32, SIGNAL( clicked() ), this, SLOT( slotPictureDepth32() ) );


    if ( depth == 0 ) {
        m_depth0->setChecked( true );
        m_depth1->setChecked( false );
        m_depth8->setChecked( false );
        m_depth16->setChecked( false );
        m_depth32->setChecked( false );
    }
    else if ( depth == 1 ) {
        m_depth0->setChecked( false );
        m_depth1->setChecked( true );
        m_depth8->setChecked( false );
        m_depth16->setChecked( false );
        m_depth32->setChecked( false );
    }
    else if ( depth == 8 ) {
        m_depth0->setChecked( false );
        m_depth1->setChecked( false );
        m_depth8->setChecked( true );
        m_depth16->setChecked( false );
        m_depth32->setChecked( false );
    }
    else if ( depth == 16 ) {
        m_depth0->setChecked( false );
        m_depth1->setChecked( false );
        m_depth8->setChecked( false );
        m_depth16->setChecked( true );
        m_depth32->setChecked( false );
    }
    else if ( depth == 32 ) {
        m_depth0->setChecked( false );
        m_depth1->setChecked( false );
        m_depth8->setChecked( false );
        m_depth16->setChecked( false );
        m_depth32->setChecked( true );
    }


    m_swapRGBCheck = new QCheckBox( i18n( "Convert from RGB image to BGR image" ), gSettings );
    connect( m_swapRGBCheck, SIGNAL( toggled( bool ) ), this, SLOT( slotSwapRGBPicture( bool ) ) );
    m_swapRGBCheck->setChecked( swapRGB );


    m_brightValue = new KIntNumInput( bright, gSettings );
    m_brightValue->setRange( -1000, 1000, 10 );
    m_brightValue->setLabel( i18n( "Bright:" ) );
    m_brightValue->setSuffix( i18n( " %" ) );
    connect( m_brightValue, SIGNAL( valueChanged( int ) ), this, SLOT( slotBrightValue( int ) ) );

    hbox->addWidget( gSettings );

    // ------------------------ preview
    picturePreview = new PicturePreview( page, "preview", mirrorType, depth, swapRGB, bright, origPixmap );
    hbox->addWidget( picturePreview );


    connect( m_normalPicture, SIGNAL( clicked() ), picturePreview, SLOT( slotNormalPicture() ) );

    connect( m_horizontalMirrorPicture, SIGNAL( clicked() ), picturePreview, SLOT( slotHorizontalMirrorPicture() ) );

    connect( m_verticalMirrorPicture, SIGNAL( clicked() ), picturePreview, SLOT( slotVerticalMirrorPicture() ) );

    connect( m_horizontalAndVerticalMirrorPicture, SIGNAL( clicked() ), picturePreview, SLOT( slotHorizontalAndVerticalMirrorPicture() ) );

    connect( m_depth0, SIGNAL( clicked() ), picturePreview, SLOT( slotPictureDepth0() ) );

    connect( m_depth1, SIGNAL( clicked() ), picturePreview, SLOT( slotPictureDepth1() ) );

    connect( m_depth8, SIGNAL( clicked() ), picturePreview, SLOT( slotPictureDepth8() ) );

    connect( m_depth16, SIGNAL( clicked() ), picturePreview, SLOT( slotPictureDepth16() ) );

    connect( m_depth32, SIGNAL( clicked() ), picturePreview, SLOT( slotPictureDepth32() ) );

    connect( m_swapRGBCheck, SIGNAL( toggled( bool ) ), picturePreview, SLOT( slotSwapRGBPicture( bool ) ) );

    connect( m_brightValue, SIGNAL( valueChanged( int ) ), picturePreview, SLOT( slotBrightValue( int ) ) );


    connect( this, SIGNAL( okClicked() ), this, SLOT( Apply() ) );
    connect( this, SIGNAL( applyClicked() ), this, SLOT( Apply() ) );
    connect( this, SIGNAL( okClicked() ), this, SLOT( accept() ) );
}

/*===================== destructor ===============================*/
ConfPictureDia::~ConfPictureDia()
{
    delete picturePreview;
}

void ConfPictureDia::slotNormalPicture()
{
    mirrorType = PM_NORMAL;
}

void ConfPictureDia::slotHorizontalMirrorPicture()
{
    mirrorType = PM_HORIZONTAL;
}

void ConfPictureDia::slotVerticalMirrorPicture()
{
    mirrorType = PM_VERTICAL;
}

void ConfPictureDia::slotHorizontalAndVerticalMirrorPicture()
{
    mirrorType = PM_HORIZONTALANDVERTICAL;
}

void ConfPictureDia::slotPictureDepth0()
{
    depth = 0;
}

void ConfPictureDia::slotPictureDepth1()
{
    depth = 1;
}

void ConfPictureDia::slotPictureDepth8()
{
    depth = 8;
}

void ConfPictureDia::slotPictureDepth16()
{
    depth = 16;
}

void ConfPictureDia::slotPictureDepth32()
{
    depth = 32;
}

void ConfPictureDia::slotSwapRGBPicture( bool _on )
{
    swapRGB = _on;
}

void ConfPictureDia::slotBrightValue( int _value )
{
    bright = _value;
}

#include <confpicturedia.moc>
