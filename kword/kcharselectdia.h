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
#include <qpushbutton.h>
#include <qlayout.h>
#include <qstrlist.h>
#include <qlist.h>
#include <qstring.h>

#include <kbuttonbox.h>
#include <kapp.h>
#include <kcharselect.h>

/******************************************************************/
/* class KCharSelectDia                                           */
/******************************************************************/

class KCharSelectDia : public QDialog
{
  Q_OBJECT

public:

  // constructor - destructor
  KCharSelectDia(QWidget *parent,const char *name,const QChar &_chr,const QString &_font,bool _enableFont);

  // select char dialog
  static bool selectChar(QString &_font,QChar &_chr,bool _enableFont = true);

  // internal
  QChar chr() { return charSelect->chr(); }
  QString font() { return charSelect->font(); }

protected:
  // dialog objects
  QGridLayout *grid;
  KButtonBox *bbox;
  QPushButton *bOk,*bCancel;
  KCharSelect *charSelect;

};

#endif
