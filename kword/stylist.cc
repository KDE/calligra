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

#include "kwdoc.h"
#include "kwstyle.h"
#include "stylist.h"
#include "stylist.moc"
#include "defs.h"
#include "kwfont.h"

#include <qwidget.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qfont.h>
#include <qcolor.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qpen.h>
#include <qbrush.h>
#include <qstrlist.h>
#include <qcombobox.h>
#include <qcheckbox.h>

#include <kbuttonbox.h>
#include <kapp.h>
#include <kcolordlg.h>
#include <klocale.h>

/******************************************************************/
/* Class: KWStyleManager                                          */
/******************************************************************/

/*================================================================*/
KWStyleManager::KWStyleManager( QWidget *_parent, KWDocument *_doc, QStringList _fontList )
    : KDialogBase(Tabbed, QString::null, Ok | Cancel, Ok, _parent, "", true )
{
    fontList = _fontList;
    doc = _doc;
    editor = 0L;

    setupTab1();
    /// setupTab2(); // The functionality isn't implemented at the moment

    setInitialSize( QSize(500, 400) );
}

/*================================================================*/
void KWStyleManager::setupTab1()
{
    tab1 = addPage( i18n( "Style Management" ) );

    grid1 = new QGridLayout( tab1, 1, 2, 15, 7 );
    QList<KWStyle> styles = const_cast<QList<KWStyle> & >(doc->styleList());
    lStyleList = new QListBox( tab1 );
    for ( unsigned int i = 0; i < styles.count(); i++ )
        lStyleList->insertItem( styles.at( i )->name() );
    connect( lStyleList, SIGNAL( selected( int ) ), this, SLOT( editStyle( int ) ) );
    grid1->addWidget( lStyleList, 0, 0 );

    bButtonBox = new KButtonBox( tab1, Vertical );
    bAdd = bButtonBox->addButton( i18n( "&Add..." ), false );
    connect( bAdd, SIGNAL( clicked() ), this, SLOT( addStyle() ) );
    bDelete = bButtonBox->addButton( i18n( "&Delete" ), false );
    connect( bDelete, SIGNAL( clicked() ), this, SLOT( deleteStyle() ) );
    bButtonBox->addStretch();
    bEdit = bButtonBox->addButton( i18n( "&Edit..." ), false );
    connect( bEdit, SIGNAL( clicked() ), this, SLOT( editStyle() ) );
    bCopy = bButtonBox->addButton( i18n( "Co&py..." ), false );
    connect( bCopy, SIGNAL( clicked() ), this, SLOT( copyStyle() ) );
    bButtonBox->addStretch();
    bUp = bButtonBox->addButton( i18n( "&Up" ), false );
    connect( bUp, SIGNAL( clicked() ), this, SLOT( upStyle() ) );
    bDown = bButtonBox->addButton( i18n( "Do&wn" ), false );
    connect( bDown, SIGNAL( clicked() ), this, SLOT( downStyle() ) );
    bButtonBox->layout();
    grid1->addWidget( bButtonBox, 0, 1 );

    grid1->addColSpacing( 0, lStyleList->width() );
    grid1->addColSpacing( 1, bButtonBox->width() );
    grid1->setColStretch( 0, 1 );

    grid1->addRowSpacing( 0, lStyleList->height() );
    grid1->addRowSpacing( 0, bButtonBox->height() );
    grid1->setRowStretch( 0, 1 );

    connect( lStyleList, SIGNAL( highlighted( const QString & ) ), this, SLOT( updateButtons( const QString & ) ) );
    lStyleList->setCurrentItem( 0 );
}

