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
/* Module: Search Dialog                                          */
/******************************************************************/

#include "kword_doc.h"
#include "kword_page.h"
#include "kword_view.h"

#include "searchdia.h"
#include "searchdia.moc"

/******************************************************************/
/* Class: KWSearchDia                                             */
/******************************************************************/

/*================================================================*/
KWSearchDia::KWSearchDia(QWidget* parent,const char* name,KWordDocument *_doc,KWPage *_page,KWordView *_view,
			 KWSearchEntry *_searchEntry,QStrList _fontlist)
  : QTabDialog(parent,name,false)
{
  doc = _doc;
  page = _page;
  view = _view;
  searchEntry = _searchEntry;
  fontlist = _fontlist;

  if (!searchEntry)
    {
      searchEntry = new KWSearchEntry();
      view->setSearchEntry(searchEntry);
    }
  
  setupTab1();
    
  setCancelButton(i18n("Close"));
  setOkButton(0L);
}

/*================================================================*/
void KWSearchDia::setupTab1()
{
  tab1 = new QWidget(this);

  grid1 = new QGridLayout(tab1,2,2,15,7);

  /**
   * ---------------- search group --------------------
   */

  gSearch = new QGroupBox(i18n("Search"),tab1);
  sGrid = new QGridLayout(gSearch,9,3,15,7);

  lSearch = new QLabel(i18n("Search String:"),gSearch);
  lSearch->resize(lSearch->sizeHint());
  lSearch->setAlignment(AlignBottom);
  sGrid->addWidget(lSearch,1,0);
  
  eSearch = new QLineEdit(gSearch);
  eSearch->resize(eSearch->sizeHint());
  sGrid->addWidget(eSearch,2,0);
  eSearch->setText(searchEntry->expr);

  cCase = new QCheckBox(i18n("Case Sensitive"),gSearch);
  cCase->resize(cCase->sizeHint());
  sGrid->addWidget(cCase,3,0);
  cCase->setChecked(searchEntry->caseSensitive);

  cRegExp = new QCheckBox(i18n("Regular Expression"),gSearch);
  cRegExp->resize(cRegExp->sizeHint());
  sGrid->addWidget(cRegExp,4,0);
  cRegExp->setChecked(searchEntry->regexp);

  cWholeWords = new QCheckBox(i18n("Find Whole Words only"),gSearch);
  cWholeWords->resize(cWholeWords->sizeHint());
  sGrid->addWidget(cWholeWords,5,0);
  cWholeWords->setChecked(searchEntry->wholeWords);

  cRev = new QCheckBox(i18n("Find Backwards"),gSearch);
  cRev->resize(cRev->sizeHint());
  sGrid->addWidget(cRev,6,0);
  cRev->setChecked(searchEntry->reverse);

  cFamily = new QCheckBox(i18n("Check Family"),gSearch);
  cFamily->resize(cFamily->sizeHint());
  sGrid->addWidget(cFamily,1,1);
  connect(cFamily,SIGNAL(clicked()),this,SLOT(slotCheckFamily()));
  cFamily->setChecked(searchEntry->checkFamily);

  cSize = new QCheckBox(i18n("Check Size"),gSearch);
  cSize->resize(cSize->sizeHint());
  sGrid->addWidget(cSize,2,1);
  connect(cSize,SIGNAL(clicked()),this,SLOT(slotCheckSize()));
  cSize->setChecked(searchEntry->checkSize);

  cColor = new QCheckBox(i18n("Check Color"),gSearch);
  cColor->resize(cColor->sizeHint());
  sGrid->addWidget(cColor,3,1);
  connect(cColor,SIGNAL(clicked()),this,SLOT(slotCheckColor()));
  cColor->setChecked(searchEntry->checkColor);

  cBold = new QCheckBox(i18n("Check Bold"),gSearch);
  cBold->resize(cBold->sizeHint());
  sGrid->addWidget(cBold,4,1);
  connect(cBold,SIGNAL(clicked()),this,SLOT(slotCheckBold()));
  cBold->setChecked(searchEntry->checkBold);

  cItalic = new QCheckBox(i18n("Check Italic"),gSearch);
  cItalic->resize(cItalic->sizeHint());
  sGrid->addWidget(cItalic,5,1);
  connect(cItalic,SIGNAL(clicked()),this,SLOT(slotCheckItalic()));
  cItalic->setChecked(searchEntry->checkItalic);

  cUnderline = new QCheckBox(i18n("Check Underline"),gSearch);
  cUnderline->resize(cUnderline->sizeHint());
  sGrid->addWidget(cUnderline,6,1);
  connect(cUnderline,SIGNAL(clicked()),this,SLOT(slotCheckUnderline()));
  cUnderline->setChecked(searchEntry->checkUnderline);

  cVertAlign = new QCheckBox(i18n("Check Vertical Alignment"),gSearch);
  cVertAlign->resize(cVertAlign->sizeHint());
  sGrid->addWidget(cVertAlign,7,1);
  connect(cVertAlign,SIGNAL(clicked()),this,SLOT(slotCheckVertAlign()));
  cVertAlign->setChecked(searchEntry->checkVertAlign);

  cmFamily = new QComboBox(true,gSearch);
  cmFamily->insertStrList(&fontlist);
  cmFamily->resize(cmFamily->sizeHint());
  sGrid->addWidget(cmFamily,1,2);
  for (int j = 0;j < cmFamily->count();j++)
    {
      if (QString(cmFamily->text(j)) == searchEntry->family)
	cmFamily->setCurrentItem(j);
    }
  connect(cmFamily,SIGNAL(activated(const char*)),this,SLOT(slotFamily(const char*)));

  cmSize = new QComboBox(true,gSearch);
  QStrList sizes;
  QString tmp;
  int curr = 0;
  for (int i = 4;i <= 100;i++)
    {
      tmp.sprintf("%d",i);
      sizes.append(tmp);
      if (i == searchEntry->size) curr = i - 4;
    }
  cmSize->insertStrList(&sizes);
  cmSize->resize(cmSize->sizeHint());
  sGrid->addWidget(cmSize,2,2);
  cmSize->setCurrentItem(curr);
  connect(cmSize,SIGNAL(activated(const char*)),this,SLOT(slotSize(const char*)));

  bColor = new KColorButton(gSearch);
  bColor->resize(bColor->sizeHint());
  sGrid->addWidget(bColor,3,2);
  bColor->setColor(searchEntry->color);
  connect(bColor,SIGNAL(changed(const QColor&)),this,SLOT(slotColor(const QColor&)));

  cmBold = new QCheckBox(i18n("Bold"),gSearch);
  cmBold->resize(cmBold->sizeHint());
  sGrid->addWidget(cmBold,4,2);
  cmBold->setChecked(searchEntry->bold);
  connect(cmBold,SIGNAL(clicked()),this,SLOT(slotBold()));

  cmItalic = new QCheckBox(i18n("Italic"),gSearch);
  cmItalic->resize(cmItalic->sizeHint());
  sGrid->addWidget(cmItalic,5,2);
  cmItalic->setChecked(searchEntry->italic);
  connect(cmItalic,SIGNAL(clicked()),this,SLOT(slotItalic()));

  cmUnderline = new QCheckBox(i18n("Underline"),gSearch);
  cmUnderline->resize(cmUnderline->sizeHint());
  sGrid->addWidget(cmUnderline,6,2);
  cmUnderline->setChecked(searchEntry->underline);
  connect(cmUnderline,SIGNAL(clicked()),this,SLOT(slotUnderline()));
  
  cmVertAlign = new QComboBox(false,gSearch);
  cmVertAlign->insertItem(i18n("Normal"),-1);
  cmVertAlign->insertItem(i18n("Subscript"),-1);
  cmVertAlign->insertItem(i18n("Superscript"),-1);
  cmVertAlign->resize(cmVertAlign->sizeHint());
  sGrid->addWidget(cmVertAlign,7,2);
  switch (searchEntry->vertAlign)
    {
    case KWFormat::VA_NORMAL:
      cmVertAlign->setCurrentItem(0);
      break;
    case KWFormat::VA_SUB:
      cmVertAlign->setCurrentItem(1);
      break;
    case KWFormat::VA_SUPER:
      cmVertAlign->setCurrentItem(2);
      break;
    }
  connect(cmVertAlign,SIGNAL(activated(int)),this,SLOT(slotVertAlign(int)));

  sGrid->addRowSpacing(0,7);
  sGrid->addRowSpacing(1,lSearch->height());
  sGrid->addRowSpacing(1,cFamily->height());
  sGrid->addRowSpacing(1,cmFamily->height());
  sGrid->addRowSpacing(2,eSearch->height());
  sGrid->addRowSpacing(2,cSize->height());
  sGrid->addRowSpacing(2,cmSize->height());
  sGrid->addRowSpacing(3,cCase->height());
  sGrid->addRowSpacing(3,cColor->height());
  sGrid->addRowSpacing(3,bColor->height());
  sGrid->addRowSpacing(4,cRegExp->height());
  sGrid->addRowSpacing(4,cBold->height());
  sGrid->addRowSpacing(4,cmBold->height());
  sGrid->addRowSpacing(5,cWholeWords->height());
  sGrid->addRowSpacing(5,cItalic->height());
  sGrid->addRowSpacing(5,cmItalic->height());
  sGrid->addRowSpacing(6,cUnderline->height());
  sGrid->addRowSpacing(6,cmUnderline->height());
  sGrid->addRowSpacing(4,cRev->height());
  sGrid->addRowSpacing(7,cVertAlign->height());
  sGrid->addRowSpacing(7,cmVertAlign->height());
  sGrid->setRowStretch(0,0);
  sGrid->setRowStretch(1,0);
  sGrid->setRowStretch(2,0);
  sGrid->setRowStretch(3,0);
  sGrid->setRowStretch(4,0);
  sGrid->setRowStretch(5,0);
  sGrid->setRowStretch(6,0);
  sGrid->setRowStretch(7,0);
  sGrid->setRowStretch(8,1);

  sGrid->addColSpacing(0,lSearch->width());
  sGrid->addColSpacing(1,cFamily->width());
  sGrid->addColSpacing(2,cmFamily->width());
  sGrid->addColSpacing(0,eSearch->width());
  sGrid->addColSpacing(1,cSize->width());
  sGrid->addColSpacing(2,cmSize->width());
  sGrid->addColSpacing(0,cRegExp->width());
  sGrid->addColSpacing(0,cCase->width());
  sGrid->addColSpacing(0,cWholeWords->width());
  sGrid->addColSpacing(0,cRev->width());
  sGrid->addColSpacing(1,cColor->width());
  sGrid->addColSpacing(2,bColor->width());
  sGrid->addColSpacing(1,cBold->width());
  sGrid->addColSpacing(2,cmBold->width());
  sGrid->addColSpacing(1,cItalic->width());
  sGrid->addColSpacing(2,cmItalic->width());
  sGrid->addColSpacing(1,cUnderline->width());
  sGrid->addColSpacing(2,cmUnderline->width());
  sGrid->addColSpacing(1,cVertAlign->width());
  sGrid->addColSpacing(2,cmVertAlign->width());
  sGrid->setColStretch(0,1);
  sGrid->setColStretch(1,0);
  sGrid->setColStretch(2,0);

  sGrid->activate();
  grid1->addWidget(gSearch,0,0);

  /**
   * ------------------ search buttonbox --------------
   */

  bbSearch = new KButtonBox(tab1,KButtonBox::VERTICAL);
  bSearchFirst = bbSearch->addButton(i18n("Find &First"),false);
  connect(bSearchFirst,SIGNAL(clicked()),this,SLOT(searchFirst()));
  bSearchNext = bbSearch->addButton(i18n("Find &Next"),false);
  connect(bSearchNext,SIGNAL(clicked()),this,SLOT(searchNext()));
  //bSearchAll = bbSearch->addButton(i18n("Search &All"),false);
  bbSearch->layout();
  bbSearch->resize(bbSearch->sizeHint());

  grid1->addWidget(bbSearch,0,1);
 
  /**
   * ----------------- general -----------------
   */

  grid1->addRowSpacing(0,gSearch->height());
  grid1->addRowSpacing(0,bbSearch->height());
  grid1->setRowStretch(0,0);
  grid1->setRowStretch(1,1);

  grid1->addColSpacing(0,gSearch->width());
  grid1->addColSpacing(1,bbSearch->width());
  grid1->setColStretch(0,1);
  grid1->setColStretch(1,0);

  grid1->activate();

  addTab(tab1,i18n("Find && Replace"));

  resize(minimumSize());

  slotCheckFamily();
  slotCheckColor();
  slotCheckSize();
  slotCheckBold();
  slotCheckItalic();
  slotCheckUnderline();
  slotCheckVertAlign();

  connect(this,SIGNAL(cancelButtonPressed()),this,SLOT(saveSettings()));
}

