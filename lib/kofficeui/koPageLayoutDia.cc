/******************************************************************/
/* KOffice Library - (c) by Reginald Stadlbauer 1998              */
/* Version: 1.0                                                   */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Page Layout Dialog (header)                            */
/******************************************************************/

#include "koPageLayoutDia.h"
#include "koPageLayoutDia.moc"

#include <klocale.h>

#define POINT_TO_MM(px) (int((float)px/2.83465))
#define MM_TO_POINT(mm) (int((float)mm*2.83465))

/******************************************************************/
/* class KoPagePreview                                            */
/******************************************************************/

/*===================== constrcutor ==============================*/
KoPagePreview::KoPagePreview(QWidget* parent,const char *name,KoPageLayout _layout)
  : QGroupBox(i18n("Page Preview"),parent,name)
{
  setPageLayout(_layout);
  columns = 1;
}

/*====================== destructor ==============================*/
KoPagePreview::~KoPagePreview()
{
}

/*=================== set layout =================================*/
void KoPagePreview::setPageLayout(KoPageLayout _layout)
{
  double fact = 1;
  if (_layout.unit == PG_CM) fact = 10;
  if (_layout.unit == PG_INCH) fact = 25.4;

  int bl = (int)(_layout.left * fact * 100)/100,br = (int)(_layout.right * fact * 100)/100;
  int bt = (int)(_layout.top * fact * 100)/100,bb = (int)(_layout.bottom * fact * 100)/100;
  int wid = (int)(_layout.width * fact * 100)/100,hei = (int)(_layout.height * fact * 100)/100;

  pgWidth = (int)(wid / 2);
  pgHeight =(int)(hei / 2);

  pgX = (int)(bl / 2);
  pgY = (int)(bt / 2);
  pgW = (int)(pgWidth - (bl + br) / 2);
  pgH = (int)(pgHeight - (bt + bb) / 2);

  repaint(true);
}

/*=================== set layout =================================*/
void KoPagePreview::setPageColumns(KoColumns _columns)
{
  columns = _columns.columns;
  repaint(true);
}

/*======================== draw contents =========================*/
void KoPagePreview::drawContents(QPainter *painter)
{
  int cw = 0;
  cw = pgW / columns;

  painter->setBrush(white);
  painter->setPen(QPen(black));

  painter->drawRect((int)((width() - pgWidth) / 2) + 1,
		    (int)((height() - pgHeight) / 2) + 1,
		    pgWidth,pgHeight);
  painter->drawRect((int)((width() - pgWidth) / 2),
		    (int)((height() - pgHeight) / 2),
		    pgWidth,pgHeight);

  painter->setBrush(QBrush(black,HorPattern));
  if (pgW == pgWidth || pgH == pgHeight)
    painter->setPen(NoPen);
  else
    painter->setPen(lightGray);
      
  for (int i = 0;i < columns;i++)
    painter->drawRect((int)((width() - pgWidth) / 2) + pgX + i * cw,
		      (int)((height() - pgHeight) / 2) + pgY,
		      cw,pgH);
}

/******************************************************************/
/* class KoPageLayoutDia                                          */
/******************************************************************/

/*==================== constructor ===============================*/
KoPageLayoutDia::KoPageLayoutDia(QWidget* parent,const char* name,KoPageLayout _layout,KoHeadFoot _hf,int tabs)
  : QTabDialog(parent,name,true)
{
  pgPreview = 0;
  pgPreview2 = 0;

  layout = _layout;
  hf = _hf;

  cl.columns = 1;

  enableBorders = true;

  if (tabs & FORMAT_AND_BORDERS) setupTab1();
  if (tabs & HEADER_AND_FOOTER) setupTab2();

  setCancelButton( i18n( "Cancel" ) );
  setOkButton( i18n( "OK" ) );

  retPressed = false;

  setCaption( i18n( "Page Layout") );
  setFocusPolicy(QWidget::StrongFocus);
  setFocus();
}

/*==================== constructor ===============================*/
KoPageLayoutDia::KoPageLayoutDia(QWidget* parent,const char* name,KoPageLayout _layout,KoHeadFoot _hf,
				 KoColumns _cl,KoKWHeaderFooter _kwhf,int tabs)
  : QTabDialog(parent,name,true)
{
  pgPreview = 0;
  pgPreview2 = 0;

  layout = _layout;
  hf = _hf;
  cl = _cl;
  kwhf = _kwhf;

  enableBorders = true;

  if (tabs & DISABLE_BORDERS) enableBorders = false;
  if (tabs & FORMAT_AND_BORDERS) setupTab1();
  if (tabs & HEADER_AND_FOOTER) setupTab2();
  if (tabs & COLUMNS) setupTab3();
  if (tabs & KW_HEADER_AND_FOOTER) setupTab4();

  setCancelButton(i18n("Cancel"));
  setOkButton(i18n("OK"));

  retPressed = false;

  setCaption(i18n("Page Layout"));
  setFocusPolicy(QWidget::StrongFocus);
  setFocus();
}

/*===================== destructor ===============================*/
KoPageLayoutDia::~KoPageLayoutDia()
{
}

/*======================= show dialog ============================*/
bool KoPageLayoutDia::pageLayout(KoPageLayout& _layout,KoHeadFoot& _hf,int _tabs)
{
  bool res = false;
  KoPageLayoutDia *dlg = new KoPageLayoutDia(0,"PageLayout",_layout,_hf,_tabs);

  if (dlg->exec() == QDialog::Accepted)
    {
      res = true;
      if (_tabs & FORMAT_AND_BORDERS) _layout = dlg->getLayout();
      if (_tabs & HEADER_AND_FOOTER) _hf = dlg->getHeadFoot();
    }

  delete dlg;

  return res;
}

