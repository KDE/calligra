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
/* class KWPagePreview                                            */
/******************************************************************/

/*================================================================*/
KWPagePreview::KWPagePreview(QWidget* parent,const char* name)
  : QGroupBox(i18n("Preview"),parent,name)
{
  left = 0;
  right = 0;
  first = 0;
  spacing = 0;
  before = 0;
  after = 0;
}

/*================================================================*/
void KWPagePreview::drawContents(QPainter* p)
{
  int wid = 148;
  int hei = 210;
  int _x = (width() - wid) / 2;
  int _y = (height() - hei) / 2;

  int dl = static_cast<int>(left / 2);
  int dr = static_cast<int>(right / 2);
  int df = static_cast<int>(first / 2);

  int spc = static_cast<int>(POINT_TO_MM(spacing) / 5);

  // draw page
  p->setPen(QPen(black));
  p->setBrush(QBrush(black));

  p->drawRect(_x + 1,_y + 1,wid,hei);
  
  p->setBrush(QBrush(white));
  p->drawRect(_x,_y,wid,hei);

  // draw parags
  p->setPen(NoPen);
  p->setBrush(QBrush(lightGray));

  for (int i = 1;i <= 4;i++)
    p->drawRect(_x + 6,_y + 6 + (i - 1) * 12 + 2,wid - 12 - ((i / 4) * 4 == i ? 50 : 0),6);
  
  p->setBrush(QBrush(darkGray));

  for (int i = 5;i <= 8;i++)
    p->drawRect((i == 5 ? df : dl) + _x + 6,_y + 6 + (i - 1) * 12 + 2 + (i - 5) * spc + static_cast<int>(before / 2),
		wid - 12 - ((i / 4) * 4 == i ? 50 : 0) - ((i == 12 ? 0 : dr) + (i == 5 ? df : dl)),6);

  p->setBrush(QBrush(lightGray));

  for (int i = 9;i <= 12;i++)
    p->drawRect(_x + 6,_y + 6 + (i - 1) * 12 + 2 + 3 * spc + static_cast<int>(before / 2) + static_cast<int>(after / 2),
		wid - 12 - ((i / 4) * 4 == i ? 50 : 0),6);

}


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
void KWParagDia::setLeftIndent(float _left)
{
  QString str;
  str.sprintf("%g",_left);
  eLeft->setText(str);
  prev1->setLeft(_left);
}

/*================================================================*/
void KWParagDia::setFirstLineIndent(float _first)
{
  QString str;
  str.sprintf("%g",_first);
  eFirstLine->setText(str);
  prev1->setFirst(_first);
}

/*================================================================*/
void KWParagDia::setSpaceBeforeParag(float _before)
{
  QString str;
  str.sprintf("%g",_before);
  eBefore->setText(str);
  prev1->setBefore(_before);
}

/*================================================================*/
void KWParagDia::setSpaceAfterParag(float _after)
{
  QString str;
  str.sprintf("%g",_after);
  eAfter->setText(str);
  prev1->setAfter(_after);
}

