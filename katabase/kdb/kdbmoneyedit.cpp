/***************************************************************************
                          kdbMoneyEdit.cpp  -  description
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


#include <iostream.h>

#include <qapplication.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include <qevent.h>
#include <qkeycode.h>
#include <qapplication.h>

#include "kdbmoneyedit.h"

kdbMoneyEdit::kdbMoneyEdit(kdbDataField *p_fld,QWidget *p, const char *n)
 : QFrame(p, n)
{
	_field = p_fld;
	initVar();
}

/**
 * This widget is primary thought off, as a part of a database
 * editing widget.  Where a person can edit currency values, that
 * aren't native to his/her own.  This will bind the current widget
 * to such a field, making sure that any changes made to the widget
 * text will be mirrored in the field.
 */
void
kdbMoneyEdit::bindField(kdbDataField *p_field)
{
	_field = p_field;
}

/**
 * Initialize variables.  All variables are given default values,
 * and locale information is initialized from the locale database.
 */
void
kdbMoneyEdit::initVar()
{
	_editing      = false;
	_painting     = false;
	_blinkPhase   = false;
	_signed       = false;
	_cursor       = 0;
	_offset       = 0;
	_markBeg      = 0;
	_decimal_point= *decimal_point;
	_symbol       = localeconv()->currency_symbol;
	_fract        = int(localeconv()->frac_digits);
	_psignp       = int(localeconv()->p_sign_posn);
	_psign        = localeconv()->positive_sign;
	_pspace       = bool(localeconv()->p_sep_by_space)?" ":"";
	_pprecedes    = bool(localeconv()->p_cs_precedes);
	_nsignp       = int(localeconv()->n_sign_posn);
	_nsign        = localeconv()->negative_sign;
	_nspace       = bool(localeconv()->n_sep_by_space)?" ":"";
	_nprecedes    = bool(localeconv()->n_cs_precedes);
	if (_fract == 127)
		_fract = 2;
	_txt.sprintf("%1.*f", _fract, 0.0);
	updateValue();
	cursorLogic();
	_timer = new QTimer();
	setBackgroundMode(PaletteBase);
	connect(_timer, SIGNAL(timeout()), this, SLOT(timeoutEvent()));
	setCursor(IbeamCursor);
	setFocusPolicy(QWidget::StrongFocus);
	setFrameStyle(QFrame::Panel|QFrame::Sunken);
	setLineWidth(2);
	setAcceptDrops(true);
}

kdbMoneyEdit::~kdbMoneyEdit()
{
}

QString
kdbMoneyEdit::scienceNotation(const QString& p_ref)
{
	QString representation = p_ref;
	unsigned int digits;
	
	digits = decimalPosition(representation);
	if (digits < representation.length()) {
		representation.remove(digits, 1);
		digits = representation.length() - digits;
		representation += QString("").sprintf("e-%d",digits);
	}
	return representation;
}

unsigned int
kdbMoneyEdit::decimalPosition(QString& p_txt)
{
	int pos = p_txt.find(_decimal_point);
	
	if (pos >= 0)
		return pos;
	return p_txt.length();
}

/**
 * Turn the cursor, on or off... depending on the value of the
 * boolen parameter.
 */
void
kdbMoneyEdit::cursorVisible(bool p_stat)
{
	QPainter paint;
	unsigned int tmp_curs = _cursor+_index;
		
	if (!_editing || _blinkPhase == p_stat)
		return;
	_blinkPhase = p_stat;
	paint.begin(this);
	QRect rect = calculateArea(paint, tmp_curs, 1);
	QFontMetrics fm(font());
	if (_blinkPhase) {
		paint.setRasterOp(XorROP);
		if (!_insert)
			paint.fillRect(rect.left(),rect.top()+1,fm.width(_value[tmp_curs]),rect.bottom()-2,colorGroup().text());
		else
			paint.drawLine(rect.left(),rect.top()+1,rect.left(),rect.bottom()-2);
	} else {
		paint.eraseRect(rect);
		paint.drawText(rect,AlignVCenter,_value.mid(tmp_curs,1));
	}
	paint.end();
}

void
kdbMoneyEdit::repaintArea(int p_x1, int p_len)
{
	QPainter paint;
	QFontMetrics fm(font());
	int x1 = (p_x1>=0?p_x1:0);
		
	paint.begin(this);
	QRect rect=calculateArea(paint,x1,p_len);
	if (p_x1 == -1)
		paint.eraseRect(0,rect.y(),rect.left(),rect.height());
	_offset = 0;
	if (rect.left() < 0) {
		_offset = x1;
		while (-rect.left() > fm.width(_value.left(_offset)))
			_offset += 1;
		if (_cursor < _offset)
			_cursor = _offset;
	}
	paint.end();
	QPaintEvent pev(rect);
	paintEvent(&pev);
}