/*================================================================*/
void KWStyleManager::setupTab2()
{
    tab2 = addPage( i18n( "Update Configuration" ) );
    grid2 = new QGridLayout( tab2, 9, 1, 15, 7 );

    cSmart = new QCheckBox( i18n( "&Smart updating of fonts and colors" ), tab2 );
    grid2->addWidget( cSmart, 0, 0 );

    cFont = new QComboBox( false, tab2 );
    cFont->insertItem( i18n( "Don't update Fonts" ) );
    cFont->insertItem( i18n( "Update Font Families of same sized Fonts" ) );
    cFont->insertItem( i18n( "Update Font Families of all sized Fonts" ) );
    cFont->insertItem( i18n( "Update Font Families and Attributes of same sized Fonts" ) );
    cFont->insertItem( i18n( "Update Font Families and Attributes of all sized Fonts" ) );
    cFont->resize( cFont->sizeHint() );
    grid2->addWidget( cFont, 1, 0 );

    cColor = new QComboBox( false, tab2 );
    cColor->insertItem( i18n( "Don't update Colors" ) );
    cColor->insertItem( i18n( "Update Colors" ) );
    cColor->resize( cFont->sizeHint() );
    grid2->addWidget( cColor, 2, 0 );

    cIndent = new QComboBox( false, tab2 );
    cIndent->insertItem( i18n( "Don't update Indents and Spacing" ) );
    cIndent->insertItem( i18n( "Update Indents and Spacing" ) );
    cIndent->resize( cFont->sizeHint() );
    grid2->addWidget( cIndent, 3, 0 );

    cAlign = new QComboBox( false, tab2 );
    cAlign->insertItem( i18n( "Don't update Alignments" ) );
    cAlign->insertItem( i18n( "Update Alignments" ) );
    cAlign->resize( cFont->sizeHint() );
    grid2->addWidget( cAlign, 4, 0 );

    cNumbering = new QComboBox( false, tab2 );
    cNumbering->insertItem( i18n( "Don't update Numbering" ) );
    cNumbering->insertItem( i18n( "Update Numbering" ) );
    cNumbering->resize( cFont->sizeHint() );
    grid2->addWidget( cNumbering, 5, 0 );

    cBorder = new QComboBox( false, tab2 );
    cBorder->insertItem( i18n( "Don't update Borders" ) );
    cBorder->insertItem( i18n( "Update Borders" ) );
    cBorder->resize( cFont->sizeHint() );
    grid2->addWidget( cBorder, 6, 0 );

    cTabs = new QComboBox( false, tab2 );
    cTabs->insertItem( i18n( "Don't update Tabulators" ) );
    cTabs->insertItem( i18n( "Update Tabulators" ) );
    cTabs->resize( cFont->sizeHint() );
    grid2->addWidget( cTabs, 7, 0 );

    grid2->addColSpacing( 0, cSmart->width() );
    grid2->addColSpacing( 0, cFont->width() );
    grid2->addColSpacing( 0, cAlign->width() );
    grid2->addColSpacing( 0, cColor->width() );
    grid2->addColSpacing( 0, cNumbering->width() );
    grid2->addColSpacing( 0, cIndent->width() );
    grid2->addColSpacing( 0, cBorder->width() );
    grid2->addColSpacing( 0, cTabs->width() );
    grid2->setColStretch( 0, 1 );

    grid2->addRowSpacing( 0, cSmart->height() );
    grid2->addRowSpacing( 1, cFont->height() );
    grid2->addRowSpacing( 2, cColor->height() );
    grid2->addRowSpacing( 3, cIndent->height() );
    grid2->addRowSpacing( 4, cAlign->height() );
    grid2->addRowSpacing( 5, cNumbering->height() );
    grid2->addRowSpacing( 6, cBorder->height() );
    grid2->addRowSpacing( 7, cTabs->height() );
    grid2->setRowStretch( 0, 0 );
    grid2->setRowStretch( 1, 0 );
    grid2->setRowStretch( 2, 0 );
    grid2->setRowStretch( 3, 0 );
    grid2->setRowStretch( 4, 0 );
    grid2->setRowStretch( 5, 0 );
    grid2->setRowStretch( 6, 0 );
    grid2->setRowStretch( 7, 0 );
    grid2->setRowStretch( 8, 1 );

    cSmart->setChecked( FALSE );
    if ( doc->applyStyleChangeMask() & KWDocument::U_FONT_FAMILY_SAME_SIZE )
        cFont->setCurrentItem( 1 );
    if ( doc->applyStyleChangeMask() & KWDocument::U_FONT_FAMILY_ALL_SIZE )
        cFont->setCurrentItem( 2 );
    if ( doc->applyStyleChangeMask() & KWDocument::U_FONT_ALL_SAME_SIZE )
        cFont->setCurrentItem( 3 );
    if ( doc->applyStyleChangeMask() & KWDocument::U_FONT_ALL_ALL_SIZE )
        cFont->setCurrentItem( 4 );
    if ( doc->applyStyleChangeMask() & KWDocument::U_COLOR )
        cColor->setCurrentItem( 1 );
    if ( doc->applyStyleChangeMask() & KWDocument::U_INDENT )
        cIndent->setCurrentItem( 1 );
    if ( doc->applyStyleChangeMask() & KWDocument::U_BORDER )
        cBorder->setCurrentItem( 1 );
    if ( doc->applyStyleChangeMask() & KWDocument::U_ALIGN )
        cAlign->setCurrentItem( 1 );
    if ( doc->applyStyleChangeMask() & KWDocument::U_NUMBERING )
        cNumbering->setCurrentItem( 1 );
    if ( doc->applyStyleChangeMask() & KWDocument::U_TABS )
        cTabs->setCurrentItem( 1 );
    if ( doc->applyStyleChangeMask() & KWDocument::U_SMART ) {
        cSmart->setChecked( TRUE );
        cFont->setEnabled( FALSE );
        cColor->setEnabled( FALSE );
    }

    connect( cSmart, SIGNAL( clicked() ),
             this, SLOT( smartClicked() ) );
}

