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
/* Module: Footnote Dialog (header)                               */
/******************************************************************/

#ifndef footnotedia_h
#define footnotedia_h

#include <stdlib.h>

#include <qtabdialog.h>

class KWordDocument;
class KWPage;
class QWidget;

/******************************************************************/
/* Class: KWFootNoteDia                                           */
/******************************************************************/

class KWFootNoteDia : public QTabDialog
{
	Q_OBJECT

public:
	KWFootNoteDia(QWidget *parent,const char *name,KWordDocument *_doc,KWPage *_page,int _start);

protected:
	void setupTab1();
	void closeEvent(QCloseEvent *e) { emit cancelButtonPressed(); }

	QWidget *tab1;

	KWordDocument *doc;
	KWPage *page;
	int start;

protected slots:
	void insertFootNote();

};

#endif


