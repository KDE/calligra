/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander <zander@kde.org>

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

#include "splitcellsdia.h"
#include "splitcellsdia.moc"
#include "tabledia.h"

#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qspinbox.h>

#include <kapp.h>
#include <klocale.h>

KWSplitCellDia::KWSplitCellDia( QWidget* parent, const char* name, int numrows, int numcols)
    : KDialogBase( Plain, i18n("Split cell"), Ok | Cancel, Ok, parent, name, true)
{
    m_cols= numcols;
    m_rows= numrows;

    setInitialSize( QSize(400, 300) );

    QWidget *page = plainPage();
    QGridLayout *grid = new QGridLayout( page, 4, 2, 15, 7 );

    QLabel *lRows = new QLabel( i18n( "Number of Rows:" ), page );
    grid->addWidget( lRows, 0, 0 );

    nRows = new QSpinBox( 1, 128, 1, page );
    nRows->setValue( m_rows );
    grid->addWidget( nRows, 1, 0 );

    QLabel *lCols = new QLabel( i18n( "Number of Columns:" ), page );
    grid->addWidget( lCols, 2, 0 );

    nCols = new QSpinBox( 1, 128, 1, page );
    nCols->setValue( m_cols );
    grid->addWidget( nCols, 3, 0 );

    preview = new KWTablePreview( page, m_rows, m_cols );
    preview->setBackgroundColor( white );
    grid->addMultiCellWidget( preview, 0, 4, 1, 1 );

    grid->addRowSpacing( 0, lRows->height() );
    grid->addRowSpacing( 1, nRows->height() );
    grid->addRowSpacing( 2, lCols->height() );
    grid->addRowSpacing( 3, nCols->height() );
    grid->addRowSpacing( 4, 150 - ( lRows->height() + nRows->height() + lCols->height() + nCols->height() ) );
    grid->setRowStretch( 0, 0 );
    grid->setRowStretch( 1, 0 );
    grid->setRowStretch( 2, 0 );
    grid->setRowStretch( 3, 0 );
    grid->setRowStretch( 4, 1 );

    grid->addColSpacing( 0, lRows->width() );
    grid->addColSpacing( 0, nRows->width() );
    grid->addColSpacing( 0, lCols->width() );
    grid->addColSpacing( 0, nCols->width() );
    grid->addColSpacing( 1, 150 );
    grid->setColStretch( 0, 0 );
    grid->setColStretch( 1, 1 );

    grid->activate();

    connect( nRows, SIGNAL( valueChanged( int ) ), this, SLOT( rowsChanged( int ) ) );
    connect( nCols, SIGNAL( valueChanged( int ) ), this, SLOT( colsChanged( int ) ) );
}

void KWSplitCellDia::rowsChanged( int rows ) {
    m_rows=rows;
    preview->setRows( m_rows );
}

void KWSplitCellDia::colsChanged( int cols ) {
    m_cols=cols;
    preview->setCols( m_cols );
}

