/****************************************************************************
** $Id$
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
#include <kapp.h>

SheetDlg::SheetDlg( QDialog *parent, const char *name )
    :QWidget( (QWidget*)parent, name )
{
  t = new Sheet(this,"Sheet",TABLE_SIZE); 
  t->move(0,0);
  
  cancel = new QPushButton(i18n("Cancel"),this);
  cancel->resize(cancel->sizeHint());

  ok = new QPushButton(i18n("OK"),this);
  ok->resize(cancel->sizeHint());
  
  connect(ok,SIGNAL(clicked()),parent,SLOT(accept()));
  connect(ok,SIGNAL(clicked()),t,SLOT(ok()));
  connect(cancel,SIGNAL(clicked()),parent,SLOT(reject()));

  resizeHandle( size() );
}

void SheetDlg::fillCell(int row,int col,double value)
{
  QString _value;
  _value.sprintf("%g",value);
  t->importText(row,col,_value);
}

void SheetDlg::fillX(int col,QString str)
{
  t->importTextHead(0,col,str);
}

int SheetDlg::cols()
{
  return t->cols();
}

int SheetDlg::rows()
{
  return t->rows();
}

QString SheetDlg::getX(int col)
{
  return t->getX(col);
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

  cancel->move(s.width() - 10 - cancel->width(),s.height() - cancel->height() - 5);
  ok->move(cancel->x() - 5 - ok->width(),s.height() - cancel->height() - 5);
}




