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

#include <kpresenter_doc.h>
#include <klocale.h>

/******************************************************************/
/* Class: KPFooterHeaderEditor                                    */
/******************************************************************/

/*================================================================*/
KPFooterHeaderEditor::KPFooterHeaderEditor(KPresenterDoc *_doc)
  : QVBox(0L), _allowClose(false)
{
  doc = _doc;

  tabwidget = new QTabWidget(this);

  setupHeader();
  setupFooter();

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
  
  QVBox *txtFrame = new QVBox(back);
  txtFrame->setMargin(2);
  txtFrame->setBackgroundColor(Qt::white);
  txtFrame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  
  doc->footer()->getKTextObject()->reparent(txtFrame,0,QPoint(0,0),true);
  doc->footer()->getKTextObject()->setBackgroundColor(Qt::white);
  
  tabwidget->addTab(back,i18n("&Footer"));
}
