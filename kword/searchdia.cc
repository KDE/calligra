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

#include "kword_doc.h"
#include "kword_page.h"
#include "kword_view.h"
#include "searchdia.h"
#include "searchdia.moc"

#include <klocale.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kcolorbtn.h>
#include <kbuttonbox.h>

#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qevent.h>
#include <qstrlist.h>
#include <kmessagebox.h>

#include <stdlib.h>

/******************************************************************/
/* Class: KWSearchDia                                             */
/******************************************************************/

/*================================================================*/
KWSearchDia::KWSearchDia( QWidget* parent, const char* name, KWordDocument *_doc, KWPage *_page, KWordView *_view,
                          KWSearchEntry *_searchEntry, KWSearchEntry *_replaceEntry, QStringList _fontlist )
    : KDialogBase(Tabbed, QString::null, Close, Close, parent, name, false )
{
    doc = _doc;
    page = _page;
    view = _view;
    searchEntry = _searchEntry;
    replaceEntry = _replaceEntry;
    fontlist = _fontlist;

    if ( !searchEntry )
    {
        searchEntry = new KWSearchEntry();
        view->setSearchEntry( searchEntry );
    }

    if ( !replaceEntry )
    {
        replaceEntry = new KWSearchEntry();
        view->setReplaceEntry( replaceEntry );
    }

    setupTab1();
    setupTab2();

    setInitialSize( QSize(600, 400) );
}

