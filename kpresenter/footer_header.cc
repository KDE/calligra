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
/* Module: Footer/Header                                          */
/******************************************************************/

#include "footer_header.h"
#include "footer_header.moc"
#include "kptextobject.h"
#include "styledia.h"
#include "global.h"
#include "kpresenter_doc.h"
#include "kpresenter_view.h"

#include <qtabwidget.h>
#include <qpoint.h>
#include <qcolor.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qhbox.h>
#include <qpixmap.h>
#include <qstrlist.h>
#include <qcombobox.h>

#include <klocale.h>
#include <kbuttonbox.h>
#include <kapp.h>
#include <kcolordlg.h>
#include <kiconloader.h>

#include <stdlib.h>

/******************************************************************/
/* Class: KPFooterHeaderEditor                                    */
/******************************************************************/

/*================================================================*/
KPFooterHeaderEditor::KPFooterHeaderEditor( KPresenterDoc *_doc )
    : QVBox( 0L ), _allowClose( false )
{
    setMargin( 10 );

    doc = _doc;

    tabwidget = new QTabWidget( this );

    setupHeader();
    setupFooter();

    QWidget *w = new QWidget( this );
    w->setMaximumHeight( 10 );
    w->setMinimumHeight( 10 );

    KButtonBox *bb = new KButtonBox( this );
    bb->addStretch();
    updatePage = bb->addButton( i18n( "Update Page" ) );
    connect( updatePage, SIGNAL( clicked() ), this, SLOT( slotUpdatePage() ) );
    closeDia = bb->addButton( i18n( "Close" ) );
    connect( closeDia, SIGNAL( clicked() ), this, SLOT( slotCloseDia() ) );

    bb->layout();
    bb->setMaximumHeight( bb->sizeHint().height() );

    resize( 600, 300 );
    htool2->updateRects( true );
    ftool2->updateRects( true );
}

/*================================================================*/
KPFooterHeaderEditor::~KPFooterHeaderEditor()
{
    doc->header()->getKTextObject()->reparent( 0L, 0, QPoint( 0, 0 ), false );
    doc->footer()->getKTextObject()->reparent( 0L, 0, QPoint( 0, 0 ), false );
}

/*================================================================*/
void KPFooterHeaderEditor::updateSizes()
{
    updateGeometry();
}

