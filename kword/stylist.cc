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
/* Module: Stylist                                                */
/******************************************************************/

#include "kword_doc.h"
#include "stylist.h"
#include "stylist.moc"

/******************************************************************/
/* Class: KWStyleManager                                          */
/******************************************************************/

/*================================================================*/
KWStyleManager::KWStyleManager(QWidget *_parent,KWordDocument *_doc)
  : QTabDialog(_parent,"",true)
{
  doc = _doc;
  setupTab1();

  setCancelButton(i18n("Cancel"));
  setOkButton(i18n("OK"));
}

/*================================================================*/
void KWStyleManager::setupTab1()
{
  tab1 = new QWidget(this);

  grid1 = new QGridLayout(tab1,1,2,15,7);

  lStyleList = new QListBox(tab1);
  for (unsigned int i = 0;i < doc->paragLayoutList.count();i++)
    lStyleList->insertItem(doc->paragLayoutList.at(i)->getName());
  grid1->addWidget(lStyleList,0,0);

  bButtonBox = new KButtonBox(tab1,KButtonBox::VERTICAL);
  bAdd = bButtonBox->addButton(i18n("&Add.."),false);
  bDelete = bButtonBox->addButton(i18n("&Delete"),false);
  bButtonBox->addStretch();
  bEdit = bButtonBox->addButton(i18n("&Edit..."),false);
  bCopy = bButtonBox->addButton(i18n("&Copy..."),false); 
  bButtonBox->addStretch();
  bUp = bButtonBox->addButton(i18n("Up"),false); 
  bDown = bButtonBox->addButton(i18n("D&own"),false); 
  bButtonBox->layout();
  grid1->addWidget(bButtonBox,0,1);

  grid1->addColSpacing(0,lStyleList->width());
  grid1->addColSpacing(1,bButtonBox->width());
  grid1->setColStretch(0,1);

  grid1->addRowSpacing(0,lStyleList->height());
  grid1->addRowSpacing(0,bButtonBox->height());
  grid1->setRowStretch(0,1);

  grid1->activate();

  addTab(tab1,i18n("Style Manager"));
}

