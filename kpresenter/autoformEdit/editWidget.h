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
/* Module: edit widget (header)                                   */
/******************************************************************/

#ifndef EDITWIDGET_H
#define EDITWIDGET_H

#include <qwidget.h>
#include <qlined.h>
#include <qevent.h>
#include <qtooltip.h>
#include <qlist.h>
#include <qmsgbox.h>

#include <ktreelist.h>
#include <kapp.h>
#include <ktoolbar.h>
#include <kbutton.h>

#include "atfinterpreter.h"

/******************************************************************/
/* Class: EditWidget                                              */
/******************************************************************/
class EditWidget : public QWidget
{
  Q_OBJECT

public:

  // constructor - destructor
  EditWidget(QWidget *parent=0, const char *name=0);
  ~EditWidget();

  // set the source
  void setSource(QList<ATFInterpreter::PointStruct> pl) {pointList = pl; parseSource();}

  // delete a point
  void deletePoint();
  
  // is changed or not (=saved)
  bool isChanged() {return changed;}
  void saved() {changed = false;}

protected:

  // resize event
  void resizeEvent(QResizeEvent*);

  // paint the autoform
  void parseSource();

  // stretch - simlify line
  QString stretch(QString);
  QString simplify(QString);

  // is number/operator/variable/ok
  bool isNum(char);
  bool isVar(char);
  bool isOperator(char);
  bool isInputOk(QString,int,int);

  // ********** variables **********

  // GUI-elements
  KButton *ok;
  KButton *cancel;
  QLineEdit *lineEdit;
  KTreeList *treeList;

  // list to points and a pointer to a point
  QList<ATFInterpreter::PointStruct> pointList;
  ATFInterpreter::PointStruct *pntPtr;

  // autoform changed?
  bool changed;

signals:

  // set source
  void getSource();

  // change variable
  void changeVar(int,int,int,QString);

  // delete point
  void delPnt(int);

private slots:

  // item selected/changed 
  void itemSelected(int);
  void itemChanged();

  // input cancled
  void cancelInput();

};

#endif //EDITWIDGET_H








