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
/* Module: Paragraph Dialog                                       */
/******************************************************************/

#include "paragdia.h"
#include "paragdia.moc"

/******************************************************************/
/* Class: KWParagDia                                              */
/******************************************************************/

/*================================================================*/
KWParagDia::KWParagDia(QWidget* parent,const char* name)
  : QTabDialog(parent,name,true)
{
  setupTab1();
  setupTab2();

  setCancelButton(i18n("Cancel"));
  setOkButton(i18n("Ok"));
}

/*================================================================*/
KWParagDia::~KWParagDia()
{
}

/*================================================================*/
void KWParagDia::setupTab1()
{
  tab1 = new QWidget(this);

  grid1 = new QGridLayout(tab1,3,2,15,7);

  // --------------- indent ---------------
  indentFrame = new QGroupBox(i18n("Indent"),tab1);
  indentGrid = new QGridLayout(indentFrame,4,2,15,7);

  lLeft = new QLabel(i18n("Left (mm):"),indentFrame);
  lLeft->resize(lLeft->sizeHint());
  lLeft->setAlignment(AlignRight);
  indentGrid->addWidget(lLeft,1,0);

  eLeft = new KRestrictedLine(indentFrame,"","1234567890.");
  eLeft->setText("0.00");
  eLeft->setMaxLength(5);
  eLeft->setEchoMode(QLineEdit::Normal);
  eLeft->setFrame(true);
  eLeft->resize(eLeft->sizeHint().width() / 2,eLeft->sizeHint().height());
  indentGrid->addWidget(eLeft,1,1);

  lRight = new QLabel(i18n("Right (mm):"),indentFrame);
  lRight->resize(lRight->sizeHint());
  lRight->setAlignment(AlignRight);
  indentGrid->addWidget(lRight,2,0);

  eRight = new KRestrictedLine(indentFrame,"","1234567890.");
  eRight->setText("0.00");
  eRight->setMaxLength(5);
  eRight->setEchoMode(QLineEdit::Normal);
  eRight->setFrame(true);
  eRight->resize(eLeft->size());
  indentGrid->addWidget(eRight,2,1);

  lFirstLine = new QLabel(i18n("First Line (mm):"),indentFrame);
  lFirstLine->resize(lFirstLine->sizeHint());
  lFirstLine->setAlignment(AlignRight);
  indentGrid->addWidget(lFirstLine,3,0);

  eFirstLine = new KRestrictedLine(indentFrame,"","1234567890.");
  eFirstLine->setText("0.00");
  eFirstLine->setMaxLength(5);
  eFirstLine->setEchoMode(QLineEdit::Normal);
  eFirstLine->setFrame(true);
  eFirstLine->resize(eLeft->size());
  indentGrid->addWidget(eFirstLine,3,1);

  // grid col spacing
  indentGrid->addColSpacing(0,lFirstLine->width());
  indentGrid->addColSpacing(1,eFirstLine->width());

  // grid row spacing
  indentGrid->addRowSpacing(0,5);
  indentGrid->addRowSpacing(1,eLeft->height());
  indentGrid->addRowSpacing(1,lLeft->height());
  indentGrid->addRowSpacing(2,eRight->height());
  indentGrid->addRowSpacing(2,lRight->height());
  indentGrid->addRowSpacing(3,eFirstLine->height());
  indentGrid->addRowSpacing(3,lFirstLine->height());

  // activate grid
  indentGrid->activate();
  grid1->addWidget(indentFrame,0,0);

  // --------------- spacing ---------------
  spacingFrame = new QGroupBox(i18n("Line Spacing"),tab1);
  spacingGrid = new QGridLayout(spacingFrame,3,1,15,7);
  
  cSpacing = new QComboBox(false,spacingFrame,"");
  cSpacing->insertItem(i18n("0.5 lines"));
  cSpacing->insertItem(i18n("1.0 line"));
  cSpacing->insertItem(i18n("1.5 lines"));
  cSpacing->insertItem(i18n("2.0 lines"));
  cSpacing->insertItem(i18n("Custom (mm)"));
  cSpacing->resize(cSpacing->sizeHint());
  spacingGrid->addWidget(cSpacing,1,0);

  eSpacing = new KRestrictedLine(spacingFrame,"","1234567890.");
  eSpacing->setText("0.00");
  eSpacing->setMaxLength(5);
  eSpacing->setEchoMode(QLineEdit::Normal);
  eSpacing->setFrame(true);
  eSpacing->resize(cSpacing->size());
  eSpacing->setEnabled(false);
  spacingGrid->addWidget(eSpacing,2,0);

  // grid col spacing
  spacingGrid->addColSpacing(0,cSpacing->width());

  // grid row spacing
  spacingGrid->addRowSpacing(0,5);
  spacingGrid->addRowSpacing(1,cSpacing->height());
  spacingGrid->addRowSpacing(2,eSpacing->height());

  // activate grid
  spacingGrid->activate();
  grid1->addWidget(spacingFrame,1,0);

  // --------------- main grid ------------------
  grid1->addColSpacing(0,indentFrame->width());
  grid1->setColStretch(1,1);

  grid1->addRowSpacing(0,indentFrame->height());
  grid1->setRowStretch(2,1);

  grid1->activate();

  addTab(tab1,i18n("Indent and Spacing"));
}

/*================================================================*/
void KWParagDia::setupTab2()
{
}
