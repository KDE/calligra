/***************************************************************************
                          moneyedit.h  -  description
                             -------------------                                         
    begin                : 27 Maí 1999
    copyright            : (C) 1999 by Ørn E. Hansen
    email                : oehansen@nettaxi.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/


#ifndef KDB_MONEYEDIT_H
#define KDB_MONEYEDIT_H

#include <qlineedit.h>
#include <langinfo.h>
#include <qtimer.h>
#include <qfont.h>
#include <qframe.h>
#include <qvalidator.h>

#include <kdbdatafield.h>

extern "C" {

#include <limits.h>
#include <ctype.h>

};

#define yes_str         nl_langinfo(YESSTR)
#define no_str          nl_langinfo(NOSTR)
#define regex_yes       nl_langinfo(YESEXPR)
#define regex_no        nl_langinfo(NOEXPR)

#define decimal_point   nl_langinfo(DECIMAL_POINT)
#define thousands_sep   nl_langinfo(THOUSANDS_SEP)

/** Edit money types, according to locale needs.
	*
	* This widget, extends the QLineEdit widget, to be able to
	* edit values.  Values that represent currency.  Currency, is
	* usually edited as right aligned text, or more specifically.
	* As aligned at the decimal point.
	*
	* By right aligning the text, with fixed number of decimals.
	* the text appears, as right aligned at the decimal point.
	* Only at the decimal point, can digits be inserted.  Or, if
	* the value doesn't allow decimals.
	*
	* moneyEdit, is fully locale compatible.  All values, will
	* be displayed according to the monetary information.  Located
	* in the locale database.  Negative values, will be displayed
	* with brackets '()', and currency symbols '£', '$', 'Kr' will
	* be displayed along side the value.  If so ordered in the
	* locale.
	*
  *@author Örn E. Hansen
  *@short Make editing of monetary values, locale specific.
  */

class kdbMoneyEdit : public QFrame {
	Q_OBJECT

public:
	enum _v_type {
		_v_neg,
		_v_pos
	};

private:
	/** Beginning position of any marked text. */
	unsigned int _markBeg;
	/** Starting position of the cursor. */
	unsigned int _offset;
	/** The position of the cursor, within the editable text. */
	unsigned int _cursor;
	/** Number of fraction digits, to display. */
	unsigned int _fract;
	/** Sign positions, as is stored within the monetary information
	 * of the locale.  There can be different signs, for negative and
	 * positive values. */
	unsigned int _nsignp, _psignp;
	/** This index, marks the starting position of the editable text,
	 * within the display representation. */
	unsigned int _index;
	/** The decimal point, used for displaying the text (not operational). */
	char _decimal_point;
	/** This is true, if the widget is being edited. */
	bool _editing;
	/** Ensure that any belate working of he paintEvent doesn't make us
	 * call it agian (through timeoutEvent), if it already working. */
	bool _painting;
	/** Not really used, but marks that widget is being initialized. */
	bool _initial;
	/** Is the cursor, on or off? */
	bool _blinkPhase;
	/** Are characters to be inserted, or overwritten? */
	bool _insert;
	/** Does space separate sign and value? */
	bool _nspace, _pspace;
	/** Does symbol precede or succeed value? */
	bool _nprecedes, _pprecedes;
	/** Is the value negative or positive? */
	bool _signed;
	/** An internal timer, used to blink the cursor. */
	QTimer *_timer;
	/** Actual value, as the user will see it represented to him. */
	QString _value;
	/** The actual sign string, used to signal a positive or negative
	 * values. */
	QString _nsign, _psign;
	/** A text representing the symbol, that represent the monetary
	 * currency being edited. */
	QString _symbol;
	QString _txt;
	kdbDataField *_field;

	/** Initializes variables. */
	void initVar();
	/** Calculates the cursor position, and decides if the editing is
		* in insert, or overwrite mode.  The cursor inside the text, is
		* not quite the same as inside the presented value.  The difference
		* between the two, is not constant either.  As the user can change
		* the representation.  This routine updates the cursor to point
		* correctly into the presentation text.
	*/
	void cursorLogic();
	/** Updates the displayed value, in accordance to changed rules
		* or text in the QLineEdit buffer. The value displayed, which
		* holds information as sign, possible symbol and perhaps even
		* parenthesis.  Are quite different from the text buffer inside
		* QLineEdit, which holds only the raw numberic text.  This
		* routine does the necessary logic, to make that text presentable
		* to the user according to the locale monetary information.
	*/
	void updateValue();