/*================================================================*/
void KWStyleManager::smartClicked()
{
    cFont->setEnabled( !cSmart->isChecked() );
    cColor->setEnabled( !cSmart->isChecked() );
}

/*================================================================*/
void KWStyleManager::editStyle()
{
    if ( editor ) {
        disconnect( editor, SIGNAL( updateStyleList() ), this, SLOT( updateStyleList() ) );
        delete editor;
        editor = 0L;
    }
    QList<KWStyle> styles = const_cast<QList<KWStyle> & >(doc->styleList());
    KWStyle *tmpStyle=styles.at( lStyleList->currentItem() );
    editor = new KWStyleEditor( this, tmpStyle, doc, fontList );
    connect( editor, SIGNAL( updateStyleList() ), this, SLOT( updateStyleList() ) );
    editor->setCaption( i18n( "Style Editor" ) );
    editor->show();
}

/*================================================================*/
void KWStyleManager::copyStyle()
{
  QList<KWStyle> styles = const_cast<QList<KWStyle> & >(doc->styleList());
  QString str=i18n("Copy-")+lStyleList->currentText();
  for ( unsigned int i = 0; i < styles.count(); i++ )
    {
      if(styles.at( i )->name()==str)
	{
	  str=str+i18n( "%1" ).arg(styles.count());
	}
    }

  KWStyle *newStyle=new KWStyle(str);
  KWStyle *oldStyle=styles.at( lStyleList->currentItem() );
  newStyle->format()=oldStyle->format();
  newStyle->paragLayout()=oldStyle->paragLayout();
  //rename style name because I copy all paragLayout
  //so I copy also name so I must rename new style
  newStyle->paragLayout().setStyleName( str );
  doc->addStyleTemplate(newStyle);
  lStyleList->insertItem( str );
  lStyleList->setCurrentItem( lStyleList->count() - 1 );
  editStyle();
  doc->applyStyleChange(str);
  doc->updateAllStyleLists();
}

