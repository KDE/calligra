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

#include <stdlib.h>

#include <qscrbar.h>

#include "sheet.h"
#include "sheet.moc"

Sheet::Sheet( QWidget *parent, const char *name, int _tableSize )
    :QWidget(parent,name)
{
    tableSize = _tableSize;

    tableView = new SheetTable(tableSize,tableSize,this,Tbl_clipCellPainting );
    table = new ParsedArray(tableSize,tableSize);

    _head = new ParsedArray(1,tableSize);
    _side = new ParsedArray(tableSize,1);
    
    head = new SheetTable(tableSize,1,this,Tbl_clipCellPainting);
    side = new SheetTable(1,tableSize,this,Tbl_clipCellPainting,"Side");
    
    extraH = head->tHeight() + 2;
    extraW = side->tWidth() + 2;
    
    head->move(extraW,0);
    side->move(0,extraH);
    tableView->move(extraW,extraH);
    
    connect( tableView, SIGNAL(selected(int,int)),
 	     this, SLOT(exportText(int,int)) );
    connect( tableView, SIGNAL(newText(int,int,QString)),
 	     this, SLOT(importText(int,int,QString)) );
    
    connect( head, SIGNAL(selected(int,int)),
 	     this, SLOT(exportTextHead(int,int)) );
    connect( head, SIGNAL(newText(int,int,QString)),
 	     this, SLOT(importTextHead(int,int,QString)) );
    
    connect( side, SIGNAL(selected(int,int)),
 	     this, SLOT(exportTextSide(int,int)) );
    connect( side, SIGNAL(newText(int,int,QString)),
 	     this, SLOT(importTextSide(int,int,QString)) );
    
    horz = new QScrollBar( QScrollBar::Horizontal,this,"scrollBar" );
    horz->resize( tableView->width(), 16 );
    horz->setRange( 0, tableView->numCols() - tableView->numColsVisible() );
    horz->setSteps( 1, tableView->numColsVisible() );
    
    connect( tableView, SIGNAL(newCol(int)), head, SLOT(scrollHorz(int)));
    connect( tableView, SIGNAL(newCol(int)), this, SLOT(setHorzBar(int)) );
    connect( head, SIGNAL(newCol(int)), this, SLOT(setHorzBar(int)) );
    connect( horz, SIGNAL(valueChanged(int)), 
    	     tableView, SLOT(scrollHorz(int)));
    connect( horz, SIGNAL(valueChanged(int)), 
	     head, SLOT(scrollHorz(int)));
    extraH += horz->height();
    
    vert = new QScrollBar( QScrollBar::Vertical,this,"scrollBar" );
    vert->resize( 16, tableView->width() );
    vert->setRange( 0, tableView->numRows() - tableView->numRowsVisible() );
    vert->setSteps( 1, tableView->numRowsVisible() );
    
    connect( tableView, SIGNAL(newRow(int)), side, SLOT(scrollVert(int)) );
    connect( tableView, SIGNAL(newRow(int)), this, SLOT(setVertBar(int)) );
    connect( side, SIGNAL(newRow(int)), this, SLOT(setVertBar(int)) );
    connect( vert, SIGNAL(valueChanged(int)), 
    	     tableView, SLOT(scrollVert(int)));
    connect( vert, SIGNAL(valueChanged(int)), 
    	     side, SLOT(scrollVert(int)));
    extraW += vert->width();

    QString str;
    for (int i = 0;i < tableSize;i++)
      {
	str.sprintf("%d",i+1);
	importTextSide(i,0,str);
      }

}

Sheet::~Sheet()
{
  delete tableView;
  delete side;
  delete head;
  delete table;
}

int Sheet::rows()
{
  int row,col,rows = 0;
  bool colValid;

  for (row = 0;row < tableSize;row++)
    {
      colValid = false;
      for (col = 0;col < tableSize;col++)
	{
	  if (tableView->hasValue(row,col))
	    colValid = true;
	}
      if (!colValid) return rows;
      else rows++;
    }
  return rows;
}

int Sheet::cols()
{
  int row,col,cols = 0;
  bool rowValid;

  for (col = 0;col < tableSize;col++)
    {
      rowValid = false;
      for (row = 0;row < tableSize;row++)
	{
	  if (tableView->hasValue(row,col))
	    rowValid = true;
	}
      if (!rowValid) return cols;
      else cols++;
    }
  return cols;
} 

QString Sheet::getX(int col)
{
  return _head->rawText( 0, col );
}

QString Sheet::getY(int row)
{
  return _side->rawText( row, 0 );
}

double Sheet::getCell(int row,int col)
{
  if (table->rawText(row,col).isEmpty())
    return 0.0;
  else
    return atof(table->rawText(row,col));
}

void Sheet::setHorzBar(int val)
{
    horz->setValue(val);
}

void Sheet::setVertBar(int val)
{
    vert->setValue(val);
}

void Sheet::importText( int row, int col, QString s )
{
    table->setText( row, col, s.copy() );
    tableView->showText( row, col, table->calc( row, col ) );
}
void Sheet::exportText( int row, int col )
{
    tableView->setInputText( table->rawText(row,col) );
}

void Sheet::importTextHead( int row, int col, QString s )
{
    _head->setText( row, col, s.copy() );
    head->showText( row, col, _head->calc( row, col ) );
}
void Sheet::exportTextHead( int row, int col )
{
    head->setInputText( _head->rawText(row,col) );
}

void Sheet::importTextSide( int row, int col, QString s )
{
    _side->setText( row, col, s.copy() );
    side->showText( row, col, _side->calc( row, col ) );
}
void Sheet::exportTextSide( int row, int col )
{
    side->setInputText( _side->rawText(row,col) );
}

void Sheet::resizeEvent( QResizeEvent * e )
{
    int w = e->size().width() - extraW;
    int h = e->size().height() - extraH;
    int c = w / tableView->cellWidth(); //### TODO: variable width
    int r = h / tableView->cellHeight(); 

    h = r * tableView->cellHeight(); 
    w = c * tableView->cellWidth();
    //side->setNumRows( r );
    side->resize( side->tWidth(), h );
    //head->setNumCols( c );
    head->resize( w, head->height() );

    tableView->resize( w + tableView->extraW, h + tableView->extraH );
    QRect cr = tableView->geometry();
    horz->setGeometry( cr.left(), cr.bottom() + 1,
		       cr.width(), horz->height() );
    horz->setRange( 0, tableView->numCols() - tableView->numColsVisible() );
    horz->setSteps( 1, tableView->numColsVisible() );

    vert->setGeometry( cr.right() + 1, cr.top(),
		       vert->width(), cr.height() );
    vert->setRange( 0, tableView->numRows() - tableView->numRowsVisible() );
    vert->setSteps( 1, tableView->numRowsVisible() );

}