	/** Calculates a rectangular area, starting at cursor position
		* and ending in another cursor position.  The integer parameters
		* are the X1 and X2 positions.  This is used internally, to avoi
		* repainting the entire widget, when a character changes in the
		* text.
	*/
	QRect calculateArea(QPainter&, int, int);

	/**
	 * Change the text and redisplay the area (x1,x2), to reflect these
	 * changes to the human user.
	 */
	void changeText(int,int,const char *);
	
	/** Make the cursor visible, or make it disappear */
	void cursorVisible(bool);
	
	/** Return the position of the decimal character. */
	unsigned int decimalPosition(QString&);
	
	/** Repaint a specific area, inside the represented text. */
	void repaintArea(int, int);
	
	/** Return a string in science notation. */
	QString scienceNotation(const QString&);
	
public:
	kdbMoneyEdit(kdbDataField*,QWidget *p=0,const char *n=0);
	~kdbMoneyEdit();

	/** Specify the number of decimals to use, when displaying the value. */
	void setFract(unsigned int);
	/** Use this to decide the position of the pos/neg sign. */
	void setSignPos(_v_type, unsigned int);
	/** Use this to change the sign for pos/neg values. */
	void setSign(_v_type, const char *);
	/** Use this to decide wheter sign/symbol precedes/succedes the value. */
	void setPrecedes(_v_type, bool);
	/** Set this to have a space between the symbol and value. */
	void setSpace(_v_type, bool);
	/** Use this to set a new symbol, to display for the value. */
	void setSymbol(const char *);
	/**
	 * Use this to set a new decimal character. The character passed
	 * must be a valid punctuation character.
	 */
	void setDecimalChar(char);

	/** Return the number of digits, displayed as decimals. */
	unsigned int fract();
	/** Return the sign position, for either pos/neg values. */
	unsigned int signPos(_v_type);
	/** Return the sign used, for pos/neg values. */
	const char  *sign(_v_type);
	/** Tell if the symbol precedes/succedes the value. */
	bool         precedes(_v_type);
	/** Tell if a space seperates the symbol and value. */
	bool         space(_v_type);
	/** Return the symbol, displayed. */
	const char  *symbol();
	/** Return the decimal character */
	char         decimalChar()                { return _decimal_point; };

	/**
	 * This function, will return the value inside the widget in
	 * a science notation.  Suitable for evaluation as a double
	 * value, within any locale.  A text with ',' as the decimal
	 * point, and containing '123,45' as the value.  Will be returned
	 * by this routine as '12345e-2'.
	 */
	QString      text()                       { return scienceNotation(_txt); };
	
	/** Bind a datafield, to this widget. */
	void bindField(kdbDataField *);
	
protected:
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void keyPressEvent(QKeyEvent *);
	virtual void paintEvent(QPaintEvent *);
	virtual void focusInEvent(QFocusEvent *);
	virtual void focusOutEvent(QFocusEvent *);
	virtual bool event(QEvent *);

signals:
	void textChanged(const QString&);
	
protected slots:
	/**
	 * This is an internal timout event, run on a private timer.  It is
	 * used to blink the cursor.
	*/
	virtual void timeoutEvent();

public slots:
	/**
	 * Set the value to be edited.  And redisplay the area to conform
	 * to the changed value.  This slot accepts a double value, as the
	 * parameter.
	 */
	void setValue(double);
	/**
	 * This slot is to provide same functional interface, as is with
	 * QLineEdit.  The widget will accept the number to edit, in a
	 * text string format.  It should be noted, that this value
	 * must contain a valid representation of a double number, to
	 * be set correctly within the widget.  As this widget will
	 * accept any character, as a decimal character.  This for a
	 * text represented value to be handled correctly within the
	 * system, it should be contained in science notation.  So,
	 * see
	 * @ref #text
	 * for a way to retrieve the text in this format, and for
	 * further discussions on it.
	 */
	void setText(const QString&);
	/**
	 * Provides a similar functional interface, as QLineEdit does. As
	 * with
	 * @ref #setText
	 * the text inserted, must in its entirety contain a valid
	 * number that can be converted to scientific notation.  The
	 * text will be transformed accordingly before displayed.
	 */
	void insert(const QString&);

};

#endif




