/*================================================================*/
void KWStyleManager::upStyle()
{
  int pos=lStyleList->currentItem()-1;
  doc->moveUpStyleTemplate (lStyleList->currentText() );
  updateStyleList();
  lStyleList->setCurrentItem(pos);
}

/*================================================================*/
void KWStyleManager::downStyle()
{
  int pos=lStyleList->currentItem()+1;
  doc->moveDownStyleTemplate (lStyleList->currentText() );
  updateStyleList();
  lStyleList->setCurrentItem(pos);
}

/*================================================================*/
void KWStyleManager::addStyle()
{
  QList<KWStyle> styles = const_cast<QList<KWStyle> & >(doc->styleList());
  QString str=i18n( "New Style Template ( %1 )" ).arg(styles.count());
  KWStyle *newStyle=new KWStyle(str);
  doc->addStyleTemplate(newStyle);
  lStyleList->insertItem( str );
  lStyleList->setCurrentItem( lStyleList->count() - 1 );
  editStyle();
  doc->updateAllStyleLists();
  doc->applyStyleChange(str);
}

/*================================================================*/
void KWStyleManager::deleteStyle()
{
  QList<KWStyle> styles = const_cast<QList<KWStyle> & >(doc->styleList());
  doc->removeStyleTemplate(lStyleList->currentText());
  styles.remove( lStyleList->currentItem() );
  doc->applyStyleChange(lStyleList->currentText());
  updateStyleList();
}

/*================================================================*/
bool KWStyleManager::apply()
{
#if 0
    int f = 0;
    // Not implemented for the moment
    if ( cFont->currentItem() == 1 )
        f = f | KWDocument::U_FONT_FAMILY_SAME_SIZE;
    else if ( cFont->currentItem() == 2 )
        f = f | KWDocument::U_FONT_FAMILY_ALL_SIZE;
    else if ( cFont->currentItem() == 3 )
        f = f | KWDocument::U_FONT_ALL_SAME_SIZE;
    else if ( cFont->currentItem() == 4 )
        f = f | KWDocument::U_FONT_ALL_ALL_SIZE;

    if ( cColor->currentItem() == 1 )
        f = f | KWDocument::U_COLOR;
    if ( cAlign->currentItem() == 1 )
        f = f | KWDocument::U_ALIGN;
    if ( cBorder->currentItem() == 1 )
        f = f | KWDocument::U_BORDER;
    if ( cNumbering->currentItem() == 1 )
        f = f | KWDocument::U_NUMBERING;
    if ( cIndent->currentItem() == 1 )
        f = f | KWDocument::U_INDENT;
    if ( cTabs->currentItem() == 1 )
        f = f | KWDocument::U_TABS;

    if ( cSmart->isChecked() )
        f = f | KWDocument::U_SMART;

    doc->setApplyStyleChangeMask( f );
#endif
    return true;
}

void KWStyleManager::slotOk()
{
   if (apply())
   {
      KDialogBase::slotOk();
   }
}


/*================================================================*/
void KWStyleManager::updateStyleList()
{
  QList<KWStyle> styles = const_cast<QList<KWStyle> & >(doc->styleList());
    lStyleList->clear();
    for ( unsigned int i = 0; i < styles.count(); i++ )
        lStyleList->insertItem( styles.at( i )->name() );
    doc->updateAllStyleLists();
    lStyleList->setCurrentItem( 0 );
}

/*================================================================*/
void KWStyleManager::updateButtons( const QString & /* s */ )
{
/* // Give user freedom :)
    if ( s == QString( "Standard" ) ||
         s == QString( "Head 1" ) ||
         s == QString( "Head 2" ) ||
         s == QString( "Head 3" ) ||
         s == QString( "Enumerated List" ) ||
         s == QString( "Bullet List" ) ||
         s == QString( "Alphabetical List" ) )
        bDelete->setEnabled( false );
    else
        bDelete->setEnabled( true );
*/
    if(lStyleList->currentItem()==0)
      bUp->setEnabled(false);
    else
      bUp->setEnabled(true);

    if(lStyleList->currentItem()==(int)(lStyleList->count()-1))
      bDown->setEnabled(false);
    else
      bDown->setEnabled(true);

}