/*================================================================*/
void KWSearchDia::setupTab1()
{
    tab1 = addPage( i18n( "Find" ) );
    grid1 = new QGridLayout( tab1, 3, 1, 7, 7 );

    /**
     * ---------------- search group --------------------
     */

    gSearch = new QGroupBox( i18n( "Search" ), tab1 );
    sGrid = new QGridLayout( gSearch, 9, 3, 7, 7 );

    lSearch = new QLabel( i18n( "Search String:" ), gSearch );
    lSearch->resize( lSearch->sizeHint() );
    lSearch->setAlignment( AlignBottom );
    sGrid->addWidget( lSearch, 1, 0 );

    eSearch = new QLineEdit( gSearch );
    eSearch->resize( eSearch->sizeHint() );
    sGrid->addWidget( eSearch, 2, 0 );
    eSearch->setText( searchEntry->expr );

    cWholeWords = new QCheckBox( i18n( "Find Whole Words only" ), gSearch );
    cWholeWords->resize( cWholeWords->sizeHint() );
    sGrid->addWidget( cWholeWords, 3, 0 );
    cWholeWords->setChecked( searchEntry->wholeWords );

    cCase = new QCheckBox( i18n( "Case Sensitive" ), gSearch );
    cCase->resize( cCase->sizeHint() );
    sGrid->addWidget( cCase, 4, 0 );
    cCase->setChecked( searchEntry->caseSensitive );

    cRegExp = new QCheckBox( i18n( "Regular Expression" ), gSearch );
    cRegExp->resize( cRegExp->sizeHint() );
    sGrid->addWidget( cRegExp, 5, 0 );
    cRegExp->setChecked( searchEntry->regexp );
    connect( cRegExp, SIGNAL( clicked() ), this, SLOT( slotRegExp() ) );

    cWildcard = new QCheckBox( i18n( "Use Wildcards" ), gSearch );
    cWildcard->resize( cWildcard->sizeHint() );
    sGrid->addWidget( cWildcard, 6, 0 );
    cWildcard->setChecked( searchEntry->wildcard );

    cRev = new QCheckBox( i18n( "Find Backwards" ), gSearch );
    cRev->resize( cRev->sizeHint() );
    sGrid->addWidget( cRev, 7, 0 );
    cRev->setChecked( searchEntry->reverse );

    cFamily = new QCheckBox( i18n( "Family" ), gSearch );
    cFamily->resize( cFamily->sizeHint() );
    sGrid->addWidget( cFamily, 1, 1 );
    connect( cFamily, SIGNAL( clicked() ), this, SLOT( slotCheckFamily() ) );
    cFamily->setChecked( searchEntry->checkFamily );

    cSize = new QCheckBox( i18n( "Size" ), gSearch );
    cSize->resize( cSize->sizeHint() );
    sGrid->addWidget( cSize, 2, 1 );
    connect( cSize, SIGNAL( clicked() ), this, SLOT( slotCheckSize() ) );
    cSize->setChecked( searchEntry->checkSize );

    cColor = new QCheckBox( i18n( "Color" ), gSearch );
    cColor->resize( cColor->sizeHint() );
    sGrid->addWidget( cColor, 3, 1 );
    connect( cColor, SIGNAL( clicked() ), this, SLOT( slotCheckColor() ) );
    cColor->setChecked( searchEntry->checkColor );

    cBold = new QCheckBox( i18n( "Bold" ), gSearch );
    cBold->resize( cBold->sizeHint() );
    sGrid->addWidget( cBold, 4, 1 );
    connect( cBold, SIGNAL( clicked() ), this, SLOT( slotCheckBold() ) );
    cBold->setChecked( searchEntry->checkBold );

    cItalic = new QCheckBox( i18n( "Italic" ), gSearch );
    cItalic->resize( cItalic->sizeHint() );
    sGrid->addWidget( cItalic, 5, 1 );
    connect( cItalic, SIGNAL( clicked() ), this, SLOT( slotCheckItalic() ) );
    cItalic->setChecked( searchEntry->checkItalic );

    cUnderline = new QCheckBox( i18n( "Underline" ), gSearch );
    cUnderline->resize( cUnderline->sizeHint() );
    sGrid->addWidget( cUnderline, 6, 1 );
    connect( cUnderline, SIGNAL( clicked() ), this, SLOT( slotCheckUnderline() ) );
    cUnderline->setChecked( searchEntry->checkUnderline );

    cVertAlign = new QCheckBox( i18n( "Vertical Alignment" ), gSearch );
    cVertAlign->resize( cVertAlign->sizeHint() );
    sGrid->addWidget( cVertAlign, 7, 1 );
    connect( cVertAlign, SIGNAL( clicked() ), this, SLOT( slotCheckVertAlign() ) );
    cVertAlign->setChecked( searchEntry->checkVertAlign );

    cmFamily = new QComboBox( true, gSearch );
    cmFamily->insertStringList( fontlist );
    cmFamily->resize( cmFamily->sizeHint() );
    sGrid->addWidget( cmFamily, 1, 2 );
    for ( int j = 0; j < cmFamily->count(); j++ )
    {
        if ( QString( cmFamily->text( j ) ) == searchEntry->family )
            cmFamily->setCurrentItem( j );
    }
    connect( cmFamily, SIGNAL( activated( const QString & ) ), this, SLOT( slotFamily( const QString & ) ) );

    cmSize = new QComboBox( true, gSearch );
    QStringList sizes;
    int curr = 0;
    for ( int i = 4; i <= 100; i++ ) {
        sizes.append( QString::number(i) );
        if ( i == searchEntry->size ) curr = i - 4;
    }
    cmSize->insertStrList( &sizes );
    cmSize->resize( cmSize->sizeHint() );
    sGrid->addWidget( cmSize, 2, 2 );
    cmSize->setCurrentItem( curr );
    connect( cmSize, SIGNAL( activated( const QString & ) ), this, SLOT( slotSize( const QString & ) ) );

    bColor = new KColorButton( gSearch );
    bColor->resize( bColor->sizeHint() );
    sGrid->addWidget( bColor, 3, 2 );
    bColor->setColor( searchEntry->color );
    connect( bColor, SIGNAL( changed( const QColor& ) ), this, SLOT( slotColor( const QColor& ) ) );

    cmBold = new QCheckBox( i18n( "Bold" ), gSearch );
    cmBold->resize( cmBold->sizeHint() );
    sGrid->addWidget( cmBold, 4, 2 );
    cmBold->setChecked( searchEntry->bold );
    connect( cmBold, SIGNAL( clicked() ), this, SLOT( slotBold() ) );

    cmItalic = new QCheckBox( i18n( "Italic" ), gSearch );
    cmItalic->resize( cmItalic->sizeHint() );
    sGrid->addWidget( cmItalic, 5, 2 );
    cmItalic->setChecked( searchEntry->italic );
    connect( cmItalic, SIGNAL( clicked() ), this, SLOT( slotItalic() ) );

    cmUnderline = new QCheckBox( i18n( "Underline" ), gSearch );
    cmUnderline->resize( cmUnderline->sizeHint() );
    sGrid->addWidget( cmUnderline, 6, 2 );
    cmUnderline->setChecked( searchEntry->underline );
    connect( cmUnderline, SIGNAL( clicked() ), this, SLOT( slotUnderline() ) );

    cmVertAlign = new QComboBox( false, gSearch );
    cmVertAlign->insertItem( i18n( "Normal" ), -1 );
    cmVertAlign->insertItem( i18n( "Subscript" ), -1 );
    cmVertAlign->insertItem( i18n( "Superscript" ), -1 );
    cmVertAlign->resize( cmVertAlign->sizeHint() );
    sGrid->addWidget( cmVertAlign, 7, 2 );
    switch ( searchEntry->vertAlign )
    {
    case KWFormat::VA_NORMAL:
        cmVertAlign->setCurrentItem( 0 );
        break;
    case KWFormat::VA_SUB:
        cmVertAlign->setCurrentItem( 1 );
        break;
    case KWFormat::VA_SUPER:
        cmVertAlign->setCurrentItem( 2 );
        break;
    }
    connect( cmVertAlign, SIGNAL( activated( int ) ), this, SLOT( slotVertAlign( int ) ) );

    sGrid->addRowSpacing( 0, 7 );
    sGrid->addRowSpacing( 1, lSearch->height() );
    sGrid->addRowSpacing( 1, cFamily->height() );
    sGrid->addRowSpacing( 1, cmFamily->height() );
    sGrid->addRowSpacing( 2, eSearch->height() );
    sGrid->addRowSpacing( 2, cSize->height() );
    sGrid->addRowSpacing( 2, cmSize->height() );
    sGrid->addRowSpacing( 4, cCase->height() );
    sGrid->addRowSpacing( 3, cColor->height() );
    sGrid->addRowSpacing( 3, bColor->height() );
    sGrid->addRowSpacing( 5, cRegExp->height() );
    sGrid->addRowSpacing( 4, cBold->height() );
    sGrid->addRowSpacing( 4, cmBold->height() );
    sGrid->addRowSpacing( 3, cWholeWords->height() );
    sGrid->addRowSpacing( 5, cItalic->height() );
    sGrid->addRowSpacing( 5, cmItalic->height() );
    sGrid->addRowSpacing( 6, cUnderline->height() );
    sGrid->addRowSpacing( 6, cmUnderline->height() );
    sGrid->addRowSpacing( 6, cWildcard->height() );
    sGrid->addRowSpacing( 7, cRev->height() );
    sGrid->addRowSpacing( 7, cVertAlign->height() );
    sGrid->addRowSpacing( 7, cmVertAlign->height() );
    sGrid->setRowStretch( 0, 0 );
    sGrid->setRowStretch( 1, 0 );
    sGrid->setRowStretch( 2, 0 );
    sGrid->setRowStretch( 3, 0 );
    sGrid->setRowStretch( 4, 0 );
    sGrid->setRowStretch( 5, 0 );
    sGrid->setRowStretch( 6, 0 );
    sGrid->setRowStretch( 7, 0 );
    sGrid->setRowStretch( 8, 1 );

    sGrid->addColSpacing( 0, lSearch->width() );
    sGrid->addColSpacing( 1, cFamily->width() );
    sGrid->addColSpacing( 2, cmFamily->width() );
    sGrid->addColSpacing( 0, eSearch->width() );
    sGrid->addColSpacing( 1, cSize->width() );
    sGrid->addColSpacing( 2, cmSize->width() );
    sGrid->addColSpacing( 0, cRegExp->width() );
    sGrid->addColSpacing( 0, cCase->width() );
    sGrid->addColSpacing( 0, cWholeWords->width() );
    sGrid->addColSpacing( 0, cRev->width() );
    sGrid->addColSpacing( 1, cColor->width() );
    sGrid->addColSpacing( 2, bColor->width() );
    sGrid->addColSpacing( 1, cBold->width() );
    sGrid->addColSpacing( 2, cmBold->width() );
    sGrid->addColSpacing( 1, cItalic->width() );
    sGrid->addColSpacing( 2, cmItalic->width() );
    sGrid->addColSpacing( 1, cUnderline->width() );
    sGrid->addColSpacing( 2, cmUnderline->width() );
    sGrid->addColSpacing( 1, cVertAlign->width() );
    sGrid->addColSpacing( 2, cmVertAlign->width() );
    sGrid->setColStretch( 0, 1 );
    sGrid->setColStretch( 1, 0 );
    sGrid->setColStretch( 2, 0 );

    grid1->addWidget( gSearch, 0, 0 );

    /**
     * ------------------ search buttonbox --------------
     */

    bbSearch = new KButtonBox( tab1, Horizontal );
    bSearchFirst = bbSearch->addButton( i18n( "Find &First" ), false );
    connect( bSearchFirst, SIGNAL( clicked() ), this, SLOT( searchFirst() ) );
    bSearchNext = bbSearch->addButton( i18n( "Find &Next" ), false );
    connect( bSearchNext, SIGNAL( clicked() ), this, SLOT( searchNext() ) );
    //bSearchAll = bbSearch->addButton( i18n( "Search &All" ), false );
    bbSearch->layout();
    bbSearch->resize( bbSearch->sizeHint() );

    grid1->addWidget( bbSearch, 1, 0 );

    /**
     * ----------------- general -----------------
     */

    grid1->addRowSpacing( 0, gSearch->height() );
    grid1->addRowSpacing( 1, bbSearch->height() );
    grid1->addRowSpacing( 2, 0 );
    grid1->setRowStretch( 0, 0 );
    grid1->setRowStretch( 1, 0 );
    grid1->setRowStretch( 2, 1 );

    grid1->addColSpacing( 0, gSearch->width() );
    grid1->addColSpacing( 0, bbSearch->width() );
    grid1->setColStretch( 0, 1 );

    slotCheckFamily();
    slotCheckColor();
    slotCheckSize();
    slotCheckBold();
    slotCheckItalic();
    slotCheckUnderline();
    slotCheckVertAlign();
    slotRegExp();

    connect( this, SIGNAL( closeClicked() ), this, SLOT( saveSettings() ) );
}

