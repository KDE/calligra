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

/******************************************************************/
/* class KoPagePreview                                            */
/******************************************************************/

/*===================== constrcutor ==============================*/
KoPagePreview::KoPagePreview(QWidget* parent,const char *name,KoPageLayout _layout)
  : QGroupBox(i18n("Page Preview"),parent,name)
{
  setPageLayout(_layout);
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

/*======================== draw contents =========================*/
void KoPagePreview::drawContents(QPainter *painter)
{
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
  painter->drawRect((int)((width() - pgWidth) / 2) + pgX,
		    (int)((height() - pgHeight) / 2) + pgY,
		    pgW,pgH);
  
}

/******************************************************************/
/* class KoPageLayoutDia                                          */
/******************************************************************/

/*==================== constructor ===============================*/
KoPageLayoutDia::KoPageLayoutDia(QWidget* parent,const char* name,KoPageLayout _layout,KoHeadFoot _hf,int tabs)
  : QTabDialog(parent,name,true)
{
  layout = _layout;
  hf = _hf;

  if (tabs & FORMAT_AND_BORDERS) setupTab1();
  if (tabs & HEADER_AND_FOOTER) setupTab2();

  setCancelButton( i18n( "Cancel" ) );
  setOkButton( i18n( "Ok" ) );

  retPressed = false;

  setCaption( i18n( "Page Layout") );
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

/*================ setup format and borders tab ==================*/
void KoPageLayoutDia::setupTab1()
{
  tab1 = new QWidget(this);

  grid1 = new QGridLayout(tab1,4,2,15,7);

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
  grid1->addMultiCellWidget(pgPreview,2,3,1,1);

  // --------------- main grid ------------------
  grid1->addColSpacing(0,lpgUnit->width());
  grid1->addColSpacing(0,cpgUnit->width());
  grid1->addColSpacing(0,formatFrame->width());
  grid1->addColSpacing(0,borderFrame->width());
  grid1->addColSpacing(1,280);
  //grid1->setColStretch(1,1);

  grid1->addRowSpacing(0,lpgUnit->height());
  grid1->addRowSpacing(1,cpgUnit->height());
  grid1->addRowSpacing(2,formatFrame->height());
  grid1->addRowSpacing(2,120);
  grid1->addRowSpacing(3,borderFrame->height());
  grid1->addRowSpacing(3,120);
  //grid1->setRowStretch(1,1);

  grid1->activate();
  tab1->resize(0,0);
  tab1->setMaximumSize(tab1->size());
  tab1->setMinimumSize(tab1->size());

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
  tab2->resize(0,0);
  tab2->setMaximumSize(tab2->size());
  tab2->setMinimumSize(tab2->size());
  
  addTab(tab2,i18n("Header and Footer"));
}

/*====================== update the preview ======================*/
void KoPageLayoutDia::updatePreview(KoPageLayout)
{
  pgPreview->setPageLayout(layout);
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

      fact = 1;
      if (_unit == PG_CM) fact = 0.1;
      if (_unit == PG_INCH) fact = 1/25.4;

      layout.width *= fact; 
      layout.height *= fact; 
      layout.left *= fact; 
      layout.right *= fact; 
      layout.top *= fact; 
      layout.bottom *= fact; 

      layout.unit = (KoUnit)_unit;
      setValuesTab1();

      layout.width = atof(epgWidth->text());
      layout.height = atof(epgHeight->text());
      layout.left = atof(ebrLeft->text());
      layout.right = atof(ebrRight->text());
      layout.top = atof(ebrTop->text());
      layout.bottom = atof(ebrBottom->text());

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
      double w,h,dtmp;
      
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
	}
      if (layout.orientation == PG_LANDSCAPE)
	{
	  dtmp = w;
	  w = h;
	  h = dtmp;
	}
      
      layout.width = w;
      layout.height = h;

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
 
      sprintf(stmp,"%.2f",layout.width);
      epgWidth->setText(stmp);
      sprintf(stmp,"%.2f",layout.height);
      epgHeight->setText(stmp);

      layout.left = atof(ebrLeft->text());
      layout.right = atof(ebrRight->text());
      layout.top = atof(ebrTop->text());
      layout.bottom = atof(ebrBottom->text());

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
  updatePreview(layout);
  retPressed = false;
}

/*===================== height changed ============================*/
void KoPageLayoutDia::heightChanged()
{
  if (strlen(epgHeight->text()) == 0 && retPressed)
    epgHeight->setText("0.00");
  layout.height = atof(epgHeight->text());
  updatePreview(layout);
  retPressed = false;
}

/*===================== left border changed =======================*/
void KoPageLayoutDia::leftChanged()
{
  if (strlen(ebrLeft->text()) == 0 && retPressed)
    ebrLeft->setText("0.00");
  layout.left = atof(ebrLeft->text());
  updatePreview(layout);
  retPressed = false;
}

/*===================== right border changed =======================*/
void KoPageLayoutDia::rightChanged()
{
  if (strlen(ebrRight->text()) == 0 && retPressed)
    ebrRight->setText("0.00");
  layout.right = atof(ebrRight->text());
  updatePreview(layout);
  retPressed = false;
}

/*===================== top border changed =======================*/
void KoPageLayoutDia::topChanged()
{
  if (strlen(ebrTop->text()) == 0 && retPressed)
    ebrTop->setText("0.00");
  layout.top = atof(ebrTop->text());
  updatePreview(layout);
  retPressed = false;
}

/*===================== bottom border changed =======================*/
void KoPageLayoutDia::bottomChanged()
{
  if (strlen(ebrBottom->text()) == 0 && retPressed)
    ebrBottom->setText("0.00");
  layout.bottom = atof(ebrBottom->text());
  updatePreview(layout);
  retPressed = false;
}