/******************************************************************/
/* Class: KWStylePreview                                          */
/******************************************************************/

/*================================================================*/
void KWStylePreview::drawContents( QPainter *painter )
{
    QRect r = contentsRect();
    QFontMetrics fm( font() );

    painter->fillRect( r.x() + fm.width( 'W' ), r.y() + fm.height(),
                       r.width() - 2 * fm.width( 'W' ), r.height() - 2 * fm.height(), white );
    painter->setClipRect( r.x() + fm.width( 'W' ), r.y() + fm.height(),
                          r.width() - 2 * fm.width( 'W' ), r.height() - 2 * fm.height() );

    QFont f( style->format().font().family(), style->format().font().pointSize() );
    f.setBold( style->format().font().weight() == 75 ? true : false );
    f.setItalic( style->format().font().italic() );
    f.setUnderline( style->format().font().underline()  );
    f.setStrikeOut( style->format().font().strikeOut()  );
    QColor c( style->format().color() );

    painter->setPen( QPen( c ) );
    painter->setFont( f );

    fm = QFontMetrics( f );
    int y = height() / 2 - fm.height() / 2;

    painter->drawText( 20 + style->paragLayout().margins[QStyleSheetItem::MarginFirstLine].pt() + style->paragLayout().margins[QStyleSheetItem::MarginLeft].pt(),
                       y, fm.width( i18n( "KWord, KOffice's Wordprocessor" ) ),
                       fm.height(), 0, i18n( "KWord, KOffice's Wordprocessor" ) );
}

/******************************************************************/
/* Class: KWStyleEditor                                           */
/******************************************************************/

/*================================================================*/
KWStyleEditor::KWStyleEditor( QWidget *_parent, KWStyle *_style, KWDocument *_doc, QStringList _fontList )
    : KDialogBase( Plain/*Tabbed*/, QString::null, Ok | Cancel, Ok, _parent, "", true )
{
    fontList = _fontList;
    paragDia = 0;
    ostyle = _style;
    style = new KWStyle(*_style);
    doc = _doc;
    setupTab1();

    setInitialSize( QSize(550, 400) );
}

/*================================================================*/
KWStyleEditor::~KWStyleEditor()
{
    delete style;
}

