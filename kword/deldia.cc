/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Delete Dialog                                          */
/******************************************************************/

#include "kword_doc.h"
#include "frame.h"
#include "kword_page.h"

#include "deldia.h"
#include <klocale.h>
#include "deldia.moc"

/******************************************************************/
/* Class: KWDeleteDia                                             */
/******************************************************************/

/*================================================================*/
KWDeleteDia::KWDeleteDia(QWidget *parent,const char *name,KWGroupManager *_grpMgr,KWordDocument *_doc,DeleteType _type,KWPage *_page)
  : QTabDialog(parent,name,true)
{
  type = _type;
  grpMgr = _grpMgr;
  doc = _doc;
  page = _page;

  setupTab1();

  setCancelButton(i18n("Cancel"));
  setOkButton(i18n("OK"));

  resize(300,150);
}

/*================================================================*/
void KWDeleteDia::setupTab1()
{
  tab1 = new QWidget(this);

  grid1 = new QGridLayout(tab1,2,1,15,7);

  rc = new QLabel(type == ROW ? i18n("Delete Row:") : i18n("Delete Column:"),tab1);
  rc->resize(rc->sizeHint());
  rc->setAlignment(AlignLeft | AlignBottom);
  grid1->addWidget(rc,0,0);

  value = new QSpinBox(1,type == ROW ? grpMgr->getRows() : grpMgr->getCols(),1,tab1);
  value->resize(value->sizeHint());
  value->setValue(type == ROW ? grpMgr->getRows() : grpMgr->getCols());
  grid1->addWidget(value,1,0);

  grid1->addRowSpacing(0,rc->height());
  grid1->addRowSpacing(1,value->height());
  grid1->setRowStretch(0,1);
  grid1->setRowStretch(1,0);

  grid1->addColSpacing(0,rc->width());
  grid1->addColSpacing(0,value->width());
  grid1->setColStretch(0,1);

  grid1->activate();

  addTab(tab1,type == ROW ? i18n("Delete Row") : i18n("Delete Column"));

  connect(this,SIGNAL(applyButtonPressed()),this,SLOT(doDelete()));

  resize(minimumSize());
}

/*================================================================*/
void KWDeleteDia::doDelete()
{
  QPainter p;
  p.begin(page);

  if (type == ROW)
    grpMgr->deleteRow(value->value() - 1,p);
  else
    grpMgr->deleteCol(value->value() - 1);

  page->getCursor()->setFrameSet(doc->getFrameSetNum(grpMgr->getFrameSet(0,0)) + 1);
  doc->drawMarker(*page->getCursor(),&p,page->getXOffset(),page->getYOffset());
  page->getCursor()->init(dynamic_cast<KWTextFrameSet*>(doc->getFrameSet(page->getCursor()->getFrameSet() - 1))->getFirstParag(),p,true,true);
  page->getCursor()->gotoStartOfParag(p);
  page->getCursor()->cursorGotoLineStart(p);
  p.end();

  doc->recalcFrames();
  doc->updateAllFrames();
  doc->updateAllViews(0L);
  page->recalcCursor();
}