/*================================================================*/
void KPFooterHeaderEditor::setupHeader()
{
    QVBox *back = new QVBox( tabwidget );

    QHBox *tool1 = new QHBox( back );
    tool1->setMargin( 5 );
    tool1->setSpacing( 5 );

    showHeader = new QCheckBox( i18n( "Show Header" ), tool1 );
    connect( showHeader, SIGNAL( clicked() ), this, SLOT( slotShowHeader() ) );

    penBrush1 = new QPushButton( i18n( "Configure Frame and Background..." ), tool1 );
    connect( penBrush1, SIGNAL( clicked() ), this, SLOT( slotHeaderPenBrush() ) );
    penBrush1->setEnabled( false );

    tool1->setMaximumHeight( penBrush1->sizeHint().height() + 10 );

    htool2 = new KToolBar( back );
    htool2->enableMoving( false );

    QStrList fontList;
    KPresenterView::getFonts( fontList );

    h_font = 99;
    htool2->insertCombo( &fontList, h_font, true,
                         SIGNAL( activated( const QString & ) ), this,
                         SLOT( headerFont( const QString & ) ), true, i18n( "Font List" ), 200 );

    h_size = 999;
    QStrList sizes;
    for ( unsigned int i = 4; i <= 100; i++ )
        sizes.append( QString( "%1" ).arg( i ) );

    htool2->insertCombo( &sizes, h_size, true,
                         SIGNAL( activated( const QString & ) ), this,
                         SLOT( headerSize( const QString & ) ), true, i18n( "Size List" ) );

    htool2->insertSeparator();

    h_bold = 1;
    htool2->insertButton( ICON( "bold.xpm" ), h_bold,
                          SIGNAL( clicked() ), this,
                          SLOT( headerBold() ), true, i18n( "Bold" ) );
    htool2->setToggle( h_bold, true );

    h_italic = 2;
    htool2->insertButton( ICON( "italic.xpm" ), h_italic,
                          SIGNAL( clicked() ), this,
                          SLOT( headerItalic() ), true, i18n( "Italic" ) );
    htool2->setToggle( h_italic, true );

    h_underline = 3;
    htool2->insertButton( ICON( "underl.xpm" ), h_underline,
                          SIGNAL( clicked() ), this,
                          SLOT( headerUnderline() ), true, i18n( "Underline" ) );
    htool2->setToggle( h_underline, true );

    htool2->insertSeparator();

    QPixmap pix( 18, 18 );
    pix.fill( doc->header()->getKTextObject()->color() );
    h_color = 4;
    htool2->insertButton( pix, h_color,
                          SIGNAL( clicked() ), this,
                          SLOT( headerColor() ), true, i18n( "Text Color" ) );

    htool2->insertSeparator();

    h_aleft = 5;
    htool2->insertButton( ICON( "alignLeft.xpm" ), h_aleft,
                          SIGNAL( clicked() ), this,
                          SLOT( headerAlignLeft() ), true, i18n( "Align Left" ) );
    htool2->setToggle( h_aleft, true );

    h_acenter = 6;
    htool2->insertButton( ICON( "alignCenter.xpm" ), h_acenter,
                          SIGNAL( clicked() ), this,
                          SLOT( headerAlignCenter() ), true, i18n( "Align Center" ) );
    htool2->setToggle( h_acenter, true );

    h_aright = 7;
    htool2->insertButton( ICON( "alignRight.xpm" ), h_aright,
                          SIGNAL( clicked() ), this,
                          SLOT( headerAlignRight() ), true, i18n( "Align Right" ) );
    htool2->setToggle( h_aright, true );

    htool2->setMaximumHeight(tool1->maximumHeight());

    connect( doc->header()->getKTextObject(), SIGNAL( fontChanged( QFont* ) ), this, SLOT( headerFontChanged( QFont* ) ) );
    connect( doc->header()->getKTextObject(), SIGNAL( colorChanged( QColor* ) ), this, SLOT( headerColorChanged( QColor* ) ) );
    connect( doc->header()->getKTextObject(), SIGNAL( horzAlignChanged( TxtParagraph::HorzAlign ) ),
             this, SLOT( headerAlignChanged( TxtParagraph::HorzAlign ) ) );

    // iiiiiiii: I hate such things..
    back->setMinimumWidth( 550 );

    QVBox *txtFrame = new QVBox( back );
    txtFrame->setMargin( 2 );
    txtFrame->setBackgroundColor( Qt::white );
    txtFrame->setFrameStyle( QFrame::WinPanel | QFrame::Sunken );

    doc->header()->getKTextObject()->reparent( txtFrame, 0, QPoint( 0, 0 ), true );
    doc->header()->getKTextObject()->setBackgroundColor( Qt::white );

    tabwidget->addTab( back, i18n( "&Header" ) );
}

