/****************************************************************************
** Copyright (C) 1992-1998 Troll Tech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

/****************************************************
 * modified by Reginald Stadlbauer <reggie@kde.org> *
 ****************************************************/


#ifndef SHEET_H
#define SHEET_H


#include "table.h"
#include "parser.h"

namespace KChart
{

class Sheet : public QWidget
{
  Q_OBJECT

public:
  Sheet( QWidget *parent=0, const char *name=0, int tableSize = 16);
  ~Sheet();

  int      tWidth()    { return tableView->tWidth()  + extraW; }
  int      tHeight()   { return tableView->tHeight() + extraH; }

  int      cols();
  int      rows();

  QString  getX(int);
  QString  getY(int);
  double   getCell(int,int);

public slots:
  void     exportText( int row, int col );
  void     importText( int row, int col, QString );
  void     exportTextHead( int row, int col );
  void     importTextHead( int row, int col, QString );
  void     exportTextSide( int row, int col );
  void     importTextSide( int row, int col, QString );
  void     setHorzBar(int);
  void     setVertBar(int);
  void     ok() {tableView->nextInput(); head->nextInput(); side->nextInput();}

protected:
  virtual void  resizeEvent( QResizeEvent *);
  
private:
  SheetTable   *tableView;
  SheetTable   *head;
  SheetTable   *side;

  QScrollBar   *horz;
  QScrollBar   *vert;
  
  int           extraW;
  int           extraH;
  ParsedArray  *table;
  ParsedArray  *_head;
  ParsedArray  *_side;

  int           tableSize;
};

}  //namespace KChart

#endif