/*================================================================*/
void KWStyleEditor::setupTab1()
{
    //tab1 = addPage( i18n( "Style Editor" ) );
    tab1 = plainPage();

    grid1 = new QGridLayout( tab1, 2, 2, 15, 7 );

    nwid = new QWidget( tab1 );
    grid2 = new QGridLayout( nwid, 3, 2, 15, 7 );

    QLabel * lName = new QLabel( i18n( "Name:" ), nwid );
    lName->resize( lName->sizeHint() );
    lName->setAlignment( AlignRight | AlignVCenter );
    grid2->addWidget( lName, 0, 0 );

    eName = new QLineEdit( nwid );
    eName->resize( eName->sizeHint() );
    eName->setText( style->name() );
    grid2->addWidget( eName, 0, 1 );

/*
        if ( style->name() == QString( "Standard" ) ||
         style->name()== QString( "Head 1" ) ||
         style->name() == QString( "Head 2" ) ||
         style->name() == QString( "Head 3" ) ||
         style->name() == QString( "Enumerated List" ) ||
         style->name() == QString( "Bullet List" ) ||
         style->name() == QString( "Alphabetical List" ) )
        eName->setEnabled( false );
*/

    QLabel * lFollowing = new QLabel( i18n( "Following Style:" ), nwid );
    lFollowing->resize( lFollowing->sizeHint() );
    lFollowing->setAlignment( AlignRight | AlignVCenter );
    grid2->addWidget( lFollowing, 1, 0 );

    cFollowing = new QComboBox( false, nwid );
     QList<KWStyle> styles = const_cast<QList<KWStyle> & >(doc->styleList());
     for ( unsigned int i = 0; i < styles.count(); i++ ) {
        cFollowing->insertItem( styles.at( i )->name() );
        if ( styles.at( i )->name() == style->followingStyle() )
	  cFollowing->setCurrentItem( i );
     }
    cFollowing->resize( cFollowing->sizeHint() );
    grid2->addWidget( cFollowing, 1, 1 );

    grid2->addRowSpacing( 0, lName->height() );
    grid2->addRowSpacing( 0, eName->height() );
    grid2->addRowSpacing( 1, lFollowing->height() );
    grid2->addRowSpacing( 1, cFollowing->height() );
    grid2->setRowStretch( 0, 0 );
    grid2->setRowStretch( 0, 0 );
    grid2->setRowStretch( 2, 1 );

    grid2->addColSpacing( 0, lName->width() );
    grid2->addColSpacing( 0, lFollowing->width() );
    grid2->addColSpacing( 1, eName->width() );
    grid2->addColSpacing( 1, cFollowing->width() );
    grid2->setColStretch( 0, 0 );
    grid2->setColStretch( 1, 1 );

    grid1->addWidget( nwid, 0, 0 );

    preview = new KWStylePreview( i18n( "Preview" ), tab1, style );
    grid1->addWidget( preview, 1, 0 );

    bButtonBox = new KButtonBox( tab1, Vertical );
    bFont = bButtonBox->addButton( i18n( "&Font..." ), false );
    connect( bFont, SIGNAL( clicked() ), this, SLOT( changeFont() ) );
    bButtonBox->addStretch();
    bColor = bButtonBox->addButton( i18n( "&Color..." ), false );
    connect( bColor, SIGNAL( clicked() ), this, SLOT( changeColor() ) );
    bButtonBox->addStretch();
    bSpacing = bButtonBox->addButton( i18n( "&Spacing and Indents..." ), false );
    connect( bSpacing, SIGNAL( clicked() ), this, SLOT( changeSpacing() ) );
    bButtonBox->addStretch();
    bAlign = bButtonBox->addButton( i18n( "&Alignment..." ), false );
    connect( bAlign, SIGNAL( clicked() ), this, SLOT( changeAlign() ) );
    bButtonBox->addStretch();
    bBorders = bButtonBox->addButton( i18n( "&Borders..." ), false );
    connect( bBorders, SIGNAL( clicked() ), this, SLOT( changeBorders() ) );
    bButtonBox->addStretch();
    bNumbering = bButtonBox->addButton( i18n( "&Numbering..." ), false );
    connect( bNumbering, SIGNAL( clicked() ), this, SLOT( changeNumbering() ) );
    bButtonBox->addStretch();
    bTabulators = bButtonBox->addButton( i18n( "&Tabulators..." ), false );
    connect( bTabulators, SIGNAL( clicked() ), this, SLOT( changeTabulators() ) );
    bButtonBox->layout();
    grid1->addMultiCellWidget( bButtonBox, 0, 1, 1, 1 );

    grid1->addColSpacing( 0, nwid->width() );
    grid1->addColSpacing( 0, preview->width() );
    grid1->addColSpacing( 1, bButtonBox->width() );
    grid1->addColSpacing( 1, bSpacing->width() + 10 );
    grid1->setColStretch( 0, 1 );

    grid1->addRowSpacing( 0, nwid->height() );
    grid1->addRowSpacing( 1, 120 );
    grid1->addRowSpacing( 0, bButtonBox->height() / 3 );
    grid1->addRowSpacing( 1, 2 * bButtonBox->height() / 3 );
    grid1->setRowStretch( 1, 1 );
}

