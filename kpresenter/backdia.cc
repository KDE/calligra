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

#include <koPictureFilePreview.h>
#include <backdia.h>
#include <kpbackground.h>
#include <kpresenter_doc.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qpainter.h>
#include <qcombobox.h>
#include <qslider.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qvbox.h>
#include <qcheckbox.h>
#include <qdatetime.h>

#include <kcolorbutton.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kimageio.h>
#include <kbuttonbox.h>
#include <kio/netaccess.h>

/*******************************************************************
 *
 * Class: BackPreview
 *
 *******************************************************************/

/*=============================================================*/
BackPreview::BackPreview( QWidget *parent, KPrPage *page )
    : QFrame( parent )
{
    setFrameStyle( WinPanel | Sunken );
    back = new KPBackGround( page );
    setMinimumSize( 300, 200 );

}

BackPreview::~BackPreview()
{
    delete back;
}

/*=============================================================*/
void BackPreview::drawContents( QPainter *p )
{
    QFrame::drawContents( p );
    p->save();
    p->translate( contentsRect().x(), contentsRect().y() );
    back->draw( p, contentsRect().size(), contentsRect(), false );
    p->restore();
}

/******************************************************************/
/* class BackDia                                                  */
/******************************************************************/

/*=============================================================*/
BackDia::BackDia( QWidget* parent, const char* name,
                  BackType backType, const QColor &backColor1,
                  const QColor &backColor2, BCType _bcType,
                  const QString &backPic, const QDateTime &picLM,
                  const QString &backClip, const QDateTime &clipLM,
                  BackView backPicView, bool _unbalanced,
                  int _xfactor, int _yfactor, KPrPage *m_page )
    : KDialogBase( parent, name, true, "",KDialogBase::Ok|KDialogBase::Apply|KDialogBase::Cancel|KDialogBase::User1|KDialogBase::User2 ), picLastModified( picLM ), clipLastModified( clipLM )
{
    lockUpdate = true;

    oldBackType=backType;
    oldBackColor1=backColor1;
    oldBackColor2 = backColor2;
    oldBcType= _bcType;
    oldBackPic=backPic;
    oldBackClip=backClip;
    oldBackPicView=backPicView;
    oldUnbalanced=_unbalanced;
    oldXFactor=_xfactor;
    oldYFactor=_yfactor;

    QWidget *page = new QWidget( this );
    setMainWidget(page);
    QVBoxLayout *layout = new QVBoxLayout( page, 0, spacingHint() );


    QHBoxLayout *hbox = new QHBoxLayout( layout );
    hbox->setSpacing( 5 );
    QVBoxLayout *vbox = new QVBoxLayout( hbox );
    vbox->setSpacing( 5 );

    vbox->addWidget( new QLabel( i18n( "Background type:" ), page ) );

    backCombo = new QComboBox( false, page );
    backCombo->insertItem( i18n( "Color/Gradient" ) );
    backCombo->insertItem( i18n( "Picture" ) );
    backCombo->insertItem( i18n( "Clipart" ) );
    backCombo->setCurrentItem( (int)backType );
    connect( backCombo, SIGNAL( activated( int ) ),
             this, SLOT( changeComboText(int) ) );

    vbox->addWidget( backCombo );

    tabWidget = new QTabWidget( page );
    vbox->addWidget( tabWidget );

    // color/gradient tab ---------------

    QVBox *colorTab = new QVBox( tabWidget );
    colorTab->setSpacing( 5 );
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

    labXFactor =new QLabel( i18n( "X-Factor:" ), colorTab );

    xfactor = new QSlider( -200, 200, 1, 100, QSlider::Horizontal, colorTab );
    connect( xfactor, SIGNAL( valueChanged( int ) ),
             this, SLOT( updateConfiguration() ) );
    xfactor->setValue( _xfactor );

    labYFactor=new QLabel( i18n( "Y-Factor:" ), colorTab );

    yfactor = new QSlider( -200, 200, 1, 100, QSlider::Horizontal, colorTab );
    connect( yfactor, SIGNAL( valueChanged( int ) ),
             this, SLOT( updateConfiguration() ) );
    yfactor->setValue( _yfactor );

    tabWidget->addTab( colorTab, i18n( "Color/Gradient" ) );

    // picture tab ---------------------

    QVBox *picTab = new QVBox( tabWidget );
    picTab->setSpacing( 5 );
    picTab->setMargin( 5 );

    QLabel *l = new QLabel( i18n( "View mode:" ), picTab );
    l->setFixedHeight( l->sizeHint().height() );

    picView = new QComboBox( false, picTab );
    picView->insertItem( i18n( "Zoomed" ) );
    picView->insertItem( i18n( "Centered" ) );
    picView->insertItem( i18n( "Tiled" ) );
    picView->setCurrentItem( (int)backPicView );
    connect( picView, SIGNAL( activated( int ) ),
             this, SLOT( updateConfiguration() ) );

    picChoose = new QPushButton( i18n( "Choose Picture..." ), picTab, "picChoose" );
    connect( picChoose, SIGNAL( clicked() ),
             this, SLOT( selectPic() ) );

    if ( !backPic.isEmpty() )
        chosenPic = backPic;
    else
        chosenPic = QString::null;

    lPicName = new QLabel( picTab, "picname" );
    lPicName->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    if ( !backPic.isEmpty() )
        lPicName->setText( backPic );
    else
        lPicName->setText( i18n( "No Picture" ) );
    lPicName->setFixedHeight( lPicName->sizeHint().height() );

    (void)new QWidget( picTab );

    tabWidget->addTab( picTab, i18n( "Picture" ) );

    // clipart tab--------------------------

    QVBox *clipTab = new QVBox( tabWidget );
    clipTab->setSpacing( 5 );
    clipTab->setMargin( 5 );

    clipChoose = new QPushButton( i18n( "Choose Clipart..." ), clipTab, "clipChoose" );
    connect( clipChoose, SIGNAL( clicked() ),
             this, SLOT( selectClip() ) );

    if ( !backClip.isEmpty() )
        chosenClip = backClip;

    lClipName = new QLabel( clipTab, "clipname" );
    lClipName->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    if ( !backClip.isEmpty() )
        lClipName->setText( backClip );
    else
        lClipName->setText( i18n( "No Clipart" ) );
    lClipName->setFixedHeight( lClipName->sizeHint().height() );

    (void)new QWidget( clipTab );

    tabWidget->addTab( clipTab, i18n( "Clipart" ) );

    // ------------------------ preview

    preview = new BackPreview( page, m_page );
    hbox->addWidget( preview );

    // ------------------------ buttons

    connect( this, SIGNAL( okClicked() ),
             this, SLOT( Ok() ) );
    connect( this, SIGNAL( applyClicked() ),
             this, SLOT( Apply() ) );
    connect( this, SIGNAL(  user1Clicked() ),
             this, SLOT( ApplyGlobal() ) );

    connect( this, SIGNAL(  user2Clicked() ),
             this, SLOT( slotReset() ) );

    connect( this, SIGNAL( okClicked() ),
             this, SLOT( accept() ) );
    setButtonText(KDialogBase::User1,i18n( "Apply &Global" ));
    setButtonText(KDialogBase::User2,i18n( "Reset" ));
    picChanged = clipChanged = true;
    lockUpdate = false;
    updateConfiguration();
}

