/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0alpha                                            */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Effect Dialog (header)                                 */
/******************************************************************/

#ifndef EFFECTDIA_H
#define EFFECTDIA_H

class KPresenterView_impl;

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qbttngrp.h>
#include <qchkbox.h>
#include <qlabel.h>

#include <kspinbox.h>
#include <krestrictedline.h>

/******************************************************************/
/* class EffectDia                                                */
/******************************************************************/

class EffectDia : public QDialog
{
  Q_OBJECT

public:

  // constructor - destructor
  EffectDia(QWidget* parent,const char*,int,int,KPresenterView_impl*);
  ~EffectDia();                                             

protected:
  QButtonGroup *general,*page; 
  QCheckBox *infinitLoop,*manualSwitch;
  QLabel *label1,*label2,*label3;
  QPushButton *cancelBut,*okBut;

  int objNum,pageNum;
  KPresenterView_impl *view;

public slots:
  void slotEffectDiaOk() {emit effectDiaOk();}

signals:
  void effectDiaOk();

};

#endif //EFFECTDIA_H