/*======================= show dialog ============================*/
bool KoPageLayoutDia::pageLayout(KoPageLayout& _layout,KoHeadFoot& _hf,KoColumns& _cl,KoKWHeaderFooter &_kwhf,int _tabs)
{
  bool res = false;
  KoPageLayoutDia *dlg = new KoPageLayoutDia(0,"PageLayout",_layout,_hf,_cl,_kwhf,_tabs);

  if (dlg->exec() == QDialog::Accepted)
    {
      res = true;
      if (_tabs & FORMAT_AND_BORDERS) _layout = dlg->getLayout();
      if (_tabs & HEADER_AND_FOOTER) _hf = dlg->getHeadFoot();
      if (_tabs & COLUMNS) _cl = dlg->getColumns();
      if (_tabs & KW_HEADER_AND_FOOTER) _kwhf = dlg->getKWHeaderFooter();
    }

  delete dlg;

  return res;
}

/*===================== get a standard page layout ===============*/
KoPageLayout KoPageLayoutDia::standardLayout()
{
  KoPageLayout  _layout;

  _layout.format = PG_DIN_A4;
  _layout.orientation = PG_PORTRAIT;
  _layout.width = PG_A4_WIDTH;
  _layout.height = PG_A4_HEIGHT;
  _layout.left = 20;
  _layout.right = 20;
  _layout.top = 20;
  _layout.bottom = 20;
  _layout.unit = PG_MM;
  _layout.ptWidth = MM_TO_POINT(PG_A4_WIDTH);
  _layout.ptHeight = MM_TO_POINT(PG_A4_HEIGHT);
  _layout.ptLeft = MM_TO_POINT(20);
  _layout.ptRight = MM_TO_POINT(20);
  _layout.ptTop = MM_TO_POINT(20);
  _layout.ptBottom = MM_TO_POINT(20);

  return  _layout;
}

/*====================== get header - footer =====================*/
KoHeadFoot KoPageLayoutDia::getHeadFoot()
{
  hf.headLeft = qstrdup(eHeadLeft->text());
  hf.headMid = qstrdup(eHeadMid->text());
  hf.headRight = qstrdup(eHeadRight->text());
  hf.footLeft = qstrdup(eFootLeft->text());
  hf.footMid = qstrdup(eFootMid->text());
  hf.footRight = qstrdup(eFootRight->text());

  return hf;
}

/*================================================================*/
KoColumns KoPageLayoutDia::getColumns()
{
  cl.columns = nColumns->getValue();
  cl.columnSpacing = nCSpacing->getValue();

  return cl;
}

/*================================================================*/
KoKWHeaderFooter KoPageLayoutDia::getKWHeaderFooter()
{
  if (rhSame->isChecked())
    kwhf.header = HF_SAME;
  else if (rhFirst->isChecked())
    kwhf.header = HF_FIRST_DIFF;
  else if (rhEvenOdd->isChecked())
    kwhf.header = HF_EO_DIFF;
  kwhf.ptHeaderBodySpacing = nHSpacing->getValue();

  if (rfSame->isChecked())
    kwhf.footer = HF_SAME;
  else if (rfFirst->isChecked())
    kwhf.footer = HF_FIRST_DIFF;
  else if (rfEvenOdd->isChecked())
    kwhf.footer = HF_EO_DIFF;
  kwhf.ptFooterBodySpacing = nFSpacing->getValue();

  return kwhf;
}

