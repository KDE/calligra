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
/* Module: Delete Dialog (header)                                 */
/******************************************************************/

#ifndef deldia_h
#define deldia_h

#include <qtabdialog.h>

class KWGroupManager;
class KWordDocument;
class KWPage;
class QWidget;
class QGridLayout;
class QLabel;
class QSpinBox;

/******************************************************************/
/* Class: KWDeleteDia                                             */
/******************************************************************/

class KWDeleteDia : public QTabDialog
{
	Q_OBJECT

public:
	enum DeleteType {ROW,COL};
	KWDeleteDia(QWidget *parent,const char *name,KWGroupManager *_grpMgr,KWordDocument *_doc,DeleteType _type,KWPage *_page);

protected:
	void setupTab1();
	void closeEvent(QCloseEvent *e) { emit cancelButtonPressed(); }

	QWidget *tab1;
	QGridLayout *grid1;
	QLabel *rc;
	QSpinBox *value;

	KWGroupManager *grpMgr;
	KWordDocument *doc;
	DeleteType type;
	KWPage *page;

protected slots:
	void doDelete();

};

#endif


