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
#include "tabledia.h"
#include "tabledia.moc"

#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qstring.h>
#include <qevent.h>
#include <qspinbox.h>
#include <qpen.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qlistbox.h>
#include <qcombobox.h>

#include <kapp.h>
#include <kcolorbtn.h>
#include <kbuttonbox.h>
#include <klocale.h>

#include <stdlib.h>


/******************************************************************/
/* Class: KWTablePreview                                          */
/******************************************************************/

/*================================================================*/
void KWTablePreview::paintEvent( QPaintEvent * )
{
    int wid = ( width() - 10 ) / cols;
    int hei = ( height() - 10 ) / rows;

    QPainter p;
    p.begin( this );

    p.setPen( QPen( black ) );

    for ( int i = 0; i < rows; i++ )
    {
        for ( int j = 0; j < cols; j++ )
            p.drawRect( j * wid + 5, i * hei + 5, wid + 1, hei + 1 );
    }

    p.end();
}

/******************************************************************/
/* Class: KWTableConf                                             */
/******************************************************************/

/*================================================================*/
KWTableConf::KWTableConf( QWidget *_parent, KWordDocument *_doc )
    : QWidget( _parent )
{
    doc = _doc;

    readTableStyles();
    setupPage();
}

/*================================================================*/
void KWTableConf::readTableStyles()
{
}

/*================================================================*/
void KWTableConf::setupPage()
{
    grid1 = new QGridLayout( this, 4, 3, 15, 7 );

    lStyles = new QLabel( i18n( "Styles" ), this );
    lStyles->resize( lStyles->sizeHint() );
    grid1->addWidget( lStyles, 0, 0 );

    lbStyles = new QListBox( this );
    lbStyles->resize( lbStyles->sizeHint() );
    grid1->addWidget( lbStyles, 1, 0 );

    preview = new QWidget( this );
    preview->setBackgroundColor( white );
    grid1->addMultiCellWidget( preview, 1, 1, 1, 2 );

    bgHeader = new QButtonGroup( i18n( "Apply for Header" ), this );
    grid1->addWidget( bgHeader, 2, 0 );

    bgFirstCol = new QButtonGroup( i18n( "Apply for First Column" ), this );
    grid1->addWidget( bgFirstCol, 2, 1 );

    bgBody = new QButtonGroup( i18n( "Apply for Body" ), this );
    grid1->addWidget( bgBody, 2, 2 );

    cbHeaderOnAllPages = new QCheckBox( i18n( "When a table flows over multiple pages, "
					      "copy header to each page begin" ), this );
    cbHeaderOnAllPages->resize( cbHeaderOnAllPages->sizeHint() );
    grid1->addMultiCellWidget( cbHeaderOnAllPages, 3, 3, 0, 2 );

    grid1->addRowSpacing( 0, lStyles->height() );
    grid1->addRowSpacing( 1, lbStyles->height() );
    grid1->addRowSpacing( 1, 200 );
    grid1->addRowSpacing( 2, bgHeader->height() );
    grid1->addRowSpacing( 2, bgFirstCol->height() );
    grid1->addRowSpacing( 2, bgBody->height() );
    grid1->addRowSpacing( 3, cbHeaderOnAllPages->height() );
    grid1->setRowStretch( 0, 0 );
    grid1->setRowStretch( 1, 1 );
    grid1->setRowStretch( 2, 0 );
    grid1->setRowStretch( 3, 0 );

    grid1->addColSpacing( 0, lStyles->width() );
    grid1->addColSpacing( 0, lbStyles->width() );
    grid1->addColSpacing( 0, bgHeader->width() );
    grid1->addColSpacing( 1, 100 );
    grid1->addColSpacing( 1, bgFirstCol->width() );
    grid1->addColSpacing( 2, 100 );
    grid1->addColSpacing( 2, bgBody->width() );
    grid1->setColStretch( 0, 1 );
    grid1->setColStretch( 1, 1 );
    grid1->setColStretch( 2, 1 );

    grid1->activate();
}

/******************************************************************/
/* Class: KWTableDia                                              */
/******************************************************************/

/*================================================================*/
KWTableDia::KWTableDia( QWidget* parent, const char* name, KWPage *_page, KWordDocument *_doc,
			int rows, int cols, KWTblCellSize wid, KWTblCellSize hei )
    : QTabDialog( parent, name, true )
{
    page = _page;
    doc = _doc;

    setupTab1( rows, cols, wid, hei );
    setupTab2();

    setCancelButton( i18n( "Cancel" ) );
    setOkButton( i18n( "OK" ) );

    resize( 500, 400 );
}

