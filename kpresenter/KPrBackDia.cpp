// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <KoPictureFilePreview.h>
#include "KPrBackDia.h"
#include "KPrBackground.h"
#include "KPrDocument.h"
#include "KPrPage.h"

#include <QLabel>
#include <qpainter.h>
#include <QComboBox>
#include <qslider.h>
#include <QLayout>
#include <qtabwidget.h>

#include <QCheckBox>
#include <qdatetime.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QShowEvent>
#include <Q3Frame>
#include <Q3VBoxLayout>
#include <kvbox.h>

#include <kcolorbutton.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kimageio.h>
#include <kbuttonbox.h>
#include <kurlrequester.h>

#include <KoPicture.h>
#include <kvbox.h>

KPrBackPreview::KPrBackPreview( QWidget *parent, KPrPage *page )
    : Q3Frame( parent )
{
    setFrameStyle( WinPanel | Sunken );
    back = new KPrBackGround( page );
    setMinimumSize( 300, 200 );
}

KPrBackPreview::~KPrBackPreview()
{
    delete back;
}

void KPrBackPreview::drawContents( QPainter *p )
{
    Q3Frame::drawContents( p );
    p->save();
    p->translate( contentsRect().x(), contentsRect().y() );
    back->drawBackground( p, contentsRect().size(), contentsRect(), false );
    p->restore();
}

