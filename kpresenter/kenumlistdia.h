/******************************************************************/
/* KEnumListDia - (c) by Reginald Stadlbauer 1998                 */
/* Version: 0.0.4                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* KEnumListDia is under GNU GPL                                  */
/******************************************************************/
/* Module: Enumerated List Dialog (header)                        */
/******************************************************************/

#ifndef KENUMLISTDIA_H
#define KENUMLISTDIA_H

#include <qdialog.h>
#include <qfont.h>
#include <qcolor.h>
#include <qstring.h>
#include <qstrlist.h>

class QWidget;
class QGridLayout;
class QLabel;
class QComboBox;
class KColorButton;
class QCheckBox;
class QRadioButton;
class KButtonBox;
class QPushButton;
class QLineEdit;

/******************************************************************/
/* class KEnumListDia                                             */
/******************************************************************/

class KEnumListDia : public QDialog
{
	Q_OBJECT

public:

	static const int NUMBER = 1;
	static const int ALPHABETH = 2;

	// constructor - destructor
	KEnumListDia( QWidget*, const char*, int, QFont, QColor, QString, QString, int, QStrList );
	~KEnumListDia();

	// show enum list dialog
	static bool enumListDia( int&, QFont&, QColor&, QString&, QString&, int&, QStrList );

	// return values
	int type() {return _type; }
	QFont font() {return _font; }
	QColor color() {return _color; }
	QString before() {return _before; }
	QString after() {return _after; }
	int start() {return _start; }

protected:

	// dialog objects
	QGridLayout *grid;
	QLabel *lFont, *lSize, *lColor, *lAttrib, *lBefore, *lAfter, *lStart;
	QLineEdit *eBefore, *eAfter, *eStart;
	QComboBox *fontCombo, *sizeCombo;
	KColorButton *colorButton;
	QCheckBox *bold, *italic, *underl;
	QRadioButton *number, *alphabeth;
	KButtonBox *bbox;
	QPushButton *bOk, *bCancel;

	// values
	int _type;
	QFont _font;
	QColor _color;
	QString _before;
	QString _after;
	int _start;

	QStrList fontList;

protected slots:
	void fontSelected( const QString & );
	void sizeSelected( int );
	void colorChanged( const QColor& );
	void boldChanged();
	void italicChanged();
	void underlChanged();
	void beforeChanged( const QString & );
	void afterChanged( const QString & );
	void startChanged( const QString & );
	void numChanged();
	void alphaChanged();

};

#endif // KENUMLISTDIA
