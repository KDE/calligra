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
/* Module: Page Configuration Dialog (header)                     */
/******************************************************************/

#ifndef PGCONFDIA_H
#define PGCONFDIA_H

#include <stdio.h>

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qbttngrp.h>
#include <qchkbox.h>
#include <qlabel.h>
#include <qcombo.h>

#include "global.h"

/******************************************************************/
/* class PgConfDia                                                */
/******************************************************************/

class PgConfDia : public QDialog
{
  Q_OBJECT

public:

  // constructor - destructor
  PgConfDia(QWidget* parent=0,const char* name=0,
	    bool infLoop=false,bool swMan=true,int pgNum=1,PageEffect pageEffect=PEF_NONE);
  ~PgConfDia();                                             
  bool getInfinitLoop() {return infinitLoop->isChecked();}
  bool getManualSwitch() {return manualSwitch->isChecked();}
  PageEffect getPageEffect() {return (PageEffect)effectCombo->currentItem();}

protected:
  QButtonGroup *general,*page; 
  QCheckBox *infinitLoop,*manualSwitch;
  QLabel *label1,*label2,*label3;
  QPushButton *cancelBut,*okBut;
  QComboBox *effectCombo;

public slots:
  void confDiaOk() {emit pgConfDiaOk();}

signals:
  void pgConfDiaOk();

};

#endif //PGCONFDIA_H