/*================ setup format and borders tab ==================*/
void KoPageLayoutDia::setupTab1()
{
  tab1 = new QWidget(this);

  grid1 = new QGridLayout(tab1,5,2,15,7);

  // ------------- unit _______________
  // label unit
  lpgUnit = new QLabel(i18n("Unit:"),tab1);
  lpgUnit->resize(lpgUnit->sizeHint());
  grid1->addWidget(lpgUnit,0,0);

  // combo unit
  cpgUnit = new QComboBox(false,tab1,"cpgUnit");
  cpgUnit->setAutoResize(false);
  cpgUnit->insertItem(i18n("Millimeters (mm)"));
  cpgUnit->insertItem(i18n("Centimeters (cm)"));
  cpgUnit->insertItem(i18n("Inches (in)"));
  cpgUnit->resize(cpgUnit->sizeHint());
  grid1->addWidget(cpgUnit,1,0);
  connect(cpgUnit,SIGNAL(activated(int)),this,SLOT(unitChanged(int)));

  // -------------- page format -----------------
  formatFrame = new QGroupBox(i18n("Page Format"),tab1);
  formatGrid = new QGridLayout(formatFrame,4,2,15,7);

  // label format
  lpgFormat = new QLabel(i18n("\nFormat:"),formatFrame);
  lpgFormat->resize(lpgFormat->sizeHint());
  formatGrid->addWidget(lpgFormat,0,0);
 
  // label orientation
  lpgOrientation = new QLabel(i18n("\nOrientation:"),formatFrame);
  lpgOrientation->resize(lpgOrientation->sizeHint());
  formatGrid->addWidget(lpgOrientation,0,1);

  // combo format
  cpgFormat = new QComboBox(false,formatFrame,"cpgFormat");
  cpgFormat->setAutoResize(false);
  cpgFormat->insertItem(i18n("DIN A3"));
  cpgFormat->insertItem(i18n("DIN A4"));
  cpgFormat->insertItem(i18n("DIN A5"));
  cpgFormat->insertItem(i18n("US Letter"));
  cpgFormat->insertItem(i18n("US Legal"));
  cpgFormat->insertItem(i18n("Screen"));
  cpgFormat->insertItem(i18n("Custom"));
  cpgFormat->insertItem(i18n("DIN B5"));
  cpgFormat->insertItem(i18n("US Executive"));
  cpgFormat->resize(cpgFormat->sizeHint());
  formatGrid->addWidget(cpgFormat,1,0);
  connect(cpgFormat,SIGNAL(activated(int)),this,SLOT(formatChanged(int)));

  // combo orientation
  cpgOrientation = new QComboBox(false,formatFrame,"cpgOrientation");
  cpgOrientation->setAutoResize(false);
  cpgOrientation->insertItem(i18n("Portrait"));
  cpgOrientation->insertItem(i18n("Landscape"));
  cpgOrientation->resize(cpgOrientation->sizeHint());
  formatGrid->addWidget(cpgOrientation,1,1);
  connect(cpgOrientation,SIGNAL(activated(int)),this,SLOT(orientationChanged(int)));

  // label width
  lpgWidth = new QLabel(i18n("Width:"),formatFrame);
  lpgWidth->resize(lpgWidth->sizeHint());
  formatGrid->addWidget(lpgWidth,2,0);

  // linedit width
  epgWidth = new KRestrictedLine(formatFrame,"Width","1234567890.");
  epgWidth->setText("000.00");
  epgWidth->setMaxLength(6);
  epgWidth->setEchoMode(QLineEdit::Normal);
  epgWidth->setFrame(true);
  epgWidth->resize(epgWidth->sizeHint().width()/2,epgWidth->sizeHint().height());
  formatGrid->addWidget(epgWidth,3,0);
  if (layout.format != PG_CUSTOM)
    epgWidth->setEnabled(false);
  connect(epgWidth,SIGNAL(returnPressed()),this,SLOT(rPressed()));
  connect(epgWidth,SIGNAL(returnPressed()),this,SLOT(widthChanged()));
  connect(epgWidth,SIGNAL(textChanged(const char*)),this,SLOT(widthChanged()));

  // label height
  lpgHeight = new QLabel(i18n("Height:"),formatFrame);
  lpgHeight->resize(lpgHeight->sizeHint());
  formatGrid->addWidget(lpgHeight,2,1);

  // linedit height
  epgHeight = new KRestrictedLine(formatFrame,"Height","1234567890.");
  epgHeight->setText("000.00");
  epgHeight->setMaxLength(6);
  epgHeight->setEchoMode(QLineEdit::Normal);
  epgHeight->setFrame(true);
  epgHeight->resize(epgHeight->sizeHint().width()/2,epgHeight->sizeHint().height());
  formatGrid->addWidget(epgHeight,3,1);
  if (layout.format != PG_CUSTOM)
    epgHeight->setEnabled(false);
  connect(epgHeight,SIGNAL(returnPressed()),this,SLOT(rPressed()));
  connect(epgHeight,SIGNAL(returnPressed()),this,SLOT(heightChanged()));
  connect(epgHeight,SIGNAL(textChanged(const char*)),this,SLOT(heightChanged()));

  // grid col spacing
  formatGrid->addColSpacing(0,lpgFormat->width());
  formatGrid->addColSpacing(0,cpgFormat->width());
  formatGrid->addColSpacing(0,lpgWidth->width());
  formatGrid->addColSpacing(0,epgWidth->width());
  formatGrid->addColSpacing(1,lpgOrientation->width());
  formatGrid->addColSpacing(1,cpgOrientation->width());
  formatGrid->addColSpacing(1,lpgHeight->width());
  formatGrid->addColSpacing(1,epgHeight->width());

  // grid row spacing
  formatGrid->addRowSpacing(0,lpgFormat->height());
  formatGrid->addRowSpacing(0,lpgOrientation->height());
  formatGrid->addRowSpacing(1,cpgFormat->height());
  formatGrid->addRowSpacing(1,cpgOrientation->height());
  formatGrid->addRowSpacing(2,lpgWidth->height());
  formatGrid->addRowSpacing(2,lpgHeight->height());
  formatGrid->addRowSpacing(3,epgWidth->height());
  formatGrid->addRowSpacing(3,epgHeight->height());

  // activate grid
  formatGrid->activate();
  formatFrame->resize(0,0);
  grid1->addWidget(formatFrame,2,0);

  // --------------- page borders ---------------
  borderFrame = new QGroupBox(i18n("Page Borders"),tab1);
  borderGrid = new QGridLayout(borderFrame,4,2,15,7);

  // label left
  lbrLeft = new QLabel(i18n("\nLeft:"),borderFrame);
  lbrLeft->resize(lbrLeft->sizeHint());
  borderGrid->addWidget(lbrLeft,0,0);

  // linedit left
  ebrLeft = new KRestrictedLine(borderFrame,"Left","1234567890.");
  ebrLeft->setText("000.00");
  ebrLeft->setMaxLength(6);
  ebrLeft->setEchoMode(QLineEdit::Normal);
  ebrLeft->setFrame(true);
  ebrLeft->resize(ebrLeft->sizeHint().width()/2,ebrLeft->sizeHint().height());
  borderGrid->addWidget(ebrLeft,1,0);
  connect(ebrLeft,SIGNAL(returnPressed()),this,SLOT(rPressed()));
  connect(ebrLeft,SIGNAL(returnPressed()),this,SLOT(leftChanged()));
  connect(ebrLeft,SIGNAL(textChanged(const char*)),this,SLOT(leftChanged()));
  if (!enableBorders) ebrLeft->setEnabled(false);

  // label right
  lbrRight = new QLabel(i18n("\nRight:"),borderFrame);
  lbrRight->resize(lbrRight->sizeHint());
  borderGrid->addWidget(lbrRight,0,1);

  // linedit right
  ebrRight = new KRestrictedLine(borderFrame,"Right","1234567890.");
  ebrRight->setText("000.00");
  ebrRight->setMaxLength(6);
  ebrRight->setEchoMode(QLineEdit::Normal);
  ebrRight->setFrame(true);
  ebrRight->resize(ebrRight->sizeHint().width()/2,ebrRight->sizeHint().height());
  borderGrid->addWidget(ebrRight,1,1);
  connect(ebrRight,SIGNAL(returnPressed()),this,SLOT(rPressed()));
  connect(ebrRight,SIGNAL(returnPressed()),this,SLOT(rightChanged()));
  connect(ebrRight,SIGNAL(textChanged(const char*)),this,SLOT(rightChanged()));
  if (!enableBorders) ebrRight->setEnabled(false);

  // label top
  lbrTop = new QLabel(i18n("Top:"),borderFrame);
  lbrTop->resize(lbrTop->sizeHint());
  borderGrid->addWidget(lbrTop,2,0);

  // linedit top
  ebrTop = new KRestrictedLine(borderFrame,"Top","1234567890.");
  ebrTop->setText("000.00");
  ebrTop->setMaxLength(6);
  ebrTop->setEchoMode(QLineEdit::Normal);
  ebrTop->setFrame(true);
  ebrTop->resize(ebrTop->sizeHint().width()/2,ebrTop->sizeHint().height());
  borderGrid->addWidget(ebrTop,3,0);
  connect(ebrTop,SIGNAL(returnPressed()),this,SLOT(rPressed()));
  connect(ebrTop,SIGNAL(returnPressed()),this,SLOT(topChanged()));
  connect(ebrTop,SIGNAL(textChanged(const char*)),this,SLOT(topChanged()));
  if (!enableBorders) ebrTop->setEnabled(false);

  // label bottom
  lbrBottom = new QLabel(i18n("Bottom:"),borderFrame);
  lbrBottom->resize(lbrBottom->sizeHint());
  borderGrid->addWidget(lbrBottom,2,1);

  // linedit bottom
  ebrBottom = new KRestrictedLine(borderFrame,"Bottom","1234567890.");
  ebrBottom->setText("000.00");
  ebrBottom->setMaxLength(6);
  ebrBottom->setEchoMode(QLineEdit::Normal);
  ebrBottom->setFrame(true);
  ebrBottom->resize(ebrBottom->sizeHint().width()/2,ebrBottom->sizeHint().height());
  borderGrid->addWidget(ebrBottom,3,1);
  connect(ebrBottom,SIGNAL(returnPressed()),this,SLOT(rPressed()));
  connect(ebrBottom,SIGNAL(returnPressed()),this,SLOT(bottomChanged()));
  connect(ebrBottom,SIGNAL(textChanged(const char*)),this,SLOT(bottomChanged()));
  if (!enableBorders) ebrBottom->setEnabled(false);

  // grid col spacing
  borderGrid->addColSpacing(0,lbrLeft->width());
  borderGrid->addColSpacing(0,ebrLeft->width());
  borderGrid->addColSpacing(0,lbrTop->width());
  borderGrid->addColSpacing(0,ebrTop->width());
  borderGrid->addColSpacing(1,lbrRight->width());
  borderGrid->addColSpacing(1,ebrRight->width());
  borderGrid->addColSpacing(1,lbrBottom->width());
  borderGrid->addColSpacing(1,ebrBottom->width());

  // grid row spacing
  borderGrid->addRowSpacing(0,lbrLeft->height());
  borderGrid->addRowSpacing(0,lbrRight->height());
  borderGrid->addRowSpacing(1,ebrLeft->height());
  borderGrid->addRowSpacing(1,ebrRight->height());
  borderGrid->addRowSpacing(2,lbrTop->height());
  borderGrid->addRowSpacing(2,lbrBottom->height());
  borderGrid->addRowSpacing(3,ebrTop->height());
  borderGrid->addRowSpacing(3,ebrBottom->height());

  // activate grid
  borderGrid->activate();
  borderFrame->resize(0,0);
  grid1->addWidget(borderFrame,3,0);

  // ------------- preview -----------
  pgPreview = new KoPagePreview(tab1,"Preview",layout);
  grid1->addMultiCellWidget(pgPreview,2,4,1,1);

  // --------------- main grid ------------------
  grid1->addColSpacing(0,lpgUnit->width());
  grid1->addColSpacing(0,cpgUnit->width());
  grid1->addColSpacing(0,formatFrame->width());
  grid1->addColSpacing(0,borderFrame->width());
  grid1->addColSpacing(1,280);
  grid1->setColStretch(1,1);

  grid1->addRowSpacing(0,lpgUnit->height());
  grid1->addRowSpacing(1,cpgUnit->height());
  grid1->addRowSpacing(2,formatFrame->height());
  grid1->addRowSpacing(2,120);
  grid1->addRowSpacing(3,borderFrame->height());
  grid1->addRowSpacing(3,120);
  grid1->setRowStretch(4,1);

  grid1->activate();

  addTab(tab1,i18n("Format and Borders"));

  setValuesTab1();
  updatePreview(layout);
}

