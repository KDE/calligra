/******************************************************************/ 
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Stylist Dialog                                         */
/******************************************************************/

#ifndef stylist_h
#define stylist_h

#include <qtabdialog.h>
#include <qwidget.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>

#include <kbuttonbox.h>
#include <kapp.h>

#include "paraglayout.h"

class KWordDocument;

/******************************************************************/
/* Class: KWStyleManager                                          */
/******************************************************************/

class KWStyleManager : public QTabDialog
{
  Q_OBJECT

public:
  KWStyleManager(QWidget *_parent,KWordDocument *_doc);

protected:
  void setupTab1();

  QWidget *tab1;
  QGridLayout *grid1;
  QListBox *lStyleList;
  QPushButton *bEdit,*bDelete,*bAdd,*bUp,*bDown,*bCopy;
  KButtonBox *bButtonBox;

  KWordDocument *doc;

};

#endif