/*================================================================*/
void KWSearchDia::setupTab2()
{
    tab2 = addPage( i18n( "Replace" ) );
    grid2 = new QGridLayout( tab2, 3, 1, 7, 7 );

    /**
     * ---------------- search group --------------------
     */

    gReplace = new QGroupBox( i18n( "Replace" ), tab2 );
    rGrid = new QGridLayout( gReplace, 9, 3, 7, 7 );

    lReplace = new QLabel( i18n( "Replace String:" ), gReplace );
    lReplace->resize( lReplace->sizeHint() );
    lReplace->setAlignment( AlignBottom );
    rGrid->addWidget( lReplace, 1, 0 );

    eReplace = new QLineEdit( gReplace );
    eReplace->resize( eReplace->sizeHint() );
    rGrid->addWidget( eReplace, 2, 0 );
    eReplace->setText( replaceEntry->expr );

    rcFamily = new QCheckBox( i18n( "Family" ), gReplace );
    rcFamily->resize( rcFamily->sizeHint() );
    rGrid->addWidget( rcFamily, 1, 1 );
    connect( rcFamily, SIGNAL( clicked() ), this, SLOT( rslotCheckFamily() ) );
    rcFamily->setChecked( replaceEntry->checkFamily );

    rcSize = new QCheckBox( i18n( "Size" ), gReplace );
    rcSize->resize( rcSize->sizeHint() );
    rGrid->addWidget( rcSize, 2, 1 );
    connect( rcSize, SIGNAL( clicked() ), this, SLOT( rslotCheckSize() ) );
    rcSize->setChecked( replaceEntry->checkSize );

    rcColor = new QCheckBox( i18n( "Color" ), gReplace );
    rcColor->resize( rcColor->sizeHint() );
    rGrid->addWidget( rcColor, 3, 1 );
    connect( rcColor, SIGNAL( clicked() ), this, SLOT( rslotCheckColor() ) );
    rcColor->setChecked( replaceEntry->checkColor );

    rcBold = new QCheckBox( i18n( "Bold" ), gReplace );
    rcBold->resize( rcBold->sizeHint() );
    rGrid->addWidget( rcBold, 4, 1 );
    connect( rcBold, SIGNAL( clicked() ), this, SLOT( rslotCheckBold() ) );
    rcBold->setChecked( replaceEntry->checkBold );

    rcItalic = new QCheckBox( i18n( "Italic" ), gReplace );
    cItalic->resize( rcItalic->sizeHint() );
    rGrid->addWidget( rcItalic, 5, 1 );
    connect( rcItalic, SIGNAL( clicked() ), this, SLOT( rslotCheckItalic() ) );
    rcItalic->setChecked( replaceEntry->checkItalic );

    rcUnderline = new QCheckBox( i18n( "Underline" ), gReplace );
    rcUnderline->resize( rcUnderline->sizeHint() );
    rGrid->addWidget( rcUnderline, 6, 1 );
    connect( rcUnderline, SIGNAL( clicked() ), this, SLOT( rslotCheckUnderline() ) );
    rcUnderline->setChecked( replaceEntry->checkUnderline );

    rcVertAlign = new QCheckBox( i18n( "Vertical Alignment" ), gReplace );
    rcVertAlign->resize( rcVertAlign->sizeHint() );
    rGrid->addWidget( rcVertAlign, 7, 1 );
    connect( rcVertAlign, SIGNAL( clicked() ), this, SLOT( rslotCheckVertAlign() ) );
    rcVertAlign->setChecked( replaceEntry->checkVertAlign );

    rcmFamily = new QComboBox( true, gReplace );
    rcmFamily->insertStringList( fontlist );
    rcmFamily->resize( rcmFamily->sizeHint() );
    rGrid->addWidget( rcmFamily, 1, 2 );
    for ( int j = 0; j < rcmFamily->count(); j++ )
    {
        if ( QString( rcmFamily->text( j ) ) == replaceEntry->family )
            rcmFamily->setCurrentItem( j );
    }
    connect( rcmFamily, SIGNAL( activated( const QString & ) ), this, SLOT( rslotFamily( const QString & ) ) );

    rcmSize = new QComboBox( true, gReplace );
    QStringList sizes;
    int curr = 0;
    for ( int i = 4; i <= 100; i++ ) {
        sizes.append( QString::number(i) );
        if ( i == replaceEntry->size ) curr = i - 4;
    }
    rcmSize->insertStrList( &sizes );
    rcmSize->resize( cmSize->sizeHint() );
    rGrid->addWidget( rcmSize, 2, 2 );
    rcmSize->setCurrentItem( curr );
    connect( rcmSize, SIGNAL( activated( const QString & ) ), this, SLOT( rslotSize( const QString & ) ) );

    rbColor = new KColorButton( gReplace );
    rbColor->resize( rbColor->sizeHint() );
    rGrid->addWidget( rbColor, 3, 2 );
    rbColor->setColor( replaceEntry->color );
    connect( rbColor, SIGNAL( changed( const QColor& ) ), this, SLOT( rslotColor( const QColor& ) ) );

    rcmBold = new QCheckBox( i18n( "Bold" ), gReplace );
    rcmBold->resize( rcmBold->sizeHint() );
    rGrid->addWidget( rcmBold, 4, 2 );
    rcmBold->setChecked( replaceEntry->bold );
    connect( rcmBold, SIGNAL( clicked() ), this, SLOT( rslotBold() ) );

    rcmItalic = new QCheckBox( i18n( "Italic" ), gReplace );
    rcmItalic->resize( rcmItalic->sizeHint() );
    rGrid->addWidget( rcmItalic, 5, 2 );
    rcmItalic->setChecked( replaceEntry->italic );
    connect( rcmItalic, SIGNAL( clicked() ), this, SLOT( rslotItalic() ) );

    rcmUnderline = new QCheckBox( i18n( "Underline" ), gReplace );
    rcmUnderline->resize( rcmUnderline->sizeHint() );
    rGrid->addWidget( rcmUnderline, 6, 2 );
    rcmUnderline->setChecked( replaceEntry->underline );
    connect( rcmUnderline, SIGNAL( clicked() ), this, SLOT( rslotUnderline() ) );

    rcmVertAlign = new QComboBox( false, gReplace );
    rcmVertAlign->insertItem( i18n( "Normal" ), -1 );
    rcmVertAlign->insertItem( i18n( "Subscript" ), -1 );
    rcmVertAlign->insertItem( i18n( "Superscript" ), -1 );
    rcmVertAlign->resize( rcmVertAlign->sizeHint() );
    rGrid->addWidget( rcmVertAlign, 7, 2 );
    switch ( replaceEntry->vertAlign )
    {
    case KWFormat::VA_NORMAL:
        rcmVertAlign->setCurrentItem( 0 );
        break;
    case KWFormat::VA_SUB:
        rcmVertAlign->setCurrentItem( 1 );
        break;
    case KWFormat::VA_SUPER:
        rcmVertAlign->setCurrentItem( 2 );
        break;
    }
    connect( rcmVertAlign, SIGNAL( activated( int ) ), this, SLOT( rslotVertAlign( int ) ) );

    rGrid->addRowSpacing( 0, 7 );
    rGrid->addRowSpacing( 1, lReplace->height() );
    rGrid->addRowSpacing( 1, rcFamily->height() );
    rGrid->addRowSpacing( 1, rcmFamily->height() );
    rGrid->addRowSpacing( 2, eReplace->height() );
    rGrid->addRowSpacing( 2, rcSize->height() );
    rGrid->addRowSpacing( 2, rcmSize->height() );
    rGrid->addRowSpacing( 3, rcColor->height() );
    rGrid->addRowSpacing( 3, rbColor->height() );
    rGrid->addRowSpacing( 4, rcBold->height() );
    rGrid->addRowSpacing( 4, rcmBold->height() );
    rGrid->addRowSpacing( 5, rcItalic->height() );
    rGrid->addRowSpacing( 5, rcmItalic->height() );
    rGrid->addRowSpacing( 6, rcUnderline->height() );
    rGrid->addRowSpacing( 6, rcmUnderline->height() );
    rGrid->addRowSpacing( 7, rcVertAlign->height() );
    rGrid->addRowSpacing( 7, rcmVertAlign->height() );
    rGrid->setRowStretch( 0, 0 );
    rGrid->setRowStretch( 1, 0 );
    rGrid->setRowStretch( 2, 0 );
    rGrid->setRowStretch( 3, 0 );
    rGrid->setRowStretch( 4, 0 );
    rGrid->setRowStretch( 5, 0 );
    rGrid->setRowStretch( 6, 0 );
    rGrid->setRowStretch( 7, 0 );
    rGrid->setRowStretch( 8, 1 );

    rGrid->addColSpacing( 0, lReplace->width() );
    rGrid->addColSpacing( 1, rcFamily->width() );
    rGrid->addColSpacing( 2, rcmFamily->width() );
    rGrid->addColSpacing( 0, eReplace->width() );
    rGrid->addColSpacing( 1, rcSize->width() );
    rGrid->addColSpacing( 2, rcmSize->width() );
    rGrid->addColSpacing( 1, rcColor->width() );
    rGrid->addColSpacing( 2, rbColor->width() );
    rGrid->addColSpacing( 1, rcBold->width() );
    rGrid->addColSpacing( 2, rcmBold->width() );
    rGrid->addColSpacing( 1, rcItalic->width() );
    rGrid->addColSpacing( 2, rcmItalic->width() );
    rGrid->addColSpacing( 1, rcUnderline->width() );
    rGrid->addColSpacing( 2, rcmUnderline->width() );
    rGrid->addColSpacing( 1, rcVertAlign->width() );
    rGrid->addColSpacing( 2, rcmVertAlign->width() );
    rGrid->setColStretch( 0, 1 );
    rGrid->setColStretch( 1, 0 );
    rGrid->setColStretch( 2, 0 );

    grid2->addWidget( gReplace, 0, 0 );

    /**
     * ------------------ replace buttonbox --------------
     */

    QWidget *wid = new QWidget( tab2 );
    QGridLayout *subgrid = new QGridLayout( wid, 1, 2, 0, 10 );

    bbReplace = new KButtonBox( wid, Horizontal );
    bReplaceFirst = bbReplace->addButton( i18n( "Replace &First" ) );
    connect( bReplaceFirst, SIGNAL( clicked() ), this, SLOT( replaceFirst() ) );
    bReplaceNext = bbReplace->addButton( i18n( "Replace &Next" ) );
    connect( bReplaceNext, SIGNAL( clicked() ), this, SLOT( replaceNext() ) );
    bReplaceAll = bbReplace->addButton( i18n( "Replace &All" ) );
    connect( bReplaceAll, SIGNAL( clicked() ), this, SLOT( replaceAll() ) );
    bbReplace->layout();
    bbReplace->resize( bbReplace->sizeHint() );
    subgrid->addWidget( bbReplace, 0, 0 );

    cAsk = new QCheckBox( i18n( "Ask before replacing" ), wid );
    cAsk->resize( cAsk->sizeHint() );
    cAsk->setChecked( replaceEntry->ask );
    subgrid->addWidget( cAsk, 0, 1 );

    subgrid->addRowSpacing( 0, bbReplace->height() );
    subgrid->addRowSpacing( 0, cAsk->height() );
    subgrid->setRowStretch( 0, 0 );

    subgrid->addColSpacing( 0, bbReplace->width() );
    subgrid->addColSpacing( 1, cAsk->width() );
    subgrid->setColStretch( 0, 0 );
    subgrid->setColStretch( 1, 1 );

    grid2->addWidget( wid, 1, 0 );

    /**
     * ----------------- general -----------------
     */

    grid2->addRowSpacing( 0, gReplace->height() );
    grid2->addRowSpacing( 1, wid->height() );
    grid2->addRowSpacing( 2, 0 );
    grid2->setRowStretch( 0, 0 );
    grid2->setRowStretch( 1, 0 );
    grid2->setRowStretch( 2, 1 );

    grid2->addColSpacing( 0, gReplace->width() );
    grid2->addColSpacing( 0, wid->width() );
    grid2->setColStretch( 0, 1 );

    rslotCheckFamily();
    rslotCheckColor();
    rslotCheckSize();
    rslotCheckBold();
    rslotCheckItalic();
    rslotCheckUnderline();
    rslotCheckVertAlign();
}

