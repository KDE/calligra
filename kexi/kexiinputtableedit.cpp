/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

#include <qregexp.h>

#include <kglobal.h>
#include <klocale.h>

#include "kexiinputtableedit.h"

KexiInputTableEdit::KexiInputTableEdit(QVariant value, QVariant::Type type, QWidget *parent, const char *name)
 : QLineEdit(parent, name)
{
	m_type = type;
	m_value = value;
	setText(value.toString());
}

void
KexiInputTableEdit::keyPressEvent(QKeyEvent *ev)
{
	switch(m_type)
	{
		case QVariant::Double:
			if(ev->text() == KGlobal::_locale->decimalSymbol() || ev->text() == KGlobal::_locale->monetaryDecimalSymbol())
				QLineEdit::keyPressEvent(ev);
		case QVariant::Int:
			if(ev->text() == KGlobal::_locale->negativeSign())
				QLineEdit::keyPressEvent(ev);

		case QVariant::UInt:
			qDebug("KexiInputTableEdit::keyPressEvent() num field");
			switch(ev->key())
			{
				case Key_0:
				case Key_1:
				case Key_2:
				case Key_3:
				case Key_4:
				case Key_5:
				case Key_6:
				case Key_7:
				case Key_8:
				case Key_9:
				case Key_Backspace:
				case Key_Return:
				case Key_Enter:
				case Key_Insert:
				case Key_Delete:
				case Key_Pause:
				case Key_Print:
				case Key_Home:
				case Key_End:
				case Key_Left:
				case Key_Up:
				case Key_Right:
				case Key_Down:
				case Key_Prior:
				case Key_Next:
					QLineEdit::keyPressEvent(ev);
					break;

				default:
					if(ev->text() == KGlobal::_locale->thousandsSeparator())
						QLineEdit::keyPressEvent(ev);
					break;
//					ev->accept();
			}
			
			break;
		
		default:
			QLineEdit::keyPressEvent(ev);
			break;
	}
}

QVariant
KexiInputTableEdit::value()
{
	//let qt&mysql understand what we mean... (numeric values)
	QString v;
	switch(m_type)
	{
		case QVariant::UInt:
		case QVariant::Int:
		case QVariant::Double:
//			QString v;
			qDebug("KexiInputTableEdit::value() converting => %s", text().latin1());
			v = text().replace(QRegExp("\\" + KGlobal::_locale->thousandsSeparator()), "");
			v = v.replace(QRegExp("\\" + KGlobal::_locale->decimalSymbol()), ".");
			v = v.replace(QRegExp("\\" + KGlobal::_locale->negativeSign()), "-");
			qDebug("KexiInputTableEdit::value() converting => %s", v.latin1());
			return QVariant(v);
		
		default:
			qDebug("KexiInputTableEdit::value()... not much"); 
			return QVariant(text());
	}
//	return QVariant(0);
}

KexiInputTableEdit::~KexiInputTableEdit()
{
}

#include "kexiinputtableedit.moc"