/*================================================================*/
void KWSearchDia::searchFirst()
{
  QString expr = eSearch->text();
  if (expr.isEmpty()) return;

  page->removeSelection();

  if (!cRev->isChecked())
    page->find(expr,searchEntry,true,cCase->isChecked(),cWholeWords->isChecked());
  else
    page->findRev(expr,searchEntry,true,cCase->isChecked(),cWholeWords->isChecked());
}

/*================================================================*/
void KWSearchDia::searchNext()
{
  QString expr = eSearch->text();
  if (expr.isEmpty()) return;

  page->removeSelection();

  if (!cRev->isChecked())
    page->find(expr,searchEntry,false,cCase->isChecked(),cWholeWords->isChecked());
  else
    page->findRev(expr,searchEntry,false,cCase->isChecked(),cWholeWords->isChecked());
}

/*================================================================*/
void KWSearchDia::slotCheckFamily()
{
  searchEntry->checkFamily = cFamily->isChecked();

  if (cFamily->isChecked())
    {
      cmFamily->setEnabled(true);
      slotFamily(cmFamily->currentText());
    }
  else cmFamily->setEnabled(false);
}

/*================================================================*/
void KWSearchDia::slotCheckColor()
{
  searchEntry->checkColor = cColor->isChecked();

  if (cColor->isChecked())
    {
      bColor->setEnabled(true);
      slotColor(bColor->color());
    }
  else bColor->setEnabled(false);
}

