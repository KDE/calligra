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


#ifndef TABLE_H
#define TABLE_H


#include <qttableview.h>
#include <qlineedit.h>
#include <qpainter.h>
#include <qstringlist.h>


class SheetTable : public QtTableView
{
    Q_OBJECT

public:
    //SheetTable( QWidget *parent=0, const char *name=0 );
    SheetTable( int cols, int rows, QWidget *parent=0, int flags = -1,
		 const char *name=0, bool _editable = true );
    ~SheetTable();

    int tWidth()           { return totalWidth()  + extraW; } 
    int tHeight()          { return totalHeight() + extraH; }

    int numColsVisible()   { return lastColVisible() - leftCell() + 1; }
    int numRowsVisible()   { return lastRowVisible() - topCell()  + 1; }
    void setText( int row, int col, QString, bool paint = TRUE );

    bool hasValue(int, int);

public slots:
    void showText( int row, int col, QString s) { setText( row, col, s); }
    void nextInput();
    void moveInput( int row, int col );
    void makeVisible( int row, int col );

signals:        
    void selected( int row, int col );
    void newText( int row, int col, QString );
    void newRow(int);
    void newCol(int);
    void recalc();
    //void recalc( int row, int col );

protected:
    virtual void paintCell( QPainter *p, int row, int col );
    virtual void mousePressEvent( QMouseEvent * );    

protected slots:
    void  scrollHorz(int);
    void  scrollVert(int);
    void  setInputText(QString);

private:
    int          index( int row, int col )   { return row * numCols() + col; }
    void         placeInput();

private:
    int          extraW;
    int          extraH;
    QStringList  table;

    QLineEdit   *input;
    int          inRow;
    int          inCol;
    bool         inCol_inRow_initialization;

    friend class Sheet; //###

    bool         editable;
};


#endif