/*================= setup values for tab one =====================*/
void KoPageLayoutDia::setValuesTab1()
{
  char tmp[10];

  // unit
  cpgUnit->setCurrentItem(layout.unit);

  // page format
  cpgFormat->setCurrentItem(layout.format);

  // orientation
  cpgOrientation->setCurrentItem(layout.orientation);

  // page width
  sprintf(tmp,"%.2f",layout.width);
  epgWidth->setText(tmp);

  // page height
  sprintf(tmp,"%.2f",layout.height);
  epgHeight->setText(tmp);

  // border left
  sprintf(tmp,"%.2f",layout.left);
  ebrLeft->setText(tmp);

  // border right
  sprintf(tmp,"%.2f",layout.right);
  ebrRight->setText(tmp);

  // border top
  sprintf(tmp,"%.2f",layout.top);
  ebrTop->setText(tmp);

  // border bottom
  sprintf(tmp,"%.2f",layout.bottom);
  ebrBottom->setText(tmp);

  pgPreview->setPageLayout(layout);
}

/*================ setup header and footer tab ===================*/
void KoPageLayoutDia::setupTab2()
{
  tab2 = new QWidget(this);

  grid2 = new QGridLayout(tab2,8,3,15,7);

  // ------------- header ---------------
  lHead = new QLabel(i18n("Head line:"),tab2);
  lHead->resize(lHead->sizeHint());
  grid2->addWidget(lHead,0,0);

  lHeadLeft = new QLabel(i18n("Left:"),tab2);
  lHeadLeft->resize(lHeadLeft->sizeHint());
  grid2->addWidget(lHeadLeft,1,0);

  eHeadLeft = new QLineEdit(tab2);
  eHeadLeft->resize(eHeadLeft->sizeHint());
  grid2->addWidget(eHeadLeft,2,0);
  eHeadLeft->setText((const char*)hf.headLeft);

  lHeadMid = new QLabel(i18n("Mid:"),tab2);
  lHeadMid->resize(lHeadMid->sizeHint());
  grid2->addWidget(lHeadMid,1,1);

  eHeadMid = new QLineEdit(tab2);
  eHeadMid->resize(eHeadMid->sizeHint());
  grid2->addWidget(eHeadMid,2,1);
  eHeadMid->setText((const char*)hf.headMid);

  lHeadRight = new QLabel(i18n("Right:"),tab2);
  lHeadRight->resize(lHeadRight->sizeHint());
  grid2->addWidget(lHeadRight,1,2);

  eHeadRight = new QLineEdit(tab2);
  eHeadRight->resize(eHeadRight->sizeHint());
  grid2->addWidget(eHeadRight,2,2);
  eHeadRight->setText((const char*)hf.headRight);

  // ------------- footer ---------------
  lFoot = new QLabel(i18n("\nFoot line:"),tab2);
  lFoot->resize(lFoot->sizeHint());
  grid2->addWidget(lFoot,3,0);

  lFootLeft = new QLabel(i18n("Left:"),tab2);
  lFootLeft->resize(lFootLeft->sizeHint());
  grid2->addWidget(lFootLeft,4,0);

  eFootLeft = new QLineEdit(tab2);
  eFootLeft->resize(eFootLeft->sizeHint());
  grid2->addWidget(eFootLeft,5,0);
  eFootLeft->setText((const char*)hf.footLeft);

  lFootMid = new QLabel(i18n("Mid:"),tab2);
  lFootMid->resize(lFootMid->sizeHint());
  grid2->addWidget(lFootMid,4,1);

  eFootMid = new QLineEdit(tab2);
  eFootMid->resize(eFootMid->sizeHint());
  grid2->addWidget(eFootMid,5,1);
  eFootMid->setText((const char*)hf.footMid);

  lFootRight = new QLabel(i18n("Right:"),tab2);
  lFootRight->resize(lFootRight->sizeHint());
  grid2->addWidget(lFootRight,4,2);

  eFootRight = new QLineEdit(tab2);
  eFootRight->resize(eFootRight->sizeHint());
  grid2->addWidget(eFootRight,5,2);
  eFootRight->setText((const char*)hf.footRight);

  // --------------- macros ---------------------
  lMacros1 = new QLabel(i18n("\nMacros:"),tab2);
  lMacros1->resize(lMacros1->sizeHint());
  grid2->addWidget(lMacros1,6,0);

  lMacros2 = new QLabel("<page>,<name>,<file>,<time>,<date>,<author>,<email>",tab2);
  lMacros2->resize(lMacros2->sizeHint());
  grid2->addMultiCellWidget(lMacros2,7,7,0,2);
  

  // --------------- main grid ------------------
  grid2->addColSpacing(0,lHead->width());
  grid2->addColSpacing(0,lHeadLeft->width());
  grid2->addColSpacing(0,2*eHeadLeft->width());
  grid2->addColSpacing(1,lHeadMid->width());
  grid2->addColSpacing(1,2*eHeadMid->width());
  grid2->addColSpacing(2,lHeadRight->width());
  grid2->addColSpacing(2,2*eHeadRight->width());
  grid2->addColSpacing(0,lFoot->width());
  grid2->addColSpacing(0,lFootLeft->width());
  grid2->addColSpacing(0,2*eFootLeft->width());
  grid2->addColSpacing(1,lFootMid->width());
  grid2->addColSpacing(1,2*eFootMid->width());
  grid2->addColSpacing(2,lFootRight->width());
  grid2->addColSpacing(2,2*eFootRight->width());
  grid2->addColSpacing(0,lMacros1->width());

  grid2->addRowSpacing(0,2*lHead->height());
  grid2->addRowSpacing(1,2*lHeadLeft->height());
  grid2->addRowSpacing(1,2*lHeadMid->height());
  grid2->addRowSpacing(1,2*lHeadRight->height());
  grid2->addRowSpacing(2,eHeadLeft->height());
  grid2->addRowSpacing(2,eHeadMid->height());
  grid2->addRowSpacing(2,eHeadRight->height());
  grid2->addRowSpacing(3,2*lFoot->height());
  grid2->addRowSpacing(4,2*lFootLeft->height());
  grid2->addRowSpacing(4,2*lFootMid->height());
  grid2->addRowSpacing(4,2*lFootRight->height());
  grid2->addRowSpacing(5,eFootLeft->height());
  grid2->addRowSpacing(5,eFootMid->height());
  grid2->addRowSpacing(5,eFootRight->height());
  grid2->addRowSpacing(6,2*lMacros1->height());
  grid2->addRowSpacing(7,2*lMacros2->height());

  grid2->activate();
  
  addTab(tab2,i18n("Header and Footer"));
}