void BackDia::slotReset()
{
    backCombo->setCurrentItem( (int)oldBackType );
    color1Choose->setColor( oldBackColor1 );
    color2Choose->setColor( oldBackColor2 );
    cType->setCurrentItem( oldBcType );

    if ( !oldBackPic.isEmpty() )
        chosenPic = oldBackPic;
    else
        chosenPic = QString::null;

    if ( !oldBackPic.isEmpty() )
        lPicName->setText( oldBackPic );
    else
        lPicName->setText( i18n( "No Picture" ) );

    if ( !oldBackClip.isEmpty() )
        chosenClip = oldBackClip;

    if ( !oldBackClip.isEmpty() )
        lClipName->setText( oldBackClip );
    else
        lClipName->setText( i18n( "No Clipart" ) );
    picView->setCurrentItem( (int)oldBackPicView );
    unbalanced->setChecked( oldUnbalanced );
    xfactor->setValue( oldXFactor );
    yfactor->setValue( oldYFactor );
    updateConfiguration();
}

void BackDia::changeComboText(int _p)
{
    if(_p!=tabWidget->currentPageIndex ())
        tabWidget->setCurrentPage(_p);
    updateConfiguration();
}

/*=============================================================*/
void BackDia::showEvent( QShowEvent *e )
{
    QDialog::showEvent( e );
    lockUpdate = false;
    updateConfiguration();
}

