/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Autoform Editor                                                */
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
/* Module: point insert dialog (header)                           */
/******************************************************************/

#ifndef PNTINSDIA_H
#define PNTINSDIA_H

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qradiobt.h>
#include <qbttngrp.h>

#include <kspinbox.h>

/******************************************************************/
/* class PntInsDia                                                */
/******************************************************************/

class PntInsDia : public QDialog
{
  Q_OBJECT

public:

  // constructor - destructor
  PntInsDia(QWidget* parent=0,const char* name=0,int points=0); 
  ~PntInsDia();                                                 

  // ********** variables **********

  // dialog objects
  QLabel *label1,*label2;
  QRadioButton *radioBefore,*radioAfter;
  QButtonGroup *btnGrp;
  KNumericSpinBox *spinBox;
  QPushButton *okBut,*cancelBut;

signals:

  // insert a point
  void insPoint(int,bool);

private slots:

  // insert a point
  void insPnt();

};
#endif //PNTINSDIA_H

