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
#include "kwcanvas.h"
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
/* Class: KWTableDia                                              */
/******************************************************************/

/*================================================================*/
KWTableDia::KWTableDia( QWidget* parent, const char* name, KWCanvas *_canvas, KWDocument *_doc,
			int rows, int cols, KWTblCellSize wid, KWTblCellSize hei )
    : KDialogBase( Tabbed, i18n("Table settings"), Ok | Cancel, Ok, parent, name, true)
{
    canvas = _canvas;
    doc = _doc;

    setupTab1( rows, cols, wid, hei );
    //setupTab2();

    setInitialSize( QSize(500, 400) );
}

/*================================================================*/
void KWTableDia::setupTab1( int rows, int cols, KWTblCellSize wid, KWTblCellSize hei )
{
    tab1 = addPage( i18n( "Geometry" ) );

    QGridLayout *grid = new QGridLayout( tab1, 9, 2, 15, 7 );

    lRows = new QLabel( i18n( "Number of Rows:" ), tab1 );
    grid->addWidget( lRows, 0, 0 );

    nRows = new QSpinBox( 1, 128, 1, tab1 );
    nRows->setValue( rows );
    grid->addWidget( nRows, 1, 0 );

    lCols = new QLabel( i18n( "Number of Columns:" ), tab1 );
    grid->addWidget( lCols, 2, 0 );

    nCols = new QSpinBox( 1, 128, 1, tab1 );
    nCols->setValue( cols );
    grid->addWidget( nCols, 3, 0 );

    lHei = new QLabel( i18n( "Cell Heights:" ), tab1 );
    grid->addWidget( lHei, 4, 0 );

    cHei = new QComboBox( FALSE, tab1 );
    cHei->insertItem( i18n( "Automatic" ) );
    cHei->insertItem( i18n( "Manual" ) );
    cHei->setCurrentItem( (int)hei );
    grid->addWidget( cHei, 5, 0 );

    lWid = new QLabel( i18n( "Cell Widths:" ), tab1 );
    grid->addWidget( lWid, 6, 0 );

    cWid = new QComboBox( FALSE, tab1 );
    cWid->insertItem( i18n( "Automatic" ) );
    cWid->insertItem( i18n( "Manual" ) );
    cWid->setCurrentItem( (int)wid );
    grid->addWidget( cWid, 7, 0 );

    preview = new KWTablePreview( tab1, rows, cols );
    preview->setBackgroundColor( white );
    grid->addMultiCellWidget( preview, 0, 8, 1, 1 );

    // Checkbox for floating/fixed location. The default is [will be] floating.
    cbIsFloating = new QCheckBox( i18n( "The table is floating" ), tab1 );
    cbIsFloating->setEnabled(false);
    //cbIsFloating->setChecked( true );

    grid->addMultiCellWidget( cbIsFloating, 9, 9, 0, 2 );

    grid->addRowSpacing( 0, lRows->height() );
    grid->addRowSpacing( 1, nRows->height() );
    grid->addRowSpacing( 2, lCols->height() );
    grid->addRowSpacing( 3, nCols->height() );
    grid->addRowSpacing( 4, lHei->height() );
    grid->addRowSpacing( 5, cHei->height() );
    grid->addRowSpacing( 6, lWid->height() );
    grid->addRowSpacing( 7, cWid->height() );
    grid->addRowSpacing( 8, 150 - ( lRows->height() + nRows->height() + lCols->height() + nCols->height() ) );
    grid->addRowSpacing( 9, cbIsFloating->height() );
    grid->setRowStretch( 0, 0 );
    grid->setRowStretch( 1, 0 );
    grid->setRowStretch( 2, 0 );
    grid->setRowStretch( 3, 0 );
    grid->setRowStretch( 4, 0 );
    grid->setRowStretch( 5, 0 );
    grid->setRowStretch( 6, 0 );
    grid->setRowStretch( 7, 0 );
    grid->setRowStretch( 8, 1 );
    grid->setRowStretch( 9, 0 );

    grid->addColSpacing( 0, lRows->width() );
    grid->addColSpacing( 0, nRows->width() );
    grid->addColSpacing( 0, lCols->width() );
    grid->addColSpacing( 0, nCols->width() );
    grid->addColSpacing( 0, lHei->width() );
    grid->addColSpacing( 0, cHei->width() );
    grid->addColSpacing( 0, lWid->width() );
    grid->addColSpacing( 0, cWid->width() );
    grid->addColSpacing( 1, 150 );
    grid->setColStretch( 0, 0 );
    grid->setColStretch( 1, 1 );

    grid->activate();

    connect( nRows, SIGNAL( valueChanged( int ) ), this, SLOT( rowsChanged( int ) ) );
    connect( nCols, SIGNAL( valueChanged( int ) ), this, SLOT( colsChanged( int ) ) );
}

