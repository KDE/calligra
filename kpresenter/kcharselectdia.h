/******************************************************************/
/* KCharSelectDia - (c) by Reginald Stadlbauer 1998               */
/* Version: 0.0.4                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* KCharSelectDia is under GNU GPL                                */
/******************************************************************/
/* Module: Character Selector Dialog (header)                     */
/******************************************************************/

#ifndef KCHARSELECTDIA_H
#define KCHARSELECTDIA_H

#include <stdio.h>

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qlayout.h>
#include <qfont.h>
#include <qcolor.h>
#include <qcombo.h>
#include <qstrlist.h>
#include <qwidget.h>
#include <qframe.h>
#include <qchkbox.h>
#include <qpalette.h>

#include <kbuttonbox.h>
#include <kapp.h>
#include <kcolorbtn.h>

#include <X11/Xlib.h>

#include "kcharselect.h"

/******************************************************************/
/* class KCharSelectDia                                           */
/******************************************************************/

class KCharSelectDia : public QDialog
{
  Q_OBJECT

public:

  // constructor - destructor
  KCharSelectDia(QWidget*,const char*,QFont,QColor,int,QStrList); 
  ~KCharSelectDia();                                    

  // select char dialog
  static bool selectChar(QFont&,QColor&,int&,QStrList);

  // internal
  QFont font() {return _font;}
  QColor color() {return _color;}
  int c() {return _c;}

protected:
  
  // dialog objects
  QGridLayout *grid,*grid2;
  QComboBox *fontCombo,*sizeCombo;
  KButtonBox *bbox;
  QPushButton *bOk,*bCancel; 
  KCharSelect *charSelect;
  QWidget *wid;
  QLabel *lFont,*lSize,*lColor,*lPreview,*lAttrib;
  KColorButton *colorButton;
  QCheckBox *bold,*italic,*underl;

  // values
  QFont _font;
  QColor _color;
  int _c;

  QStrList fontList;

protected slots:
  void fontSelected(const char*);
  void sizeSelected(int);
  void colorChanged(const QColor&);
  void boldChanged();
  void italicChanged();
  void underlChanged();
  void charChanged(int);

};
#endif //KCHARSELECTDIA_H
