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
#include <qevent.h>
#include <qlayout.h>

#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <kglobalsettings.h>

#include "kexiinputtableedit.h"

KexiInputTableEdit::KexiInputTableEdit(QVariant value, QVariant::Type type, QString ov, bool mark, QWidget *parent, const char *name)
 : KexiTableEdit()
{
	m_type = type;
	m_value = value;
	m_view = new QLineEdit(this, "tableLineEdit");
	m_view->installEventFilter(this);
	static_cast<QLineEdit*>(m_view)->setFrame(false);
	QPalette p(m_view->palette());
	p.setColor(QColorGroup::Base, KGlobalSettings::alternateBackgroundColor()); //QColor(200,200,255));
	m_view->setPalette(p);
	
	if(ov != QString::null)
	{
		switch(m_type)
		{
			case QVariant::Double:
				if(ov == KGlobal::_locale->decimalSymbol() || ov == KGlobal::_locale->monetaryDecimalSymbol())
					static_cast<QLineEdit*>(m_view)->setText(ov);
					
			case QVariant::Int:
				if(ov == KGlobal::_locale->negativeSign())
					static_cast<QLineEdit*>(m_view)->setText(ov);

			case QVariant::UInt:
				if(ov == "1" || ov == "2" || ov == "3" || ov == "4" || ov == "5" || ov == "6" || ov == "7" || ov == "8" || ov == "9" || ov == "0")
					static_cast<QLineEdit*>(m_view)->setText(ov); 
					break;
				
				if(ov == "=")
					m_calculatedCell = true;
					static_cast<QLineEdit*>(m_view)->setText(ov);
					break;
				

			default:
				static_cast<QLineEdit*>(m_view)->setText(ov);
				break;
		}
	}
	else
	{
//		qDebug("KexiInputTableEdit::KexiInputTableEdit(): have to mark! (%i)", text().length());
//		setText(value.toString());
		static_cast<QLineEdit*>(m_view)->insert(value.toString());
		qDebug("KexiInputTableEdit::KexiInputTableEdit(): have to mark! (%i)", static_cast<QLineEdit*>(m_view)->text().length());
		static_cast<QLineEdit*>(m_view)->setSelection(0, static_cast<QLineEdit*>(m_view)->text().length());
		static_cast<QLineEdit*>(m_view)->selectAll();
	}

	m_calculatedCell = false;
}

bool
KexiInputTableEdit::eventFilter(QObject* watched, QEvent* e)
{
	if(watched == m_view)
	{
		if(e->type() == QEvent::KeyPress)
		{
			QKeyEvent* ev = static_cast<QKeyEvent*>(e);
			kdDebug() << "KeyPress = " << ev->text() << endl;
			
			switch(m_type)
			{
				case QVariant::Double:
					if(ev->text() == KGlobal::_locale->decimalSymbol() || ev->text() == KGlobal::_locale->monetaryDecimalSymbol())
						return false;
						
					return true;
				
				case QVariant::Int:
					if(ev->text() == KGlobal::_locale->negativeSign())
						return false;
						
					return true;

				case QVariant::UInt:
					if(m_calculatedCell)
					{
						return false;
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
							return false;
							break;

						case Key_Equal:
							m_calculatedCell = true;
							return true;
							break;

						case Key_Backspace:
							if(static_cast<QLineEdit*>(m_view)->text() == "=")
								m_calculatedCell = false;
							return false;

						default:
							return true;
							break;
					}

					break;

				default:
					return false;
					break;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return KexiTableEdit::eventFilter(watched, e);
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
				qDebug("KexiInputTableEdit::value() converting => %s", static_cast<QLineEdit*>(m_view)->text().latin1());
				v = static_cast<QLineEdit*>(m_view)->text().replace(QRegExp("\\" + KGlobal::_locale->thousandsSeparator()), "");
				v = v.replace(QRegExp("\\" + KGlobal::_locale->decimalSymbol()), ".");
				v = v.replace(QRegExp("\\" + KGlobal::_locale->negativeSign()), "-");
				qDebug("KexiInputTableEdit::value() converting => %s", v.latin1());
				return QVariant(v);
			}
			else
			{
				//ok here should the formula be parsed so, just feel like in perl :)
				double result = 0;
				QString real = static_cast<QLineEdit*>(m_view)->text().right(static_cast<QLineEdit*>(m_view)->text().length() - 1);
				real = real.replace(QRegExp("\\" + KGlobal::_locale->thousandsSeparator()), "");
				real = real.replace(QRegExp("\\" + KGlobal::_locale->decimalSymbol()), ".");
//				qDebug("KexiInputTableEdit::value() calculating '%s'", real.latin1());
				QStringList values = QStringList::split(QRegExp("[\\+|\\*|\\/|-]"), real, false);
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
			return QVariant(static_cast<QLineEdit*>(m_view)->text());
	}
//	return QVariant(0);
}

void
KexiInputTableEdit::end(bool mark)
{
	static_cast<QLineEdit*>(m_view)->end(mark);
}

void
KexiInputTableEdit::backspace()
{
	static_cast<QLineEdit*>(m_view)->backspace();
}

#include "kexiinputtableedit.moc"
