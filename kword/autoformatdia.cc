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
/* Module: Autoformat Dialog                                      */
/******************************************************************/

#include "kword_doc.h"
#include "kword_page.h"

#include "autoformatdia.h"
#include "autoformatdia.moc"

#include "autoformat.h"

#include <klocale.h>

/******************************************************************/
/* Class: KWAutoFormatDia                                         */
/******************************************************************/

/*================================================================*/
KWAutoFormatDia::KWAutoFormatDia(QWidget *parent,const char *name,KWordDocument *_doc,KWPage *_page)
  : QTabDialog(parent,name,true)
{
  doc = _doc;
  page = _page;

  setupTab1();

  setCancelButton(i18n("Cancel"));
  setOkButton(i18n("OK"));

  resize(300,250);
  connect(this,SIGNAL(applyButtonPressed()),this,SLOT(applyConfig()));
}

/*================================================================*/
void KWAutoFormatDia::setupTab1()
{
  tab1 = new QWidget(this);

  addTab(tab1,i18n("Simpe Autoformat"));

  resize(minimumSize());
}

/*================================================================*/
void KWAutoFormatDia::applyConfig()
{
}