/**
 * Change the edited text, and reflect those changes by updating
 * the displayed value.  Finish by repainting the area, starting
 * at p_x1 and ending at p_x2.
 */
void
kdbMoneyEdit::changeText(int p_x1, int p_x2, const char *p_txt=0)
{
	if ( p_txt )
		_txt = p_txt;
	if ( _field )
		_field->set( _txt );
	updateValue();
	cursorLogic();
	if ( p_x1 >= 0 )
		p_x1 += _index;
	repaintArea( p_x1,p_x2-p_x1+1 );
}

/**
 * Blink the cursor, every 500 ms.
 */
void
kdbMoneyEdit::timeoutEvent()
{
	if (_editing)
		cursorVisible(!_blinkPhase);
}

void
kdbMoneyEdit::mousePressEvent(QMouseEvent *p_ev)
{
}

/**
 * Clicking the mouse inside the widget, will update the
 * cursor position, but also make sure the cursor points
 * only within the editable text and doesn't roam outside
 * into the display form text (symbol, sign).
 */
void
kdbMoneyEdit::mouseReleaseEvent(QMouseEvent *p_ev)
{
	QPainter paint;

	if (p_ev->button() == LeftButton) {
		if (_initial == false) {
			cursorVisible(false);
			paint.begin(this);
			QFontMetrics fm(font());
			QRect rect = paint.boundingRect(contentsRect(),AlignRight|AlignVCenter,_value.data(),_value.length());
			int w = fm.width(_value.left(_index));
			unsigned int i;
			for(i=0;(rect.left() + w) < p_ev->x() && i < _txt.length();i++)
				w += fm.width(_txt[i]);
			_cursor = i;
			cursorLogic();
			paint.end();
			cursorVisible(true);
		} else
			_initial = false;
	}
}

/**
 * When this widget receives focus, we need to do some basic stuff
 * to display the data within it, and start the cursor blinking.
 */
void
kdbMoneyEdit::focusInEvent(QFocusEvent *p_ev)
{
	_blinkPhase = false;
	_editing    = true;
	_initial    = true;
	_cursor     = _txt.length();
	if ( _txt.find(_decimal_point) >= 0 )
		_cursor = _txt.find( _decimal_point );
	cursorLogic();
	repaint();
	cursorVisible( true );
	_timer->start( QApplication::cursorFlashTime()/2 );
}

/**
 * When the widget loses focus, we must turn off the blinking of the
 * cursor and redraw the widget.
 */
void
kdbMoneyEdit::focusOutEvent(QFocusEvent *p_ev)
{
	_editing = false;
	_timer->stop();
	repaint();
}

/**
 * Calculate the cursor position, and change between insert and
 * overwrite cursor in accordance with the cursor position.  Simply
 * this means, that iff we have a value with decimals.  We allow
 * inserting of text only at the decimal position, roaming outside
 * the decimal point will turn overwrite cursor on.  Iff we do not
 * have a value with decimals, value can be inserted anywhere within
 * the text (constant insert mode).
 */
void
kdbMoneyEdit::cursorLogic()
{
	int fpos;

	if (_cursor >= _txt.length())
		_cursor = _txt.length()>0?_txt.length():0;
	_insert = true;
	fpos = _txt.find(_decimal_point);
	if (fpos >= 0 && _cursor != (unsigned int)fpos) {
		_insert = false;
		if (_cursor == _txt.length() && !_insert)
			--_cursor;
	}
}

/**
 * Slot: setValue(double)
 * Change the editable text, to represent a specific value.
 */
void
kdbMoneyEdit::setValue(double p_val)
{
	_signed = p_val < 0;
	_txt.sprintf("%1.*f", _fract, fabs(p_val));
	_cursor=0;
	_markBeg=0;
	if (_field != 0)
		_field->set(_txt);
	updateValue();
	cursorLogic();
	repaint();
	emit textChanged( _txt );
}

/**
 * Slot: setText(const char *)
 * This slot simply grabs the slot from QLineEdit, to ensure that
 * any programmable changes to the text will be correctly handled
 * and cursor position, value evaluation done.
 */
void
kdbMoneyEdit::setText(const QString& p_str)
{
	QString str = scienceNotation(p_str);
	
	setValue(str.toDouble());
}

/**
 * Slot: insert(const char *)
 * This slot simply grabs the slot from QLineEdit to ensure that
 * any programmable pasting will be correctly handled, for text
 * evaluation and cursor position calculation.
 */
void
kdbMoneyEdit::insert(const QString& p_str)
{
	QString str;
	
	_txt.insert( _cursor,p_str );
	str = scienceNotation( _txt );
	_txt.sprintf( "%1.*f", _fract, str.toDouble() );
	if ( _field != 0 )
		_field->set( _txt );
	updateValue();
	cursorLogic();
	repaint();
	emit textChanged( _txt );
}

