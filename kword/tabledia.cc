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

#include "kwcanvas.h"
#include "tabledia.h"
#include "tabledia.moc"

#include "kwtabletemplateselector.h"

#include <qlabel.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qcombobox.h>

#include <klocale.h>

#include <stdlib.h>


/******************************************************************/
/* Class: KWTablePreview                                          */
/******************************************************************/

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

KWTableDia::KWTableDia( QWidget* parent, const char* name, KWCanvas *_canvas, KWDocument *_doc,
			int rows, int cols, CellSize wid, CellSize hei, bool floating )
    : KDialogBase( Tabbed, i18n("Table Settings"), Ok | Cancel, Ok, parent, name, true)
{
    canvas = _canvas;
    doc = _doc;

    setupTab1( rows, cols, wid, hei, floating );
    setupTab2();

    setInitialSize( QSize(500, 450) );
}

void KWTableDia::setupTab1( int rows, int cols, CellSize wid, CellSize hei, bool floating )
{
    tab1 = addPage( i18n( "Geometry" ) );

    QGridLayout *grid = new QGridLayout( tab1, 9, 2, KDialog::marginHint(), KDialog::spacingHint() );

    lRows = new QLabel( i18n( "Number of rows:" ), tab1 );
    grid->addWidget( lRows, 0, 0 );

    nRows = new QSpinBox( 1, 128, 1, tab1 );
    nRows->setValue( rows );
    grid->addWidget( nRows, 1, 0 );

    lCols = new QLabel( i18n( "Number of columns:" ), tab1 );
    grid->addWidget( lCols, 2, 0 );

    nCols = new QSpinBox( 1, 128, 1, tab1 );
    nCols->setValue( cols );
    grid->addWidget( nCols, 3, 0 );

    lHei = new QLabel( i18n( "Cell heights:" ), tab1 );
    grid->addWidget( lHei, 4, 0 );

    cHei = new QComboBox( FALSE, tab1 );
    cHei->insertItem( i18n( "Automatic" ) );
    cHei->insertItem( i18n( "Manual" ) );
    cHei->setCurrentItem( (int)hei );
    grid->addWidget( cHei, 5, 0 );

    lWid = new QLabel( i18n( "Cell widths:" ), tab1 );
    grid->addWidget( lWid, 6, 0 );

    cWid = new QComboBox( FALSE, tab1 );
    cWid->insertItem( i18n( "Automatic" ) );
    cWid->insertItem( i18n( "Manual" ) );
    cWid->setCurrentItem( (int)wid );
    grid->addWidget( cWid, 7, 0 );

    preview = new KWTablePreview( tab1, rows, cols );
    preview->setBackgroundColor( white );
    grid->addMultiCellWidget( preview, 0, 8, 1, 1 );

    // Checkbox for floating/fixed location. The default is floating (aka inline).
    cbIsFloating = new QCheckBox( i18n( "The table is &inline" ), tab1 );
    //cbIsFloating->setEnabled(false);
    cbIsFloating->setChecked( floating );

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

void KWTableDia::setupTab2()
{
    QWidget *tab2 = addPage( i18n("Templates"));

    QGridLayout *grid = new QGridLayout( tab2, 1, 1, KDialog::marginHint(), KDialog::spacingHint() );

    tableTemplateSelector = new KWTableTemplateSelector( doc, tab2 );
    grid->addWidget(tableTemplateSelector, 0, 0);

    grid->activate();
}

void KWTableDia::slotOk()
{
    canvas->createTable( nRows->value(), nCols->value(),
                         cWid->currentItem(),
                         cHei->currentItem(),
                         cbIsFloating->isChecked(),
                         tableTemplateSelector->getTableTemplate() );
    KDialogBase::slotOk();
}

void KWTableDia::rowsChanged( int _rows )
{
    preview->setRows( _rows );
}

void KWTableDia::colsChanged( int _cols )
{
    preview->setCols( _cols );
}