/*================================================================*/
void KoPageLayoutDia::setupTab3()
{
  tab3 = new QWidget(this);

  grid3 = new QGridLayout(tab3,5,2,15,7);

  lColumns = new QLabel(i18n("Columns:"),tab3);
  lColumns->resize(lColumns->sizeHint());
  grid3->addWidget(lColumns,0,0);

  nColumns = new KNumericSpinBox(tab3);
  nColumns->resize(nColumns->sizeHint());
  grid3->addWidget(nColumns,1,0);
  nColumns->setEditable(false);
  nColumns->setRange(1,9);
  nColumns->setValue(cl.columns);
  connect(nColumns,SIGNAL(valueIncreased()),this,SLOT(nColChanged()));
  connect(nColumns,SIGNAL(valueDecreased()),this,SLOT(nColChanged()));

  lCSpacing = new QLabel(i18n("Columns Spacing (mm):"),tab3);
  lCSpacing->resize(lCSpacing->sizeHint());
  grid3->addWidget(lCSpacing,2,0);

  nCSpacing = new KNumericSpinBox(tab3);
  nCSpacing->resize(nCSpacing->sizeHint());
  grid3->addWidget(nCSpacing,3,0);
  nCSpacing->setEditable(false);
  nCSpacing->setRange(0,100);
  nCSpacing->setValue(cl.columnSpacing);
  connect(nCSpacing,SIGNAL(valueIncreased()),this,SLOT(nSpaceChanged()));
  connect(nCSpacing,SIGNAL(valueDecreased()),this,SLOT(nSpaceChanged()));

  // ------------- preview -----------
  pgPreview2 = new KoPagePreview(tab3,"Preview",layout);
  grid3->addMultiCellWidget(pgPreview2,0,4,1,1);

  // --------------- main grid ------------------
  grid3->addColSpacing(0,lColumns->width());
  grid3->addColSpacing(0,nColumns->width());
  grid3->addColSpacing(0,lCSpacing->width());
  grid3->addColSpacing(0,nCSpacing->width());
  grid3->addColSpacing(1,pgPreview2->width());
  grid3->setColStretch(1,1);

  grid3->addRowSpacing(0,lColumns->height());
  grid3->addRowSpacing(1,nColumns->height());
  grid3->addRowSpacing(2,lCSpacing->height());
  grid3->addRowSpacing(3,nCSpacing->height());
  grid3->setRowStretch(4,1);

  grid3->activate();
  
  addTab(tab3,i18n("Columns"));
  if (pgPreview) pgPreview->setPageColumns(cl);
  pgPreview2->setPageColumns(cl);
}