/*================================================================*/
void KPFooterHeaderEditor::setupFooter()
{
    QVBox *back = new QVBox( tabwidget );

    QHBox *tool1 = new QHBox( back );
    tool1->setMargin( 5 );
    tool1->setSpacing( 5 );

    showFooter = new QCheckBox( i18n( "Show Footer" ), tool1 );
    connect( showFooter, SIGNAL( clicked() ), this, SLOT( slotShowFooter() ) );

    penBrush2 = new QPushButton( i18n( "Configure Frame and Background..." ), tool1 );
    connect( penBrush2, SIGNAL( clicked() ), this, SLOT( slotFooterPenBrush() ) );
    penBrush2->setEnabled( false );

    tool1->setMaximumHeight( penBrush2->sizeHint().height() + 10 );

    ftool2 = new KToolBar( back );
    ftool2->enableMoving( false );

    QStrList fontList;
    KPresenterView::getFonts( fontList );

    f_font = 99;
    ftool2->insertCombo( &fontList, f_font, true,
                         SIGNAL( activated( const QString & ) ), this,
                         SLOT( footerFont( const QString & ) ), true, i18n( "Font List" ), 200 );

    f_size = 999;
    QStrList sizes;
    for ( unsigned int i = 4; i <= 100; i++ )
        sizes.append( QString( "%1" ).arg( i ) );

    ftool2->insertCombo( &sizes, f_size, true,
                         SIGNAL( activated( const QString & ) ), this,
                         SLOT( footerSize( const QString & ) ), true, i18n( "Size List" ) );

    ftool2->insertSeparator();

    f_bold = 1;
    ftool2->insertButton( ICON( "bold.xpm" ), f_bold,
                          SIGNAL( clicked() ), this,
                          SLOT( footerBold() ), true, i18n( "Bold" ) );
    ftool2->setToggle( f_bold, true );

    f_italic = 2;
    ftool2->insertButton( ICON( "italic.xpm" ), f_italic,
                          SIGNAL( clicked() ), this,
                          SLOT( footerItalic() ), true, i18n( "Italic" ) );
    ftool2->setToggle( f_italic, true );

    f_underline = 3;
    ftool2->insertButton( ICON( "underl.xpm" ), f_underline,
                          SIGNAL( clicked() ), this,
                          SLOT( footerUnderline() ), true, i18n( "Underline" ) );
    ftool2->setToggle( f_underline, true );

    ftool2->insertSeparator();

    QPixmap pix( 18, 18 );
    pix.fill( doc->footer()->getKTextObject()->color() );
    f_color = 4;
    ftool2->insertButton( pix, f_color,
                          SIGNAL( clicked() ), this,
                          SLOT( footerColor() ), true, i18n( "Text Color" ) );

    ftool2->insertSeparator();

    f_aleft = 5;
    ftool2->insertButton( ICON( "alignLeft.xpm" ), f_aleft,
                          SIGNAL( clicked() ), this,
                          SLOT( footerAlignLeft() ), true, i18n( "Align Left" ) );
    ftool2->setToggle( f_aleft, true );

    f_acenter = 6;
    ftool2->insertButton( ICON( "alignCenter.xpm" ), f_acenter,
                          SIGNAL( clicked() ), this,
                          SLOT( footerAlignCenter() ), true, i18n( "Align Center" ) );
    ftool2->setToggle( f_acenter, true );

    f_aright = 7;
    ftool2->insertButton( ICON( "alignRight.xpm" ), f_aright,
                          SIGNAL( clicked() ), this,
                          SLOT( footerAlignRight() ), true, i18n( "Align Right" ) );
    ftool2->setToggle( f_aright, true );

    ftool2->setMaximumHeight(tool1->maximumHeight());

    connect( doc->footer()->getKTextObject(), SIGNAL( fontChanged( QFont* ) ), this, SLOT( footerFontChanged( QFont* ) ) );
    connect( doc->footer()->getKTextObject(), SIGNAL( colorChanged( QColor* ) ), this, SLOT( footerColorChanged( QColor* ) ) );
    connect( doc->footer()->getKTextObject(), SIGNAL( horzAlignChanged( TxtParagraph::HorzAlign ) ),
             this, SLOT( footerAlignChanged( TxtParagraph::HorzAlign ) ) );

    // iiiiiiii: I hate such things..
    back->setMinimumWidth( 550 );

    QVBox *txtFrame = new QVBox( back );
    txtFrame->setMargin( 2 );
    txtFrame->setBackgroundColor( Qt::white );
    txtFrame->setFrameStyle( QFrame::WinPanel | QFrame::Sunken );

    doc->footer()->getKTextObject()->reparent( txtFrame, 0, QPoint( 0, 0 ), true );
    doc->footer()->getKTextObject()->setBackgroundColor( Qt::white );

    tabwidget->addTab( back, i18n( "&Footer" ) );
}

