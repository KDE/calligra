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
/* Module: Search Dialog (header)                                 */
/******************************************************************/

#ifndef search_h
#define search_h

#include <stdlib.h>

#include <qtabdialog.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qstring.h>
#include <qcolor.h>
#include <qstrlist.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qevent.h>

#include <kapp.h>
#include <kcolorbtn.h>
#include <kbuttonbox.h>

#include "format.h"

class KWordDocument;
class KWPage;

/******************************************************************/
/* Class: KWSearchDia                                             */
/******************************************************************/

class KWSearchDia : public QTabDialog
{
  Q_OBJECT

public:
  struct KWSearchEntry
  {
    QString expr;
    bool checkFamily,checkColor,checkSize,checkBold,checkItalic,checkUnderline,checkVertAlign;
    QString family;
    QColor color;
    int size;
    bool bold,italic,underline;
    KWFormat::VertAlign vertAlign;
    bool caseSensitive;
  };

  KWSearchDia(QWidget *parent,const char *name,KWordDocument *_doc,KWPage *_page,KWSearchEntry *_searchEntry,QStrList _fontlist);

protected:
  void setupTab1();
  void closeEvent(QCloseEvent *e) { emit cancelButtonPressed(); }

  QWidget *tab1;
  QGridLayout *grid1,*sGrid;
  QGroupBox *gSearch;
  QCheckBox *cRegExp,*cFamily,*cSize,*cColor,*cBold,*cItalic,*cUnderline,*cVertAlign,*cmBold,*cmItalic,*cmUnderline,*cCase;
  QComboBox *cmFamily,*cmSize,*cmVertAlign;
  KColorButton *bColor;
  QLabel *lSearch;
  KButtonBox *bbSearch;
  QPushButton *bSearchFirst,*bSearchNext,*bSearchAll;
  QLineEdit *eSearch;

  KWordDocument *doc;
  KWPage *page;
  KWSearchEntry *searchEntry;
  QStrList fontlist;

protected slots:
  void searchFirst();
  void searchNext();

};

#endif


