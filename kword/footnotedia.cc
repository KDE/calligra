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
/* Module: Footnote Dialog                                        */
/******************************************************************/

#include "kword_doc.h"
#include "kword_page.h"
#include "footnotedia.h"
#include "footnotedia.moc"
#include "footnote.h"

#include <qlist.h>
#include <qwidget.h>

#include <klocale.h>

/******************************************************************/
/* Class: KWFootNoteDia                                           */
/******************************************************************/

/*================================================================*/
KWFootNoteDia::KWFootNoteDia(QWidget *parent,const char *name,KWordDocument *_doc,KWPage *_page,int _start)
	: QTabDialog(parent,name,true), start(_start)
{
	doc = _doc;
	page = _page;

	setupTab1();

	setCancelButton(i18n("Cancel"));
	setOkButton(i18n("OK"));

	resize(300,250);
}

/*================================================================*/
void KWFootNoteDia::setupTab1()
{
	tab1 = new QWidget(this);

	addTab(tab1,i18n("Configure Footnote/Endnote"));

	QLabel *l = new QLabel(i18n("Currently there is nothing to configure for\n"
								"footnotes/endnotes. Click ok to insert one!"),tab1);
	l->resize(l->sizeHint());
	l->move(5,5);
			
	connect(this,SIGNAL(applyButtonPressed()),this,SLOT(insertFootNote()));

	resize(minimumSize());
}

/*================================================================*/
void KWFootNoteDia::insertFootNote()
{
	KWFootNote::KWFootNoteInternal *fi = new KWFootNote::KWFootNoteInternal;
	fi->from = start;
	fi->to = -1;
	fi->space = "-";

	QList<KWFootNote::KWFootNoteInternal> *lfi = new QList<KWFootNote::KWFootNoteInternal>();
	lfi->setAutoDelete(false);
	lfi->append(fi);

	KWFootNote *fn = new KWFootNote(doc,lfi);
	fn->setBefore("[");
	fn->setAfter("]");

	page->insertFootNote(fn);
}
