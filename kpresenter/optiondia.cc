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
/* Module: Option Dialog                                            */
/******************************************************************/

#include "optiondia.h"
#include "optiondia.moc"

/******************************************************************/
/* class OptionDia                                                */
/******************************************************************/

/*==================== constructor ===============================*/
OptionDia::OptionDia(QWidget *parent=0,const char *name=0)
  :QTabDialog(parent,name,true)
{
  int col1 = 20,dummy,w,h;
  
  /* Tab: General */ 
  general = new QWidget(this,i18n("General"));

  lRastX = new QLabel(general,"lRastX");
  lRastX->setText(i18n("Horizontal Raster: "));
  lRastX->move(col1,20);
  lRastX->resize(lRastX->sizeHint());

  lRastY = new QLabel(general,"lRastY");
  lRastY->setText(i18n("Vertikal Raster: "));
  lRastY->move(col1,lRastX->y()+lRastX->height()+15);
  lRastY->resize(lRastY->sizeHint());

  dummy = max(lRastX->sizeHint().width(),lRastY->sizeHint().width());
  lRastX->resize(dummy,lRastX->height());
  lRastY->resize(dummy,lRastY->height());

  eRastX = new KRestrictedLine(general,"eRastX","0123456789");
  eRastX->setText("20");
  eRastX->move(lRastX->x()+lRastX->width()+10,lRastX->y());
  eRastX->resize(eRastX->sizeHint().width()/2,eRastX->sizeHint().height());
  eRastX->setMaxLength(2);

  eRastY = new KRestrictedLine(general,"eRastY","0123456789");
  eRastY->setText("20");
  eRastY->move(lRastY->x()+lRastY->width()+10,lRastY->y());
  eRastY->resize(eRastY->sizeHint().width()/2,eRastY->sizeHint().height());
  eRastY->setMaxLength(2);

  lRastX->resize(lRastX->width(),eRastX->height());
  lRastY->resize(lRastY->width(),eRastY->height());

  //general->resize(col1+toolPres->x()+toolPres->width(),col1+toolPres->y()+toolPres->height());

  /* Tab: Objects */
  objects = new QWidget(this,"objects");

  txtObj = new QGroupBox(i18n("Objects in Editing-Mode"),objects,"txtObjs");
  txtObj->move(col1,col1);

  lBackCol = new QLabel(txtObj,"lBackCol");
  lBackCol->setText(i18n("Backgroud color:"));
  lBackCol->move(10,20);
 
  lSelCol = new QLabel(txtObj,"lSelCol");
  lSelCol->setText(i18n("Selection color:"));
  lSelCol->move(10,lBackCol->y()+lBackCol->height()+10);

  dummy = max(lBackCol->sizeHint().width(),lSelCol->sizeHint().width());
  lBackCol->resize(dummy,lBackCol->height());
  lSelCol->resize(dummy,lSelCol->height());

  bBackCol = new KColorButton(white,txtObj,"bBackCol");
  bBackCol->setGeometry(lBackCol->x()+lBackCol->width()+10,lBackCol->y(),100,25);

  bSelCol = new KColorButton(lightGray,txtObj,"bSelCol");
  bSelCol->setGeometry(lSelCol->x()+lSelCol->width()+10,lSelCol->y(),100,25);

  txtObj->resize(20+bBackCol->x()+bBackCol->width(),20+bSelCol->y()+bSelCol->height());

  rect = new QGroupBox(i18n("Rounded Rectangles"),objects,"rect");
  rect->move(col1,txtObj->y()+txtObj->height()+20);

  lRndX = new QLabel(rect,"lRndX");
  lRndX->setTexti18n(("Roundedness X:"));
  lRndX->move(10,20);

  lRndY = new QLabel(rect,"lRndY");
  lRndY->setText(i18n("Roundedness Y:"));
  lRndY->move(10,lRndX->y()+lRndX->height()+10);

  dummy = max(lRndX->sizeHint().width(),lRndY->sizeHint().width());
  lRndX->resize(dummy,lRndX->height());
  lRndY->resize(dummy,lRndY->height());

  eRndX = new KRestrictedLine(rect,"eRndX","0123456789");
  eRndX->move(lRndX->x()+lRndX->width()+10,lRndX->y());
  eRndX->setMaxLength(2);
  eRndX->resize(eRndX->sizeHint().width()/2,eRndX->sizeHint().height());

  eRndY = new KRestrictedLine(rect,"eRndY","0123456789");
  eRndY->move(lRndY->x()+lRndY->width()+10,lRndY->y());
  eRndY->setMaxLength(2);
  eRndY->resize(eRndY->sizeHint().width()/2,eRndY->sizeHint().height());

  lRndX->resize(lRndX->width(),eRndX->height());
  lRndY->resize(lRndY->width(),eRndY->height());

  rect->resize(txtObj->width(),20+eRndY->y()+eRndY->height());

  objects->resize(col1+rect->x()+rect->width(),
		  col1+rect->y()+rect->height());

  w = objects->width();
  h = objects->height();

  general->resize(w,h);
  general->setMinimumSize(general->width(),general->height());
  general->setMaximumSize(general->width(),general->height());

  objects->resize(w,h);
  objects->setMinimumSize(objects->width(),objects->height());
  objects->setMaximumSize(objects->width(),objects->height());

  /* build dialog */
  addTab(general,i18n("General"));
  addTab(objects,i18n("Objects"));

  setCancelButton(i18n("Cancel"));
  setApplyButton(i18n("Apply"));
  setOkButton(i18n("OK"));
}

/*===================== destructor ===============================*/
OptionDia::~OptionDia()
{
  delete lRastX;
  delete lRastY;
  delete eRastX;
  delete eRastY;

  delete lBackCol;
  delete lSelCol;
  delete bBackCol;
  delete bSelCol;
  delete lRndX;
  delete lRndY;
  delete eRndX;
  delete eRndY;

  delete rect;
  delete txtObj;

  delete objects;
  delete general;
}
