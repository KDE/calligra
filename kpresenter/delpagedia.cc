/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
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
/* Module: delete page dialog                                     */
/******************************************************************/

#include "kpresenter_doc.h"

#include "delpagedia.h"
#include "delpagedia.moc"

/******************************************************************/
/* class DelPageDia                                               */
/******************************************************************/

/*==================== constructor ===============================*/
DelPageDia::DelPageDia(QWidget* parent,const char* name,KPresenterDoc *_doc,int currPageNum)
  : QDialog(parent,name,true)
{
  doc = _doc;

  label = new QLabel(i18n("Delete Page Number: "),this);
  label->resize(label->sizeHint());
  label->move(20,20);

  spinBox = new QSpinBox(1,doc->getPageNums(),1,this);
  spinBox->setValue(currPageNum);
  spinBox->resize(spinBox->sizeHint());
  spinBox->move(label->x() + label->width() + 5,label->y());
  label->move(label->x(),label->y() + (spinBox->height() - label->height()) / 2);

  leave = new QRadioButton(i18n("&Leave all objects untouched."),this);
  leave->resize(leave->sizeHint());
  leave->move(label->x(),spinBox->y() + spinBox->height() + 20);
  connect(leave,SIGNAL(clicked()),this,SLOT(leaveClicked()));

  _move = new QRadioButton(i18n("&Move the objects which are behind the deleted page \n"
			       "one page backwards, so that they stay on their current page, \n"
			       "and don't touch the objects, which are on the deleted page."),this);
  _move->resize(_move->sizeHint());
  _move->move(label->x(),leave->y() + leave->height() + 10);
  connect(_move,SIGNAL(clicked()),this,SLOT(moveClicked()));

  del = new QRadioButton(i18n("&Delete the objects which are on the deleted page and \n"
			       "leave the other objects untouched."),this);
  del->resize(del->sizeHint());
  del->move(label->x(),_move->y() + _move->height() + 10);
  connect(del,SIGNAL(clicked()),this,SLOT(delClicked()));

  move_del = new QRadioButton(i18n("M&ove the objects which are behind the deleted page \n"
				   "one page backwards, so that they stay on their current page, \n"
				   "and delete the objects which are on the deleted page."),this);
  move_del->resize(move_del->sizeHint());
  move_del->move(label->x(),del->y() + del->height() + 10);
  connect(move_del,SIGNAL(clicked()),this,SLOT(moveDelClicked()));

  cancel = new QPushButton(this,"BCancel");
  cancel->setText(i18n("Cancel"));
  cancel->resize(cancel->sizeHint());
  cancel->move(max(max(max(leave->width(),_move->width()),del->width()),move_del->width()) + leave->x() - cancel->width(),
	       move_del->y() + move_del->height() + 20);
  connect(cancel,SIGNAL(clicked()),this,SLOT(reject()));

  ok = new QPushButton(this,"BOK");
  ok->setText(i18n("OK"));
  ok->setAutoRepeat(false);
  ok->setAutoResize(false);
  ok->setAutoDefault(true);
  ok->setDefault(true);
  ok->resize(cancel->size());
  ok->move(cancel->x() - ok->width() - 10,cancel->y());
  connect(ok,SIGNAL(clicked()),this,SLOT(okClicked()));
  connect(ok,SIGNAL(clicked()),this,SLOT(accept()));

  resize(cancel->x() + cancel->width() + 20,cancel->y() + cancel->height() + 20);
  uncheckAll();
  move_del->setChecked(true);
}

/*================================================================*/
void DelPageDia::uncheckAll()
{
  leave->setChecked(false);
  _move->setChecked(false);
  del->setChecked(false);
  move_del->setChecked(false);
}

/*================================================================*/
void DelPageDia::leaveClicked()
{
  uncheckAll();
  leave->setChecked(true);
}

/*================================================================*/
void DelPageDia::moveClicked()
{
  uncheckAll();
  _move->setChecked(true);
}

/*================================================================*/
void DelPageDia::delClicked()
{
  uncheckAll();
  del->setChecked(true);
}

/*================================================================*/
void DelPageDia::moveDelClicked()
{
  uncheckAll();
  move_del->setChecked(true);
}

/*================================================================*/
void DelPageDia::okClicked()
{
  DelPageMode dpl = DPM_LET_OBJS;

  if (leave->isChecked())
    dpl = DPM_LET_OBJS;
  else if (_move->isChecked())
    dpl = DPM_MOVE_OBJS;
  else if (del->isChecked())
    dpl = DPM_DEL_OBJS;
  else if (move_del->isChecked())
    dpl = DPM_DEL_MOVE_OBJS;

  emit deletePage(spinBox->value() - 1,dpl);
}

