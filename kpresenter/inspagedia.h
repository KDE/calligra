/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: insert page dialog (header)                            */
/******************************************************************/

#ifndef inspagedia_h
#define inspagedia_h

#include <qdialog.h>

#include "global.h"

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

class KPresenterDoc;
class QWidget;
class QSpinBox;
class QRadioButton;
class QPushButton;

/******************************************************************/
/* class InsPageDia                                               */
/******************************************************************/

class InsPageDia : public QDialog
{
	Q_OBJECT

public:
	InsPageDia( QWidget* parent, const char* name, KPresenterDoc *_doc, int _currPage );

protected:
	void uncheckAllPos();
	void uncheckAllMode();

	KPresenterDoc *doc;

	QSpinBox *spinBox;
	QRadioButton *before, *after, *leave, *_move;
	QPushButton *ok, *cancel;

protected slots:
	void leaveClicked();
	void moveClicked();
	void beforeClicked();
	void afterClicked();
	void okClicked();

signals:
	void insertPage( int, InsPageMode, InsertPos );

};

#endif
