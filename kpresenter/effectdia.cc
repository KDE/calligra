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
/* Module: Effect Dialog                                          */
/******************************************************************/

#include "effectdia.h"
#include "effectdia.moc"
#include "kpresenter_view.h"

/******************************************************************/
/* class EffectDia                                                */
/******************************************************************/

/*==================== constructor ===============================*/
EffectDia::EffectDia(QWidget* parent,const char* name,int _pageNum,int _objNum,KPresenterView_impl *_view)
  :QDialog(parent,name,true)
{
  pageNum = _pageNum;
  objNum = _objNum;
  view = _view;

  lNum = new QLabel("Number: ",this);
  lNum->move(10,10);
  lNum->resize(lNum->sizeHint());

  eNum =  new KRestrictedLine(this,"eNum","0123456789");
  eNum->move(lNum->width()+15,10);
  eNum->resize(eNum->sizeHint().width()/2,eNum->sizeHint().height());
  char str[5];
  sprintf(str,"%d",view->KPresenterDoc()->objList()->at(_objNum-1)->presNum);
  eNum->setText(str);

  lEffect = new QLabel("Effect (appearing): ",this);
  lEffect->move(10,eNum->y()+eNum->height()+20);
  lEffect->resize(lEffect->sizeHint());

  cEffect = new QComboBox(false,this,"cEffect");
  cEffect->insertItem("No Effect");
  cEffect->insertItem("Come from right");
  cEffect->insertItem("Come from left");
  cEffect->insertItem("Come from top");
  cEffect->insertItem("Come from bottom");
  cEffect->insertItem("Come from right/top");
  cEffect->insertItem("Come from right/bottom");
  cEffect->insertItem("Come from left/top");
  cEffect->insertItem("Come from left/bottom");
  cEffect->setCurrentItem((int)view->KPresenterDoc()->objList()->at(_objNum-1)->effect);
  cEffect->move(max(lEffect->width(),lNum->width())+15,lEffect->y()-5);
  cEffect->resize(cEffect->sizeHint());

  lEffect2 = new QLabel("Effect (object specific): ",this);
  lEffect2->move(cEffect->x()+cEffect->width()+20,eNum->y()+eNum->height()+20);
  lEffect2->resize(lEffect2->sizeHint());

  cEffect2 = new QComboBox(false,this,"cEffect2");
  cEffect2->insertItem("No Effect");

  switch (view->KPresenterDoc()->objList()->at(_objNum-1)->objType)
    {
    case OT_TEXT:
      {
	cEffect2->insertItem("Paragraph after paragraph");
      } break;
    }

  if (view->KPresenterDoc()->objList()->at(_objNum-1)->effect2 == EF_NONE)
    cEffect2->setCurrentItem((int)view->KPresenterDoc()->objList()->at(_objNum-1)->effect2);
  else
    {
      switch (view->KPresenterDoc()->objList()->at(_objNum-1)->objType)
	{
	case OT_TEXT:
	  cEffect2->setCurrentItem((int)view->KPresenterDoc()->objList()->at(_objNum-1)->effect2+TxtObjOffset);
	  break;
	}
    }

  cEffect2->move(lEffect2->x()+lEffect2->width()+5,lEffect2->y()-5);
  cEffect2->resize(cEffect2->sizeHint());

  resize(cEffect2->x()+cEffect2->width()+10,cEffect->y()+cEffect->height()+10);

  cancelBut = new QPushButton(this,"BCancel");
  cancelBut->setText("Cancel");

  okBut = new QPushButton(this,"BOK");
  okBut->setText("OK");
  okBut->setAutoRepeat(false);
  okBut->setAutoResize(false);
  okBut->setAutoDefault(true);
  okBut->setDefault(true);

  int butW = max(cancelBut->sizeHint().width(),okBut->sizeHint().width());
  int butH = cancelBut->sizeHint().height();

  cancelBut->resize(butW,butH);
  okBut->resize(butW,butH);

  cancelBut->move(width()-10-cancelBut->width(),cEffect->y()+cEffect->height()+20);
  okBut->move(cancelBut->x()-okBut->width()-5,cancelBut->y());

  connect(okBut,SIGNAL(clicked()),this,SLOT(slotEffectDiaOk()));
  connect(cancelBut,SIGNAL(clicked()),this,SLOT(reject()));
  connect(okBut,SIGNAL(clicked()),this,SLOT(accept()));

  resize(cEffect2->x()+cEffect2->width()+10,okBut->y()+okBut->height()+10);
}

/*===================== destructor ===============================*/
EffectDia::~EffectDia()
{
}

/*====================== effect dia ok ===========================*/
void EffectDia::slotEffectDiaOk()
{
  view->KPresenterDoc()->objList()->at(objNum-1)->presNum = atoi(eNum->text());
  view->KPresenterDoc()->objList()->at(objNum-1)->effect = (Effect)cEffect->currentItem();
  view->KPresenterDoc()->objList()->at(objNum-1)->effect2 = (Effect2)cEffect2->currentItem();

  emit effectDiaOk();
}












