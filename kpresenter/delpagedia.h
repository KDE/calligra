/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: delete page dialog (header)                            */
/******************************************************************/

#ifndef delpagedia_h
#define delpagedia_h

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qradiobutton.h>

#include <kapp.h>
#include <qspinbox.h>

#include "global.h"

class KPresenterDoc;

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

/******************************************************************/
/* class DelPageDia                                               */
/******************************************************************/

class DelPageDia : public QDialog
{
  Q_OBJECT

public:
  DelPageDia(QWidget* parent,const char* name,KPresenterDoc *_doc,int currPageNum);  

protected:
  void uncheckAll();

  KPresenterDoc *doc;
  
  QGridLayout *grid;
  QLabel *label;
  QSpinBox *spinBox;
  QRadioButton *leave,*_move,*del,*move_del;
  QPushButton *ok,*cancel;
  
protected slots:
  void leaveClicked();
  void moveClicked();
  void delClicked();
  void moveDelClicked();
  void okClicked();

signals:
  void deletePage(int,DelPageMode);

};

#endif
