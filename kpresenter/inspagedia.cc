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
/* Module: insert page dialog                                     */
/******************************************************************/

#include "kpresenter_doc.h"

#include "inspagedia.h"
#include "inspagedia.moc"

/******************************************************************/
/* class InsPageDia                                               */
/******************************************************************/

/*==================== constructor ===============================*/
InsPageDia::InsPageDia(QWidget* parent,const char* name,KPresenterDoc *_doc,int currPageNum)
  : QDialog(parent,name,true)
{
  doc = _doc;

  before = new QRadioButton(i18n("&Before page:"),this);
  before->resize(before->sizeHint());
  before->move(20,20);
  connect(before,SIGNAL(clicked()),this,SLOT(beforeClicked()));

  after = new QRadioButton(i18n("&After page:"),this);
  after->resize(after->sizeHint());
  after->move(before->x(),before->y() + before->height());
  connect(after,SIGNAL(clicked()),this,SLOT(afterClicked()));

  spinBox = new KNumericSpinBox(this);
  spinBox->setRange(1,doc->getPageNums());
  spinBox->setValue(currPageNum);
  spinBox->setEditable(false);
  spinBox->resize(spinBox->sizeHint().width() / 2,spinBox->sizeHint().height());
  spinBox->move(max(before->x() + before->width(),after->x() + after->width()) + 5,before->y() + before->height() / 2);

  leave = new QRadioButton(i18n("&Leave all objects untouched."),this);
  leave->resize(leave->sizeHint());
  leave->move(after->x(),spinBox->y() + spinBox->height() + 20);
  connect(leave,SIGNAL(clicked()),this,SLOT(leaveClicked()));

  _move = new QRadioButton(i18n("&Move the objects which are behind the inserted page \n"
			       "one page forward, so that they stay on their current page."),this);
  _move->resize(_move->sizeHint());
  _move->move(after->x(),leave->y() + leave->height() + 10);
  connect(_move,SIGNAL(clicked()),this,SLOT(moveClicked()));

  cancel = new QPushButton(this,"BCancel");
  cancel->setText(i18n("Cancel"));
  cancel->resize(cancel->sizeHint());
  cancel->move(max(leave->width(),_move->width()) + leave->x() - cancel->width(),
	       _move->y() + _move->height() + 20);
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
  uncheckAllPos();
  uncheckAllMode();
  after->setChecked(true);
  _move->setChecked(true);
}

/*================================================================*/
void InsPageDia::uncheckAllMode()
{
  leave->setChecked(false);
  _move->setChecked(false);
}

/*================================================================*/
void InsPageDia::uncheckAllPos()
{
  before->setChecked(false);
  after->setChecked(false);
}

/*================================================================*/
void InsPageDia::leaveClicked()
{
  uncheckAllMode();
  leave->setChecked(true);
}

/*================================================================*/
void InsPageDia::moveClicked()
{
  uncheckAllMode();
  _move->setChecked(true);
}

/*================================================================*/
void InsPageDia::beforeClicked()
{
  uncheckAllPos();
  before->setChecked(true);
}

/*================================================================*/
void InsPageDia::afterClicked()
{
  uncheckAllPos();
  after->setChecked(true);
}

/*================================================================*/
void InsPageDia::okClicked()
{
  InsPageMode ipm = IPM_LET_OBJS;

  if (leave->isChecked())
    ipm = IPM_LET_OBJS;
  else if (_move->isChecked())
    ipm = IPM_MOVE_OBJS;

  InsertPos ip = IP_AFTER;

  if (before->isChecked())
    ip = IP_BEFORE;
  else if (after->isChecked())
    ip = IP_AFTER;

  emit insertPage(spinBox->getValue() - 1,ipm,ip);
}