/*================================================================*/
void KWSearchDia::searchFirst()
{
    QString expr = eSearch->text();
    if ( expr.isEmpty() ) return;

    bool addlen = true;

    if ( !cRev->isChecked() )
        page->find( expr, searchEntry, true, cCase->isChecked(), cWholeWords->isChecked(), cRegExp->isChecked(), cWildcard->isChecked(), addlen );
    else
        page->findRev( expr, searchEntry, true, cCase->isChecked(), cWholeWords->isChecked(), cRegExp->isChecked(), cWildcard->isChecked(), addlen );
}

/*================================================================*/
void KWSearchDia::searchNext()
{
    QString expr = eSearch->text();
    if ( expr.isEmpty() ) return;

    bool addlen = true;

    if ( !cRev->isChecked() )
        page->find( expr, searchEntry, false, cCase->isChecked(), cWholeWords->isChecked(), cRegExp->isChecked(), cWildcard->isChecked(), addlen );
    else
        page->findRev( expr, searchEntry, false, cCase->isChecked(), cWholeWords->isChecked(), cRegExp->isChecked(), cWildcard->isChecked(), addlen );
}

/*================================================================*/
void KWSearchDia::slotCheckFamily()
{
    searchEntry->checkFamily = cFamily->isChecked();

    if ( cFamily->isChecked() )
    {
        cmFamily->setEnabled( true );
        slotFamily( cmFamily->currentText() );
    }
    else
    {
        cmFamily->setEnabled( false );
        eSearch->setFont( KGlobalSettings::generalFont() );
    }
}

