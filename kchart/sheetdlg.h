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

#ifndef SHEETDLG_H
#define SHEETDLG_H

#include <qwidget.h>

class QSpinBox;
class QLabel;
class QPushButton;

namespace KChart
{

class Sheet;

class SheetDlg : public QWidget
{
  Q_OBJECT
public:
  SheetDlg( QWidget * parent = 0, const char * name = 0 );
  
  void fillCell(int,int,double);
  void fillX(int,QString);
  void fillY(int,QString);
  
  int cols();
  int rows();
  int usedCols();
  int usedRows();
  void setUsedCols( int );
  void setUsedRows( int );
 
  QString getX(int);
  QString getY(int);
  double getCell(int,int);

  static const int TABLE_SIZE;
  
protected:
  virtual void resizeEvent( QResizeEvent *);
  
private:

   Sheet *t;
  QPushButton *cancel,*ok;
  void resizeHandle( QSize );
  QLabel *usedrowsLA, *usedcolsLA;
  QSpinBox *usedrowsSB, *usedcolsSB;
};

}  //namespace KChart

#endif