/*================================================================*/
void KoPageLayoutDia::setupTab4()
{
  tab4 = new QWidget(this);
  grid4 = new QGridLayout(tab4,3,1,15,7);

  gHeader = new QButtonGroup(i18n("Header"),tab4);
  gHeader->setExclusive(true);
  headerGrid = new QGridLayout(gHeader,5,2,15,7);
  
  rhSame = new QRadioButton(i18n("Same header for all pages"),gHeader);
  rhSame->resize(rhSame->sizeHint());
  gHeader->insert(rhSame);
  headerGrid->addMultiCellWidget(rhSame,1,1,0,1);
  if (kwhf.header == HF_SAME) rhSame->setChecked(true);

  rhFirst = new QRadioButton(i18n("Different header for the first page"),gHeader);
  rhFirst->resize(rhFirst->sizeHint());
  gHeader->insert(rhFirst);
  headerGrid->addMultiCellWidget(rhFirst,2,2,0,1);
  if (kwhf.header == HF_FIRST_DIFF) rhFirst->setChecked(true);

  rhEvenOdd = new QRadioButton(i18n("Different header for even and odd pages"),gHeader);
  rhEvenOdd->resize(rhEvenOdd->sizeHint());
  gHeader->insert(rhEvenOdd);
  headerGrid->addMultiCellWidget(rhEvenOdd,3,3,0,1);
  if (kwhf.header == HF_EO_DIFF) rhEvenOdd->setChecked(true);

  lHSpacing = new QLabel(i18n("Spacing between header and body (in pt):"),gHeader);
  lHSpacing->resize(lHSpacing->sizeHint());
  lHSpacing->setAlignment(AlignRight | AlignVCenter);
  headerGrid->addWidget(lHSpacing,4,0);

  nHSpacing = new KNumericSpinBox(gHeader);
  nHSpacing->resize(nHSpacing->sizeHint());
  headerGrid->addWidget(nHSpacing,4,1);
  nHSpacing->setEditable(false);
  nHSpacing->setRange(0,100);
  nHSpacing->setValue(kwhf.ptHeaderBodySpacing);
  
  headerGrid->addColSpacing(0,rhSame->width() / 2);
  headerGrid->addColSpacing(1,rhSame->width() / 2);
  headerGrid->addColSpacing(0,rhFirst->width() / 2);
  headerGrid->addColSpacing(1,rhFirst->width() / 2);
  headerGrid->addColSpacing(0,rhEvenOdd->width() / 2);
  headerGrid->addColSpacing(1,rhEvenOdd->width() / 2);
  headerGrid->addColSpacing(0,lHSpacing->width());
  headerGrid->addColSpacing(1,nHSpacing->width());
  headerGrid->setColStretch(1,1);

  headerGrid->addRowSpacing(0,7);
  headerGrid->addRowSpacing(1,rhSame->height());
  headerGrid->addRowSpacing(2,rhFirst->height());
  headerGrid->addRowSpacing(3,rhEvenOdd->height());
  headerGrid->addRowSpacing(4,lHSpacing->height());
  headerGrid->addRowSpacing(4,nHSpacing->height());
  headerGrid->setRowStretch(0,0);
  headerGrid->setRowStretch(1,0);
  headerGrid->setRowStretch(2,0);
  headerGrid->setRowStretch(3,0);
  headerGrid->setRowStretch(4,0);
  
  headerGrid->activate();
  grid4->addWidget(gHeader,0,0);

  gFooter = new QButtonGroup(i18n("Footer"),tab4);
  gFooter->setExclusive(true);
  footerGrid = new QGridLayout(gFooter,5,2,15,7);
  
  rfSame = new QRadioButton(i18n("Same footer for all pages"),gFooter);
  rfSame->resize(rfSame->sizeHint());
  gFooter->insert(rfSame);
  footerGrid->addMultiCellWidget(rfSame,1,1,0,1);
  if (kwhf.footer == HF_SAME) rfSame->setChecked(true);

  rfFirst = new QRadioButton(i18n("Different footer for the first page"),gFooter);
  rfFirst->resize(rfFirst->sizeHint());
  gFooter->insert(rfFirst);
  footerGrid->addMultiCellWidget(rfFirst,2,2,0,1);
  if (kwhf.footer == HF_FIRST_DIFF) rfFirst->setChecked(true);

  rfEvenOdd = new QRadioButton(i18n("Different footer for even and odd pages"),gFooter);
  rfEvenOdd->resize(rfEvenOdd->sizeHint());
  gFooter->insert(rfEvenOdd);
  footerGrid->addMultiCellWidget(rfEvenOdd,3,3,0,1);
  if (kwhf.footer == HF_EO_DIFF) rfEvenOdd->setChecked(true);

  lFSpacing = new QLabel(i18n("Spacing between footer and body (in pt):"),gFooter);
  lFSpacing->resize(lFSpacing->sizeHint());
  lFSpacing->setAlignment(AlignRight | AlignVCenter);
  footerGrid->addWidget(lFSpacing,4,0);

  nFSpacing = new KNumericSpinBox(gFooter);
  nFSpacing->resize(nFSpacing->sizeHint());
  footerGrid->addWidget(nFSpacing,4,1);
  nFSpacing->setEditable(false);
  nFSpacing->setRange(0,100);
  nFSpacing->setValue(kwhf.ptFooterBodySpacing);
  
  footerGrid->addColSpacing(0,rfSame->width() / 2);
  footerGrid->addColSpacing(1,rfSame->width() / 2);
  footerGrid->addColSpacing(0,rfFirst->width() / 2);
  footerGrid->addColSpacing(1,rfFirst->width() / 2);
  footerGrid->addColSpacing(0,rfEvenOdd->width() / 2);
  footerGrid->addColSpacing(1,rfEvenOdd->width() / 2);
  footerGrid->addColSpacing(0,lFSpacing->width());
  footerGrid->addColSpacing(1,nFSpacing->width());
  footerGrid->setColStretch(1,1);

  footerGrid->addRowSpacing(0,7);
  footerGrid->addRowSpacing(1,rfSame->height());
  footerGrid->addRowSpacing(2,rfFirst->height());
  footerGrid->addRowSpacing(3,rfEvenOdd->height());
  footerGrid->addRowSpacing(4,lFSpacing->height());
  footerGrid->addRowSpacing(4,nFSpacing->height());
  footerGrid->setRowStretch(0,0);
  footerGrid->setRowStretch(1,0);
  footerGrid->setRowStretch(2,0);
  footerGrid->setRowStretch(3,0);
  footerGrid->setRowStretch(4,0);
  
  footerGrid->activate();
  grid4->addWidget(gFooter,1,0);

  grid4->addColSpacing(0,gHeader->width());
  grid4->addColSpacing(0,gFooter->width());
  grid4->setColStretch(0,1);
  
  grid4->addRowSpacing(0,gHeader->height());
  grid4->addRowSpacing(1,gFooter->height());
  grid4->setRowStretch(2,0);
  grid4->setRowStretch(2,1);

  grid4->activate();
  
  addTab(tab4,i18n("Header and Footer"));
}