/*================================================================*/
void KWSearchDia::slotCheckColor()
{
    searchEntry->checkColor = cColor->isChecked();

    if ( cColor->isChecked() )
    {
        bColor->setEnabled( true );
        slotColor( bColor->color() );
    }
    else bColor->setEnabled( false );
}

/*================================================================*/
void KWSearchDia::slotCheckSize()
{
    searchEntry->checkSize = cSize->isChecked();

    if ( cSize->isChecked() )
    {
        cmSize->setEnabled( true );
        slotSize( cmSize->currentText() );
    }
    else cmSize->setEnabled( false );
}

/*================================================================*/
void KWSearchDia::slotCheckBold()
{
    searchEntry->checkBold = cBold->isChecked();

    if ( cBold->isChecked() )
    {
        cmBold->setEnabled( true );
        slotBold();
    }
    else cmBold->setEnabled( false );
}

/*================================================================*/
void KWSearchDia::slotCheckItalic()
{
    searchEntry->checkItalic = cItalic->isChecked();

    if ( cItalic->isChecked() )
    {
        cmItalic->setEnabled( true );
        slotItalic();
    }
    else cmItalic->setEnabled( false );
}

/*================================================================*/
void KWSearchDia::slotCheckUnderline()
{
    searchEntry->checkUnderline = cUnderline->isChecked();

    if ( cUnderline->isChecked() )
    {
        cmUnderline->setEnabled( true );
        slotUnderline();
    }
    else cmUnderline->setEnabled( false );
}

