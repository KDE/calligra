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

#include "pgconfdia.h"
#include "pgconfdia.moc"

/******************************************************************/
/* class PgConfDia                                                */
/******************************************************************/

/*==================== constructor ===============================*/
PgConfDia::PgConfDia(QWidget* parent,const char* name,
		     bool infLoop,bool swMan,int pgNum=1)
  :QDialog(parent,name,true)
{
  general = new QButtonGroup(this,"general");
  general->setFrameStyle(QFrame::Box|QFrame::Sunken);
  general->move(20,20);
  general->setTitle("General");

  infinitLoop = new QCheckBox("&Infinite Loop",general);
  infinitLoop->resize(infinitLoop->sizeHint());
  infinitLoop->move(10,20);
  infinitLoop->setChecked(infLoop);

  manualSwitch = new QCheckBox("&Manual switch to next step",general);
  manualSwitch->resize(manualSwitch->sizeHint());
  manualSwitch->move(infinitLoop->x(),infinitLoop->y()+infinitLoop->height()+10);
  manualSwitch->setChecked(swMan);
  connect(manualSwitch,SIGNAL(clicked()),this,SLOT(msClicked()));

  page = new QButtonGroup(this,"page");
  page->setFrameStyle(QFrame::Box|QFrame::Sunken);
  page->setTitle("Page Configuration");

  char str[30];
  sprintf(str,"Page number: %d",pgNum);
  label1 = new QLabel(str,page);
  label1->resize(label1->sizeHint());
  label1->move(10,20);

  label2 = new QLabel("Effect for changing to next page:",page);
  label2->resize(label2->sizeHint());
  label2->move(label1->x(),label1->y()+label1->height()+20);

  page->resize(label2->width()+20,label2->y()+label2->height()+10);
  general->resize(manualSwitch->x()+manualSwitch->width()+20,manualSwitch->y()+manualSwitch->height()+10);
  page->move(20,general->y()+general->height()+20);

  cancelBut = new QPushButton(this,"BCancel");
  cancelBut->setText("Cancel");
  cancelBut->resize(cancelBut->sizeHint());
  cancelBut->move(general->x()+general->width()-cancelBut->width(),page->y()+page->height()+20);
 
  okBut = new QPushButton(this,"BOK");
  okBut->setText("OK");
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
//   delete infinitLoop;
//   delete manualSwitch;
  
//   delete label1;
//   delete label2;
//   delete label3;
//   delete spinBox;
//   delete time;
//   delete timeEd;
  
//   delete general;
//   delete page;

//   delete cancelBut;
//   delete okBut;
}