/*====================== update the preview ======================*/
void KoPageLayoutDia::updatePreview(KoPageLayout)
{
  if (pgPreview) pgPreview->setPageLayout(layout);
  if (pgPreview) pgPreview->setPageColumns(cl);
  if (pgPreview2) pgPreview2->setPageLayout(layout);
  if (pgPreview2) pgPreview2->setPageColumns(cl);
}

/*===================== unit changed =============================*/
void KoPageLayoutDia::unitChanged(int _unit)
{
  if ((KoUnit)_unit != layout.unit)
    {
      double fact = 1;
      if (layout.unit == PG_CM) fact = 10;
      if (layout.unit == PG_INCH) fact = 25.4;
      
      layout.width *= fact; 
      layout.height *= fact; 
      layout.left *= fact; 
      layout.right *= fact; 
      layout.top *= fact; 
      layout.bottom *= fact; 

      layout.ptWidth = MM_TO_POINT(layout.width);
      layout.ptHeight = MM_TO_POINT(layout.height);
      layout.ptLeft = MM_TO_POINT(layout.left);
      layout.ptRight = MM_TO_POINT(layout.right);
      layout.ptTop = MM_TO_POINT(layout.top);
      layout.ptBottom = MM_TO_POINT(layout.bottom);

      fact = 1;
      if (_unit == PG_CM) fact = 0.1;
      if (_unit == PG_INCH) fact = 1/25.4;

      layout.width *= fact; 
      layout.height *= fact; 
      layout.left *= fact; 
      layout.right *= fact; 
      layout.top *= fact; 
      layout.bottom *= fact; 

      layout.ptWidth = MM_TO_POINT(layout.width);
      layout.ptHeight = MM_TO_POINT(layout.height);
      layout.ptLeft = MM_TO_POINT(layout.left);
      layout.ptRight = MM_TO_POINT(layout.right);
      layout.ptTop = MM_TO_POINT(layout.top);
      layout.ptBottom = MM_TO_POINT(layout.bottom);

      layout.unit = (KoUnit)_unit;
      setValuesTab1();

      layout.width = atof(epgWidth->text());
      layout.height = atof(epgHeight->text());
      layout.left = atof(ebrLeft->text());
      layout.right = atof(ebrRight->text());
      layout.top = atof(ebrTop->text());
      layout.bottom = atof(ebrBottom->text());

      layout.ptWidth = MM_TO_POINT(layout.width);
      layout.ptHeight = MM_TO_POINT(layout.height);
      layout.ptLeft = MM_TO_POINT(layout.left);
      layout.ptRight = MM_TO_POINT(layout.right);
      layout.ptTop = MM_TO_POINT(layout.top);
      layout.ptBottom = MM_TO_POINT(layout.bottom);

      updatePreview(layout);
    }
}

