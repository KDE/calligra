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
/* Module: Insert Dialog (header)                                 */
/******************************************************************/

#ifndef insdia_h
#define insdia_h

#include <qtabdialog.h>

class KWGroupManager;
class KWordDocument;
class KWPage;
class QWidget;
class QGridLayout;
class QButtonGroup;
class QRadioButton;
class QLabel;
class QSpinBox;

/******************************************************************/
/* Class: KWInsertDia                                             */
/******************************************************************/

class KWInsertDia : public QTabDialog
{
	Q_OBJECT

public:
	enum InsertType {ROW,COL};
	KWInsertDia(QWidget *parent,const char *name,KWGroupManager *_grpMgr,KWordDocument *_doc,InsertType _type,KWPage *_page);

protected:
	void setupTab1();
	void closeEvent(QCloseEvent *e) { emit cancelButtonPressed(); }

	QWidget *tab1;
	QGridLayout *grid1,*grid2;
	QButtonGroup *grp;
	QRadioButton *rBefore,*rAfter;
	QLabel *rc;
	QSpinBox *value;

	KWGroupManager *grpMgr;
	KWordDocument *doc;
	InsertType type;
	KWPage *page;

protected slots:
	void doInsert();

};

#endif