/*================================================================*/
void KPFooterHeaderEditor::slotShowHeader()
{
    //showHeader->setChecked( !showHeader->isChecked() );
    doc->setHeader( showHeader->isChecked() );
    penBrush1->setEnabled( showHeader->isChecked() );
    slotUpdatePage();
}

/*================================================================*/
void KPFooterHeaderEditor::slotShowFooter()
{
    //showFooter->setChecked( !showFooter->isChecked() );
    doc->setFooter( showFooter->isChecked() );
    penBrush2->setEnabled( showFooter->isChecked() );
    slotUpdatePage();
}

/*================================================================*/
void KPFooterHeaderEditor::slotUpdatePage()
{
    doc->repaint( false );
}

/*================================================================*/
void KPFooterHeaderEditor::slotCloseDia()
{
    hide();
}

/*================================================================*/
void KPFooterHeaderEditor::slotHeaderPenBrush()
{
    KPTextObject *header = doc->header();

    StyleDia *styleDia = new StyleDia( 0, "StyleDia", SD_PEN | SD_BRUSH );
    styleDia->setMaximumSize( styleDia->width(), styleDia->height() );
    styleDia->setMinimumSize( styleDia->width(), styleDia->height() );
    styleDia->setPen( header->getPen() );
    styleDia->setBrush( header->getBrush() );
    styleDia->setLineBegin( L_NORMAL );
    styleDia->setLineEnd( L_NORMAL );
    styleDia->setFillType( header->getFillType() );
    styleDia->setGradient( header->getGColor1(),
                           header->getGColor2(),
                           header->getGType() );
    styleDia->setCaption( i18n( "Configure Header Frame and Background" ) );

    if ( styleDia->exec() == QDialog::Accepted )
    {
        header->setPen( styleDia->getPen() );
        header->setBrush( styleDia->getBrush() );
        header->setFillType( styleDia->getFillType() );
        header->setGColor1( styleDia->getGColor2() );
        header->setGColor2( styleDia->getGColor1() );
        header->setGType( styleDia->getGType() );
    }

    delete styleDia;
    slotUpdatePage();
}

/*================================================================*/
void KPFooterHeaderEditor::slotFooterPenBrush()
{
    KPTextObject *footer = doc->footer();

    StyleDia *styleDia = new StyleDia( 0, "StyleDia", SD_PEN | SD_BRUSH );
    styleDia->setMaximumSize( styleDia->width(), styleDia->height() );
    styleDia->setMinimumSize( styleDia->width(), styleDia->height() );
    styleDia->setPen( footer->getPen() );
    styleDia->setBrush( footer->getBrush() );
    styleDia->setLineBegin( L_NORMAL );
    styleDia->setLineEnd( L_NORMAL );
    styleDia->setFillType( footer->getFillType() );
    styleDia->setGradient( footer->getGColor1(),
                           footer->getGColor2(),
                           footer->getGType() );
    styleDia->setCaption( i18n( "Configure Footer Frame and Background" ) );

    if ( styleDia->exec() == QDialog::Accepted )
    {
        footer->setPen( styleDia->getPen() );
        footer->setBrush( styleDia->getBrush() );
        footer->setFillType( styleDia->getFillType() );
        footer->setGColor1( styleDia->getGColor2() );
        footer->setGColor2( styleDia->getGColor1() );
        footer->setGType( styleDia->getGType() );
    }

    delete styleDia;
    slotUpdatePage();
}

/*================================================================*/
void KPFooterHeaderEditor::headerFont( const QString &f )
{
    QFont fn = doc->header()->getKTextObject()->font();
    fn.setFamily( f );
    doc->header()->getKTextObject()->setFont( fn );
    doc->header()->getKTextObject()->setFocus();
}

/*================================================================*/
void KPFooterHeaderEditor::headerSize( const QString &s )
{
    QFont fn = doc->header()->getKTextObject()->font();
    fn.setPointSize( atoi( s ) );
    doc->header()->getKTextObject()->setFont( fn );
    doc->header()->getKTextObject()->setFocus();
}

