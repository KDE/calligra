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

/*================================================================*/
KCharSelectDia::KCharSelectDia(QWidget *parent,const char *name,const QChar &_chr,const QString &_font,bool _enableFont)
  : QDialog(parent,name,true)
{
  setCaption("Select a character");

  grid = new QGridLayout(this,3,1,15,7);

  charSelect = new KCharSelect(this,"",_font,_chr);
  charSelect->resize(charSelect->sizeHint());
  charSelect->enableFontCombo(_enableFont);
  grid->addWidget(charSelect,0,0);

  grid->addWidget(new QWidget(this),1,0);

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
  grid->addWidget(bbox,2,0);

  grid->addColSpacing(0,charSelect->width());

  grid->addRowSpacing(0,charSelect->height());
  grid->addRowSpacing(1,0);
  grid->addRowSpacing(2,bCancel->height());
  grid->setRowStretch(0,0);
  grid->setRowStretch(1,1);
  grid->setRowStretch(2,0);

  grid->activate();

  charSelect->setFocus();
}

/*================================================================*/
bool KCharSelectDia::selectChar(QString &_font,QChar &_chr,bool _enableFont)
{
  bool res = false;

  KCharSelectDia *dlg = new KCharSelectDia(0L,"Select Character",_chr,_font,_enableFont);
  
  if (dlg->exec() == QDialog::Accepted)
    {
      _font = dlg->font();
      _chr = dlg->chr();
      res = true;
    }

  delete dlg;

  return res;
}