/*================================================================*/
void KWSearchDia::slotCheckSize()
{
  searchEntry->checkSize = cSize->isChecked();

  if (cSize->isChecked())
    {
      cmSize->setEnabled(true);
      slotSize(cmSize->currentText());
    }
  else cmSize->setEnabled(false);
}

/*================================================================*/
void KWSearchDia::slotCheckBold()
{
  searchEntry->checkBold = cBold->isChecked();

  if (cBold->isChecked())
    {
      cmBold->setEnabled(true);
      slotBold();
    }
  else cmBold->setEnabled(false);
}

/*================================================================*/
void KWSearchDia::slotCheckItalic()
{
  searchEntry->checkItalic = cItalic->isChecked();

  if (cItalic->isChecked())
    {
      cmItalic->setEnabled(true);
      slotItalic();
    }
  else cmItalic->setEnabled(false);
}

/*================================================================*/
void KWSearchDia::slotCheckUnderline()
{
  searchEntry->checkUnderline = cUnderline->isChecked();

  if (cUnderline->isChecked())
    {
      cmUnderline->setEnabled(true);
      slotUnderline();
    }
  else cmUnderline->setEnabled(false);
}

/*================================================================*/
void KWSearchDia::slotCheckVertAlign()
{
  searchEntry->checkVertAlign = cVertAlign->isChecked();

  if (cVertAlign->isChecked())
    {
      cmVertAlign->setEnabled(true);
      slotVertAlign(cmVertAlign->currentItem());
    }
  else cmVertAlign->setEnabled(false);
}