/*================================================================*/
void KWSearchDia::slotCheckVertAlign()
{
    searchEntry->checkVertAlign = cVertAlign->isChecked();

    if ( cVertAlign->isChecked() )
    {
        cmVertAlign->setEnabled( true );
        slotVertAlign( cmVertAlign->currentItem() );
    }
    else cmVertAlign->setEnabled( false );
}

/*================================================================*/
void KWSearchDia::slotFamily( const QString & family )
{
    searchEntry->family = family;
    view->setSearchEntry( searchEntry );

    QFont f = QFont( KGlobalSettings::generalFont() );
    f.setFamily( family );
    eSearch->setFont( f );
}

/*================================================================*/
void KWSearchDia::slotSize( const QString & size )
{
    searchEntry->size = size.toInt();
    view->setSearchEntry( searchEntry );
}

/*================================================================*/
void KWSearchDia::slotColor( const QColor& color )
{
    searchEntry->color = QColor( color );
    view->setSearchEntry( searchEntry );
}

/*================================================================*/
void KWSearchDia::slotBold()
{
    searchEntry->bold = cmBold->isChecked();
    view->setSearchEntry( searchEntry );
}

/*================================================================*/
void KWSearchDia::slotItalic()
{
    searchEntry->italic = cmItalic->isChecked();
    view->setSearchEntry( searchEntry );
}

