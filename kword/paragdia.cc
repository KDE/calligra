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
/* class KWPagePreview2                                           */
/******************************************************************/

/*================================================================*/
KWPagePreview2::KWPagePreview2(QWidget* parent,const char* name)
  : QGroupBox(i18n("Preview"),parent,name)
{
  flow = KWParagLayout::LEFT;
}

/*================================================================*/
void KWPagePreview2::drawContents(QPainter* p)
{
  int wid = 148;
  int hei = 210;
  int _x = (width() - wid) / 2;
  int _y = (height() - hei) / 2;

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

  int __x = 0,__w = 0;
  for (int i = 5;i <= 8;i++)
    {
      switch (i)
	{
	case 5: __w = wid - 12;
	  break;
	case 6: __w = wid - 52;
	  break;
	case 7: __w = wid - 33;
	  break;
	case 8: __w = wid - 62;
	default: break;
	}

      switch (flow)
	{
	case KWParagLayout::LEFT:
	  __x = _x + 6;
	  break;
	case KWParagLayout::CENTER:
	  __x = _x + (wid - __w) / 2;
	  break;
	case KWParagLayout::RIGHT:
	  __x = _x + (wid - __w) - 6;
	  break;
	case KWParagLayout::BLOCK:
	  {
	    if (i < 8) __w = wid - 12;
	    __x = _x + 6;
	  } break;
	}

      p->drawRect(__x,_y + 6 + (i - 1) * 12 + 2 + (i - 5),__w,6);
    }

  p->setBrush(QBrush(lightGray));

  for (int i = 9;i <= 12;i++)
    p->drawRect(_x + 6,_y + 6 + (i - 1) * 12 + 2 + 3,wid - 12 - ((i / 4) * 4 == i ? 50 : 0),6);

}

/******************************************************************/
/* class KWBorderPreview                                          */
/******************************************************************/

/*================================================================*/
KWBorderPreview::KWBorderPreview(QWidget* parent,const char* name)
  : QGroupBox(i18n("Preview"),parent,name)
{
}

/*================================================================*/
void KWBorderPreview::drawContents(QPainter* painter)
{
  QRect r = contentsRect();
  QFontMetrics fm(font());

  painter->fillRect(r.x() + fm.width('W'),r.y() + fm.height(),r.width() - 2 * fm.width('W'),r.height() - 2 * fm.height(),white);
  painter->setClipRect(r.x() + fm.width('W'),r.y() + fm.height(),r.width() - 2 * fm.width('W'),r.height() - 2 * fm.height());

  if (topBorder.ptWidth > 0)
    {
      painter->setPen(setBorderPen(topBorder));
      painter->drawLine(r.x() + 20,r.y() + 20,r.right() - 20,r.y() + 20);
    }

  if (bottomBorder.ptWidth > 0)
    {
      painter->setPen(setBorderPen(bottomBorder));
      painter->drawLine(r.x() + 20,r.bottom() - 20,r.right() - 20,r.bottom() - 20);
    }

  if (leftBorder.ptWidth > 0)
    {
      painter->setPen(setBorderPen(leftBorder));
      painter->drawLine(r.x() + 20,r.y() + 20,r.x() + 20,r.bottom() - 20);
    }

  if (rightBorder.ptWidth > 0)
    {
      painter->setPen(setBorderPen(rightBorder));
      painter->drawLine(r.right() - 20,r.y() + 20,r.right() - 20,r.bottom() - 20);
    }
}

/*================================================================*/
QPen KWBorderPreview::setBorderPen(KWParagLayout::Border _brd)
{
  QPen pen(black,1,SolidLine);

  pen.setWidth(_brd.ptWidth);
  pen.setColor(_brd.color);
  
  switch (_brd.style)
    {
    case KWParagLayout::SOLID:
      pen.setStyle(SolidLine);
      break;
    case KWParagLayout::DASH:
      pen.setStyle(DashLine);
      break;
    case KWParagLayout::DOT:
      pen.setStyle(DotLine);
      break;
    case KWParagLayout::DASH_DOT:
      pen.setStyle(DashDotLine);
      break;
    case KWParagLayout::DASH_DOT_DOT:
      pen.setStyle(DashDotDotLine);
      break;
    }

  return QPen(pen);
}