KPrBackDialog::KPrBackDialog( QWidget* parent, const char* name,
                  BackType backType, const QColor &backColor1,
                  const QColor &backColor2, BCType _bcType,
                  const KoPicture &backPic,
                  BackView backPicView, bool _unbalanced,
                  int _xfactor, int _yfactor, KPrPage *_page )
    : KDialog( parent ),
      m_useMasterBackground( 0 ), m_picture(backPic),m_oldpicture(backPic), m_page( _page )
{
    setButtons(Ok|Apply|Cancel|User1|User2);

    lockUpdate = true;

    oldUseMasterBackground=m_page->useMasterBackground();
    oldBackType=backType;
    oldBackColor1=backColor1;
    oldBackColor2 = backColor2;
    oldBcType= _bcType;
    oldBackPicView=backPicView;
    oldUnbalanced=_unbalanced;
    oldXFactor=_xfactor;
    oldYFactor=_yfactor;

    QWidget *page = new QWidget( this );
    setMainWidget(page);
    Q3VBoxLayout *layout = new Q3VBoxLayout( page, 0, spacingHint() );


    Q3HBoxLayout *hbox = new Q3HBoxLayout( layout );
    hbox->setSpacing( KDialog::spacingHint() );
    Q3VBoxLayout *vbox = new Q3VBoxLayout( hbox );
    vbox->setSpacing( KDialog::spacingHint() );

    if ( !m_page->isMasterPage() )
    {
        m_useMasterBackground = new QCheckBox( i18n( "Use slide master background" ), page );
        connect( m_useMasterBackground, SIGNAL( clicked() ),
                 this, SLOT( updateConfiguration() ) );
        m_useMasterBackground->setChecked( m_page->useMasterBackground() );
        vbox->addWidget( m_useMasterBackground );
    }
    vbox->addWidget( new QLabel( i18n( "Background type:" ), page ) );

    backCombo = new QComboBox( false, page );
    backCombo->insertItem( i18n( "Color/Gradient" ) );
    backCombo->insertItem( i18n( "Picture" ) );
    backCombo->setCurrentItem( (int)backType );
    connect( backCombo, SIGNAL( activated( int ) ),
             this, SLOT( changeComboText(int) ) );

    vbox->addWidget( backCombo );

    tabWidget = new QTabWidget( page );
    vbox->addWidget( tabWidget );

    // color/gradient tab ---------------

    KVBox *colorTab = new KVBox( tabWidget );
    colorTab->setSpacing( KDialog::spacingHint() );
    colorTab->setMargin( 5 );

    cType = new QComboBox( false, colorTab );
    cType->insertItem( i18n( "Plain" ) );
    cType->insertItem( i18n( "Vertical Gradient" ) );
    cType->insertItem( i18n( "Horizontal Gradient" ) );
    cType->insertItem( i18n( "Diagonal Gradient 1" ) );
    cType->insertItem( i18n( "Diagonal Gradient 2" ) );
    cType->insertItem( i18n( "Circle Gradient" ) );
    cType->insertItem( i18n( "Rectangle Gradient" ) );
    cType->insertItem( i18n( "PipeCross Gradient" ) );
    cType->insertItem( i18n( "Pyramid Gradient" ) );
    cType->setCurrentItem( _bcType );
    connect( cType, SIGNAL( activated( int ) ),
             this, SLOT( updateConfiguration() ) );

    color1Choose = new KColorButton( backColor1, colorTab );
    connect( color1Choose, SIGNAL( changed( const QColor& ) ),
             this, SLOT( updateConfiguration() ) );

    color2Choose = new KColorButton( backColor2, colorTab );
    connect( color2Choose, SIGNAL( changed( const QColor& ) ),
             this, SLOT( updateConfiguration() ) );

    unbalanced = new QCheckBox( i18n( "Unbalanced" ), colorTab );
    connect( unbalanced, SIGNAL( clicked() ),
             this, SLOT( updateConfiguration() ) );
    unbalanced->setChecked( _unbalanced );

    labXFactor =new QLabel( i18n( "X-factor:" ), colorTab );

    xfactor = new QSlider( -200, 200, 1, 100, Qt::Horizontal, colorTab );
    connect( xfactor, SIGNAL( valueChanged( int ) ),
             this, SLOT( updateConfiguration() ) );
    xfactor->setValue( _xfactor );

    labYFactor=new QLabel( i18n( "Y-factor:" ), colorTab );

    yfactor = new QSlider( -200, 200, 1, 100, Qt::Horizontal, colorTab );
    connect( yfactor, SIGNAL( valueChanged( int ) ),
             this, SLOT( updateConfiguration() ) );
    yfactor->setValue( _yfactor );

    tabWidget->addTab( colorTab, i18n( "Color/Gradient" ) );

    // picture tab ---------------------

    KVBox *picTab = new KVBox( tabWidget );
    picTab->setSpacing( KDialog::spacingHint() );
    picTab->setMargin( 5 );

    QLabel *l = new QLabel( i18n( "View mode:" ), picTab );
    l->setFixedHeight( l->sizeHint().height() );

    picView = new QComboBox( false, picTab );
    picView->insertItem( i18n( "Scaled" ) );
    picView->insertItem( i18n( "Centered" ) );
    picView->insertItem( i18n( "Tiled" ) );
    picView->setCurrentItem( (int)backPicView );
    connect( picView, SIGNAL( activated( int ) ),
             this, SLOT( updateConfiguration() ) );

    picChooseLabel = new QLabel( i18n("&Location:"), picTab );
    picChooseLabel->setFixedHeight( picChooseLabel->sizeHint().height() );

    picChoose = new KUrlRequester( picTab );
    picChoose->setFixedHeight( picChoose->sizeHint().height() );
    picChoose->setMode( KFile::ExistingOnly );
    connect( picChoose, SIGNAL( openFileDialog( KUrlRequester * ) ),
             SLOT( aboutToSelectPic() ) );
    connect( picChoose, SIGNAL( urlSelected( const QString & ) ),
             SLOT( afterSelectPic( const QString & ) ) );

    picChooseLabel->setBuddy( picChoose );

    (void)new QWidget( picTab );

    tabWidget->addTab( picTab, i18n( "Picture" ) );

    // ------------------------ preview

    preview = new KPrBackPreview( page, m_page );
    hbox->addWidget( preview );

    // ------------------------ buttons

    connect( this, SIGNAL( okClicked() ),
             this, SLOT( ok() ) );
    connect( this, SIGNAL( applyClicked() ),
             this, SLOT( apply() ) );
    connect( this, SIGNAL(  user1Clicked() ),
             this, SLOT( applyGlobal() ) );

    connect( this, SIGNAL(  user2Clicked() ),
             this, SLOT( slotReset() ) );

    connect( this, SIGNAL( okClicked() ),
             this, SLOT( accept() ) );
    setButtonText(KDialog::User1,i18n( "Apply &Global" ));
    setButtonText(KDialog::User2,i18n( "&Reset" ));
    picChanged = true;
    lockUpdate = false;
    updateConfiguration();
}

void KPrBackDialog::slotReset()
{
    if ( m_useMasterBackground )
        m_useMasterBackground->setChecked( oldUseMasterBackground );
    backCombo->setCurrentItem( (int)oldBackType );
    color1Choose->setColor( oldBackColor1 );
    color2Choose->setColor( oldBackColor2 );
    cType->setCurrentItem( oldBcType );

    m_picture=m_oldpicture;

    if ( !m_picture.isNull() )
        picChoose->setUrl( m_picture.getKey().filename() );
    else
        picChoose->setUrl( KUrl(""));

    picView->setCurrentItem( (int)oldBackPicView );
    unbalanced->setChecked( oldUnbalanced );
    xfactor->setValue( oldXFactor );
    yfactor->setValue( oldYFactor );
    updateConfiguration();
}

void KPrBackDialog::changeComboText(int _p)
{
    if(_p!=tabWidget->currentPageIndex ())
        tabWidget->setCurrentPage(_p);
    updateConfiguration();
}

