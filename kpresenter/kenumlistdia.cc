/******************************************************************/
/* KEnumListDia - (c) by Reginald Stadlbauer 1998                 */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* KEnumListDia is under GNU GPL                                  */
/******************************************************************/
/* Module: Enumerated List Dialog                                 */
/******************************************************************/

#include "kenumlistdia.h"
#include "kenumlistdia.moc"

/******************************************************************/
/* class KEnumListDia                                             */
/******************************************************************/

/*======================= constructor ============================*/
KEnumListDia::KEnumListDia(QWidget* parent,const char* name,int __type,QFont __font,
			   QColor __color,QString __before,QString __after,int __start)
  : QDialog(parent,name,true)
{
  _type = __type;
  _font = __font;
  _color = __color;
  _before = __before;
  _after = __after;
  _start = __start;

  setCaption("Configure the list");

  grid = new QGridLayout(this,8,5,15,7);

  getFonts();

  lFont = new QLabel("Font:",this);
  lFont->resize(lFont->sizeHint());
  grid->addWidget(lFont,0,0);

  fontCombo = new QComboBox(false,this);
  fontCombo->insertStrList(&fontList);
  fontCombo->resize(fontCombo->sizeHint());
  fontList.find(_font.family());
  fontCombo->setCurrentItem(fontList.at());
  grid->addMultiCellWidget(fontCombo,1,1,0,2);
  connect(fontCombo,SIGNAL(activated(const char*)),this,SLOT(fontSelected(const char*)));

  lSize = new QLabel("Size:",this);
  lSize->resize(lSize->sizeHint());
  grid->addWidget(lSize,0,3);

  sizeCombo = new QComboBox(false,this);
  char chr[5];
  for (unsigned int i = 4;i <= 100;i++)
    {
      sprintf(chr,"%d",i);
      sizeCombo->insertItem(chr,-1);
    }
  sizeCombo->resize(sizeCombo->sizeHint());
  grid->addWidget(sizeCombo,1,3);
  sizeCombo->setCurrentItem(_font.pointSize()-4);
  connect(sizeCombo,SIGNAL(activated(int)),this,SLOT(sizeSelected(int)));

  lColor = new QLabel("Color:",this);
  lColor->resize(lColor->sizeHint());
  grid->addWidget(lColor,0,4);

  colorButton = new KColorButton(_color,this);
  colorButton->resize(colorButton->sizeHint());
  grid->addWidget(colorButton,1,4);
  connect(colorButton,SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));

  lAttrib = new QLabel("Attributes:",this);
  lAttrib->resize(lAttrib->sizeHint());
  grid->addMultiCellWidget(lAttrib,2,2,0,2);

  bold = new QCheckBox("Bold",this);
  bold->resize(bold->sizeHint());
  grid->addWidget(bold,3,0);
  bold->setChecked(_font.bold());
  connect(bold,SIGNAL(clicked()),this,SLOT(boldChanged()));

  italic= new QCheckBox("Italic",this);
  italic->resize(italic->sizeHint());
  grid->addWidget(italic,3,1);
  italic->setChecked(_font.italic());
  connect(italic,SIGNAL(clicked()),this,SLOT(italicChanged()));

  underl = new QCheckBox("Underlined",this);
  underl->resize(underl->sizeHint());
  grid->addWidget(underl,3,2);
  underl->setChecked(_font.underline());
  connect(underl,SIGNAL(clicked()),this,SLOT(underlChanged()));

  lBefore = new QLabel("Before:",this);
  lBefore->resize(lBefore->sizeHint());
  grid->addWidget(lBefore,4,0);

  eBefore = new QLineEdit(this);
  eBefore->resize(lBefore->width(),eBefore->sizeHint().height());
  eBefore->setMaxLength(4);
  eBefore->setText((const char*)_before);
  grid->addWidget(eBefore,5,0);

  lAfter = new QLabel("After:",this);
  lAfter->resize(lAfter->sizeHint());
  grid->addWidget(lAfter,4,1);

  eAfter = new QLineEdit(this);
  eAfter->resize(lAfter->width(),eAfter->sizeHint().height());
  eAfter->setMaxLength(4);
  eAfter->setText((const char*)_after);
  grid->addWidget(eAfter,5,1);

  lStart = new QLabel("Start:",this);
  lStart->resize(lStart->sizeHint());
  grid->addWidget(lStart,4,2);

  eStart = new QLineEdit(this);
  eStart->resize(lStart->width(),eStart->sizeHint().height());
  eStart->setMaxLength(1);
  if (_type == NUMBER)
    sprintf(chr,"%d",_start);
  else
    sprintf(chr,"%c",_start);
  eStart->setText(chr);
  grid->addWidget(eStart,5,2);

  number = new QRadioButton("Numeric",this);
  number->resize(number->sizeHint());
  grid->addWidget(number,4,4);

  alphabeth = new QRadioButton("Alphabethic",this);
  alphabeth->resize(alphabeth->sizeHint());
  grid->addWidget(alphabeth,5,4);

  if (_type == NUMBER)
    number->setChecked(true);
  else
    alphabeth->setChecked(true);

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
  grid->addWidget(bbox,7,4);

  grid->addColSpacing(0,lFont->width());
  grid->addColSpacing(0,fontCombo->width()/3);
  grid->addColSpacing(0,bold->width());
  grid->addColSpacing(0,lBefore->width());
  grid->addColSpacing(1,fontCombo->width()/3);
  grid->addColSpacing(1,italic->width());
  grid->addColSpacing(1,lAfter->width());
  grid->addColSpacing(2,fontCombo->width()/3);
  grid->addColSpacing(2,underl->width());
  grid->addColSpacing(2,lStart->width());
  grid->addColSpacing(3,lSize->width());
  grid->addColSpacing(3,sizeCombo->width());
  grid->addColSpacing(4,lColor->width());
  grid->addColSpacing(4,colorButton->width());
  grid->addColSpacing(4,number->width());
  grid->addColSpacing(4,alphabeth->width());

  grid->addRowSpacing(0,lFont->height());
  grid->addRowSpacing(0,lSize->height());
  grid->addRowSpacing(0,lColor->height());
  grid->addRowSpacing(1,fontCombo->height());
  grid->addRowSpacing(1,sizeCombo->height());
  grid->addRowSpacing(1,colorButton->height());
  grid->addRowSpacing(2,lAttrib->height());
  grid->addRowSpacing(3,bold->height());
  grid->addRowSpacing(3,italic->height());
  grid->addRowSpacing(3,underl->height());
  grid->addRowSpacing(4,lBefore->height());
  grid->addRowSpacing(4,lAfter->height());
  grid->addRowSpacing(4,lStart->height());
  grid->addRowSpacing(4,number->height());
  grid->addRowSpacing(5,eBefore->height());
  grid->addRowSpacing(5,eAfter->height());
  grid->addRowSpacing(5,eStart->height());
  grid->addRowSpacing(5,alphabeth->height());
  grid->addRowSpacing(6,10);
  grid->activate();

  resize(0,0);
  setMaximumSize(size());
  setMinimumSize(size());
}

/*========================= destructor ===========================*/
KEnumListDia::~KEnumListDia()
{
}

/*====================== show enum list dialog ===================*/
bool KEnumListDia::enumListDia(int& __type,QFont& __font,QColor& __color,
			       QString& __before,QString& __after,int& __start)
{
  bool res = false;

  KEnumListDia *dlg = new KEnumListDia(0,"EnumListDia",__type,__font,__color,
				       __before,__after,__start);

  if (dlg->exec() == QDialog::Accepted)
    {
      res = true;
    }

  delete dlg;

  return res;
}

/*=========================== get Fonts =========================*/
void KEnumListDia::getFonts()
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