/*=============================================================*/
void BackDia::updateConfiguration()
{
    if ( lockUpdate )
        return;

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

    picChanged = getBackType() == BT_PICTURE;
    clipChanged = getBackType() == BT_CLIPART;
    preview->backGround()->setBackType( getBackType() );
    preview->backGround()->setBackView( getBackView() );
    preview->backGround()->setBackColor1( getBackColor1() );
    preview->backGround()->setBackColor2( getBackColor2() );
    preview->backGround()->setBackColorType( getBackColorType() );
    preview->backGround()->setBackUnbalanced( getBackUnbalanced() );
    preview->backGround()->setBackXFactor( getBackXFactor() );
    preview->backGround()->setBackYFactor( getBackYFactor() );
    if ( !chosenPic.isEmpty() && picChanged )
        preview->backGround()->setBackPixmap( getBackPixFilename(), picLastModified );
    if ( !chosenClip.isEmpty() && clipChanged )
        preview->backGround()->setBackClipart( getBackClipFilename(), clipLastModified );
    preview->backGround()->setBackType( getBackType() );
    if ( preview->isVisible() && isVisible() ) {
        preview->backGround()->reload();
        preview->repaint( false );
    }

    picChanged = clipChanged = false;
}

/*=============================================================*/
BackType BackDia::getBackType() const
{
    return (BackType)backCombo->currentItem();
}

/*=============================================================*/
BackView BackDia::getBackView() const
{
    return (BackView)picView->currentItem();
}

/*=============================================================*/
QColor BackDia::getBackColor1() const
{
    return color1Choose->color();
}

/*=============================================================*/
QColor BackDia::getBackColor2() const
{
    return color2Choose->color();
}

/*=============================================================*/
BCType BackDia::getBackColorType() const
{
    return (BCType)cType->currentItem();
}

/*=============================================================*/
bool BackDia::getBackUnbalanced() const
{
    return unbalanced->isChecked();
}

/*=============================================================*/
int BackDia::getBackXFactor() const
{
    return xfactor->value();
}

/*=============================================================*/
int BackDia::getBackYFactor() const
{
    return yfactor->value();
}



/*=============================================================*/
QString BackDia::selectPicture( const QString& pattern )
{
    KURL url;

    KFileDialog fd( QString::null, pattern, 0, 0, true );
    fd.setPreviewWidget( new KoPictureFilePreview( &fd ) );

    if ( fd.exec() == QDialog::Accepted )
    {
        KURL url = fd.selectedURL();
        QString strTemp = QString::null;
        // ### FIXME: Problem: for a remote file, we have no typical image/clipart extension for the temp file name.
        if (!KIO::NetAccess::download( url, strTemp ))
            return QString::null;
        lPicName->setText( url.prettyURL() );
        return strTemp;
        // ### TODO/FIXME Problem : when to remove the temp file ?
    }
    return QString::null;
}

/*=============================================================*/
void BackDia::selectPic()
{
    QString strResult=selectPicture( KImageIO::pattern(KImageIO::Reading) );
    if ( strResult.isEmpty() )
        return;

    backCombo->setCurrentItem( 1 );
    chosenPic=strResult;
    picChanged = true;
    picLastModified = QDateTime();
    updateConfiguration();
}

/*=============================================================*/
void BackDia::selectClip()
{
    QString strResult=selectPicture( KoPictureFilePreview::clipartPattern() );
    if ( strResult.isEmpty() )
        return;

    backCombo->setCurrentItem( 2 );
    chosenClip=strResult;
    clipChanged = true;
    clipLastModified = QDateTime();
    updateConfiguration();
}

#include <backdia.moc>