/*================================================================*/
void KWSearchDia::slotFamily(const char* family)
{
  searchEntry->family = qstrdup(family);
  view->setSearchEntry(searchEntry);
}

/*================================================================*/
void KWSearchDia::slotSize(const char* size)
{
  searchEntry->size = atoi(size);
  view->setSearchEntry(searchEntry);
}

/*================================================================*/
void KWSearchDia::slotColor(const QColor& color)
{
  searchEntry->color = QColor(color);
  view->setSearchEntry(searchEntry);
}

/*================================================================*/
void KWSearchDia::slotBold()
{
  searchEntry->bold = cmBold->isChecked();
  view->setSearchEntry(searchEntry);
}

/*================================================================*/
void KWSearchDia::slotItalic()
{
  searchEntry->italic = cmItalic->isChecked();
  view->setSearchEntry(searchEntry);
}

/*================================================================*/
void KWSearchDia::slotUnderline()
{
  searchEntry->underline = cmUnderline->isChecked();
  view->setSearchEntry(searchEntry);
}

/*================================================================*/
void KWSearchDia::slotVertAlign(int num)
{
  switch (num)
    {
    case 0:
      searchEntry->vertAlign = KWFormat::VA_NORMAL;
      break;
    case 1:
      searchEntry->vertAlign = KWFormat::VA_SUB;
      break;
    case 3:
      searchEntry->vertAlign = KWFormat::VA_SUPER;
      break;
    }
  view->setSearchEntry(searchEntry);
}

/*================================================================*/
void KWSearchDia::saveSettings()
{
  searchEntry->expr = eSearch->text();
  searchEntry->regexp = cRegExp->isChecked();
  searchEntry->caseSensitive = cCase->isChecked();
  searchEntry->wholeWords = cWholeWords->isChecked();
  searchEntry->reverse = cRev->isChecked();

  view->setSearchEntry(searchEntry);
}
