/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.0.1                                                 */
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

#include <stdio.h>
#include <stdlib.h>

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qbttngrp.h>
#include <qcombo.h>
#include <qlabel.h>

#include <krestrictedline.h>

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

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
  QComboBox *cEffect,*cEffect2;
  QLabel *lEffect,*lEffect2,*lNum;
  KRestrictedLine *eNum;
  QPushButton *cancelBut,*okBut;

  int objNum,pageNum;
  KPresenterView_impl *view;

public slots:
  void slotEffectDiaOk();

signals:
  void effectDiaOk();

};

#endif //EFFECTDIA_H


