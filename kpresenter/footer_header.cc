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

#include <footer_header.h>
#include <kptextobject.h>
#include <styledia.h>
#include <kpresenter_doc.h>

#include <qtabwidget.h>
#include <qpoint.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qhbox.h>
#include <qpixmap.h>
#include <qstringlist.h>

#include <klocale.h>
#include <kbuttonbox.h>
#include <kcombobox.h>
#include <kcolordlg.h>
#include <kiconloader.h>
#include <kfontdialog.h>

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

    connect( tabwidget, SIGNAL( selected( const QString & ) ),
             this, SLOT( tabSelected( const QString & ) ) );
}

/*================================================================*/
void KPFooterHeaderEditor::tabSelected( const QString &s )
{
    if ( s == i18n( "&Header" ) )
        doc->header()->getKTextObject()->setFocus();
    else
        doc->footer()->getKTextObject()->setFocus();
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

    QStringList fontList;
    KFontChooser::getFontList( fontList, false );

    h_font = 99;
    QStringList lst;
    QValueList<QString>::Iterator it = fontList.begin();
    for ( ; it != fontList.end(); ++it )
        lst.append( *it );

    htool2->insertCombo( lst, h_font, true,
                         SIGNAL( activated( const QString & ) ), this,
                         SLOT( headerFont( const QString & ) ), true, i18n( "Font List" ), 200 );

    h_size = 999;
    QStringList sizes;
    for ( unsigned int i = 4; i <= 100; i++ )
        sizes.append( QString( "%1" ).arg( i ) );

    htool2->insertCombo( sizes, h_size, true,
                         SIGNAL( activated( const QString & ) ), this,
                         SLOT( headerSize( const QString & ) ), true, i18n( "Size List" ) );

    htool2->insertSeparator();

    h_bold = 1;
    htool2->insertButton( KPBarIcon( "text_bold" ), h_bold,
                          SIGNAL( clicked() ), this,
                          SLOT( headerBold() ), true, i18n( "Bold" ) );
    htool2->setToggle( h_bold, true );

    h_italic = 2;
    htool2->insertButton( KPBarIcon( "text_italic" ), h_italic,
                          SIGNAL( clicked() ), this,
                          SLOT( headerItalic() ), true, i18n( "Italic" ) );
    htool2->setToggle( h_italic, true );

    h_underline = 3;
    htool2->insertButton( KPBarIcon( "text_under" ), h_underline,
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
    htool2->insertButton( KPBarIcon( "text_left" ), h_aleft,
                          SIGNAL( clicked() ), this,
                          SLOT( headerAlignLeft() ), true, i18n( "Align Left" ) );
    htool2->setToggle( h_aleft, true );

    h_acenter = 6;
    htool2->insertButton( KPBarIcon( "text_center" ), h_acenter,
                          SIGNAL( clicked() ), this,
                          SLOT( headerAlignCenter() ), true, i18n( "Align Center" ) );
    htool2->setToggle( h_acenter, true );

    h_aright = 7;
    htool2->insertButton( KPBarIcon( "text_right" ), h_aright,
                          SIGNAL( clicked() ), this,
                          SLOT( headerAlignRight() ), true, i18n( "Align Right" ) );
    htool2->setToggle( h_aright, true );

    htool2->insertSeparator();
    headerAlignLeft();
    h_pgnum = 8;
#if 0 // note: Don't forget to add the functionality before enabling again :)
    htool2->insertButton( KPBarIcon( "pgnum" ), h_pgnum,
                          SIGNAL( clicked() ), this,
                          SLOT( headerInsertPageNum() ), true, i18n( "Insert Page Number" ) );
#endif

    htool2->setMaximumHeight(tool1->maximumHeight());
    htool2->setMinimumHeight(tool1->maximumHeight());

    connect( doc->header()->getKTextObject(), SIGNAL( currentFontChanged( const QFont & ) ),
             this, SLOT( headerFontChanged( const QFont & ) ) );
    connect( doc->header()->getKTextObject(), SIGNAL( currentColorChanged( const QColor & ) ),
             this, SLOT( headerColorChanged( const QColor & ) ) );
    connect( doc->header()->getKTextObject(), SIGNAL( currentAlignmentChanged( int ) ),
             this, SLOT( headerAlignChanged( int ) ) );

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

    QStringList fontList;
    KFontChooser::getFontList( fontList, false );

    QStringList lst;
    QValueList<QString>::Iterator it = fontList.begin();
    for ( ; it != fontList.end(); ++it )
        lst.append( *it );

    f_font = 99;
    ftool2->insertCombo( lst, f_font, true,
                         SIGNAL( activated( const QString & ) ), this,
                         SLOT( footerFont( const QString & ) ), true, i18n( "Font List" ), 200 );

    f_size = 999;
    QStringList sizes;
    for ( unsigned int i = 4; i <= 100; i++ )
        sizes.append( QString( "%1" ).arg( i ) );

    ftool2->insertCombo( sizes, f_size, true,
                         SIGNAL( activated( const QString & ) ), this,
                         SLOT( footerSize( const QString & ) ), true, i18n( "Size List" ) );

    ftool2->insertSeparator();

    f_bold = 1;
    ftool2->insertButton( BarIcon( "text_bold" ), f_bold,
                          SIGNAL( clicked() ), this,
                          SLOT( footerBold() ), true, i18n( "Bold" ) );
    ftool2->setToggle( f_bold, true );

    f_italic = 2;
    ftool2->insertButton( BarIcon( "text_italic" ), f_italic,
                          SIGNAL( clicked() ), this,
                          SLOT( footerItalic() ), true, i18n( "Italic" ) );
    ftool2->setToggle( f_italic, true );

    f_underline = 3;
    ftool2->insertButton( BarIcon( "text_under" ), f_underline,
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
    ftool2->insertButton( BarIcon( "text_left" ), f_aleft,
                          SIGNAL( clicked() ), this,
                          SLOT( footerAlignLeft() ), true, i18n( "Align Left" ) );
    ftool2->setToggle( f_aleft, true );

    f_acenter = 6;
    ftool2->insertButton( BarIcon( "text_center" ), f_acenter,
                          SIGNAL( clicked() ), this,
                          SLOT( footerAlignCenter() ), true, i18n( "Align Center" ) );
    ftool2->setToggle( f_acenter, true );

    f_aright = 7;
    ftool2->insertButton( BarIcon( "text_right" ), f_aright,
                          SIGNAL( clicked() ), this,
                          SLOT( footerAlignRight() ), true, i18n( "Align Right" ) );
    ftool2->setToggle( f_aright, true );
    footerAlignLeft();
    ftool2->insertSeparator();

    f_pgnum = 8;
#if 0 // note: Don't forget to add the functionality before enabling again :)
    ftool2->insertButton( KPBarIcon( "pgnum" ), f_pgnum,
                          SIGNAL( clicked() ), this,
                          SLOT( footerInsertPageNum() ), true, i18n( "Insert Page Number" ) );
#endif

    ftool2->setMaximumHeight(tool1->maximumHeight());
    ftool2->setMinimumHeight(tool1->maximumHeight());

    connect( doc->footer()->getKTextObject(), SIGNAL( currentFontChanged( const QFont & ) ),
             this, SLOT( footerFontChanged( const QFont & ) ) );
    connect( doc->footer()->getKTextObject(), SIGNAL( currentColorChanged( const QColor & ) ),
             this, SLOT( footerColorChanged( const QColor & ) ) );
    connect( doc->footer()->getKTextObject(), SIGNAL( currentAlignmentChanged( int ) ),
             this, SLOT( footerAlignChanged( int ) ) );

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

    StyleDia *styleDia = new StyleDia( 0, "StyleDia" );
    styleDia->setMaximumSize( styleDia->width(), styleDia->height() );
    styleDia->setMinimumSize( styleDia->width(), styleDia->height() );
    styleDia->setPen( header->getPen() );
    styleDia->setBrush( header->getBrush() );
    styleDia->setLineBegin( L_NORMAL );
    styleDia->setLineEnd( L_NORMAL );
    styleDia->setFillType( header->getFillType() );
    styleDia->setGradient( header->getGColor1(),
                           header->getGColor2(),
                           header->getGType(),
                           header->getGUnbalanced(),
                           header->getGXFactor(),
                           header->getGYFactor() );
    styleDia->setCaption( i18n( "Configure Header Frame and Background" ) );

    if ( styleDia->exec() == QDialog::Accepted ) {
        header->setPen( styleDia->getPen() );
        header->setBrush( styleDia->getBrush() );
        header->setFillType( styleDia->getFillType() );
        header->setGColor1( styleDia->getGColor1() );
        header->setGColor2( styleDia->getGColor2() );
        header->setGType( styleDia->getGType() );
        header->setGUnbalanced( styleDia->getGUnbalanced() );
        header->setGXFactor( styleDia->getGXFactor() );
        header->setGYFactor( styleDia->getGYFactor() );
    }

    delete styleDia;
    slotUpdatePage();
}

/*================================================================*/
void KPFooterHeaderEditor::slotFooterPenBrush()
{
    KPTextObject *footer = doc->footer();

    StyleDia *styleDia = new StyleDia( 0, "StyleDia" );
    styleDia->setMaximumSize( styleDia->width(), styleDia->height() );
    styleDia->setMinimumSize( styleDia->width(), styleDia->height() );
    styleDia->setPen( footer->getPen() );
    styleDia->setBrush( footer->getBrush() );
    styleDia->setLineBegin( L_NORMAL );
    styleDia->setLineEnd( L_NORMAL );
    styleDia->setFillType( footer->getFillType() );
    styleDia->setGradient( footer->getGColor1(),
                           footer->getGColor2(),
                           footer->getGType(),
                           footer->getGUnbalanced(),
                           footer->getGXFactor(),
                           footer->getGYFactor() );
    styleDia->setCaption( i18n( "Configure Footer Frame and Background" ) );

    if ( styleDia->exec() == QDialog::Accepted ) {
        footer->setPen( styleDia->getPen() );
        footer->setBrush( styleDia->getBrush() );
        footer->setFillType( styleDia->getFillType() );
        footer->setGColor1( styleDia->getGColor1() );
        footer->setGColor2( styleDia->getGColor2() );
        footer->setGType( styleDia->getGType() );
        footer->setGUnbalanced( styleDia->getGUnbalanced() );
        footer->setGXFactor( styleDia->getGXFactor() );
        footer->setGYFactor( styleDia->getGYFactor() );
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
    fn.setPointSize( s.toInt() );
    doc->header()->getKTextObject()->setFont( fn );
    doc->header()->getKTextObject()->setFocus();
}

/*================================================================*/
void KPFooterHeaderEditor::headerBold()
{
    doc->header()->getKTextObject()->setBold( htool2->isButtonOn( h_bold ) );
}

/*================================================================*/
void KPFooterHeaderEditor::headerItalic()
{
    doc->header()->getKTextObject()->setItalic( htool2->isButtonOn( h_italic ) );
}

/*================================================================*/
void KPFooterHeaderEditor::headerUnderline()
{
    doc->header()->getKTextObject()->setUnderline( htool2->isButtonOn( h_underline ) );
}

/*================================================================*/
void KPFooterHeaderEditor::headerColor()
{
    QColor col = doc->header()->getKTextObject()->color();
    if ( KColorDialog::getColor( col ) ) {
        QPixmap pix( 18, 18 );
        pix.fill( col );
        doc->header()->getKTextObject()->setColor( col );
        htool2->setButtonPixmap( h_color, pix );
    }
}

/*================================================================*/
void KPFooterHeaderEditor::headerAlignLeft()
{
    doc->header()->getKTextObject()->setAlignment( Qt::AlignLeft );

    htool2->setButton( h_aleft, true );
    htool2->setButton( h_acenter, false );
    htool2->setButton( h_aright, false );
}

/*================================================================*/
void KPFooterHeaderEditor::headerAlignCenter()
{
    doc->header()->getKTextObject()->setAlignment( Qt::AlignCenter );

    htool2->setButton( h_aleft, false );
    htool2->setButton( h_acenter, true );
    htool2->setButton( h_aright, false );
}

/*================================================================*/
void KPFooterHeaderEditor::headerAlignRight()
{
    doc->header()->getKTextObject()->setAlignment( Qt::AlignRight );

    htool2->setButton( h_aleft, false );
    htool2->setButton( h_acenter, false );
    htool2->setButton( h_aright, true );
}

/*================================================================*/
void KPFooterHeaderEditor::headerInsertPageNum()
{
#if 0
    doc->header()->getKTextObject()->insertPageNum();
#endif
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
    fn.setPointSize( s.toInt() );
    doc->footer()->getKTextObject()->setFont( fn );
   doc->footer()->getKTextObject()->setFocus();
}

/*================================================================*/
void KPFooterHeaderEditor::footerBold()
{
    doc->footer()->getKTextObject()->setBold( ftool2->isButtonOn( f_bold ) );
}

/*================================================================*/
void KPFooterHeaderEditor::footerItalic()
{
    doc->footer()->getKTextObject()->setItalic( ftool2->isButtonOn( f_italic ) );
}

/*================================================================*/
void KPFooterHeaderEditor::footerUnderline()
{
    doc->footer()->getKTextObject()->setUnderline( ftool2->isButtonOn( f_underline ) );
}

/*================================================================*/
void KPFooterHeaderEditor::footerColor()
{
    QColor col = doc->footer()->getKTextObject()->color();
    if ( KColorDialog::getColor( col ) ) {
        QPixmap pix( 18, 18 );
        pix.fill( col );
        doc->footer()->getKTextObject()->setColor( col );
        ftool2->setButtonPixmap( f_color, pix );
    }
}

/*================================================================*/
void KPFooterHeaderEditor::footerAlignLeft()
{
    doc->footer()->getKTextObject()->setAlignment( Qt::AlignLeft );

    ftool2->setButton( f_aleft, true );
    ftool2->setButton( f_acenter, false );
    ftool2->setButton( f_aright, false );
}

/*================================================================*/
void KPFooterHeaderEditor::footerAlignCenter()
{
    doc->footer()->getKTextObject()->setAlignment( Qt::AlignCenter );

    ftool2->setButton( f_aleft, false );
    ftool2->setButton( f_acenter, true );
    ftool2->setButton( f_aright, false );
}

/*================================================================*/
void KPFooterHeaderEditor::footerAlignRight()
{
    doc->footer()->getKTextObject()->setAlignment( Qt::AlignRight );

    ftool2->setButton( f_aleft, false );
    ftool2->setButton( f_acenter, false );
    ftool2->setButton( f_aright, true );
}

/*================================================================*/
void KPFooterHeaderEditor::headerFontChanged( const QFont &f )
{
    QComboBox *combo = htool2->getCombo( h_font );

    for ( int i = 0; i < combo->count(); i++ ) {
        if ( combo->text( i ).lower() == f.family().lower() ) {
            combo->setCurrentItem( i );
            break;
        }
    }

    combo = htool2->getCombo( h_size );

    for ( int j = 0; j < combo->count(); j++ ) {
        if ( combo->text( j ).toInt() == f.pointSize() ) {
            combo->setCurrentItem( j );
            break;
        }
    }

    htool2->setButton( h_bold, f.bold() );
    htool2->setButton( h_italic, f.italic() );
    htool2->setButton( h_underline, f.underline() );
}

/*================================================================*/
void KPFooterHeaderEditor::headerColorChanged( const QColor &c )
{
    QPixmap pix( 18, 18 );
    pix.fill( c );
    htool2->setButtonPixmap( h_color, pix );
}

/*================================================================*/
void KPFooterHeaderEditor::headerAlignChanged( int ha )
{
    htool2->setButton( h_aleft, ( ha & Qt::AlignLeft ) );
    htool2->setButton( h_acenter, ( ha & Qt::AlignCenter ) );
    htool2->setButton( h_aright, ( ha & Qt::AlignRight ) );
}

/*================================================================*/
void KPFooterHeaderEditor::footerFontChanged( const QFont &f )
{
    QComboBox *combo = ftool2->getCombo( f_font );

    for ( int i = 0; i < combo->count(); i++ ) {
        if ( combo->text( i ).lower() == f.family().lower() ) {
            combo->setCurrentItem( i );
            break;
        }
    }

    combo = ftool2->getCombo( f_size );

    for ( int j = 0; j < combo->count(); j++ ) {
        if ( combo->text( j ).toInt() == f.pointSize() ) {
            combo->setCurrentItem( j );
            break;
        }
    }

    ftool2->setButton( f_bold, f.bold() );
    ftool2->setButton( f_italic, f.italic() );
    ftool2->setButton( f_underline, f.underline() );
}

/*================================================================*/
void KPFooterHeaderEditor::footerColorChanged( const QColor &c )
{
    QPixmap pix( 18, 18 );
    pix.fill( c );
    ftool2->setButtonPixmap( f_color, pix );
}

/*================================================================*/
void KPFooterHeaderEditor::footerAlignChanged( int ha )
{
    ftool2->setButton( f_aleft, ( ha & Qt::AlignLeft ) );
    ftool2->setButton( f_acenter, ( ha & Qt::AlignCenter ) );
    ftool2->setButton( f_aright, ( ha & Qt::AlignRight ) );
}

/*================================================================*/
void KPFooterHeaderEditor::setShowHeader( bool b )
{
    showHeader->setChecked( b );
}

/*================================================================*/
void KPFooterHeaderEditor::setShowFooter( bool b )
{
    showFooter->setChecked( b );
}

/*================================================================*/
void KPFooterHeaderEditor::footerInsertPageNum()
{
#if 0
    doc->footer()->getKTextObject()->insertPageNum();
#endif
}

#include <footer_header.moc>