/*================================================================*/
void KWSearchDia::slotUnderline()
{
    searchEntry->underline = cmUnderline->isChecked();
    view->setSearchEntry( searchEntry );
}

/*================================================================*/
void KWSearchDia::slotVertAlign( int num )
{
    switch ( num )
    {
    case 0:
        searchEntry->vertAlign = KWFormat::VA_NORMAL;
        break;
    case 1:
        searchEntry->vertAlign = KWFormat::VA_SUB;
        break;
    case 3:
        searchEntry->vertAlign = KWFormat::VA_SUPER;
        break;
    }
    view->setSearchEntry( searchEntry );
}

/*================================================================*/
void KWSearchDia::saveSettings()
{
    searchEntry->expr = eSearch->text();
    searchEntry->regexp = cRegExp->isChecked();
    searchEntry->caseSensitive = cCase->isChecked();
    searchEntry->wholeWords = cWholeWords->isChecked();
    searchEntry->reverse = cRev->isChecked();
    searchEntry->wildcard = cWildcard->isChecked();

    view->setSearchEntry( searchEntry );

    replaceEntry->expr = eReplace->text();
    replaceEntry->ask = cAsk->isChecked();

    view->setReplaceEntry( replaceEntry );
}

/*================================================================*/
void KWSearchDia::replaceFirst()
{
    QString expr = eSearch->text();
    if ( expr.isEmpty() ) return;

    bool addlen = false;
    bool replace = false;

    if ( !cRev->isChecked() )
        replace = page->find( expr, searchEntry, true, cCase->isChecked(), cWholeWords->isChecked(), cRegExp->isChecked(),
                              cWildcard->isChecked(), addlen, false );
    else
        replace = page->findRev( expr, searchEntry, true, cCase->isChecked(), cWholeWords->isChecked(), cRegExp->isChecked(),
                                 cWildcard->isChecked(), addlen, false );

    if ( replace )
        page->replace( eReplace->text(), replaceEntry, addlen );
}

/*================================================================*/
void KWSearchDia::replaceNext()
{
    QString expr = eSearch->text();
    if ( expr.isEmpty() ) return;

    bool addlen = false;
    bool replace = false;

    if ( !cRev->isChecked() )
        replace = page->find( expr, searchEntry, false, cCase->isChecked(), cWholeWords->isChecked(), cRegExp->isChecked(),
                              cWildcard->isChecked(), addlen, false );
    else
        replace = page->findRev( expr, searchEntry, false, cCase->isChecked(), cWholeWords->isChecked(), cRegExp->isChecked(),
                                 cWildcard->isChecked(), addlen, false );

    if ( replace )
        page->replace( eReplace->text(), replaceEntry, addlen );
}

/*================================================================*/
void KWSearchDia::replaceAll()
{
    QString expr = eSearch->text();
    if ( expr.isEmpty() ) return;

    bool first = true;
    bool addlen = false;
    bool replace = false;
    bool select = cAsk->isChecked();

    while ( true )
    {
        if ( !cRev->isChecked() )
            replace = page->find( expr, searchEntry, first, cCase->isChecked(), cWholeWords->isChecked(), cRegExp->isChecked(),
                                  cWildcard->isChecked(), addlen, select );
        else
            replace = page->findRev( expr, searchEntry, first, cCase->isChecked(), cWholeWords->isChecked(), cRegExp->isChecked(),
                                     cWildcard->isChecked(), addlen, select );
        first = false;

        if ( replace && cAsk->isChecked() )
        {
            int result = KMessageBox::warningYesNoCancel( this,
                                i18n( "Replace selected text?" ),
                                i18n( "Replace" ),
                                i18n( "&Replace" ), i18n( "&Skip" ) );

            if (result == KMessageBox::Cancel)
            {
               // Cancel
               break;
            }

            if (result == KMessageBox::No)
            {
                // Skip
                if ( addlen ) page->addLen();
                page->repaintScreen( false );
                continue;
            };

            // KMessageBox::Yes
            // Replace continues
        }

        if ( replace )
            page->replace( eReplace->text(), replaceEntry, addlen );
        else
            break;
    }

    page->repaintScreen( false );
}