/*================================================================*/
void KWStyleEditor::changeFont()
{
    QFont f( style->format().font().family(), style->format().font().pointSize() );
    f.setBold( style->format().font().weight() == 75 ? true : false );
    f.setItalic( style->format().font().italic() );
    f.setUnderline( style->format().font().underline() );

    KWFontDia *fontDia = new KWFontDia( this, "",f,false, false,false );
    connect( fontDia, SIGNAL( okClicked() ), this, SLOT( slotFontDiaOk() ) );

    fontDia->show();
    delete fontDia;
#if 0
    if ( KFontDialog::getFont( f ) ) {
        style->format().setFont(f);
        /*style->getFormat().setPTFontSize( f.pointSize() );
          style->getFormat().setWeight( f.bold() ? 75 : 50 );
          style->getFormat().setItalic( static_cast<int>( f.italic() ) );
          style->getFormat().setUnderline( static_cast<int>( f.underline() ) );*/
        preview->repaint( true );
    }
#endif
}

void KWStyleEditor::slotFontDiaOk()
{
     const KWFontDia * fontDia = static_cast<const KWFontDia*>(sender());
     style->format().setFont(fontDia->getNewFont());
     preview->repaint( true );
}

/*================================================================*/
void KWStyleEditor::changeColor()
{
  QColor c( style->format().color() );
  if ( KColorDialog::getColor( c ) ) {
    style->format().setColor( c );
    preview->repaint( true );
  }
}

/*================================================================*/
void KWStyleEditor::changeSpacing()
{
    if ( paragDia ) {
        paragDia->close();
        delete paragDia;
        paragDia = 0;
    }
    paragDia = new KWParagDia( this, "", fontList, KWParagDia::PD_SPACING, doc );
    paragDia->setCaption( i18n( "Paragraph Spacing" ) );
    connect( paragDia, SIGNAL( okClicked() ), this, SLOT( paragDiaOk() ) );
    paragDia->setSpaceBeforeParag( style->paragLayout().margins[QStyleSheetItem::MarginTop] );
    paragDia->setSpaceAfterParag( style->paragLayout().margins[QStyleSheetItem::MarginBottom] );
    paragDia->setLineSpacing( style->paragLayout().lineSpacing );
    paragDia->setLeftIndent( style->paragLayout().margins[QStyleSheetItem::MarginLeft] );
    paragDia->setFirstLineIndent( style->paragLayout().margins[QStyleSheetItem::MarginFirstLine] );
    paragDia->setRightIndent( style->paragLayout().margins[QStyleSheetItem::MarginRight] );
    paragDia->show();
}

/*================================================================*/
void KWStyleEditor::changeAlign()
{
    if ( paragDia ) {
        paragDia->close();
        delete paragDia;
        paragDia = 0;
    }
    paragDia = new KWParagDia( this, "", fontList, KWParagDia::PD_ALIGN, doc );
    paragDia->setCaption( i18n( "Paragraph Alignment" ) );
    connect( paragDia, SIGNAL( okClicked() ), this, SLOT( paragDiaOk() ) );
    paragDia->setAlign( style->paragLayout().alignment );
    paragDia->show();
}

/*================================================================*/
void KWStyleEditor::changeBorders()
{
    if ( paragDia ) {
        paragDia->close();
        delete paragDia;
        paragDia = 0;
    }
    paragDia = new KWParagDia( this, "", fontList, KWParagDia::PD_BORDERS, doc );
    paragDia->setCaption( i18n( "Paragraph Borders" ) );
    connect( paragDia, SIGNAL( okClicked() ), this, SLOT( paragDiaOk() ) );
    paragDia->setLeftBorder( style->paragLayout().leftBorder );
    paragDia->setRightBorder( style->paragLayout().rightBorder );
    paragDia->setTopBorder( style->paragLayout().topBorder );
    paragDia->setBottomBorder( style->paragLayout().bottomBorder);
    paragDia->setAfterInitBorder(true);
    paragDia->show();
}

