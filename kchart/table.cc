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

#include <qpainter.h>
#include <qdrawutil.h>
#include <qvaluelist.h>

#include "table.h"
#include "table.moc"

#include <kapplication.h>

namespace KChart
{

SheetTable::SheetTable( int cols, int rows, QWidget *parent,
                          int flags, const char *name, bool _editable)
    :QtTableView(parent,name)
{
    editable = _editable;
    if ( flags < 0 )
        setTableFlags( Tbl_clipCellPainting| Tbl_vScrollBar | Tbl_hScrollBar |
                       Tbl_snapToGrid| Tbl_cutCells );
    else
        setTableFlags( flags );

    setNumRows(rows);
    setNumCols(cols);

    table = QStringList();
    //table.setAutoDelete(true);
    for (int i = 0;i < rows*cols;i++)
      table.append(0);

    setCellWidth(100);
    setCellHeight(30);

    extraW = width() - viewWidth();
    extraH = height() - viewHeight();

    if (editable)
      {
        input = new QLineEdit(this);
        input->setFrame(false);
        input->resize( cellWidth()-2, cellHeight()-2 );
	inCol_inRow_initialization = TRUE;
	moveInput(0,0);
        input->setFocus();
        connect( input, SIGNAL(returnPressed()), this, SLOT(nextInput()) );
      }

    setBackgroundColor(colorGroup().base());
}


SheetTable::~SheetTable()
{
  //delete[] table;
}


void SheetTable::setText( int row, int col, QString s, bool paint )
{
  //table[index(row,col)].operator=( s.copy() );

  table.remove(table.at(index(row,col)));
  table.insert(table.at(index(row,col)), s);

  int x,y;
    if ( paint && rowYPos( row, &y ) && colXPos( col, &x ))
        repaint( x,y, cellWidth(col), cellHeight(row));
    if (row == inRow && col == inCol && editable)
      input->setText(s);
}

bool SheetTable::hasValue(int row,int col)
{
 //return !QString(table.at(index(row,col))).simplifyWhiteSpace().isEmpty();
 return !(table[index(row,col)].simplifyWhiteSpace().isEmpty());
}

void SheetTable::placeInput()
{
    // makeVisible( inRow, inCol );
    int x,y;
    if ( colXPos(inCol,&x) && rowYPos(inRow,&y) ) {
        input->move(x+1,y+1);
        input->show();
        if (!input->hasFocus())
            input->setFocus();
    } else
        input->hide();
}

void SheetTable::paintCell( QPainter *p, int row, int col )
{

  int w = cellWidth( col );
  int h = cellHeight( row );
  int x2 = w - 1;
  int y2 = h - 1;

  p->setPen(black);
  p->drawLine( x2, 0, x2, y2 );
  p->drawLine( 0, y2, x2, y2 );

  if (row == topCell())
    p->drawLine(0,0,x2,0);
  if (col == leftCell())
    p->drawLine(0,0,0,y2);

    QString str;
    if (!table.isEmpty())
      str = table[index(row,col)];
    //    if ( str.isEmpty() )
    //  str.sprintf( "%c%d", col+'A', row );
    p->drawText( 1, 1, cellWidth()-2, cellHeight()-2,
                 AlignCenter, str );

    if ( row == inRow && col == inCol && editable)
        placeInput();

}

void SheetTable::setInputText( QString s )
{
    input->setText( s );
}

void SheetTable::nextInput()
{
    int c = inCol;
    int r = ( inRow + 1 ) % numRows();
    if ( !r )
        c = ( inCol + 1 ) % numCols();

    inCol_inRow_initialization = FALSE;
    moveInput( r, c );
}

void SheetTable::moveInput( int row, int col )
{
    if ( col < 0 || row < 0 )
        return;
    if ( col == inCol && row == inRow )
        return;

  if ( col == 0 && row == 0 && inCol_inRow_initialization ) {
        inCol = col;
        inRow = row;
    }
   if ( inRow >= 0 && inCol >= 0 ) {
        QString str = input->text();
        setText( inRow, inCol, str );
        emit newText(inRow, inCol, str );
    }
    inCol = col;
    inRow = row;
    makeVisible( inRow, inCol );
    placeInput();
    emit selected(row,col);
}

void SheetTable::makeVisible( int row, int col )
{
    if ( col < leftCell() ) {
        setLeftCell(col);
        emit newCol(col);
    } else if ( col > lastColVisible() ) {
        int c = leftCell() + col - lastColVisible();
        setLeftCell(c);
        emit newCol(c);
    }

    if ( row < topCell() ) {
        setTopCell(row);
        emit newRow(row);
    } else if ( row > lastRowVisible() ) {
        int r = topCell() + row - lastRowVisible();
        setTopCell(r);
        emit newRow(r);
    }
}

void SheetTable::mousePressEvent( QMouseEvent * e )
{
  if (editable)
    {
      int col = findCol(e->pos().x());
      int row = findRow(e->pos().y());
      inCol_inRow_initialization = FALSE;
      moveInput( row, col );
    }
}


void SheetTable::scrollHorz( int col )
{
    setLeftCell( col );
    if (editable) placeInput();
    repaint();
}

void SheetTable::scrollVert(int row )
{
    setTopCell( row );
    if (editable) placeInput();
    repaint();
}

}  //namespace KChart