/*===================== format changed =============================*/
void KoPageLayoutDia::formatChanged(int _format)
{
  if ((KoFormat)_format != layout.format)
    {
      bool enable = true;
      char stmp[10];
      double w = 0,h = 0,dtmp = 0;
      
      layout.format = (KoFormat)_format;
      if ((KoFormat)_format != PG_CUSTOM) enable = false;
      epgWidth->setEnabled(enable);
      epgHeight->setEnabled(enable);

      switch (layout.format)
	{
	case PG_DIN_A4: case PG_CUSTOM:
	  {
	    w = PG_A4_WIDTH;
	    h = PG_A4_HEIGHT;
	  } break;
	case PG_DIN_A3:
	  {
	    w = PG_A3_WIDTH;
	    h = PG_A3_HEIGHT;
	  } break;
	case PG_DIN_A5:
	  {
	    w = PG_A5_WIDTH;
	    h = PG_A5_HEIGHT;
	  } break;
	case PG_US_LETTER:
	  {
	    w = PG_US_LETTER_WIDTH;
	    h = PG_US_LETTER_HEIGHT;
	  } break;
	case PG_US_LEGAL:
	  {
	    w = PG_US_LEGAL_WIDTH;
	    h = PG_US_LEGAL_HEIGHT;
	  } break;
	case PG_SCREEN:
	  {
	    w = PG_SCREEN_WIDTH;
	    h = PG_SCREEN_HEIGHT;
	  } break;
	case PG_DIN_B5:
	  {
	    w = PG_B5_WIDTH;
	    h = PG_B5_HEIGHT;
	  } break;
	case PG_US_EXECUTIVE:
	  {
	    w = PG_US_EXECUTIVE_WIDTH;
	    h = PG_US_EXECUTIVE_HEIGHT;
	  } break;
	}
      if (layout.orientation == PG_LANDSCAPE)
	{
	  dtmp = w;
	  w = h;
	  h = dtmp;
	}
      
      layout.width = w;
      layout.height = h;

      layout.ptWidth = MM_TO_POINT(layout.width);
      layout.ptHeight = MM_TO_POINT(layout.height);

      if (layout.unit == PG_CM)
	{
	  layout.width *= 0.1; 
	  layout.height *= 0.1; 
	}
      else if (layout.unit == PG_INCH)
	{
	  layout.width *= 1/25.4; 
	  layout.height *= 1/25.4;
	}
 
      layout.ptWidth = MM_TO_POINT(layout.width);
      layout.ptHeight = MM_TO_POINT(layout.height);

      sprintf(stmp,"%.2f",layout.width);
      epgWidth->setText(stmp);
      sprintf(stmp,"%.2f",layout.height);
      epgHeight->setText(stmp);

      layout.left = atof(ebrLeft->text());
      layout.right = atof(ebrRight->text());
      layout.top = atof(ebrTop->text());
      layout.bottom = atof(ebrBottom->text());

      layout.ptLeft = MM_TO_POINT(layout.left);
      layout.ptRight = MM_TO_POINT(layout.right);
      layout.ptTop = MM_TO_POINT(layout.top);
      layout.ptBottom = MM_TO_POINT(layout.bottom);

      updatePreview(layout);
    }
}

/*===================== format changed =============================*/
void KoPageLayoutDia::orientationChanged(int _orientation)
{
  if ((KoOrientation)_orientation != layout.orientation)
    {
      double tmp;

      layout.width = atof(epgWidth->text());
      layout.height = atof(epgHeight->text());
      layout.left = atof(ebrLeft->text());
      layout.right = atof(ebrRight->text());
      layout.top = atof(ebrTop->text());
      layout.bottom = atof(ebrBottom->text());
	
      tmp = layout.width;
      layout.width = layout.height;
      layout.height = tmp;

      layout.ptWidth = MM_TO_POINT(layout.width);
      layout.ptHeight = MM_TO_POINT(layout.height);
      layout.ptLeft = MM_TO_POINT(layout.left);
      layout.ptRight = MM_TO_POINT(layout.right);
      layout.ptTop = MM_TO_POINT(layout.top);
      layout.ptBottom = MM_TO_POINT(layout.bottom);

      if ((KoOrientation)_orientation == PG_LANDSCAPE)
	{
	  tmp = layout.left;
	  layout.left = layout.bottom;
	  layout.bottom = layout.right;
	  layout.right = layout.top;
	  layout.top = tmp;
	}
      else
	{
	  tmp = layout.top;
	  layout.top = layout.right;
	  layout.right = layout.bottom;
	  layout.bottom = layout.left;
	  layout.left = tmp;
	}

      layout.ptLeft = MM_TO_POINT(layout.left);
      layout.ptRight = MM_TO_POINT(layout.right);
      layout.ptTop = MM_TO_POINT(layout.top);
      layout.ptBottom = MM_TO_POINT(layout.bottom);

      layout.orientation = (KoOrientation)_orientation;
      setValuesTab1();
      updatePreview(layout);
    }
}

/*===================== width changed =============================*/
void KoPageLayoutDia::widthChanged()
{
  if (strlen(epgWidth->text()) == 0 && retPressed)
    epgWidth->setText("0.00");
  layout.width = atof(epgWidth->text());
  layout.ptWidth = MM_TO_POINT(layout.width);
  updatePreview(layout);
  retPressed = false;
}

/*===================== height changed ============================*/
void KoPageLayoutDia::heightChanged()
{
  if (strlen(epgHeight->text()) == 0 && retPressed)
    epgHeight->setText("0.00");
  layout.height = atof(epgHeight->text());
  layout.ptHeight = MM_TO_POINT(layout.height);
  updatePreview(layout);
  retPressed = false;
}

/*===================== left border changed =======================*/
void KoPageLayoutDia::leftChanged()
{
  if (strlen(ebrLeft->text()) == 0 && retPressed)
    ebrLeft->setText("0.00");
  layout.left = atof(ebrLeft->text());
  layout.ptLeft = MM_TO_POINT(layout.left);
  updatePreview(layout);
  retPressed = false;
}

/*===================== right border changed =======================*/
void KoPageLayoutDia::rightChanged()
{
  if (strlen(ebrRight->text()) == 0 && retPressed)
    ebrRight->setText("0.00");
  layout.right = atof(ebrRight->text());
  layout.ptRight = MM_TO_POINT(layout.right);
  updatePreview(layout);
  retPressed = false;
}

/*===================== top border changed =========================*/
void KoPageLayoutDia::topChanged()
{
  if (strlen(ebrTop->text()) == 0 && retPressed)
    ebrTop->setText("0.00");
  layout.top = atof(ebrTop->text());
  layout.ptTop = MM_TO_POINT(layout.top);
  updatePreview(layout);
  retPressed = false;
}

/*===================== bottom border changed ======================*/
void KoPageLayoutDia::bottomChanged()
{
  if (strlen(ebrBottom->text()) == 0 && retPressed)
    ebrBottom->setText("0.00");
  layout.bottom = atof(ebrBottom->text());
  layout.ptBottom = MM_TO_POINT(layout.bottom);
  updatePreview(layout);
  retPressed = false;
}

/*==================================================================*/
void KoPageLayoutDia::nColChanged()
{
  cl.columns = nColumns->getValue();
  updatePreview(layout);
}

/*==================================================================*/
void KoPageLayoutDia::nSpaceChanged()
{
  cl.columnSpacing = nCSpacing->getValue();
  updatePreview(layout);
}
