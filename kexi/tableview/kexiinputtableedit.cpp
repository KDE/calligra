/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

#include <qregexp.h>
#include <qevent.h>
#include <qlayout.h>
#include <qtimer.h>

#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <kcompletionbox.h>
#include <knumvalidator.h>
	
#include "kexiinputtableedit.h"

KexiInputTableEdit::KexiInputTableEdit(QVariant value, QVariant::Type type, QString ov, bool mark, QWidget *parent, const char *name, QStringList comp)
 : KexiTableEdit()
{
	m_type = type; //TODO(js) remove m_type !
	kdDebug() << "KexiInputTableEdit: value.typeName()==" << value.typeName() << endl;
	kdDebug() << "KexiInputTableEdit: type== " << m_type << endl;
	m_value = value;

	QHBoxLayout *lyr = new QHBoxLayout(this);
	lyr->setAutoAdd(true);
	m_cview = new KLineEdit(this, "tableLineEdit");

	m_view = m_cview;
	m_view->installEventFilter(this);
	m_cview->setFrame(false);
	m_calculatedCell = false;

	connect(m_cview->completionBox(), SIGNAL(activated(const QString &)),
	 this, SLOT(completed(const QString &)));
	connect(m_cview->completionBox(), SIGNAL(highlighted(const QString &)),
	 this, SLOT(completed(const QString &)));

	 m_cview->completionBox()->setTabHandling(true);


	if(!ov.isEmpty())
	{
		switch(m_type)
		{
			case QVariant::Double:
				if(ov == KGlobal::locale()->decimalSymbol() || ov == KGlobal::locale()->monetaryDecimalSymbol())
					m_cview->setText(ov);

			case QVariant::Int:
				if(ov == KGlobal::locale()->negativeSign())
					m_cview->setText(ov);

			case QVariant::UInt:
				if(ov == "1" || ov == "2" || ov == "3" || ov == "4" || ov == "5" || ov == "6" || ov == "7" || ov == "8" || ov == "9" || ov == "0")
					m_cview->setText(ov);
//					break;

				if(ov == "=")
				{
					kdDebug() << "KexiInputTableEdit::KexiInputTableEdit(): calculated!" << endl;
					m_calculatedCell = true;
					m_cview->setText(ov);
				}
				break;


			default:
				m_cview->setText(ov);
				QTimer::singleShot(1, this, SLOT(setRestrictedCompletion()));
				break;
		}
	}
	else
	{
		if (m_type==QVariant::Double) {
			QString tmp_val = value.toString();
			//TODO(js): get decimal places settings here...
			QStringList sl = QStringList::split(".", tmp_val);
			if (tmp_val.isEmpty())
				m_cview->setText("");
			else if (sl.count()==2) {
				kdDebug() << "sl.count()=="<<sl.count()<< " " <<sl[0] << " | " << sl[1] << endl;
				tmp_val = sl[0] + KGlobal::locale()->decimalSymbol() + sl[1];
			}
			m_cview->setText(tmp_val);
			QValidator *validator = new KDoubleValidator(m_cview);
			m_cview->setValidator( validator );
		}
		else {
			m_cview->setText(value.toString());
		}
		kdDebug() << "KexiInputTableEdit::KexiInputTableEdit(): have to mark! "
			<< m_cview->text().length() << endl;
//		m_cview->setSelection(0, m_cview->text().length());
//		QTimer::singleShot(0, m_view, SLOT(selectAll()));

#if 0
//move to end is better by default
//TODO(js): add configuration for this
		m_cview->selectAll();
#else
		m_cview->end(false);
#endif
//		setRestrictedCompletion();
	}

	m_comp = comp;
	setFocusProxy(m_view);
}