/*================================================================*/
void KWTableDia::setupTab2()
{
    readTableStyles();

    tab2 = addPage( i18n("Properties"));
    QGridLayout *grid = new QGridLayout( tab2, 4, 3, 15, 7 );

    lStyles = new QLabel( i18n( "Styles" ), tab2 );
    grid->addWidget( lStyles, 0, 0 );

    lbStyles = new QListBox( tab2 );
    grid->addWidget( lbStyles, 1, 0 );

    preview2 = new QWidget( tab2 );
    preview2->setBackgroundColor( white );
    grid->addMultiCellWidget( preview2, 1, 1, 1, 2 );

    bgHeader = new QButtonGroup( i18n( "Apply for Header" ), tab2 );
    grid->addWidget( bgHeader, 2, 0 );

    bgFirstCol = new QButtonGroup( i18n( "Apply for First Column" ), tab2 );
    grid->addWidget( bgFirstCol, 2, 1 );

    bgBody = new QButtonGroup( i18n( "Apply for Body" ), tab2 );
    grid->addWidget( bgBody, 2, 2 );

    cbHeaderOnAllPages = new QCheckBox( i18n( "When a table flows over multiple pages, "
					      "copy header to each page begin" ), tab2 );
    grid->addMultiCellWidget( cbHeaderOnAllPages, 3, 3, 0, 2 );

    grid->addRowSpacing( 0, lStyles->height() );
    grid->addRowSpacing( 1, lbStyles->height() );
    grid->addRowSpacing( 1, 200 );
    grid->addRowSpacing( 2, bgHeader->height() );
    grid->addRowSpacing( 2, bgFirstCol->height() );
    grid->addRowSpacing( 2, bgBody->height() );
    grid->addRowSpacing( 3, cbHeaderOnAllPages->height() );
    grid->setRowStretch( 0, 0 );
    grid->setRowStretch( 1, 1 );
    grid->setRowStretch( 2, 0 );
    grid->setRowStretch( 3, 0 );

    grid->addColSpacing( 0, lStyles->width() );
    grid->addColSpacing( 0, lbStyles->width() );
    grid->addColSpacing( 0, bgHeader->width() );
    grid->addColSpacing( 1, 100 );
    grid->addColSpacing( 1, bgFirstCol->width() );
    grid->addColSpacing( 2, 100 );
    grid->addColSpacing( 2, bgBody->width() );
    grid->setColStretch( 0, 1 );
    grid->setColStretch( 1, 1 );
    grid->setColStretch( 2, 1 );

    grid->activate();
}

/*================================================================*/
void KWTableDia::readTableStyles()
{
#ifdef __GNUC__
#warning TODO
#endif
}

void KWTableDia::slotOk()
{
    canvas->createTable( nRows->value(), nCols->value(),
                         (KWTblCellSize)cWid->currentItem(),
                         (KWTblCellSize)cHei->currentItem(),
                         cbIsFloating->isChecked() );
    KDialogBase::slotOk();
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