/**
 * Update the represented value, according to user wishes.  This
 * routine makes a text, representing the editable value within
 * a format specified by monetary settings.
 *@ref langinfo.h
 */
void
kdbMoneyEdit::updateValue()
{
	int signp;
	bool precedes;
	QString sign;
	QString space;

	if (!_signed) {
		signp    = _psignp;
		sign     = _psign;
		space    = _pspace?" ":"";
		precedes = _pprecedes;
	} else {
		signp    = _nsignp;
		sign     = _nsign;
		space    = _nspace?" ":"";
		precedes = _nprecedes;
	}
	switch(signp) {
		case 0:  // surrounded by parenthesis
			_value = (precedes?(_symbol+space+_txt):(_txt+space+_symbol));
			_value = QString("( ") + _value + QString(" )");
			break;
		case CHAR_MAX:
		case 1:  // precedes value and symbol
			_value = sign + (precedes?(_symbol+space+_txt):(_txt+space+_symbol));
			break;
		case 2:  // succedes value and symbol
			_value = (precedes?(_symbol+space+_txt):(_txt+space+_symbol)) + sign;
			break;
		case 3:  // precedes symbol
			_value = (precedes?(sign+_symbol+space+_txt):(_txt+space+sign+_symbol));
			break;
		case 4:  // succedes symbol
			_value = (precedes?(_symbol+sign+space+_txt):(_txt+space+_symbol+sign));
			break;
	}
	_index = _value.find(_txt);
}

/**
 * An event to handle any keys pressed on the keyboard, by
 * a user.
 */
void
kdbMoneyEdit::keyPressEvent(QKeyEvent *p_ev)
{
	unsigned int pos;
	static unsigned int isign;

	// We don't want the cursor to blink, inside this routine.
	_timer->stop();
	cursorLogic();
	cursorVisible(false);
	pos = _cursor;
	switch(p_ev->key()) {
		case Key_Right: // Move the cursor right, one character position.
			if (_cursor < _txt.length()-1)
				_cursor++;  // Cursor will not roam beyond the text length.
			else if (_insert && _cursor < _txt.length())
				_cursor++;  // Roam beyond text length, iff insert mode.
			break;
		case Key_Left:  // Move cursor left, one character position.
			if (_cursor > _offset) // iff cursor is beyond first character.
				_cursor--;
			break;
		case Key_Delete: // Delete a character, at cursor position.
			if (_cursor != decimalPosition(_txt)) { // Unless the character is the decimal point.
				if (_insert && _cursor < decimalPosition(_txt) && _cursor > 0)
					_txt.remove(_cursor,1);
				else {
					if (_cursor < decimalPosition(_txt)-1 || (_cursor > 0 && _cursor < decimalPosition(_txt)))
						_txt.remove(_cursor,1);
					else
						_txt[_cursor++] = '0';
				}
				changeText( -1,pos );
				emit textChanged( _txt );
			}
			break;
		case Key_Backspace: // Delete a character, to the left of cursor.
			if (_cursor > 0) { // unless it is a decimal point
				if (_insert || pos < decimalPosition(_txt))
					if (_cursor == 1 && pos == decimalPosition(_txt))
						_txt[_cursor-1] = '0';
					else
						_txt.remove(--_cursor,1);
				else {
					--_cursor;
					if (_cursor != decimalPosition(_txt))
						_txt[_cursor] = '0';
				}
				changeText( -1,pos );
				emit textChanged( _txt );
			}
			break;
		case Key_Home:
		case Key_End:
		case Key_Tab:
//			QLineEdit::keyPressEvent(p_ev);
			break;
		default:
			if (_txt[_cursor] == _decimal_point && p_ev->ascii() == _decimal_point)
				_cursor += 1;
			else if (isdigit(p_ev->ascii())) {
				if (!_insert) {
					_txt[_cursor++] = (char)p_ev->ascii();
					if (_cursor >= _txt.length())
						_cursor = _txt.length()-1;
				} else {
					if (_cursor <= 1 && _txt[0] == '0')
						_txt[0] = (char)p_ev->ascii();
					else
						_txt.insert(_cursor++, (char)p_ev->ascii());
				}
				if (_insert)
					changeText(-1,pos);
				else
					changeText(pos,pos);
				emit textChanged( _txt );
			} else if (p_ev->ascii() == _nsign[isign]) {
					if (++isign < _nsign.length())
						return;
					_signed = !_signed;
					updateValue();
					repaint();
			} else if (p_ev->ascii() == _psign[isign]) {
				if (++isign < _psign.length())
					return;
				_signed = false;
				updateValue();
				repaint();
			}
			break;
	}
	isign = 0;
	cursorLogic();
	cursorVisible(true);
	if (_editing)
		_timer->start(500);
}

