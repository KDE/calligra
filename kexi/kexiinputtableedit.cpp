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

KexiInputTableEdit::KexiInputTableEdit(QVariant value, QVariant::Type type, QString ov, bool mark, QWidget *parent, const char *name)
 : QLineEdit(parent, name)
{
	m_type = type;
	m_value = value;
	
	if(ov != QString::null)
	{
		switch(m_type)
		{
			case QVariant::Double:
				if(ov == KGlobal::_locale->decimalSymbol() || ov == KGlobal::_locale->monetaryDecimalSymbol())
					setText(ov);
					
			case QVariant::Int:
				if(ov == KGlobal::_locale->negativeSign())
					setText(ov);

			case QVariant::UInt:
				if(ov == "1" || ov == "2" || ov == "3" || ov == "4" || ov == "5" || ov == "6" || ov == "7" || ov == "8" || ov == "9" || ov == "0")
					setText(ov); 
					break;
				
				if(ov == "=")
					m_calculatedCell = true;
					setText(ov);
					break;
				

			default:
				setText(ov);
				break;
		}
	}
	else
	{
//		qDebug("KexiInputTableEdit::KexiInputTableEdit(): have to mark! (%i)", text().length());
//		setText(value.toString());
		insert(value.toString());
		qDebug("KexiInputTableEdit::KexiInputTableEdit(): have to mark! (%i)", text().length());
		setSelection(0, text().length());
		selectAll();
	}

	m_calculatedCell = false;
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
			if(m_calculatedCell)
			{
				QLineEdit::keyPressEvent(ev);
				break;
			}
			
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
				case Key_Escape:
					QLineEdit::keyPressEvent(ev);
					break;
				
				case Key_Equal:
					m_calculatedCell = true;
					break;
				
				case Key_Backspace:
					if(text() == "=")
						m_calculatedCell = false;
					QLineEdit::keyPressEvent(ev);

				default:
					break;
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
			if(!m_calculatedCell)
			{
				qDebug("KexiInputTableEdit::value() converting => %s", text().latin1());
				v = text().replace(QRegExp("\\" + KGlobal::_locale->thousandsSeparator()), "");
				v = v.replace(QRegExp("\\" + KGlobal::_locale->decimalSymbol()), ".");
				v = v.replace(QRegExp("\\" + KGlobal::_locale->negativeSign()), "-");
				qDebug("KexiInputTableEdit::value() converting => %s", v.latin1());
				return QVariant(v);
			}
			else
			{
				//ok here should the formula be parsed so, just feel like in perl :)
				double result = 0;
				QString real = text().right(text().length() - 1);
				real = real.replace(QRegExp("\\" + KGlobal::_locale->thousandsSeparator()), "");
				real = real.replace(QRegExp("\\" + KGlobal::_locale->decimalSymbol()), ".");
//				qDebug("KexiInputTableEdit::value() calculating '%s'", real.latin1());
				QStringList values = QStringList::split(QRegExp("[\+|\*|\/|-]"), real, false);
				QStringList ops = QStringList::split(QRegExp("[0-9]{1,8}(?:\\.[0-9]+)?"), real, false);

				double lastValue = 0;
				QString lastOp = "";
				for(int i=0; i < values.count(); i++)
				{
					double next;
					
					QString op = QString((*ops.at(i))).stripWhiteSpace();
					
					if((*values.at(i+1)) && i == 0)
					{
						double local = (*values.at(i)).toDouble();
						next = (*values.at(i+1)).toDouble();

						QString op = (*ops.at(i));
						if(op == "+")
							result = local + next;
						else if(op == "-")
							result = local - next;
						else if(op == "*")
							result = local * next;
						else
							result = local / next;
					}
					else if((*values.at(i+1)))
					{
						next = (*values.at(i+1)).toDouble();

						QString op = QString((*ops.at(i))).stripWhiteSpace();
						if(op == "+")
							result = result + next;
						else if(op == "-")
							result = result - next;
						else if(op == "*")
							result = result * next;
						else
							result = result / next;	
					}
					
				}
				
				return QVariant(result);
				
			}
			break;
		
		default:
			return QVariant(text());
	}
//	return QVariant(0);
}

KexiInputTableEdit::~KexiInputTableEdit()
{
}

#include "kexiinputtableedit.moc"