/*================================================================*/
void KWStyleEditor::changeNumbering()
{
    if ( paragDia ) {
        paragDia->close();
        delete paragDia;
        paragDia = 0;
    }
    paragDia = new KWParagDia( this, "", fontList, KWParagDia::PD_NUMBERING, doc );
    paragDia->setCaption( i18n( "Numbering" ) );
    connect( paragDia, SIGNAL( okClicked() ), this, SLOT( paragDiaOk() ) );
    if ( !style->paragLayout().counter )
        style->paragLayout().counter = new Counter; // default one if none set
    paragDia->setCounter( *style->paragLayout().counter );
    paragDia->show();
}

/*================================================================*/
void KWStyleEditor::changeTabulators()
{
    if ( paragDia ) {
        paragDia->close();
        delete paragDia;
        paragDia = 0;
    }
    paragDia = new KWParagDia( this, "", fontList, KWParagDia::PD_TABS, doc );
    paragDia->setCaption( i18n( "Tabulators" ) );
    connect( paragDia, SIGNAL( okClicked() ), this, SLOT( paragDiaOk() ) );
    paragDia->setTabList( style->paragLayout().tabList() );
    paragDia->show();
}

/*================================================================*/
void KWStyleEditor::paragDiaOk()
{
   switch ( paragDia->getFlags() ) {
   case KWParagDia::PD_SPACING: {
     style->paragLayout().margins[QStyleSheetItem::MarginTop]=paragDia->spaceBeforeParag() ;
     style->paragLayout().margins[QStyleSheetItem::MarginBottom]=paragDia->spaceAfterParag() ;
     style->paragLayout().lineSpacing= paragDia->lineSpacing() ;
     style->paragLayout().margins[QStyleSheetItem::MarginLeft]=paragDia->leftIndent() ;
     style->paragLayout().margins[QStyleSheetItem::MarginFirstLine]= paragDia->firstLineIndent();
     style->paragLayout().margins[QStyleSheetItem::MarginRight]= paragDia->rightIndent();
   } break;
    case KWParagDia::PD_ALIGN:
      style->paragLayout().alignment=paragDia->align() ;
      break;
   case KWParagDia::PD_BORDERS: {
     style->paragLayout().leftBorder= paragDia->leftBorder() ;
     style->paragLayout().rightBorder= paragDia->rightBorder() ;
     style->paragLayout().topBorder= paragDia->topBorder() ;
     style->paragLayout().bottomBorder= paragDia->bottomBorder() ;
   } break;
   case KWParagDia::PD_NUMBERING:
     delete style->paragLayout().counter;
     style->paragLayout().counter = new Counter( paragDia->counter() );
     break;
   case KWParagDia::PD_TABS:
       style->paragLayout().setTabList(paragDia->tabListTabulator());
     break;
   default: break;
   }

    preview->repaint( true );
}

/*================================================================*/
bool KWStyleEditor::apply()
{
    *ostyle = *style;

    // Apply name change
    if ( eName->text() != style->name() )
    {
        bool same = false;
	QList<KWStyle> styles = const_cast<QList<KWStyle> & >(doc->styleList());
        for ( unsigned int i = 0; i < styles.count() && !same; i++ ) {
            if ( styles.at( i )->name() == eName->text() )
                same = true;
        }

        if ( !same ) {
            ostyle->paragLayout().setStyleName( eName->text() );
            emit updateStyleList();
        }
    }

    // Apply "following style" change
    ostyle->setFollowingStyle( cFollowing->currentText() );

    // ### TODO a dirty flag for this !
    doc->applyStyleChange(eName->text());
    return true;
}

/*================================================================*/
void KWStyleEditor::slotOk()
{
   if (apply())
   {
      KDialogBase::slotOk();
   }
}