/*================================================================*/
void KPFooterHeaderEditor::headerBold()
{
    QFont fn = doc->header()->getKTextObject()->font();
    fn.setBold( htool2->isButtonOn( h_bold ) );
    doc->header()->getKTextObject()->setFont( fn );
}

/*================================================================*/
void KPFooterHeaderEditor::headerItalic()
{
    QFont fn = doc->header()->getKTextObject()->font();
    fn.setItalic( htool2->isButtonOn( h_italic ) );
    doc->header()->getKTextObject()->setFont( fn );
}

/*================================================================*/
void KPFooterHeaderEditor::headerUnderline()
{
    QFont fn = doc->header()->getKTextObject()->font();
    fn.setUnderline( htool2->isButtonOn( h_underline ) );
    doc->header()->getKTextObject()->setFont( fn );
}

/*================================================================*/
void KPFooterHeaderEditor::headerColor()
{
    QColor col = doc->header()->getKTextObject()->color();
    if ( KColorDialog::getColor( col ) )
    {
        QPixmap pix( 18, 18 );
        pix.fill( col );
        doc->header()->getKTextObject()->setColor( col );
        htool2->setButtonPixmap( h_color, pix );
    }
}

/*================================================================*/
void KPFooterHeaderEditor::headerAlignLeft()
{
    doc->header()->getKTextObject()->setHorzAlign( TxtParagraph::LEFT );

    htool2->setButton( h_aleft, true );
    htool2->setButton( h_acenter, false );
    htool2->setButton( h_aright, false );
}

/*================================================================*/
void KPFooterHeaderEditor::headerAlignCenter()
{
    doc->header()->getKTextObject()->setHorzAlign( TxtParagraph::CENTER );

    htool2->setButton( h_aleft, false );
    htool2->setButton( h_acenter, true );
    htool2->setButton( h_aright, false );
}

/*================================================================*/
void KPFooterHeaderEditor::headerAlignRight()
{
    doc->header()->getKTextObject()->setHorzAlign( TxtParagraph::RIGHT );

    htool2->setButton( h_aleft, false );
    htool2->setButton( h_acenter, false );
    htool2->setButton( h_aright, true );
}

/*================================================================*/
void KPFooterHeaderEditor::footerFont( const QString &f )
{
    QFont fn = doc->footer()->getKTextObject()->font();
    fn.setFamily( f );
    doc->footer()->getKTextObject()->setFont( fn );
    doc->footer()->getKTextObject()->setFocus();
}

/*================================================================*/
void KPFooterHeaderEditor::footerSize( const QString &s )
{
    QFont fn = doc->footer()->getKTextObject()->font();
    fn.setPointSize( atoi( s ) );
    doc->footer()->getKTextObject()->setFont( fn );
    doc->footer()->getKTextObject()->setFocus();
}

/*================================================================*/
void KPFooterHeaderEditor::footerBold()
{
    QFont fn = doc->footer()->getKTextObject()->font();
    fn.setBold( ftool2->isButtonOn( f_bold ) );
    doc->footer()->getKTextObject()->setFont( fn );
}

/*================================================================*/
void KPFooterHeaderEditor::footerItalic()
{
    QFont fn = doc->footer()->getKTextObject()->font();
    fn.setItalic( ftool2->isButtonOn( f_italic ) );
    doc->footer()->getKTextObject()->setFont( fn );
}

/*================================================================*/
void KPFooterHeaderEditor::footerUnderline()
{
    QFont fn = doc->footer()->getKTextObject()->font();
    fn.setUnderline( ftool2->isButtonOn( f_underline ) );
    doc->footer()->getKTextObject()->setFont( fn );
}

