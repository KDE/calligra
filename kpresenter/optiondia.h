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
/* Module: Options Dialog (header)                                */
/******************************************************************/

#ifndef OPTIONDIA_H
#define OPTIONDIA_H

#include <qtabdlg.h>
#include <qcolor.h>
#include <qstring.h>

#include <kcolorbtn.h>
#include <krestrictedline.h>

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

class QWidget;
class QLabel;
class QGroupBox;

/******************************************************************/
/* class OptionDia                                                */
/******************************************************************/

class OptionDia : public QTabDialog
{
	Q_OBJECT

public:

	// constructor - destrcutor
	OptionDia( QWidget *parent=0, const char *name=0 );
	~OptionDia();

	// set values
	void setRastX( int rx ) { eRastX->setText( QString().setNum( rx ) ); }
	void setRastY( int ry ) { eRastY->setText( QString().setNum( ry ) ); }
	void setBackCol( const QColor &c ) { bBackCol->setColor( c ); }

	// get values
	int getRastX() { return QString( eRastX->text() ).toInt(); }
	int getRastY() { return QString( eRastY->text() ).toInt(); }
	QColor getBackCol() { return bBackCol->color(); }

private:

	// dialog objecsts
	QWidget *general, *objects;
	QLabel *lRastX, *lRastY, *lBackCol;
	KRestrictedLine *eRastX, *eRastY;
	QGroupBox *txtObj;
	KColorButton *bBackCol;

};
#endif //OPTIONDIA_H






