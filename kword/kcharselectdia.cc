/******************************************************************/
/* KCharSelectDia - (c) by Reginald Stadlbauer 1998               */
/* Version: 0.0.4                                                 */
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
KCharSelectDia::KCharSelectDia(QWidget* parent,const char* name,int __c,QFont __font,QStrList _fontList)
  : QDialog(parent,name,true)
{
  _c = __c;
  _font = __font;

  setCaption("Select a character");

  grid = new QGridLayout(this,5,2,15,7);

  fontList = _fontList;

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

  charSelect = new KCharSelect(this,"",__font,_c);
  charSelect->resize(charSelect->sizeHint());
  grid->addWidget(charSelect,2,0);
  connect(charSelect,SIGNAL(activated(int)),this,SLOT(charChanged(int)));

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
  grid->addWidget(bbox,3,0);

  grid->addColSpacing(0,charSelect->width());
  grid->addColSpacing(1,0);
  grid->setColStretch(1,1);

  grid->addRowSpacing(0,lFont->height());
  grid->addRowSpacing(1,fontCombo->height());
  grid->addRowSpacing(2,charSelect->height());
  grid->addRowSpacing(3,bOk->height());
  grid->addRowSpacing(3,bCancel->height());
  grid->setRowStretch(4,1);
  grid->activate();
}

/*===================== destructor ===============================*/
KCharSelectDia::~KCharSelectDia()
{
}

/*======================= about application ======================*/
bool KCharSelectDia::selectChar(QFont &__font,int &__c,QStrList _fontList)
{
  bool res = false;

    KCharSelectDia *dlg = new KCharSelectDia(0,"Select Character",__c,__font,_fontList);
    if (dlg->exec() == QDialog::Accepted)
      {
	__font = dlg->font();
	__c = dlg->c();
	res = true;
      }

    delete dlg;

    return res;
}

/*================== character changed ==========================*/
void KCharSelectDia::charChanged(int __c)
{
  _c = __c;
}

/*=========================== Font selected =====================*/
void KCharSelectDia::fontSelected(const char *_family)
{
  _font.setFamily(_family);
  charSelect->setFont(_family);
}
