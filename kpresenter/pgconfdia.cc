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
/* Module: Page Configuration Dialog                              */
/******************************************************************/

#include <kapp.h>
#include "pgconfdia.h"
#include "pgconfdia.moc"

/******************************************************************/
/* class PgConfDia                                                */
/******************************************************************/

/*==================== constructor ===============================*/
PgConfDia::PgConfDia(QWidget* parent,const char* name,
		     bool infLoop,bool swMan,int pgNum=1,PageEffect pageEffect=PEF_NONE)
  :QDialog(parent,name,true)
{
  general = new QButtonGroup(this,"general");
  general->setFrameStyle(QFrame::Box|QFrame::Sunken);
  general->move(20,20);
  general->setTitle(i18n("General"));

  infinitLoop = new QCheckBox(i18n("&Infinite Loop"),general);
  infinitLoop->resize(infinitLoop->sizeHint());
  infinitLoop->move(10,20);
  infinitLoop->setChecked(infLoop);

  manualSwitch = new QCheckBox(i18n("&Manual switch to next step"),general);
  manualSwitch->resize(manualSwitch->sizeHint());
  manualSwitch->move(infinitLoop->x(),infinitLoop->y()+infinitLoop->height()+10);
  manualSwitch->setChecked(swMan);

  page = new QButtonGroup(this,"page");
  page->setFrameStyle(QFrame::Box|QFrame::Sunken);
  page->setTitle(i18n("Page Configuration"));

  char str[30];
  sprintf(str,i18n("Page number: %d"),pgNum);
  label1 = new QLabel(str,page);
  label1->resize(label1->sizeHint());
  label1->move(10,20);

  label2 = new QLabel(i18n("Effect for changing to next page:"),page);
  label2->resize(label2->sizeHint());
  label2->move(label1->x(),label1->y()+label1->height()+20);

  effectCombo = new QComboBox(false,page);
  effectCombo->move(label2->x(),label2->y()+label2->height()+5);
  effectCombo->insertItem(i18n("No effect"));
  effectCombo->insertItem(i18n("Close horizontal"));
  effectCombo->insertItem(i18n("Close vertical"));
  effectCombo->insertItem(i18n("Close from all directions"));
  effectCombo->insertItem(i18n("Open horizontal"));
  effectCombo->insertItem(i18n("Open vertical"));
  effectCombo->insertItem(i18n("Open from all directions"));
  effectCombo->resize(effectCombo->sizeHint());
  effectCombo->setCurrentItem((int)pageEffect);
  
  page->resize(label2->width()+20,effectCombo->y()+effectCombo->height()+10);
  general->resize(manualSwitch->x()+manualSwitch->width()+20,manualSwitch->y()+manualSwitch->height()+10);
  page->move(20,general->y()+general->height()+20);

  cancelBut = new QPushButton(this,"BCancel");
  cancelBut->setText(i18n("Cancel"));
  cancelBut->resize(cancelBut->sizeHint());
  cancelBut->move(general->x()+general->width()-cancelBut->width(),page->y()+page->height()+20);
 
  okBut = new QPushButton(this,"BOK");
  okBut->setText(i18n("OK"));
  okBut->setAutoRepeat(FALSE);
  okBut->setAutoResize(FALSE);
  okBut->setAutoDefault(TRUE);
  okBut->setDefault(TRUE);
  okBut->resize(cancelBut->width(),cancelBut->height());
  okBut->move(cancelBut->x()-10-okBut->width(),cancelBut->y());

  connect(okBut,SIGNAL(clicked()),this,SLOT(confDiaOk()));
  connect(cancelBut,SIGNAL(clicked()),this,SLOT(reject()));
  connect(okBut,SIGNAL(clicked()),this,SLOT(accept()));

  resize(general->width()+40,okBut->y()+okBut->height()+10);
}

/*===================== destructor ===============================*/
PgConfDia::~PgConfDia()
{
}














