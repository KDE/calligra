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
#include <kcolordialog.h>
#include <kiconloader.h>
#include <kfontdialog.h>

#include <qrichtext_p.h>
#include <kotextobject.h>
#include <kdebug.h>
#include <stdlib.h>

/******************************************************************/
/* Class: KPFooterHeaderEditor                                    */
/******************************************************************/

/*================================================================*/
KPFooterHeaderEditor::KPFooterHeaderEditor( KPresenterDoc *_doc )
    : QVBox( 0L ), _allowClose( false )
{
    setMargin( 10 );
    kdDebug()<<"KPFooterHeaderEditor::KPFooterHeaderEditor \n";
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
#if 0
    if ( s == i18n( "&Header" ) )
        doc->header()->textObjectView()->showCursor();
    else
        doc->footer()->textObjectView()->showCursor();
#endif
}

/*================================================================*/
KPFooterHeaderEditor::~KPFooterHeaderEditor()
{
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
    KFontChooser::getFontList( fontList, 0 );

    h_font = 99;
    QStringList lst;
    QValueList<QString>::ConstIterator it = fontList.begin();
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
    //pix.fill( doc->header()->textColor() );
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

#if 0

    connect( doc->header()->textObject(), SIGNAL( currentFontChanged( const QFont & ) ),
             this, SLOT( headerFontChanged( const QFont & ) ) );
    connect( doc->header()->textObject(), SIGNAL( currentColorChanged( const QColor & ) ),
             this, SLOT( headerColorChanged( const QColor & ) ) );
    connect( doc->header()->textObject(), SIGNAL( currentAlignmentChanged( int ) ),
             this, SLOT( headerAlignChanged( int ) ) );
#endif
    // iiiiiiii: I hate such things..
    back->setMinimumWidth( 550 );

    QVBox *txtFrame = new QVBox( back );
    txtFrame->setMargin( 2 );
    txtFrame->setBackgroundColor( Qt::white );
    txtFrame->setFrameStyle( QFrame::WinPanel | QFrame::Sunken );

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
    KFontChooser::getFontList( fontList, 0 );

    QStringList lst;
    QValueList<QString>::ConstIterator it = fontList.begin();
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
    //pix.fill( doc->footer()->textColor() );
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
#if 0

    connect( doc->footer()->textObject(), SIGNAL( currentFontChanged( const QFont & ) ),
             this, SLOT( footerFontChanged( const QFont & ) ) );
    connect( doc->footer()->textObject(), SIGNAL( currentColorChanged( const QColor & ) ),
             this, SLOT( footerColorChanged( const QColor & ) ) );
    connect( doc->footer()->textObject(), SIGNAL( currentAlignmentChanged( int ) ),
             this, SLOT( footerAlignChanged( int ) ) );
#endif
    // iiiiiiii: I hate such things..
    back->setMinimumWidth( 550 );

    QVBox *txtFrame = new QVBox( back );
    txtFrame->setMargin( 2 );
    txtFrame->setBackgroundColor( Qt::white );
    txtFrame->setFrameStyle( QFrame::WinPanel | QFrame::Sunken );

    tabwidget->addTab( back, i18n( "&Footer" ) );
}

/*================================================================*/
void KPFooterHeaderEditor::slotShowHeader()
{
    //showHeader->setChecked( !showHeader->isChecked() );
    doc->setHeader( showHeader->isChecked() );
    penBrush1->setEnabled( showHeader->isChecked() );
    doc->setModified(true);
    slotUpdatePage();
}

/*================================================================*/
void KPFooterHeaderEditor::slotShowFooter()
{
    //showFooter->setChecked( !showFooter->isChecked() );
    doc->setFooter( showFooter->isChecked() );
    penBrush2->setEnabled( showFooter->isChecked() );
    doc->setModified(true);
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
    int flags = 0;
    flags = flags | StyleDia::SdPen;
    flags = flags | StyleDia::SdBrush | StyleDia::SdGradient;

    StyleDia *styleDia = new StyleDia( 0, "StyleDia",flags );
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
    int flags = 0;
    flags = flags | StyleDia::SdPen;
    flags = flags | StyleDia::SdBrush | StyleDia::SdGradient;
    StyleDia *styleDia = new StyleDia( 0, "StyleDia",flags );
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
#if 0
    if(doc->header())
        doc->header()->setFamily(f);
#endif
}

/*================================================================*/
void KPFooterHeaderEditor::headerSize( const QString &s )
{
#if 0
    if(doc->header())
        doc->header()->setPointSize(s.toInt());
#endif
}

/*================================================================*/
void KPFooterHeaderEditor::headerBold()
{
#if 0
    if(doc->header())
        doc->header()->setBold( htool2->isButtonOn( h_bold ) );
#endif
}

