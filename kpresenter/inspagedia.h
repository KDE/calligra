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
/* Module: insert page dialog (header)                            */
/******************************************************************/

#ifndef inspagedia_h
#define inspagedia_h

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qradiobutton.h>

#include <kapp.h>
#include <kspinbox.h>

#include "global.h"

class KPresenterDocument_impl;

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

/******************************************************************/
/* class InsPageDia                                               */
/******************************************************************/

class InsPageDia : public QDialog
{
  Q_OBJECT

public:
  InsPageDia(QWidget* parent,const char* name,KPresenterDocument_impl *_doc,int _currPage);  

protected:
  void uncheckAllPos();
  void uncheckAllMode();

  KPresenterDocument_impl *doc;
  
  KNumericSpinBox *spinBox;
  QRadioButton *before,*after,*leave,*_move;
  QPushButton *ok,*cancel;
  
protected slots:
  void leaveClicked();
  void moveClicked();
  void beforeClicked();
  void afterClicked();
  void okClicked();

signals:
  void insertPage(int,InsPageMode,InsertPos);

};

#endif
