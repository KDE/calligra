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
/* Module: Autoformat Dialog (header)                             */
/******************************************************************/

#ifndef autoformatdia_h
#define autoformatdia_h

#include <stdlib.h>

#include <qtabdialog.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>

#include "kcharselect.h"

class KWordDocument;
class KWPage;

/******************************************************************/
/* Class: KWAutoFormatDia                                         */
/******************************************************************/

class KWAutoFormatDia : public QTabDialog
{
  Q_OBJECT

public:
  KWAutoFormatDia(QWidget *parent,const char *name,KWordDocument *_doc,KWPage *_page);

protected:
  void setupTab1();

  QVBox *tab1;
  QCheckBox *cbTypographicQuotes,*cbUpperCase,*cbUpperUpper;
  QPushButton *pbQuote1,*pbQuote2;
  KCharSelect *charselect;
  QChar oBegin,oEnd;
  bool quotesChanged;
  
  KWordDocument *doc;
  KWPage *page;

protected slots:
  void applyConfig();

  void chooseQuote1();
  void chooseQuote2();
  
};

#endif


