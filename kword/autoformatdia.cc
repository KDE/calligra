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
/* Module: Autoformat Dialog                                      */
/******************************************************************/

#include "kword_doc.h"
#include "kword_page.h"

#include "autoformatdia.h"
#include "autoformatdia.moc"

#include "autoformat.h"

#include <klocale.h>

/******************************************************************/
/* Class: KWAutoFormatDia                                         */
/******************************************************************/

/*================================================================*/
KWAutoFormatDia::KWAutoFormatDia(QWidget *parent,const char *name,KWordDocument *_doc,KWPage *_page)
  : QTabDialog(parent,name,true)
{
  doc = _doc;
  page = _page;

  setupTab1();

  setCancelButton(i18n("Cancel"));
  setOkButton(i18n("OK"));

  resize(500,300);
  connect(this,SIGNAL(applyButtonPressed()),this,SLOT(applyConfig()));
}

/*================================================================*/
void KWAutoFormatDia::setupTab1()
{
  tab1 = new QVBox(this);

  tab1->setMargin(10);

  cbTypographicQuotes = new QCheckBox(tab1);
  cbTypographicQuotes->setText(i18n("Replace &Quotes by Typographical Quotes:"));
  cbTypographicQuotes->resize(cbTypographicQuotes->sizeHint());
  cbTypographicQuotes->setChecked(doc->getAutoFormat().getConfigTypographicQuotes().replace);

  QHBox *quotes = new QHBox(tab1);
  pbQuote1 = new QPushButton(quotes);
  pbQuote1->setText(doc->getAutoFormat().getConfigTypographicQuotes().begin);
  pbQuote1->resize(pbQuote1->sizeHint());
  pbQuote2 = new QPushButton(quotes);
  pbQuote2->setText(doc->getAutoFormat().getConfigTypographicQuotes().end);
  pbQuote2->resize(pbQuote2->sizeHint());
  (void)new QWidget(quotes);
  quotes->setMaximumHeight(pbQuote1->sizeHint().height());

  (void)new QWidget(tab1);

  cbUpperCase = new QCheckBox(tab1);
  cbUpperCase->setText(i18n("Convert first letter from the first word of a sentence automatically\n"
			    "to &Upper Case (e.g. \"bla. this is a Test\" to \"bla. This is a Test\")"));
  cbUpperCase->resize(cbUpperCase->sizeHint());
  cbUpperCase->setChecked(doc->getAutoFormat().getConfigUpperCase());

  (void)new QWidget(tab1);

  cbUpperUpper = new QCheckBox(tab1);
  cbUpperUpper->setText(i18n("Concert two Upper &Case letters to one Upper Case and one Lower Case letter.\n"
			     "(e.g. HAllo to Hallo)"));
  cbUpperUpper->resize(cbUpperUpper->sizeHint());
  cbUpperUpper->setChecked(doc->getAutoFormat().getConfigUpperUpper());

  (void)new QWidget(tab1);

  addTab(tab1,i18n("Simple Autocorrection"));

  resize(minimumSize());
}

/*================================================================*/
void KWAutoFormatDia::applyConfig()
{
  KWAutoFormat::TypographicQuotes tq = doc->getAutoFormat().getConfigTypographicQuotes();
  tq.replace = cbTypographicQuotes->isChecked();
  doc->getAutoFormat().configTypographicQuotes(tq);

  doc->getAutoFormat().configUpperCase(cbUpperCase->isChecked());
  doc->getAutoFormat().configUpperUpper(cbUpperUpper->isChecked());

  doc->getAutoFormat().setEnabled(true);
  doc->updateAllViews(0L);
  doc->updateAllCursors();
  doc->getAutoFormat().setEnabled(false);
}