/******************************************************************/
/* class KWNumPreview                                             */
/******************************************************************/

/*================================================================*/
KWNumPreview::KWNumPreview(QWidget* parent,const char* name)
  : QGroupBox(i18n("Preview"),parent,name)
{
}

/*================================================================*/
void KWNumPreview::drawContents(QPainter* painter)
{
}

/******************************************************************/
/* Class: KWParagDia                                              */
/******************************************************************/

/*================================================================*/
KWParagDia::KWParagDia(QWidget* parent,const char* name,int _flags = PD_SPACING | PD_FLOW | PD_BORDERS | PD_NUMBERING)
  : QTabDialog(parent,name,true)
{
  flags = _flags;

  if (_flags & PD_SPACING)
    setupTab1();
  if (_flags & PD_FLOW)
    setupTab2();
  if (_flags & PD_BORDERS)
    setupTab3();
  if (_flags & PD_NUMBERING)
    setupTab4();

  setCancelButton(i18n("Cancel"));
  setOkButton(i18n("OK"));
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
void KWParagDia::setLineSpacing(unsigned int  _spacing)
{
  QString str;
  str.sprintf("%d",_spacing);
  eSpacing->setText(str);
}

/*================================================================*/
void KWParagDia::setFlow(KWParagLayout::Flow _flow)
{
  prev2->setFlow(_flow);

  clearFlows();
  switch (_flow)
    {
    case KWParagLayout::LEFT:
      rLeft->setChecked(true);
      break;
    case KWParagLayout::CENTER:
      rCenter->setChecked(true);
      break;
    case KWParagLayout::RIGHT:
      rRight->setChecked(true);
      break;
    case KWParagLayout::BLOCK:
      rBlock->setChecked(true);
      break;
    }
}

/*================================================================*/
KWParagLayout::Flow KWParagDia::getFlow()
{
  if (rLeft->isChecked()) return KWParagLayout::LEFT;
  else if (rCenter->isChecked()) return KWParagLayout::CENTER;
  else if (rRight->isChecked()) return KWParagLayout::RIGHT;
  else if (rBlock->isChecked()) return KWParagLayout::BLOCK;
  
  return KWParagLayout::LEFT;
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
  cSpacing->insertItem(i18n("Space (pt)"));
  cSpacing->resize(cSpacing->sizeHint());
  connect(cSpacing,SIGNAL(activated(int)),this,SLOT(spacingActivated(int)));
  spacingGrid->addWidget(cSpacing,1,0);

  eSpacing = new KRestrictedLine(spacingFrame,"","1234567890");
  eSpacing->setText("0");
  eSpacing->setMaxLength(2);
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


  cSpacing->setCurrentItem(4);
  cSpacing->setEnabled(false);
  eSpacing->setEnabled(true);

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
  tab2 = new QWidget(this);

  grid2 = new QGridLayout(tab2,6,2,15,7);

  lFlow = new QLabel(i18n("Flow:"),tab2);
  lFlow->resize(lFlow->sizeHint());
  grid2->addWidget(lFlow,0,0);

  rLeft = new QRadioButton(i18n("Left"),tab2);
  rLeft->resize(rLeft->sizeHint());
  grid2->addWidget(rLeft,1,0);
  connect(rLeft,SIGNAL(clicked()),this,SLOT(flowLeft()));

  rCenter = new QRadioButton(i18n("Center"),tab2);
  rCenter->resize(rCenter->sizeHint());
  grid2->addWidget(rCenter,2,0);
  connect(rCenter,SIGNAL(clicked()),this,SLOT(flowCenter()));

  rRight = new QRadioButton(i18n("Right"),tab2);
  rRight->resize(rRight->sizeHint());
  grid2->addWidget(rRight,3,0);
  connect(rRight,SIGNAL(clicked()),this,SLOT(flowRight()));

  rBlock = new QRadioButton(i18n("Block"),tab2);
  rBlock->resize(rBlock->sizeHint());
  grid2->addWidget(rBlock,4,0);
  connect(rBlock,SIGNAL(clicked()),this,SLOT(flowBlock()));

  clearFlows();
  rLeft->setChecked(true);

  // --------------- preview --------------------
  prev2 = new KWPagePreview2(tab2,"");
  grid2->addMultiCellWidget(prev2,0,5,1,1);

  // --------------- main grid ------------------
  grid2->addColSpacing(0,lFlow->width());
  grid2->addColSpacing(0,rLeft->width());
  grid2->addColSpacing(0,rCenter->width());
  grid2->addColSpacing(0,rRight->width());
  grid2->addColSpacing(0,rBlock->width());
  grid2->addColSpacing(1,250);
  grid2->setColStretch(1,1);

  grid2->addRowSpacing(0,lFlow->height());
  grid2->addRowSpacing(1,rLeft->height());
  grid2->addRowSpacing(2,rCenter->height());
  grid2->addRowSpacing(3,rRight->height());
  grid2->addRowSpacing(4,rBlock->height());
  grid2->addRowSpacing(5,20);
  grid2->setRowStretch(5,1);

  grid2->activate();

  addTab(tab2,i18n("Flows"));
}

/*================================================================*/
void KWParagDia::setupTab3()
{
  tab3 = new QWidget(this);

  grid3 = new QGridLayout(tab3,8,2,15,7);

  lStyle = new QLabel(i18n("Style:"),tab3);
  lStyle->resize(lStyle->sizeHint());
  grid3->addWidget(lStyle,0,0);

  cStyle = new QComboBox(false,tab3);
  cStyle->insertItem(i18n("solid line"));
  cStyle->insertItem(i18n("dash line (----)"));
  cStyle->insertItem(i18n("dot line (****)"));
  cStyle->insertItem(i18n("dash dot line (-*-*)"));
  cStyle->insertItem(i18n("dash dot dot line (-**-)"));
  cStyle->resize(cStyle->sizeHint());
  grid3->addWidget(cStyle,1,0);
  connect(cStyle,SIGNAL(activated(const char*)),this,SLOT(brdStyleChanged(const char*)));

  lWidth = new QLabel(i18n("Width:"),tab3);
  lWidth->resize(lWidth->sizeHint());
  grid3->addWidget(lWidth,2,0);

  cWidth = new QComboBox(false,tab3);
  for(unsigned int i = 1;i <= 10;i++)
    {
      char buffer[10];
      sprintf(buffer,"%i",i);
      cWidth->insertItem(buffer);
    }
  grid3->addWidget(cWidth,3,0);
  cWidth->resize(cStyle->size());
  connect(cWidth,SIGNAL(activated(const char*)),this,SLOT(brdWidthChanged(const char*)));

  lColor = new QLabel(i18n("Color:"),tab3);
  lColor->resize(lColor->sizeHint());
  grid3->addWidget(lColor,4,0);
  
  bColor = new KColorButton(tab3,"");
  bColor->resize(bColor->sizeHint());
  grid3->addWidget(bColor,5,0);
  connect(bColor,SIGNAL(changed(const QColor&)),this,SLOT(brdColorChanged(const QColor&)));
  
  bb = new KButtonBox(tab3);
  bb->addStretch();
  bLeft = bb->addButton("",true);
  bLeft->setPixmap(QPixmap(kapp->kde_datadir() + "/kword/toolbar/borderleft.xpm"));
  bLeft->setToggleButton(true);
  bRight = bb->addButton("",true);
  bRight->setPixmap(QPixmap(kapp->kde_datadir() + "/kword/toolbar/borderright.xpm"));
  bRight->setToggleButton(true);
  bTop = bb->addButton("",true);
  bTop->setPixmap(QPixmap(kapp->kde_datadir() + "/kword/toolbar/bordertop.xpm"));
  bTop->setToggleButton(true);
  bBottom = bb->addButton("",true);
  bBottom->setPixmap(QPixmap(kapp->kde_datadir() + "/kword/toolbar/borderbottom.xpm"));
  bBottom->setToggleButton(true);
  bb->addStretch();
  bb->layout();
  grid3->addWidget(bb,6,0);

  connect(bLeft,SIGNAL(toggled(bool)),this,SLOT(brdLeftToggled(bool)));
  connect(bRight,SIGNAL(toggled(bool)),this,SLOT(brdRightToggled(bool)));
  connect(bTop,SIGNAL(toggled(bool)),this,SLOT(brdTopToggled(bool)));
  connect(bBottom,SIGNAL(toggled(bool)),this,SLOT(brdBottomToggled(bool)));

  prev3 = new KWBorderPreview(tab3,"");
  grid3->addMultiCellWidget(prev3,0,7,1,1);

  grid3->addRowSpacing(0,lStyle->height());
  grid3->addRowSpacing(1,cStyle->height());
  grid3->addRowSpacing(2,lWidth->height());
  grid3->addRowSpacing(3,cWidth->height());
  grid3->addRowSpacing(4,lColor->height());
  grid3->addRowSpacing(5,bColor->height());
  grid3->addRowSpacing(6,bb->height());
  grid3->setRowStretch(0,0);
  grid3->setRowStretch(1,0);
  grid3->setRowStretch(2,0);
  grid3->setRowStretch(3,0);
  grid3->setRowStretch(4,0);
  grid3->setRowStretch(5,0);
  grid3->setRowStretch(6,0);
  grid3->setRowStretch(7,1);

  grid3->addColSpacing(0,lStyle->width());
  grid3->addColSpacing(0,cStyle->width());
  grid3->addColSpacing(0,lWidth->width());
  grid3->addColSpacing(0,cWidth->width());
  grid3->addColSpacing(0,lColor->width());
  grid3->addColSpacing(0,bColor->width());
  grid3->addColSpacing(0,bb->width());
  grid3->addColSpacing(1,250);
  grid3->setColStretch(0,0);
  grid3->setColStretch(1,1);

  grid3->activate();

  addTab(tab3,i18n("Borders"));
}

/*================================================================*/
void KWParagDia::setupTab4()
{
  tab4 = new QWidget(this);

  grid4 = new QGridLayout(tab4,2,2,15,7);

  gType = new QGroupBox("Configuration",tab4);
  tgrid = new QGridLayout(gType,10,2,5,5);
  
  rANums = new QRadioButton(i18n("&Arabic Numbers (1, 2, 3, 4, ...)"),gType);
  rANums->resize(rANums->sizeHint());
  tgrid->addWidget(rANums,1,0);

  rLRNums = new QRadioButton(i18n("&Lower Roman Numbers (i, ii, iii, iv, ...)"),gType);
  rLRNums->resize(rLRNums->sizeHint());
  tgrid->addWidget(rLRNums,2,0);

  rURNums = new QRadioButton(i18n("&Upper Roman Numbers (I, II, III, IV, ...)"),gType);
  rURNums->resize(rURNums->sizeHint());
  tgrid->addWidget(rURNums,3,0);

  rLAlph = new QRadioButton(i18n("L&ower Alphabetical (a, b, c, d, ...)"),gType);
  rLAlph->resize(rLAlph->sizeHint());
  tgrid->addWidget(rLAlph,4,0);

  rUAlph = new QRadioButton(i18n("U&pper Alphabetical (A, B, C, D, ...)"),gType);
  rUAlph->resize(rUAlph->sizeHint());
  tgrid->addWidget(rUAlph,5,0);

  rBullets = new QRadioButton(i18n("&Bullets"),gType);
  rBullets->resize(rBullets->sizeHint());
  tgrid->addWidget(rBullets,6,0);

  bBullets = new QPushButton(gType);
  bBullets->resize(30,30);
  tgrid->addWidget(bBullets,6,1);

  lDepth = new QLabel(i18n("Depth:"),gType);
  lDepth->setAlignment(AlignRight | AlignVCenter);
  lDepth->resize(lDepth->sizeHint());
  tgrid->addWidget(lDepth,7,0);

  sDepth = new KNumericSpinBox(gType);
  sDepth->setRange(1,16);
  sDepth->setEditable(false);
  sDepth->resize(sDepth->sizeHint().width() / 2,sDepth->sizeHint().height());
  tgrid->addWidget(sDepth,7,1);

  rList = new QRadioButton(i18n("&List Numbering"),gType);
  rList->resize(rList->sizeHint());
  tgrid->addWidget(rList,8,0);

  rChapter = new QRadioButton(i18n("&Chapter Numbering"),gType);
  rChapter->resize(rChapter->sizeHint());
  tgrid->addWidget(rChapter,8,1);

  tgrid->addRowSpacing(0,7);
  tgrid->addRowSpacing(1,rANums->height());
  tgrid->addRowSpacing(2,rLRNums->height());
  tgrid->addRowSpacing(3,rURNums->height());
  tgrid->addRowSpacing(4,rLAlph->height());
  tgrid->addRowSpacing(5,rUAlph->height());
  tgrid->addRowSpacing(6,rBullets->height());
  tgrid->addRowSpacing(6,bBullets->height());
  tgrid->addRowSpacing(7,lDepth->height());
  tgrid->addRowSpacing(7,sDepth->height());
  tgrid->addRowSpacing(8,rList->height());
  tgrid->addRowSpacing(8,rChapter->height());
  tgrid->setRowStretch(0,0);
  tgrid->setRowStretch(1,0);
  tgrid->setRowStretch(2,0);
  tgrid->setRowStretch(3,0);
  tgrid->setRowStretch(4,0);
  tgrid->setRowStretch(5,0);
  tgrid->setRowStretch(6,0);
  tgrid->setRowStretch(7,0);
  tgrid->setRowStretch(8,0);
  tgrid->setRowStretch(9,1);

  tgrid->addColSpacing(0,rANums->width());
  tgrid->addColSpacing(0,rLRNums->width());
  tgrid->addColSpacing(0,rURNums->width());
  tgrid->addColSpacing(0,rLAlph->width());
  tgrid->addColSpacing(0,rUAlph->width());
  tgrid->addColSpacing(0,rBullets->width());
  tgrid->addColSpacing(0,lDepth->width());
  tgrid->addColSpacing(0,rList->width());
  tgrid->addColSpacing(1,bBullets->width());
  tgrid->addColSpacing(1,sDepth->width());
  tgrid->addColSpacing(1,rChapter->width());
  tgrid->setColStretch(0,0);
  tgrid->setColStretch(1,1);

  tgrid->activate();

  grid4->addWidget(gType,0,0);

  prev4 = new KWNumPreview(tab4,"");
  grid4->addMultiCellWidget(prev4,0,1,1,1);

  grid4->addRowSpacing(0,gType->height());
  grid4->setRowStretch(0,0);
  grid4->setRowStretch(1,1);

  grid4->addColSpacing(0,gType->width());
  grid4->addColSpacing(1,250);
  grid4->setColStretch(0,1);
  grid4->setColStretch(1,1);

  grid4->activate();

  addTab(tab4,i18n("Numbering"));
}

/*================================================================*/
void KWParagDia::clearFlows()
{
  rLeft->setChecked(false);
  rCenter->setChecked(false);
  rRight->setChecked(false);
  rBlock->setChecked(false);
}

/*================================================================*/
void KWParagDia::updateBorders()
{
  if (leftBorder.ptWidth == 0)
    bLeft->setOn(false);
  else
    bLeft->setOn(true);

  if (rightBorder.ptWidth == 0)
    bRight->setOn(false);
  else
    bRight->setOn(true);

  if (topBorder.ptWidth == 0)
    bTop->setOn(false);
  else
    bTop->setOn(true);

  if (bottomBorder.ptWidth == 0)
    bBottom->setOn(false);
  else
    bBottom->setOn(true);

  prev3->setLeftBorder(leftBorder);
  prev3->setRightBorder(rightBorder);
  prev3->setTopBorder(topBorder);
  prev3->setBottomBorder(bottomBorder);
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

/*================================================================*/
void KWParagDia::flowLeft()
{
  prev2->setFlow(KWParagLayout::LEFT);
  clearFlows();
  rLeft->setChecked(true);
}

/*================================================================*/
void KWParagDia::flowCenter()
{
  prev2->setFlow(KWParagLayout::CENTER);
  clearFlows();
  rCenter->setChecked(true);
}

/*================================================================*/
void KWParagDia::flowRight()
{
  prev2->setFlow(KWParagLayout::RIGHT);
  clearFlows();
  rRight->setChecked(true);
}

/*================================================================*/
void KWParagDia::flowBlock()
{
  prev2->setFlow(KWParagLayout::BLOCK);
  clearFlows();
  rBlock->setChecked(true);
}

/*================================================================*/
void KWParagDia::brdLeftToggled(bool _on)
{
  if (!_on)
    leftBorder.ptWidth = 0;
  else
    {
      leftBorder.ptWidth = atoi(cWidth->currentText());
      leftBorder.color = QColor(bColor->color());
      QString stl(cStyle->currentText());

      if (stl == i18n("solid line"))
	leftBorder.style = KWParagLayout::SOLID;
      else if (stl == i18n("dash line (----)"))
	leftBorder.style = KWParagLayout::DASH;
      else if (stl == i18n("dot line (****)"))
	leftBorder.style = KWParagLayout::DOT;
      else if (stl == i18n("dash dot line (-*-*)"))
	leftBorder.style = KWParagLayout::DASH_DOT;
      else if (stl == i18n("dash dot dot line (-**-)"))
	leftBorder.style = KWParagLayout::DASH_DOT_DOT;
    }
  prev3->setLeftBorder(leftBorder);
}

/*================================================================*/
void KWParagDia::brdRightToggled(bool _on)
{
  if (!_on)
    rightBorder.ptWidth = 0;
  else
    {
      rightBorder.ptWidth = atoi(cWidth->currentText());
      rightBorder.color = QColor(bColor->color());
      QString stl(cStyle->currentText());

      if (stl == i18n("solid line"))
	rightBorder.style = KWParagLayout::SOLID;
      else if (stl == i18n("dash line (----)"))
	rightBorder.style = KWParagLayout::DASH;
      else if (stl == i18n("dot line (****)"))
	rightBorder.style = KWParagLayout::DOT;
      else if (stl == i18n("dash dot line (-*-*)"))
	rightBorder.style = KWParagLayout::DASH_DOT;
      else if (stl == i18n("dash dot dot line (-**-)"))
	rightBorder.style = KWParagLayout::DASH_DOT_DOT;
    }
  prev3->setRightBorder(rightBorder);
}

/*================================================================*/
void KWParagDia::brdTopToggled(bool _on)
{
  if (!_on)
    topBorder.ptWidth = 0;
  else
    {
      topBorder.ptWidth = atoi(cWidth->currentText());
      topBorder.color = QColor(bColor->color());
      QString stl(cStyle->currentText());

      if (stl == i18n("solid line"))
	topBorder.style = KWParagLayout::SOLID;
      else if (stl == i18n("dash line (----)"))
	topBorder.style = KWParagLayout::DASH;
      else if (stl == i18n("dot line (****)"))
	topBorder.style = KWParagLayout::DOT;
      else if (stl == i18n("dash dot line (-*-*)"))
	topBorder.style = KWParagLayout::DASH_DOT;
      else if (stl == i18n("dash dot dot line (-**-)"))
	topBorder.style = KWParagLayout::DASH_DOT_DOT;
    }
  prev3->setTopBorder(topBorder);
}

/*================================================================*/
void KWParagDia::brdBottomToggled(bool _on)
{
  if (!_on)
    bottomBorder.ptWidth = 0;
  else
    {
      bottomBorder.ptWidth = atoi(cWidth->currentText());
      bottomBorder.color = QColor(bColor->color());
      QString stl(cStyle->currentText());

      if (stl == i18n("solid line"))
	bottomBorder.style = KWParagLayout::SOLID;
      else if (stl == i18n("dash line (----)"))
	bottomBorder.style = KWParagLayout::DASH;
      else if (stl == i18n("dot line (****)"))
	bottomBorder.style = KWParagLayout::DOT;
      else if (stl == i18n("dash dot line (-*-*)"))
	bottomBorder.style = KWParagLayout::DASH_DOT;
      else if (stl == i18n("dash dot dot line (-**-)"))
	bottomBorder.style = KWParagLayout::DASH_DOT_DOT;
    }
  prev3->setBottomBorder(bottomBorder);
}

/*================================================================*/
void KWParagDia::brdStyleChanged(const char *_style)
{
}

/*================================================================*/
void KWParagDia::brdWidthChanged(const char *_width)
{
}

/*================================================================*/
void KWParagDia::brdColorChanged(const QColor &_color)
{
}