/*================================================================*/
void KWSearchDia::rslotCheckFamily()
{
    replaceEntry->checkFamily = rcFamily->isChecked();

    if ( rcFamily->isChecked() )
    {
        rcmFamily->setEnabled( true );
        rslotFamily( rcmFamily->currentText() );
    }
    else
    {
        rcmFamily->setEnabled( false );
        eReplace->setFont( KGlobalSettings::generalFont() );
    }
}

/*================================================================*/
void KWSearchDia::rslotCheckColor()
{
    replaceEntry->checkColor = rcColor->isChecked();

    if ( rcColor->isChecked() )
    {
        rbColor->setEnabled( true );
        rslotColor( rbColor->color() );
    }
    else rbColor->setEnabled( false );
}

/*================================================================*/
void KWSearchDia::rslotCheckSize()
{
    replaceEntry->checkSize = rcSize->isChecked();

    if ( rcSize->isChecked() )
    {
        rcmSize->setEnabled( true );
        rslotSize( rcmSize->currentText() );
    }
    else rcmSize->setEnabled( false );
}

/*================================================================*/
void KWSearchDia::rslotCheckBold()
{
    replaceEntry->checkBold = rcBold->isChecked();

    if ( rcBold->isChecked() )
    {
        rcmBold->setEnabled( true );
        rslotBold();
    }
    else rcmBold->setEnabled( false );
}

/*================================================================*/
void KWSearchDia::rslotCheckItalic()
{
    replaceEntry->checkItalic = rcItalic->isChecked();

    if ( rcItalic->isChecked() )
    {
        rcmItalic->setEnabled( true );
        rslotItalic();
    }
    else rcmItalic->setEnabled( false );
}

/*================================================================*/
void KWSearchDia::rslotCheckUnderline()
{
    replaceEntry->checkUnderline = rcUnderline->isChecked();

    if ( rcUnderline->isChecked() )
    {
        rcmUnderline->setEnabled( true );
        rslotUnderline();
    }
    else rcmUnderline->setEnabled( false );
}

/*================================================================*/
void KWSearchDia::rslotCheckVertAlign()
{
    replaceEntry->checkVertAlign = rcVertAlign->isChecked();

    if ( rcVertAlign->isChecked() )
    {
        rcmVertAlign->setEnabled( true );
        rslotVertAlign( rcmVertAlign->currentItem() );
    }
    else rcmVertAlign->setEnabled( false );
}

/*================================================================*/
void KWSearchDia::rslotFamily( const QString & family )
{
    replaceEntry->family = family;
    view->setReplaceEntry( replaceEntry );

    QFont f = QFont( KGlobalSettings::generalFont() );
    f.setFamily( family );
    eReplace->setFont( f );
}

/*================================================================*/
void KWSearchDia::rslotSize( const QString & size )
{
    replaceEntry->size = size.toInt();
    view->setReplaceEntry( replaceEntry );
}

/*================================================================*/
void KWSearchDia::rslotColor( const QColor& color )
{
    replaceEntry->color = QColor( color );
    view->setReplaceEntry( replaceEntry );
}

/*================================================================*/
void KWSearchDia::rslotBold()
{
    replaceEntry->bold = rcmBold->isChecked();
    view->setReplaceEntry( replaceEntry );
}

/*================================================================*/
void KWSearchDia::rslotItalic()
{
    replaceEntry->italic = rcmItalic->isChecked();
    view->setReplaceEntry( replaceEntry );
}

/*================================================================*/
void KWSearchDia::rslotUnderline()
{
    replaceEntry->underline = rcmUnderline->isChecked();
    view->setReplaceEntry( replaceEntry );
}

/*================================================================*/
void KWSearchDia::rslotVertAlign( int num )
{
    switch ( num )
    {
    case 0:
        replaceEntry->vertAlign = KWFormat::VA_NORMAL;
        break;
    case 1:
        replaceEntry->vertAlign = KWFormat::VA_SUB;
        break;
    case 3:
        replaceEntry->vertAlign = KWFormat::VA_SUPER;
        break;
    }
    view->setReplaceEntry( replaceEntry );
}

/*================================================================*/
void KWSearchDia::slotRegExp()
{
    if ( cRegExp->isChecked() )
    {
        cWholeWords->setEnabled( false );
        cWildcard->setEnabled( true );
        cRev->setEnabled( false );
    }
    else
    {
        cWholeWords->setEnabled( true );
        cWildcard->setEnabled( false );
        cRev->setEnabled( true );
    }
}
