/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Footer/Header                                          */
/******************************************************************/

#include "footer_header.h"
#include "footer_header.moc"
#include "kptextobject.h"
#include "ktextobject.h"
#include "styledia.h"
#include "global.h"

#include <qtabwidget.h>
#include <qpoint.h>
#include <qcolor.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qhbox.h>

#include <kpresenter_doc.h>
#include <klocale.h>
#include <kbuttonbox.h>

/******************************************************************/
/* Class: KPFooterHeaderEditor                                    */
/******************************************************************/

/*================================================================*/
KPFooterHeaderEditor::KPFooterHeaderEditor(KPresenterDoc *_doc)
  : QVBox(0L), _allowClose(false)
{
  setMargin(10);

  doc = _doc;

  tabwidget = new QTabWidget(this);

  setupHeader();
  setupFooter();

  QWidget *w = new QWidget(this);
  w->setMaximumHeight(10);
  w->setMinimumHeight(10);

  KButtonBox *bb = new KButtonBox(this);
  bb->addStretch();
  updatePage = bb->addButton(i18n("Update Page"));
  connect(updatePage,SIGNAL(clicked()),this,SLOT(slotUpdatePage()));
  closeDia = bb->addButton(i18n("Close"));
  connect(closeDia,SIGNAL(clicked()),this,SLOT(slotCloseDia()));

  bb->layout();
  bb->setMaximumHeight(bb->sizeHint().height());

  resize(500,300);
}

/*================================================================*/
KPFooterHeaderEditor::~KPFooterHeaderEditor()
{
  doc->header()->getKTextObject()->reparent(0L,0,QPoint(0,0),false);
  doc->footer()->getKTextObject()->reparent(0L,0,QPoint(0,0),false);
}

/*================================================================*/
void KPFooterHeaderEditor::updateSizes()
{
  updateGeometry();
}

/*================================================================*/
void KPFooterHeaderEditor::setupHeader()
{
  QVBox *back = new QVBox(tabwidget);

  QHBox *tool1 = new QHBox(back);
  tool1->setMargin(5);
  tool1->setSpacing(5);

  showHeader = new QCheckBox(i18n("Show Header"),tool1);
  connect(showHeader,SIGNAL(clicked()),this,SLOT(slotShowHeader()));

  penBrush1 = new QPushButton(i18n("Configure Frame and Background..."),tool1);
  connect(penBrush1,SIGNAL(clicked()),this,SLOT(slotHeaderPenBrush()));
  penBrush1->setEnabled(false);

  tool1->setMaximumHeight(penBrush1->sizeHint().height() + 10);

  QVBox *txtFrame = new QVBox(back);
  txtFrame->setMargin(2);
  txtFrame->setBackgroundColor(Qt::white);
  txtFrame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);

  doc->header()->getKTextObject()->reparent(txtFrame,0,QPoint(0,0),true);
  doc->header()->getKTextObject()->setBackgroundColor(Qt::white);

  tabwidget->addTab(back,i18n("&Header"));
}

/*================================================================*/
void KPFooterHeaderEditor::setupFooter()
{
  QVBox *back = new QVBox(tabwidget);

  QHBox *tool1 = new QHBox(back);
  tool1->setMargin(5);
  tool1->setSpacing(5);

  showFooter = new QCheckBox(i18n("Show Footer"),tool1);
  connect(showFooter,SIGNAL(clicked()),this,SLOT(slotShowFooter()));

  penBrush2 = new QPushButton(i18n("Configure Frame and Background..."),tool1);
  connect(penBrush2,SIGNAL(clicked()),this,SLOT(slotFooterPenBrush()));
  penBrush2->setEnabled(false);

  tool1->setMaximumHeight(penBrush2->sizeHint().height() + 10);

  QVBox *txtFrame = new QVBox(back);
  txtFrame->setMargin(2);
  txtFrame->setBackgroundColor(Qt::white);
  txtFrame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);

  doc->footer()->getKTextObject()->reparent(txtFrame,0,QPoint(0,0),true);
  doc->footer()->getKTextObject()->setBackgroundColor(Qt::white);

  tabwidget->addTab(back,i18n("&Footer"));
}

/*================================================================*/
void KPFooterHeaderEditor::slotShowHeader()
{
  //showHeader->setChecked(!showHeader->isChecked());
  doc->setHeader(showHeader->isChecked());
  penBrush1->setEnabled(showHeader->isChecked());
  slotUpdatePage();
}

/*================================================================*/
void KPFooterHeaderEditor::slotShowFooter()
{
  //showFooter->setChecked(!showFooter->isChecked());
  doc->setFooter(showFooter->isChecked());
  penBrush2->setEnabled(showFooter->isChecked());
  slotUpdatePage();
}

/*================================================================*/
void KPFooterHeaderEditor::slotUpdatePage()
{
  doc->repaint(false);
}

/*================================================================*/
void KPFooterHeaderEditor::slotCloseDia()
{
  hide();
}

/*================================================================*/
void KPFooterHeaderEditor::slotHeaderPenBrush()
{
  KPTextObject *header = doc->header();

  StyleDia *styleDia = new StyleDia(0,"StyleDia",SD_PEN | SD_BRUSH);
  styleDia->setMaximumSize(styleDia->width(),styleDia->height());
  styleDia->setMinimumSize(styleDia->width(),styleDia->height());
  styleDia->setPen(header->getPen());
  styleDia->setBrush(header->getBrush());
  styleDia->setLineBegin(L_NORMAL);
  styleDia->setLineEnd(L_NORMAL);
  styleDia->setFillType(header->getFillType());
  styleDia->setGradient(header->getGColor1(),
			header->getGColor2(),
			header->getGType());
  styleDia->setCaption(i18n("Configure Header Frame and Background"));

  if (styleDia->exec() == QDialog::Accepted)
    {
      header->setPen(styleDia->getPen());
      header->setBrush(styleDia->getBrush());
      header->setFillType(styleDia->getFillType());
      header->setGColor1(styleDia->getGColor2());
      header->setGColor2(styleDia->getGColor1());
      header->setGType(styleDia->getGType());
    }

  delete styleDia;
  slotUpdatePage();
}

/*================================================================*/
void KPFooterHeaderEditor::slotFooterPenBrush()
{
  KPTextObject *footer = doc->footer();

  StyleDia *styleDia = new StyleDia(0,"StyleDia",SD_PEN | SD_BRUSH);
  styleDia->setMaximumSize(styleDia->width(),styleDia->height());
  styleDia->setMinimumSize(styleDia->width(),styleDia->height());
  styleDia->setPen(footer->getPen());
  styleDia->setBrush(footer->getBrush());
  styleDia->setLineBegin(L_NORMAL);
  styleDia->setLineEnd(L_NORMAL);
  styleDia->setFillType(footer->getFillType());
  styleDia->setGradient(footer->getGColor1(),
			footer->getGColor2(),
			footer->getGType());
  styleDia->setCaption(i18n("Configure Footer Frame and Background"));

  if (styleDia->exec() == QDialog::Accepted)
    {
      footer->setPen(styleDia->getPen());
      footer->setBrush(styleDia->getBrush());
      footer->setFillType(styleDia->getFillType());
      footer->setGColor1(styleDia->getGColor2());
      footer->setGColor2(styleDia->getGColor1());
      footer->setGType(styleDia->getGType());
    }

  delete styleDia;
  slotUpdatePage();
}