/*================================================================*/
void KWParagDia::setupTab1()
{
  tab1 = new QWidget(this);

  grid1 = new QGridLayout(tab1,4,2,15,7);

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
  connect(eLeft,SIGNAL(textChanged(const char*)),this,SLOT(leftChanged(const char*)));

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
  connect(eRight,SIGNAL(textChanged(const char*)),this,SLOT(rightChanged(const char*)));
  eRight->setEnabled(false);

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
  connect(eFirstLine,SIGNAL(textChanged(const char*)),this,SLOT(firstChanged(const char*)));
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
  cSpacing->insertItem(i18n("Custom (pt)"));
  cSpacing->resize(cSpacing->sizeHint());
  connect(cSpacing,SIGNAL(activated(int)),this,SLOT(spacingActivated(int)));
  spacingGrid->addWidget(cSpacing,1,0);

  eSpacing = new KRestrictedLine(spacingFrame,"","1234567890.");
  eSpacing->setText("0.00");
  eSpacing->setMaxLength(5);
  eSpacing->setEchoMode(QLineEdit::Normal);
  eSpacing->setFrame(true);
  eSpacing->resize(cSpacing->size());
  eSpacing->setEnabled(false);
  connect(eSpacing,SIGNAL(textChanged(const char*)),this,SLOT(spacingChanged(const char*)));
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

  // --------------- paragraph spacing ---------------
  pSpaceFrame = new QGroupBox(i18n("Paragraph Space"),tab1);
  pSpaceGrid = new QGridLayout(pSpaceFrame,3,2,15,7);
  
  lBefore = new QLabel(i18n("Before (mm):"),pSpaceFrame);
  lBefore->resize(lBefore->sizeHint());
  lBefore->setAlignment(AlignRight);
  pSpaceGrid->addWidget(lBefore,1,0);

  eBefore = new KRestrictedLine(pSpaceFrame,"","1234567890.");
  eBefore->setText("0.00");
  eBefore->setMaxLength(5);
  eBefore->setEchoMode(QLineEdit::Normal);
  eBefore->setFrame(true);
  eBefore->resize(eBefore->sizeHint().width() / 2,eBefore->sizeHint().height());
  connect(eBefore,SIGNAL(textChanged(const char*)),this,SLOT(beforeChanged(const char*)));
  pSpaceGrid->addWidget(eBefore,1,1);
 
  lAfter = new QLabel(i18n("After (mm):"),pSpaceFrame);
  lAfter->resize(lAfter->sizeHint());
  lAfter->setAlignment(AlignRight);
  pSpaceGrid->addWidget(lAfter,2,0);

  eAfter = new KRestrictedLine(pSpaceFrame,"","1234567890.");
  eAfter->setText("0.00");
  eAfter->setMaxLength(5);
  eAfter->setEchoMode(QLineEdit::Normal);
  eAfter->setFrame(true);
  eAfter->resize(eAfter->sizeHint().width() / 2,eAfter->sizeHint().height());
  connect(eAfter,SIGNAL(textChanged(const char*)),this,SLOT(afterChanged(const char*)));
  pSpaceGrid->addWidget(eAfter,2,1);

  // grid col spacing
  pSpaceGrid->addColSpacing(0,lBefore->width());
  pSpaceGrid->addColSpacing(0,lAfter->width());
  pSpaceGrid->addColSpacing(1,eBefore->width());
  pSpaceGrid->addColSpacing(1,eAfter->width());

  // grid row spacing
  pSpaceGrid->addRowSpacing(0,5);
  pSpaceGrid->addRowSpacing(1,eBefore->height());
  pSpaceGrid->addRowSpacing(2,eAfter->height());

  // activate grid
  pSpaceGrid->activate();
  grid1->addWidget(pSpaceFrame,2,0);

  // --------------- preview --------------------
  prev1 = new KWPagePreview(tab1,"");
  grid1->addMultiCellWidget(prev1,0,3,1,1);

  // --------------- main grid ------------------
  grid1->addColSpacing(0,indentFrame->width());
  grid1->addColSpacing(0,spacingFrame->width());
  grid1->addColSpacing(0,pSpaceFrame->width());
  grid1->addColSpacing(1,250);
  grid1->setColStretch(1,1);

  grid1->addRowSpacing(0,indentFrame->height());
  grid1->addRowSpacing(1,spacingFrame->height());
  grid1->addRowSpacing(2,pSpaceFrame->height());
  grid1->addRowSpacing(3,20);
  grid1->setRowStretch(3,1);

  grid1->activate();

  addTab(tab1,i18n("Indent and Spacing"));
}

/*================================================================*/
void KWParagDia::setupTab2()
{
}

/*================================================================*/
void KWParagDia::leftChanged(const char* _text)
{
  prev1->setLeft(atof(_text));
}

/*================================================================*/
void KWParagDia::rightChanged(const char* _text)
{
  prev1->setRight(atof(_text));
}

/*================================================================*/
void KWParagDia::firstChanged(const char* _text)
{
  prev1->setFirst(atof(_text));
}

/*================================================================*/
void KWParagDia::spacingActivated(int _index)
{
  if (_index == 4)
    {
      eSpacing->setEnabled(true);
      eSpacing->setText("12.0");
      eSpacing->setFocus();
    }
  else
    {
      eSpacing->setEnabled(false);
      switch (_index)
	{
	case 0: eSpacing->setText("14.0");
	  break;
	case 1: eSpacing->setText("28.0");
	  break;
	case 2: eSpacing->setText("42.0");
	  break;
	case 3: eSpacing->setText("56.0");
	  break;
	}
    }
  prev1->setSpacing(atof(eSpacing->text()));
}

/*================================================================*/
void KWParagDia::spacingChanged(const char* _text)
{
  prev1->setSpacing(atof(_text));
}

/*================================================================*/
void KWParagDia::beforeChanged(const char* _text)
{
  prev1->setBefore(atof(_text));
}

/*================================================================*/
void KWParagDia::afterChanged(const char* _text)
{
  prev1->setAfter(atof(_text));
}