bool
KexiInputTableEdit::eventFilter(QObject* watched, QEvent* e)
{
	if(watched == m_view)
	{
		if(e->type() == QEvent::KeyPress)
		{
			QKeyEvent* ev = static_cast<QKeyEvent*>(e);

			switch(m_type)
			{
				case QVariant::Double:
					if(ev->text() == KGlobal::locale()->decimalSymbol() || ev->text() == KGlobal::locale()->monetaryDecimalSymbol())
						return false;

				case QVariant::Int:
					if(ev->text() == KGlobal::locale()->negativeSign())
						return false;

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

						case Key_Equal:
							m_calculatedCell = true;
							return true;

						case Key_Backspace:
							if(m_cview->text() == "=")
								m_calculatedCell = false;
							return false;

						default:
							return true;
					}

					break;

				default:
//					static_cast<KLineEdit*>(m_view)->setCompletedItems(m_comp);
//					static_cast<KLineEdit*>(m_view)->setKeyBinding(KGlobalSettings::CompletionPopup, Qt::Key_Return);

					switch(ev->key())
					{
						case Key_Enter:
						case Key_Return:
						case Key_Tab:
							return false;

						default:
							QTimer::singleShot(1, this, SLOT(setRestrictedCompletion()));
							break;
					}
					return false;
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

void
KexiInputTableEdit::setRestrictedCompletion()
{
	kdDebug() << "KexiInputTableEdit::setRestrictedCompletion()" << endl;
	KLineEdit *content = static_cast<KLineEdit*>(m_view);
	if(content->text().isEmpty())
		return;

	kdDebug() << "KexiInputTableEdit::setRestrictedCompletion(): something to do" << endl;

	content->useGlobalKeyBindings();

	QStringList newC;
	for(QStringList::Iterator it = m_comp.begin(); it != m_comp.end(); ++it)
	{
		if((*it).startsWith(content->text()))
			newC.append(*it);
	}
	content->setCompletedItems(newC);
}

void
KexiInputTableEdit::completed(const QString &s)
{
	kdDebug() << "KexiInputTableEdit::completed(): " << s << endl;
	static_cast<KLineEdit*>(m_view)->setText(s);
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
				qDebug("KexiInputTableEdit::value() converting => %s", m_cview->text().latin1());
				v = m_cview->text().replace(QRegExp("\\" + KGlobal::locale()->thousandsSeparator()), "");
				v = v.replace(QRegExp("\\" + KGlobal::locale()->decimalSymbol()), ".");
				v = v.replace(QRegExp("\\" + KGlobal::locale()->negativeSign()), "-");
				kdDebug() << "KexiInputTableEdit::value() converting => " << v.latin1() << endl;
				return QVariant(v);
			}
			else
			{
				//ok here should the formula be parsed so, just feel like in perl :)
				kdDebug() << "KexiInputTableEdit::value() calculating..." << endl;
				double result = 0;
				QString real = m_cview->text().right(m_cview->text().length() - 1);
				real = real.replace(QRegExp("\\" + KGlobal::locale()->thousandsSeparator()), "");
				real = real.replace(QRegExp("\\" + KGlobal::locale()->decimalSymbol()), ".");
//				qDebug("KexiInputTableEdit::value() calculating '%s'", real.latin1());
				QStringList values = QStringList::split(QRegExp("[\\+|\\*|\\/|-]"), real, false);
				QStringList ops = QStringList::split(QRegExp("[0-9]{1,8}(?:\\.[0-9]+)?"), real, false);

				double lastValue = 0;
				QString lastOp = "";
				for(int i=0; i < (int)values.count(); i++)
				{
					double next;

					QString op = QString((*ops.at(i))).stripWhiteSpace();

					if(!((*values.at(i+1)).isEmpty()) && i == 0)
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
					else if(!(*values.at(i+1)).isEmpty())
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
			kdDebug() << "KexiInputTableEdit::value() default..." << endl;
			return QVariant(m_cview->text());
	}
//	return QVariant(0);
}

void
KexiInputTableEdit::end(bool mark)
{
	m_cview->end(mark);
}

void
KexiInputTableEdit::backspace()
{
	m_cview->backspace();
}

void
KexiInputTableEdit::clear()
{
	m_cview->clear();
}

#include "kexiinputtableedit.moc"
