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

#include <qtabwidget.h>
#include <qpoint.h>
#include <qcolor.h>
#include <qcheckbox.h>
#include <qpushbutton.h>

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
void KPFooterHeaderEditor::setupHeader()
{
  QVBox *back = new QVBox(tabwidget);

  showHeader = new QCheckBox(i18n("Show Header"),back);
  connect(showHeader,SIGNAL(clicked()),this,SLOT(slotShowHeader()));
  
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
  
  showFooter = new QCheckBox(i18n("Show Footer"),back);
  connect(showFooter,SIGNAL(clicked()),this,SLOT(slotShowFooter()));
  
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
  slotUpdatePage();
}

/*================================================================*/
void KPFooterHeaderEditor::slotShowFooter()
{
  //showFooter->setChecked(!showFooter->isChecked());
  doc->setFooter(showFooter->isChecked());
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