/*================================================================*/
void KWTableDia::setupTab1( int rows, int cols, KWTblCellSize wid, KWTblCellSize hei )
{
    tab1 = new QWidget( this );

    grid1 = new QGridLayout( tab1, 9, 2, 15, 7 );

    lRows = new QLabel( i18n( "Number of Rows:" ), tab1 );
    lRows->resize( lRows->sizeHint() );
    grid1->addWidget( lRows, 0, 0 );

    nRows = new QSpinBox( 1, 128, 1, tab1 );
    nRows->resize( nRows->sizeHint() );
    nRows->setValue( rows );
    grid1->addWidget( nRows, 1, 0 );

    lCols = new QLabel( i18n( "Number of Columns:" ), tab1 );
    lCols->resize( lCols->sizeHint() );
    grid1->addWidget( lCols, 2, 0 );

    nCols = new QSpinBox( 1, 128, 1, tab1 );
    nCols->resize( nCols->sizeHint() );
    nCols->setValue( cols );
    grid1->addWidget( nCols, 3, 0 );

    lHei = new QLabel( i18n( "Cell Heights:" ), tab1 );
    lHei->resize( lHei->sizeHint() );
    grid1->addWidget( lHei, 4, 0 );

    cHei = new QComboBox( FALSE, tab1 );
    cHei->resize( cHei->sizeHint() );
    cHei->insertItem( i18n( "Automatic" ) );
    cHei->insertItem( i18n( "Manual" ) );
    cHei->setCurrentItem( (int)hei );
    grid1->addWidget( cHei, 5, 0 );

    lWid = new QLabel( i18n( "Cell Widths:" ), tab1 );
    lWid->resize( lWid->sizeHint() );
    grid1->addWidget( lWid, 6, 0 );

    cWid = new QComboBox( FALSE, tab1 );
    cWid->resize( cWid->sizeHint() );
    cWid->insertItem( i18n( "Automatic" ) );
    cWid->insertItem( i18n( "Manual" ) );
    cWid->setCurrentItem( (int)wid );
    grid1->addWidget( cWid, 7, 0 );

    preview = new KWTablePreview( tab1, rows, cols );
    preview->setBackgroundColor( white );
    grid1->addMultiCellWidget( preview, 0, 8, 1, 1 );

    grid1->addRowSpacing( 0, lRows->height() );
    grid1->addRowSpacing( 1, nRows->height() );
    grid1->addRowSpacing( 2, lCols->height() );
    grid1->addRowSpacing( 3, nCols->height() );
    grid1->addRowSpacing( 4, lHei->height() );
    grid1->addRowSpacing( 5, cHei->height() );
    grid1->addRowSpacing( 6, lWid->height() );
    grid1->addRowSpacing( 7, cWid->height() );
    grid1->addRowSpacing( 8, 150 - ( lRows->height() + nRows->height() + lCols->height() + nCols->height() ) );
    grid1->setRowStretch( 0, 0 );
    grid1->setRowStretch( 1, 0 );
    grid1->setRowStretch( 2, 0 );
    grid1->setRowStretch( 3, 0 );
    grid1->setRowStretch( 4, 0 );
    grid1->setRowStretch( 5, 0 );
    grid1->setRowStretch( 6, 0 );
    grid1->setRowStretch( 7, 0 );
    grid1->setRowStretch( 8, 1 );

    grid1->addColSpacing( 0, lRows->width() );
    grid1->addColSpacing( 0, nRows->width() );
    grid1->addColSpacing( 0, lCols->width() );
    grid1->addColSpacing( 0, nCols->width() );
    grid1->addColSpacing( 0, lHei->width() );
    grid1->addColSpacing( 0, cHei->width() );
    grid1->addColSpacing( 0, lWid->width() );
    grid1->addColSpacing( 0, cWid->width() );
    grid1->addColSpacing( 1, 150 );
    grid1->setColStretch( 0, 0 );
    grid1->setColStretch( 1, 1 );

    grid1->activate();

    addTab( tab1, i18n( "Geometry" ) );

    connect( this, SIGNAL( applyButtonPressed() ), this, SLOT( insertTable() ) );
    connect( nRows, SIGNAL( valueChanged( int ) ), this, SLOT( rowsChanged( int ) ) );
    connect( nCols, SIGNAL( valueChanged( int ) ), this, SLOT( colsChanged( int ) ) );
}

/*================================================================*/
void KWTableDia::setupTab2()
{
    tab2 = new KWTableConf( this, doc );
    addTab( tab2, i18n( "Properties" ) );
}

/*================================================================*/
void KWTableDia::insertTable()
{
    page->setTableConfig( nRows->value(), nCols->value(),
			  (KWTblCellSize)cWid->currentItem(),
			  (KWTblCellSize)cHei->currentItem() );
    page->mmTable();
}

/*================================================================*/
void KWTableDia::rowsChanged( int _rows )
{
    preview->setRows( _rows );
}

/*================================================================*/
void KWTableDia::colsChanged( int _cols )
{
    preview->setCols( _cols );
}
