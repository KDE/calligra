/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Spacing Dialog                                         */
/******************************************************************/

#include "spacingdia.h"
#include "spacingdia.moc"

/******************************************************************/
/* class SpacingDia                                               */
/******************************************************************/

/*==================== constructor ===============================*/
SpacingDia::SpacingDia(QWidget* parent,int _lineSpacing,int _distBefore,int _distAfter)
  : QDialog(parent,"",true)
{
  QString str;

  lLineSpacing = new QLabel(i18n("Line Spacing (pt)"),this);
  lLineSpacing->move(10,10);
  lLineSpacing->resize(lLineSpacing->sizeHint());

  eLineSpacing = new KRestrictedLine(this,"","0123456789");
  eLineSpacing->move(lLineSpacing->width() + 15,10);
  eLineSpacing->resize(eLineSpacing->sizeHint().width() / 2,eLineSpacing->sizeHint().height());
  str.sprintf("%d",_lineSpacing);
  eLineSpacing->setText(str);

  lDistBefore = new QLabel(i18n("Distance before the Paragraph (pt)"),this);
  lDistBefore->move(10,eLineSpacing->y() + eLineSpacing->height() + 10);
  lDistBefore->resize(lDistBefore->sizeHint());

  eDistBefore = new KRestrictedLine(this,"","0123456789");
  eDistBefore->move(lDistBefore->width() + 15,lDistBefore->y());
  eDistBefore->resize(eDistBefore->sizeHint().width() / 2,eDistBefore->sizeHint().height());
  str.sprintf("%d",_distBefore);
  eDistBefore->setText(str);

  lDistAfter = new QLabel(i18n("Distance after the Paragraph (pt)"),this);
  lDistAfter->move(10,eDistBefore->y() + eDistBefore->height() + 10);
  lDistAfter->resize(lDistAfter->sizeHint());

  eDistAfter = new KRestrictedLine(this,"","0123456789");
  eDistAfter->move(lDistAfter->width() + 15,lDistAfter->y());
  eDistAfter->resize(eDistAfter->sizeHint().width() / 2,eDistAfter->sizeHint().height());
  str.sprintf("%d",_distAfter);
  eDistAfter->setText(str);

  eLineSpacing->move(eDistBefore->x(),eLineSpacing->y());
  eDistAfter->move(eDistBefore->x(),eDistAfter->y());

  cancelBut = new QPushButton(this,"BCancel");
  cancelBut->setText(i18n("Cancel"));

  okBut = new QPushButton(this,"BOK");
  okBut->setText(i18n("OK"));
  okBut->setAutoRepeat(false);
  okBut->setAutoResize(false);
  okBut->setAutoDefault(true);
  okBut->setDefault(true);

  int butW = max(cancelBut->sizeHint().width(),okBut->sizeHint().width());
  int butH = cancelBut->sizeHint().height();

  cancelBut->resize(butW,butH);
  okBut->resize(butW,butH);

  cancelBut->move(eDistAfter->x() + eDistAfter->width() - cancelBut->width(),eDistAfter->y() + eDistAfter->height() + 20);
  okBut->move(cancelBut->x() - okBut->width() - 5,cancelBut->y());

  connect(okBut,SIGNAL(clicked()),this,SLOT(slotSpacingDiaOk()));
  connect(cancelBut,SIGNAL(clicked()),this,SLOT(reject()));
  connect(okBut,SIGNAL(clicked()),this,SLOT(accept()));

  resize(cancelBut->x() + cancelBut->width() + 10,okBut->y()+okBut->height()+10);
}

/*====================== spacing dia ok ==========================*/
void SpacingDia::slotSpacingDiaOk()
{
  int _lineSpacing = atoi(eLineSpacing->text());
  int _distBefore = atoi(eDistBefore->text());
  int _distAfter = atoi(eDistAfter->text());

  emit spacingDiaOk(_lineSpacing,_distBefore,_distAfter);
}












