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
		     bool infLoop,bool swMan)
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

//   page = new QButtonGroup(this,"page");
//   page->setFrameStyle(QFrame::Box|QFrame::Sunken);
//   page->setTitle("Page Configuration");

//   label1 = new QLabel("Page number: ",page);
//   label1->resize(label1->sizeHint());
//   label1->move(10,20);

//   spinBox = new KNumericSpinBox(page);
//   spinBox->setRange(1,pages);
//   spinBox->setValue(1);
//   spinBox->resize(spinBox->sizeHint().width()/4,spinBox->sizeHint().height());
//   spinBox->move(label1->x()+label1->width()+20,label1->y());
//   label1->resize(label1->width(),spinBox->height());
//   currPageNum = 1;
//   if (pages == 1) 
//     {
//       spinBox->hide();
//       label1->setText("Page number 1:");
//       label1->resize(label1->sizeHint());
//     }
//   else
//     {
//       connect(spinBox,SIGNAL(valueIncreased()),this,SLOT(slotPageNumChanged()));
//       connect(spinBox,SIGNAL(valueDecreased()),this,SLOT(slotPageNumChanged()));
//     }
//   label2 = new QLabel("Time of this page: ",page);
//   label2->resize(label2->sizeHint());
//   label2->move(label1->x(),label1->y()+label1->height()+20);

//   timeEd = new KRestrictedLine(page,"TimeEd","0123456789");
//   timeEd->setText("60");
//   timeEd->move(label2->x()+label2->width()+20,label2->y());
//   timeEd->resize(timeEd->sizeHint().width()/3,timeEd->sizeHint().height());
//   timeEd->setMaxLength(3);
//   connect(timeEd,SIGNAL(returnPressed()),this,SLOT(slotTimeChangedEd()));

//   label3 = new QLabel("seconds",page);
//   label3->resize(label3->sizeHint());
//   label3->move(timeEd->x()+timeEd->width()+5,timeEd->y());

//   label2->resize(label2->width(),timeEd->height());
//   label3->resize(label3->width(),timeEd->height());

//   time = new KSlider(KSlider::Horizontal,page); 
//   time->move(label2->x(),label2->y()+label2->height()+10);
//   time->setRange(0,600);
//   time->setSteps(1,30);
//   time->setValue(60);
//   time->resize(600,time->height());
//   connect(time,SIGNAL(valueChanged(int)),this,SLOT(slotTimeChanged(int)));

//   page->resize(time->width()+20,time->y()+time->height()+10);
  general->resize(manualSwitch->x()+manualSwitch->width()+20,manualSwitch->y()+manualSwitch->height()+10);
//   page->move(20,general->y()+general->height()+20);

  cancelBut = new QPushButton(this,"BCancel");
  cancelBut->setText("Cancel");
  cancelBut->resize(cancelBut->sizeHint());
  cancelBut->move(general->x()+general->width()-cancelBut->width(),general->y()+general->height()+20);
 
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