void KPrBackDialog::showEvent( QShowEvent *e )
{
    QDialog::showEvent( e );
    lockUpdate = false;
    updateConfiguration();
}

void KPrBackDialog::updateConfiguration()
{
    if ( lockUpdate )
        return;

    if ( m_useMasterBackground )
    {
        tabWidget->setEnabled( !m_useMasterBackground->isChecked() );
        backCombo->setEnabled( !m_useMasterBackground->isChecked() );
    }
    else
    {
        tabWidget->setEnabled( true );
        backCombo->setEnabled( true );
    }
    if ( getBackColorType() == BCT_PLAIN )
    {
        unbalanced->setEnabled( false );
        xfactor->setEnabled( false );
        yfactor->setEnabled( false );
        labXFactor->setEnabled(false);
        labYFactor->setEnabled(false);
        color2Choose->setEnabled( false );
    }
    else
    {
        unbalanced->setEnabled( true );
        if ( unbalanced->isChecked() )
        {
            xfactor->setEnabled( true );
            yfactor->setEnabled( true );
            labXFactor->setEnabled(true);
            labYFactor->setEnabled(true);
        }
        else
        {
            xfactor->setEnabled( false );
            yfactor->setEnabled( false );
            labXFactor->setEnabled(false);
            labYFactor->setEnabled(false);
        }
        color2Choose->setEnabled( true );
    }

    if ( m_useMasterBackground && m_useMasterBackground->isChecked() )
    {
        kdDebug(33001) << "set backgound to master" << endl;
        preview->backGround()->setBackGround( m_page->masterPage()->background()->getBackGround() );
        preview->repaint( true );
    }
    else
    {
        picChanged = (getBackType() == BT_PICTURE);
        preview->backGround()->setBackType( getBackType() );
        preview->backGround()->setBackView( getBackView() );
        preview->backGround()->setBackColor1( getBackColor1() );
        preview->backGround()->setBackColor2( getBackColor2() );
        preview->backGround()->setBackColorType( getBackColorType() );
        preview->backGround()->setBackUnbalanced( getBackUnbalanced() );
        preview->backGround()->setBackXFactor( getBackXFactor() );
        preview->backGround()->setBackYFactor( getBackYFactor() );
        if ( !m_picture.isNull() && picChanged )
            preview->backGround()->setBackPicture( m_picture );
        preview->backGround()->setBackType( getBackType() );
        if ( preview->isVisible() && isVisible() ) {
            preview->backGround()->reload(); // ### TODO: instead of reloading, load or remove the picture correctly.
            preview->repaint( true );
        }

        picChanged  = false;
    }
}

BackType KPrBackDialog::getBackType() const
{
    return (BackType)backCombo->currentItem();
}

BackView KPrBackDialog::getBackView() const
{
    return (BackView)picView->currentItem();
}

QColor KPrBackDialog::getBackColor1() const
{
    return color1Choose->color();
}

QColor KPrBackDialog::getBackColor2() const
{
    return color2Choose->color();
}

BCType KPrBackDialog::getBackColorType() const
{
    return (BCType)cType->currentItem();
}

bool KPrBackDialog::getBackUnbalanced() const
{
    return unbalanced->isChecked();
}

int KPrBackDialog::getBackXFactor() const
{
    return xfactor->value();
}

int KPrBackDialog::getBackYFactor() const
{
    return yfactor->value();
}

KPrBackGround::Settings KPrBackDialog::getBackGround() const
{
    return KPrBackGround::Settings( getBackType(), getBackColor1(),
                                   getBackColor2(), getBackColorType(),
                                   getBackUnbalanced(), getBackXFactor(),
                                   getBackYFactor(), getBackPicture().getKey(),
                                   getBackView() );
}

bool KPrBackDialog::useMasterBackground() const
{
    return m_useMasterBackground ? m_useMasterBackground->isChecked():false;
}

void KPrBackDialog::aboutToSelectPic()
{
    QStringList mimetypes;
    mimetypes += KImageIO::mimeTypes( KImageIO::Reading );
    mimetypes += KoPictureFilePreview::clipartMimeTypes();

    picChoose->fileDialog()->setMimeFilter( mimetypes );
    picChoose->fileDialog()->setPreviewWidget( new KoPictureFilePreview( picChoose->fileDialog() ) );
}

void KPrBackDialog::afterSelectPic( const QString &url )
{
    KoPicture picture;
    picture.setKeyAndDownloadPicture(url, tabWidget);

    if ( picture.isNull() )
        return;

    backCombo->setCurrentItem( 1 );
    m_picture=picture;
    picChanged = true;
    updateConfiguration();
}

#include "KPrGradient.h"
#include "KPrBackDia.moc"