/*================================================================*/
void KPFooterHeaderEditor::footerColor()
{
    QColor col = doc->footer()->getKTextObject()->color();
    if ( KColorDialog::getColor( col ) )
    {
        QPixmap pix( 18, 18 );
        pix.fill( col );
        doc->footer()->getKTextObject()->setColor( col );
        ftool2->setButtonPixmap( f_color, pix );
    }
}

/*================================================================*/
void KPFooterHeaderEditor::footerAlignLeft()
{
    doc->footer()->getKTextObject()->setHorzAlign( TxtParagraph::LEFT );

    ftool2->setButton( f_aleft, true );
    ftool2->setButton( f_acenter, false );
    ftool2->setButton( f_aright, false );
}

/*================================================================*/
void KPFooterHeaderEditor::footerAlignCenter()
{
    doc->footer()->getKTextObject()->setHorzAlign( TxtParagraph::CENTER );

    ftool2->setButton( f_aleft, false );
    ftool2->setButton( f_acenter, true );
    ftool2->setButton( f_aright, false );
}

/*================================================================*/
void KPFooterHeaderEditor::footerAlignRight()
{
    doc->footer()->getKTextObject()->setHorzAlign( TxtParagraph::RIGHT );

    ftool2->setButton( f_aleft, false );
    ftool2->setButton( f_acenter, false );
    ftool2->setButton( f_aright, true );
}

/*================================================================*/
void KPFooterHeaderEditor::headerFontChanged( QFont *f )
{
    QComboBox *combo = htool2->getCombo( h_font );

    for ( int i = 0; i < combo->count(); i++ )
    {
        if ( combo->text( i ) == f->family() )
        {
            combo->setCurrentItem( i );
            break;
        }
    }

    combo = htool2->getCombo( h_size );

    for ( int j = 0; j < combo->count(); j++ )
    {
        if ( atoi( combo->text( j ) ) == f->pointSize() )
        {
            combo->setCurrentItem( j );
            break;
        }
    }

    htool2->setButton( h_bold, f->bold() );
    htool2->setButton( h_italic, f->italic() );
    htool2->setButton( h_underline, f->underline() );
}

/*================================================================*/
void KPFooterHeaderEditor::headerColorChanged( QColor *c )
{
    QPixmap pix( 18, 18 );
    pix.fill( *c );
    htool2->setButtonPixmap( h_color, pix );
}

/*================================================================*/
void KPFooterHeaderEditor::headerAlignChanged( TxtParagraph::HorzAlign ha )
{
    htool2->setButton( h_aleft, ha == TxtParagraph::LEFT );
    htool2->setButton( h_acenter, ha == TxtParagraph::CENTER );
    htool2->setButton( h_aright, ha == TxtParagraph::RIGHT );
}

/*================================================================*/
void KPFooterHeaderEditor::footerFontChanged( QFont *f )
{
    QComboBox *combo = ftool2->getCombo( f_font );

    for ( int i = 0; i < combo->count(); i++ )
    {
        if ( combo->text( i ) == f->family() )
        {
            combo->setCurrentItem( i );
            break;
        }
    }

    combo = ftool2->getCombo( f_size );

    for ( int j = 0; j < combo->count(); j++ )
    {
        if ( atoi( combo->text( j ) ) == f->pointSize() )
        {
            combo->setCurrentItem( j );
            break;
        }
    }

    ftool2->setButton( f_bold, f->bold() );
    ftool2->setButton( f_italic, f->italic() );
    ftool2->setButton( f_underline, f->underline() );
}

/*================================================================*/
void KPFooterHeaderEditor::footerColorChanged( QColor *c )
{
    QPixmap pix( 18, 18 );
    pix.fill( *c );
    ftool2->setButtonPixmap( f_color, pix );
}

/*================================================================*/
void KPFooterHeaderEditor::footerAlignChanged( TxtParagraph::HorzAlign ha )
{
    ftool2->setButton( f_aleft, ha == TxtParagraph::LEFT );
    ftool2->setButton( f_acenter, ha == TxtParagraph::CENTER );
    ftool2->setButton( f_aright, ha == TxtParagraph::RIGHT );
}