/*================================================================*/
void KPFooterHeaderEditor::headerItalic()
{
#if 0
    if(doc->header())
        doc->header()->setItalic( htool2->isButtonOn( h_italic ) );
#endif
}

/*================================================================*/
void KPFooterHeaderEditor::headerUnderline()
{
#if 0
    if(doc->header())
        doc->header()->setUnderline( htool2->isButtonOn( h_underline ) );
#endif
}

/*================================================================*/
void KPFooterHeaderEditor::headerColor()
{
#if 0
    if(doc->header())
    {
        QColor col = doc->header()->textColor();
        if ( KColorDialog::getColor( col ) ) {
            QPixmap pix( 18, 18 );
            pix.fill( col );
            doc->header()->setTextColor( col );
            htool2->setButtonPixmap( h_color, pix );
        }
    }
#endif
}

/*================================================================*/
void KPFooterHeaderEditor::headerAlignLeft()
{
#if 0
    if(doc->header())
    {
        KCommand *cmd=doc->header()->setAlignCommand(Qt::AlignLeft);
        if(cmd)
            doc->addCommand(cmd);

        htool2->setButton( h_aleft, true );
        htool2->setButton( h_acenter, false );
        htool2->setButton( h_aright, false );
    }
#endif
}

/*================================================================*/
void KPFooterHeaderEditor::headerAlignCenter()
{
#if 0
    if(doc->header())
    {
        KCommand* cmd=doc->header()->setAlignCommand(Qt::AlignCenter);
        if(cmd)
            doc->addCommand(cmd);

        htool2->setButton( h_aleft, false );
        htool2->setButton( h_acenter, true );
        htool2->setButton( h_aright, false );
    }
#endif
}

/*================================================================*/
void KPFooterHeaderEditor::headerAlignRight()
{
#if 0
    if(doc->header())
    {
        KCommand *cmd=doc->header()->setAlignCommand(Qt::AlignRight);
        if(cmd)
            doc->addCommand(cmd);

        htool2->setButton( h_aleft, false );
        htool2->setButton( h_acenter, false );
        htool2->setButton( h_aright, true );
    }
#endif
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
#if 0
    if(doc->footer())
        doc->footer()->setFamily(f);
#endif
}

/*================================================================*/
void KPFooterHeaderEditor::footerSize( const QString &s )
{
#if 0
    if(doc->footer())
        doc->footer()->setPointSize(s.toInt() );
#endif
}

/*================================================================*/
void KPFooterHeaderEditor::footerBold()
{
#if 0
    if(doc->footer())
        doc->footer()->setBold( ftool2->isButtonOn( f_bold ) );
#endif
}

/*================================================================*/
void KPFooterHeaderEditor::footerItalic()
{
#if 0
    if(doc->footer())
        doc->footer()->setItalic( ftool2->isButtonOn( f_italic ) );
#endif
}

/*================================================================*/
void KPFooterHeaderEditor::footerUnderline()
{
#if 0
    if(doc->footer())
        doc->footer()->setUnderline( ftool2->isButtonOn( f_underline ) );
#endif
}

/*================================================================*/
void KPFooterHeaderEditor::footerColor()
{
#if 0
    if(doc->footer())
    {
        QColor col = doc->footer()->textColor();
        if ( KColorDialog::getColor( col ) ) {
            QPixmap pix( 18, 18 );
            pix.fill( col );
            doc->footer()->setTextColor( col );
            ftool2->setButtonPixmap( f_color, pix );
        }
    }
#endif
}

/*================================================================*/
void KPFooterHeaderEditor::footerAlignLeft()
{
#if 0
    if(doc->footer())
    {
        KCommand *cmd=doc->footer()->setAlignCommand(Qt::AlignLeft);
        if(cmd)
            doc->addCommand(cmd);

        ftool2->setButton( f_aleft, true );
        ftool2->setButton( f_acenter, false );
        ftool2->setButton( f_aright, false );
    }
#endif
}

/*================================================================*/
void KPFooterHeaderEditor::footerAlignCenter()
{
#if 0
    if(doc->footer())
    {
        KCommand *cmd=doc->header()->setAlignCommand(Qt::AlignCenter);
        if(cmd)
            doc->addCommand(cmd);

        ftool2->setButton( f_aleft, false );
        ftool2->setButton( f_acenter, true );
        ftool2->setButton( f_aright, false );
    }
#endif
}

/*================================================================*/
void KPFooterHeaderEditor::footerAlignRight()
{
#if 0
    if(doc->footer())
    {
        KCommand *cmd=doc->header()->setAlignCommand(Qt::AlignRight);
        if(cmd)
            doc->addCommand(cmd);

        ftool2->setButton( f_aleft, false );
        ftool2->setButton( f_acenter, false );
        ftool2->setButton( f_aright, true );
    }
#endif
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
    doc->footer()->insertPageNum();
#endif
}

#include <footer_header.moc>