QRect
kdbMoneyEdit::calculateArea(QPainter& p_paint, int p_beg, int p_len)
{
	if (p_len <= 0)
		p_len = 1;
	QRect rect = p_paint.boundingRect(contentsRect(),AlignRight|AlignVCenter,_value,_value.length());
  QFontMetrics fm(font());
	rect.setX(rect.left()+fm.width(_value.left(p_beg)));
	rect.setWidth(fm.width(_value.mid(p_beg,p_len)));
	return rect;
}

void
kdbMoneyEdit::paintEvent(QPaintEvent *p_ev)
{
	QPainter paint;
	unsigned int tmp_curs, ch_beg, ch_len;
	bool area = false;

	if (!_painting) {
		_painting = true;
		paint.begin(this);
		area =  rect() != p_ev->rect();
		if (!area || hasFocus() == false)
			drawFrame(&paint);
		ch_beg = 0;
		ch_len = _value.length();
		tmp_curs = _index+_cursor;
		QRect rect = paint.boundingRect(contentsRect(),AlignRight|AlignVCenter,_value,_value.length());
		QFontMetrics fm(font());
		if (area) {
			while(ch_beg < _value.length() && rect.left()+fm.width(_value,_index+ch_beg) < p_ev->rect().left())
				ch_beg++;
			ch_len = _value.length() - ch_beg;
			while(ch_len > 0 && fm.width(_value.mid(ch_beg,ch_len)) > p_ev->rect().width())
				ch_len--;
			if (ch_len <= 0)
				ch_len = 1;
			rect.setX(rect.left()+fm.width(_value.left(ch_beg)));
			rect.setWidth(fm.width(_value.mid(ch_beg,ch_len)));
		}
		paint.eraseRect(rect);
		paint.setPen(colorGroup().text());
		paint.drawText(rect,AlignVCenter,_value.mid(ch_beg, ch_len));
		paint.end();
		_painting = false;
	}
}

bool
kdbMoneyEdit::event(QEvent *p_ev)
{
	bool rv = QWidget::event(p_ev);
	
	if (p_ev->type() == QEvent::Show && _field != 0)
		setText(_field->text());
	return rv;
}

void
kdbMoneyEdit::setDecimalChar(char p_dchar)
{
	unsigned int dp = decimalPosition(_txt);
	
	_decimal_point = p_dchar;
	if (dp < _txt.length()) {
		_txt[dp] = p_dchar;
		updateValue();
		repaint();
	}
}

void
kdbMoneyEdit::setFract(unsigned int p_fract)
{
	_fract = p_fract;
	if (_fract == CHAR_MAX)
		_fract = 2;
	setValue(_signed?-_txt.toDouble():_txt.toDouble());
}

void
kdbMoneyEdit::setSignPos(_v_type p_typ, unsigned int p_pos)
{
	if (p_typ == _v_pos)
		_psignp = p_pos;
	else
		_nsignp = p_pos;
	updateValue();
	repaint();
}

void
kdbMoneyEdit::setSign(_v_type p_typ, const char *p_str)
{
	if (p_typ == _v_pos)
		_psign = p_str;
	else
		_nsign = p_str;
	updateValue();
	repaint();
}

void
kdbMoneyEdit::setSpace(_v_type p_typ, bool p_space)
{
	if (p_typ == _v_pos)
		_pspace = p_space;
	else
		_nspace = p_space;
	updateValue();
	repaint();
}

void
kdbMoneyEdit::setPrecedes(_v_type p_typ, bool p_prec)
{
	if (p_typ == _v_pos)
		_pprecedes = p_prec;
	else
		_nprecedes = p_prec;
	updateValue();
	repaint();
}

void
kdbMoneyEdit::setSymbol(const char *p_sym)
{
	_symbol = p_sym;
	updateValue();
	repaint();
}

unsigned int
kdbMoneyEdit::fract()
{
	return _fract;
}

unsigned int
kdbMoneyEdit::signPos(_v_type p_typ)
{
	if (p_typ == _v_pos)
		return _psignp;
	return _nsignp;
}

const char *
kdbMoneyEdit::sign(_v_type p_typ)
{
	if (p_typ == _v_pos)
		return _psign.data();
	return _nsign.data();
}

bool
kdbMoneyEdit::space(_v_type p_typ)
{
	if (p_typ == _v_pos)
		return _pspace;
	return _nspace;
}

bool
kdbMoneyEdit::precedes(_v_type p_typ)
{
	if (p_typ == _v_pos)
		return _pprecedes;
	return _nprecedes;
}

const char *
kdbMoneyEdit::symbol()
{
	return _symbol;
}

#include "kdbmoneyedit.moc"

