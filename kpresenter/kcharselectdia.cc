/******************************************************************/
/* KCharSelectDia - (c) by Reginald Stadlbauer 1998               */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* KCharSelectDia is under GNU GPL                                */
/******************************************************************/
/* Module: Character Selector Dialog                              */
/******************************************************************/

#include "kcharselectdia.h"
#include "kcharselectdia.moc"

/******************************************************************/
/* class KCharSelectDia                                           */
/******************************************************************/

/*==================== constructor ===============================*/
KCharSelectDia::KCharSelectDia(QWidget* parent,const char* name,QFont __font,QColor __color,int __c)
  : QDialog(parent,name,true)
{
  _font = __font;
  _color = __color;
  _c = __c;

  setCaption("Select a character");

  grid = new QGridLayout(this,5,1,15,7);

  getFonts();

  lFont = new QLabel("Font:",this);
  lFont->resize(lFont->sizeHint());
  grid->addWidget(lFont,0,0);

  fontCombo = new QComboBox(false,this);
  fontCombo->insertStrList(&fontList);
  fontCombo->resize(fontCombo->sizeHint());
  fontList.find(_font.family());
  fontCombo->setCurrentItem(fontList.at());
  grid->addWidget(fontCombo,1,0);
  connect(fontCombo,SIGNAL(activated(const char*)),this,SLOT(fontSelected(const char*)));

  charSelect = new KCharSelect(this,"",_font,_color,_c);
  charSelect->resize(charSelect->sizeHint());
  grid->addWidget(charSelect,2,0);
  connect(charSelect,SIGNAL(activated(int)),this,SLOT(charChanged(int)));

  wid = new QWidget(this);
  
  grid2 = new QGridLayout(wid,2,6,0,7);

  lSize = new QLabel("Size:",wid);
  lSize->resize(lSize->sizeHint());
  grid2->addWidget(lSize,0,0);

  lColor = new QLabel("Color:",wid);
  lColor->resize(lColor->sizeHint());
  grid2->addWidget(lColor,0,1);

  sizeCombo = new QComboBox(false,wid);
  char chr[5];
  for (unsigned int i = 4;i <= 100;i++)
    {
      sprintf(chr,"%d",i);
      sizeCombo->insertItem(chr,-1);
    }
  sizeCombo->resize(sizeCombo->sizeHint());
  grid2->addWidget(sizeCombo,1,0);
  sizeCombo->setCurrentItem(_font.pointSize()-4);
  connect(sizeCombo,SIGNAL(activated(int)),this,SLOT(sizeSelected(int)));
  
  colorButton = new KColorButton(_color,wid);
  colorButton->resize(colorButton->sizeHint());
  grid2->addWidget(colorButton,1,1);
  connect(colorButton,SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
  
  lAttrib = new QLabel("Attributes:",wid);
  lAttrib->resize(lAttrib->sizeHint());
  grid2->addMultiCellWidget(lAttrib,0,0,2,4);

  bold = new QCheckBox("Bold",wid);
  bold->resize(bold->sizeHint());
  grid2->addWidget(bold,1,2);
  bold->setChecked(_font.bold());
  connect(bold,SIGNAL(clicked()),this,SLOT(boldChanged()));

  italic= new QCheckBox("Italic",wid);
  italic->resize(italic->sizeHint());
  grid2->addWidget(italic,1,3);
  italic->setChecked(_font.italic());
  connect(italic,SIGNAL(clicked()),this,SLOT(italicChanged()));

  underl = new QCheckBox("Underlined",wid);
  underl->resize(underl->sizeHint());
  grid2->addWidget(underl,1,4);
  underl->setChecked(_font.underline());
  connect(underl,SIGNAL(clicked()),this,SLOT(underlChanged()));

  lPreview = new QLabel(wid);
  lPreview->setBackgroundColor(white);
  lPreview->resize(7+colorButton->height()+lColor->height(),
		   7+colorButton->height()+lColor->height());
  lPreview->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  sprintf(chr,"%c",_c);
  lPreview->setText(chr);
  lPreview->setAlignment(AlignHCenter | AlignVCenter);
  lPreview->setFont(_font);
  colorChanged(_color);
  grid2->addMultiCellWidget(lPreview,0,1,5,5);

  grid2->addColSpacing(0,lSize->width());
  grid2->addColSpacing(0,sizeCombo->width());
  grid2->addColSpacing(1,lColor->width());
  grid2->addColSpacing(1,colorButton->width());
  grid2->addColSpacing(2,bold->width());
  grid2->addColSpacing(3,italic->width());
  grid2->addColSpacing(4,underl->width());
  grid2->addColSpacing(5,lPreview->width());

  grid2->addRowSpacing(0,lSize->height());
  grid2->addRowSpacing(0,lColor->height());
  grid2->addRowSpacing(0,lAttrib->height());
  grid2->addRowSpacing(1,sizeCombo->height());
  grid2->addRowSpacing(1,colorButton->height());
  grid2->addRowSpacing(1,bold->height());
  grid2->addRowSpacing(1,italic->height());
  grid2->addRowSpacing(1,underl->height());

  grid2->activate();
  wid->resize(0,0);
  wid->setMaximumSize(wid->size());
  wid->setMinimumSize(wid->size());
  grid->addWidget(wid,3,0);

  bbox = new KButtonBox(this,KButtonBox::HORIZONTAL,7);
  bbox->addStretch(20);
  bOk = bbox->addButton("OK");
  bOk->setAutoRepeat(false);
  bOk->setAutoResize(false);
  bOk->setAutoDefault(true);
  bOk->setDefault(true);
  connect(bOk,SIGNAL(clicked()),SLOT(accept()));
  bCancel = bbox->addButton("Cancel");
  connect(bCancel,SIGNAL(clicked()),SLOT(reject()));
  bbox->layout();
  grid->addWidget(bbox,4,0);

  grid->addColSpacing(0,lFont->width());
  grid->addColSpacing(0,fontCombo->width());
  grid->addColSpacing(0,charSelect->width());
  grid->addColSpacing(0,wid->width());

  grid->addRowSpacing(0,lFont->height());
  grid->addRowSpacing(1,fontCombo->height());
  grid->addRowSpacing(2,charSelect->height());
  grid->addRowSpacing(3,wid->height());
  grid->addRowSpacing(4,bOk->height());
  grid->addRowSpacing(4,bCancel->height());
  grid->activate();

  resize(0,0);
  setMaximumSize(size());
  setMinimumSize(size());
}

/*===================== destructor ===============================*/
KCharSelectDia::~KCharSelectDia()
{
}

/*======================= about application ======================*/
bool KCharSelectDia::selectChar(QFont &__font,QColor &__color,int &__c)
{
  bool res = false;

    KCharSelectDia *dlg = new KCharSelectDia(0,"Select Character",__font,__color,__c);
    if (dlg->exec() == QDialog::Accepted)
      {
	__font = dlg->font();
	__color = dlg->color();
	__c = dlg->c();
	res = true;
      }

    delete dlg;

    return res;
}

/*=========================== get Fonts =========================*/
void KCharSelectDia::getFonts()
{
  int numFonts;
  Display *kde_display;
  char** fontNames;
  char** fontNames_copy;
  QString qfontname;
  
  kde_display = XOpenDisplay(0L);

  // now try to load the KDE fonts
  bool have_installed = kapp->getKDEFonts(&fontList);
  
  // if available we are done, the kde fonts are now in the family_combo
  if (have_installed)
    return;

  fontNames = XListFonts(kde_display,"*",32767,&numFonts);
  fontNames_copy = fontNames;
  
  for(int i = 0; i < numFonts; i++){
    
    if (**fontNames != '-')
      { 
      
      // The font name doesn't start with a dash -- an alias
      // so we ignore it. It is debatable whether this is the right
      // behaviour so I leave the following snippet of code around.
      // Just uncomment it if you want those aliases to be inserted as well.
      /*
	qfontname = "";
      qfontname = *fontNames;
      if(fontlist.find(qfontname) == -1)
          fontlist.inSort(qfontname);
      */

      fontNames ++;
      continue;
    };
      
    qfontname = "";
    qfontname = *fontNames;
    int dash = qfontname.find ('-', 1, TRUE); // find next dash

    if (dash == -1)  // No such next dash -- this shouldn't happen.
                      // but what do I care -- lets skip it.
      {
	fontNames ++;
	continue;
      }

    // the font name is between the second and third dash so:
    // let's find the third dash:
    int dash_two = qfontname.find ('-', dash + 1 , TRUE); 

    if (dash == -1)  // No such next dash -- this shouldn't happen.
                      // But what do I care -- lets skip it.
      {
	fontNames ++;
	continue;
      }

    // fish the name of the font info string
    qfontname = qfontname.mid(dash +1, dash_two - dash -1);

    if( !qfontname.contains("open look", TRUE))
      {
	if(qfontname != "nil"){
	  if(fontList.find(qfontname) == -1)
	    fontList.inSort(qfontname);
	}
      }
    
    
    fontNames ++;

  }
  
  XFreeFontNames(fontNames_copy);
  XCloseDisplay(kde_display);
}

/*=========================== Font selected =====================*/
void KCharSelectDia::fontSelected(const char *_family)
{
  _font.setFamily(_family);
  charSelect->setFont(_family);
  lPreview->setFont(_font);
}

/*===================== size selected ===========================*/
void KCharSelectDia::sizeSelected(int i)
{
  _font.setPointSize(i+4);
  lPreview->setFont(_font);
}

/*===================== color selected ==========================*/
void KCharSelectDia::colorChanged(const QColor& __color)
{
 _color = __color;
 QColorGroup cgrp(__color,lPreview->colorGroup().background(),
		  lPreview->colorGroup().light(),lPreview->colorGroup().dark(),
		  lPreview->colorGroup().mid(),__color,lPreview->colorGroup().base());
 QPalette p(cgrp,cgrp,cgrp);
 
 lPreview->setPalette(p);
}

/*====================== bold clicked ===========================*/
void KCharSelectDia::boldChanged()
{
  _font.setBold(bold->isChecked());
  lPreview->setFont(_font);
}

/*====================== italic clicked =========================*/
void KCharSelectDia::italicChanged()
{
  _font.setItalic(italic->isChecked());
  lPreview->setFont(_font);
}

/*====================== underline clicked ======================*/
void KCharSelectDia::underlChanged()
{
  _font.setUnderline(underl->isChecked());
  lPreview->setFont(_font);
}

/*================== character changed ==========================*/
void KCharSelectDia::charChanged(int __c)
{
  char chr[5];

  sprintf(chr,"%c",__c);

  lPreview->setText(chr);
  _c = __c;
}
