/******************************************************************/
/* KCharSelectDia - (c) by Reginald Stadlbauer 1998               */
/* Version: 0.0.4                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* KCharSelectDia is under GNU GPL                                */
/******************************************************************/
/* Module: Character Selector Dialog (header)                     */
/******************************************************************/

#ifndef KCHARSELECTDIA_H
#define KCHARSELECTDIA_H

#include <qdialog.h>
#include <qfont.h>
#include <qcolor.h>
#include <qlist.h>

class QWidget;
class QGridLayout;
class QComboBox;
class KButtonBox;
class QPushButton;
class KCharSelect;
class QLabel;
class KColorButton;
class QComboBox;
class QCheckBox;

/******************************************************************/
/* class KCharSelectDia                                           */
/******************************************************************/

class KCharSelectDia : public QDialog
{
	Q_OBJECT

public:
	// constructor - destructor
	KCharSelectDia( QWidget *parent, const char *name, QList<QFont> *_font, QList<QColor>* _color, QList<QChar>* _chr );

	// select char dialog
	static bool selectChar( QList<QFont> *_font, QList<QColor> *_color, QList<QChar> *_chr );

	// internal
	QList<QFont> *font() { return vFont; }
	QList<QColor> *color() { return vColor; }
	QList<QChar> *chr() { return vChr; }

protected:
	// dialog objects
	QGridLayout *grid, *grid2;
	QComboBox *sizeCombo, *depthCombo;
	KButtonBox *bbox;
	QPushButton *bOk, *bCancel;
	KCharSelect *charSelect;
	QWidget *wid;
	QLabel *lSize, *lColor, *lPreview, *lAttrib, *lDepth;
	KColorButton *colorButton;
	QCheckBox *bold, *italic, *underl;

	// values
	QList<QFont> *vFont;
	QList<QColor> *vColor;
	QList<QChar> *vChr;
	int _depth;

protected slots:
	void fontSelected( const QString & );
	void sizeSelected( int );
	void depthSelected( int );
	void colorChanged( const QColor& );
	void boldChanged();
	void italicChanged();
	void underlChanged();
	void charChanged( const QChar & );

};
#endif //KCHARSELECTDIA_H
