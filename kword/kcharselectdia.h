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
#include <qcombo.h>
#include <qstrlist.h>
#include <qlist.h>

#include <kbuttonbox.h>
#include <kapp.h>
#include <kcolorbtn.h>

#include "kcharselect.h"

/******************************************************************/
/* class KCharSelectDia                                           */
/******************************************************************/

class KCharSelectDia : public QDialog
{
  Q_OBJECT

public:

  // constructor - destructor
  KCharSelectDia(QWidget*,const char*,int,QFont,QStrList _fontList); 
  ~KCharSelectDia();               

  // select char dialog
  static bool selectChar(QFont &__font,int &__c,QStrList _fontList);

  // internal
  int c() {return _c;}
  QFont font() { return _font; }

protected:
  
  // dialog objects
  QGridLayout *grid;
  KButtonBox *bbox;
  QPushButton *bOk,*bCancel; 
  KCharSelect *charSelect;
  QComboBox *fontCombo;
  QLabel *lFont;

  // values
  int _c;
  QFont _font;
  QStrList fontList;

protected slots:
  void charChanged(int);
  void fontSelected(const char*);

};
#endif //KCHARSELECTDIA_H
