/****************************************************************************
**
** Copyright (C) 1992-1998 Troll Tech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

/****************************************************
 * modified by Reginald Stadlbauer <reggie@kde.org> *
 ****************************************************/

#include "sheet.h"
#include "sheetdlg.h"
#include "sheetdlg.moc"
#include <kapplication.h>
#include <klocale.h>
#include <kstdguiitem.h>
#include <kpushbutton.h>

#include <qlabel.h>
#include <qspinbox.h>

#include <qwhatsthis.h>
#include <qtooltip.h>

const int SheetDlg::TABLE_SIZE = 16;

SheetDlg::SheetDlg( QWidget *parent, const char *name )
    :QWidget( parent, name )
{
  t = new Sheet(this,"Sheet",TABLE_SIZE);
  t->move(0,0);
  
  
  QString rowwhatsthis = i18n("<p><b>Sets the number of rows in the data table."
      "</b><br><br>Each row represents one data set.</p>");
  QString colwhatsthis = i18n("<p><b>Sets the number of columns in the data table."
      "</b><br><br>The number of columns defines the number of data values in each data set (row).</p>");
  
  usedrowsLA = new QLabel( i18n("# Rows:" ), this );
  usedrowsLA->resize( usedrowsLA->sizeHint() );
  QWhatsThis::add(usedrowsLA,rowwhatsthis);
  usedrowsSB = new QSpinBox( this );
  usedrowsSB->resize( usedrowsSB->sizeHint() );
  QToolTip::add(usedrowsSB,i18n("Number of active data rows"));
  QWhatsThis::add(usedrowsSB,rowwhatsthis);
  
  usedcolsLA = new QLabel( i18n("# Cols:" ), this );
  usedcolsLA->resize( usedcolsLA->sizeHint() );
  QWhatsThis::add(usedcolsLA,colwhatsthis);
  usedcolsSB = new QSpinBox( this );
  usedcolsSB->resize( usedcolsSB->sizeHint() );
  QToolTip::add(usedcolsSB,i18n("Number of active data columns"));
  QWhatsThis::add(usedcolsSB,colwhatsthis);
  
  ok = new KPushButton(KStdGuiItem::ok(),this);
  ok->resize(ok->sizeHint());
  QToolTip::add(ok,i18n("Accept changes to data table and quit dialog"));
  
  cancel = new KPushButton(KStdGuiItem::cancel(),this);
  cancel->resize(cancel->sizeHint());
  QToolTip::add(cancel,i18n("Discard changes to data table and quit dialog"));
  
  connect(ok,SIGNAL(clicked()),parent,SLOT(accept()));
  connect(ok,SIGNAL(clicked()),t,SLOT(ok()));
  connect(cancel,SIGNAL(clicked()),parent,SLOT(reject()));
  ok->setDefault(true);

  resizeHandle( size() );
}

void SheetDlg::fillCell(int row,int col,double value)
{
  QString _value;
  _value.sprintf("%g",value);
  t->importText(row,col,_value);

  if( col+1 > usedCols() )
      usedcolsSB->setValue( col+1 );
  if( row+1 > usedRows() )
      usedrowsSB->setValue( row+1 );
}

void SheetDlg::fillX(int col,QString str)
{
  t->importTextHead(0,col,str);

  if( col+1 > usedCols() )
      usedcolsSB->setValue( col+1 );
}

void SheetDlg::fillY( int row, QString str )
{
  t->importTextSide( row, 0, str );

  if( row+1 > usedRows() )
      usedrowsSB->setValue( row+1 );
}

int SheetDlg::cols()
{
  return t->cols();
}

int SheetDlg::rows()
{
  return t->rows();
}

int SheetDlg::usedCols()
{
    return usedcolsSB->value();
}


int SheetDlg::usedRows()
{
    return usedrowsSB->value();
}


void SheetDlg::setUsedCols( int val )
{
  usedcolsSB->setValue( val );
}


void SheetDlg::setUsedRows( int val )
{
  usedrowsSB->setValue( val );
}


QString SheetDlg::getX(int col)
{
  return t->getX(col);
}

QString SheetDlg::getY(int row)
{
  return t->getY(row);
}

double SheetDlg::getCell(int row,int col)
{
  return t->getCell(row,col);
}

void SheetDlg::resizeEvent( QResizeEvent * e )
{
  resizeHandle( e->size() );
}

void SheetDlg::resizeHandle( QSize s )
{
  t->resize(s.width(), s.height() - (cancel->height()+10));

  int height = s.height() - cancel->height() - 5;

  usedrowsLA->move( 0, height );
  usedrowsSB->move( usedrowsLA->width() + usedrowsLA->x() + 5,
		    height );
  usedcolsLA->move( usedrowsSB->width() + usedrowsSB->x() + 5,
		    height );
  usedcolsSB->move( usedcolsLA->width() + usedcolsLA->x() + 5,
		    height );

  cancel->move(s.width() - 10 - cancel->width(), height );
  ok->move(cancel->x() - 5 - ok->width(), height );
}




