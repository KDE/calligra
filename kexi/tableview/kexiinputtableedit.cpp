/* This file is part of the KDE project
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexiinputtableedit.h"

#include <kexidb/field.h>

#include <qregexp.h>
#include <qevent.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qpainter.h>

#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <kcompletionbox.h>
#include <knumvalidator.h>


class MyLineEdit : public KLineEdit
{
	public:
		MyLineEdit(QWidget *parent, const char *name) : KLineEdit(parent,name)
		{}
	protected:
		virtual void drawFrame ( QPainter * p ) {
			p->setPen( QPen( colorGroup().text() ) );
			QRect r = rect();
			p->moveTo( r.topLeft() );
			p->lineTo( r.topRight() );
			p->lineTo( r.bottomRight() );
			p->lineTo( r.bottomLeft() );
			if (pos().x() == 0) //draw left side only when it is @ the edge
				p->lineTo( r.topLeft() );
		}
};

//======================================================

KexiInputTableEdit::KexiInputTableEdit(KexiDB::Field &f, QWidget *parent, const char* name)
 : KexiTableEdit(f, parent, name ? name : "KexiInputTableEdit")
{
//	m_type = f.type(); //copied because the rest of code uses m_type
//	m_field = &f;
//	m_origValue = value;//original value
	init();
}

KexiInputTableEdit::~KexiInputTableEdit()
{
}

/*
//KexiInputTableEdit::KexiInputTableEdit(QVariant value, int type, const QString& add,
//	QWidget *parent, const char *name, QStringList comp)
KexiInputTableEdit::KexiInputTableEdit(QVariant value, int type, const QString& add,
	QWidget *parent)
 : KexiTableEdit(parent,"KexiInputTableEdit")
{
	m_type = type;
	m_origValue = value;//original value
	init(add);
}*/

void KexiInputTableEdit::init()
{
	kdDebug() << "KexiInputTableEdit: m_origValue.typeName()==" << m_origValue.typeName() << endl;
	kdDebug() << "KexiInputTableEdit: type== " << m_field->typeName() << endl;

	//init settings
	m_decsym = KGlobal::locale()->decimalSymbol();
	if (m_decsym.isEmpty())
		m_decsym=".";//default

	const bool align_right = m_field->isNumericType();

	if (!align_right) {
		//create layer for internal editor
		QHBoxLayout *lyr = new QHBoxLayout(this);
		lyr->addSpacing(4);
		lyr->setAutoAdd(true);
	}

	//create internal editor
	m_lineedit = new MyLineEdit(this, "KexiInputTableEdit-KLineEdit");
	setView(m_lineedit);
	if (align_right)
		m_lineedit->setAlignment(AlignRight);
//	m_cview->setFrame(false);
//	m_cview->setFrameStyle( QFrame::Plain | QFrame::Box );
//	m_cview->setLineWidth( 1 );
	m_calculatedCell = false;

#if 0 //js TODO
	connect(m_cview->completionBox(), SIGNAL(activated(const QString &)),
	 this, SLOT(completed(const QString &)));
	connect(m_cview->completionBox(), SIGNAL(highlighted(const QString &)),
	 this, SLOT(completed(const QString &)));
	 m_cview->completionBox()->setTabHandling(true);
#endif

}
	
void KexiInputTableEdit::init(const QString& add)
{
#if 0 //js
	if(!ov.isEmpty())
	{
		if (m_type==KexiDB::Field::Double || m_type==KexiDB::Field::Float) {
			if(ov == KGlobal::locale()->decimalSymbol() || ov == KGlobal::locale()->monetaryDecimalSymbol())
				m_cview->setText(ov);
		}
		else if (m_type==KexiDB::Field::Integer) {
			if(ov == KGlobal::locale()->negativeSign())
				m_cview->setText(ov);
		}
		else if (	case QVariant::UInt:
				m_cview->setAlignment(AlignRight);
				if(ov == "1" || ov == "2" || ov == "3" || ov == "4" || ov == "5" || ov == "6" || ov == "7" || ov == "8" || ov == "9" || ov == "0")
					m_cview->setText(ov);
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
#endif
		QString tmp_val = m_origValue.toString();

		if (m_field->isFPNumericType()) {
			if (m_origValue.toDouble() == 0.0) {
				tmp_val=add; //eat 0
			}
			else {
				//TODO(js): get decimal places settings here...
				QStringList sl = QStringList::split(".", tmp_val);
				if (tmp_val.isEmpty())
					m_lineedit->setText("");
				else if (sl.count()==2) {
					kdDebug() << "sl.count()=="<<sl.count()<< " " <<sl[0] << " | " << sl[1] << endl;
					tmp_val = sl[0] + m_decsym + sl[1];
				}
				tmp_val+=add;
			}
//			m_lineedit->setText(tmp_val);
			QValidator *validator = new KDoubleValidator(m_lineedit);
			m_lineedit->setValidator( validator );
		}
		else if (m_field->isIntegerType()) {
			if (m_origValue.toInt() == 0) {
				tmp_val=add; //eat 0
			}
			else {
				tmp_val += add;
			}
//			m_lineedit->setText(tmp_val);
			//js: @todo implement ranges here!
			QValidator *validator = new KIntValidator(m_lineedit);
			m_lineedit->setValidator( validator );
		}
		else {//default: text
			tmp_val+=add;
//			m_lineedit->setText(tmp_val);
		}

		if (tmp_val.isEmpty()) {
			if (m_origValue.isNull()) {
				//we have to set NULL initial value:
				m_lineedit->setText(QString::null);
			}
		}
		else {
			m_lineedit->setText(tmp_val);
		}
		
		kdDebug() << "KexiInputTableEdit:  " << m_lineedit->text().length() << endl;
//		m_cview->setSelection(0, m_cview->text().length());
//		QTimer::singleShot(0, m_view, SLOT(selectAll()));

#if 0
//move to end is better by default
		m_cview->selectAll();
#else
//js TODO: by default we're moving to the end of editor, ADD OPTION allowing "select all chars"
		m_lineedit->end(false);
#endif
//		setRestrictedCompletion();

//	m_comp = comp;
//	setFocusProxy(m_lineedit);

	//orig. editor's text
	m_origText = m_lineedit->text();
}

void KexiInputTableEdit::paintEvent ( QPaintEvent *e )
{
	QPainter p(this);
	p.setPen( QPen( colorGroup().text() ) );
	p.drawRect( rect() );
}


void
KexiInputTableEdit::setRestrictedCompletion()
{
#if 0 //js TODO
kdDebug() << "KexiInputTableEdit::setRestrictedCompletion()" << endl;
//	KLineEdit *content = static_cast<KLineEdit*>(m_view);
	if(m_cview->text().isEmpty())
		return;

	kdDebug() << "KexiInputTableEdit::setRestrictedCompletion(): something to do" << endl;

	m_cview->useGlobalKeyBindings();

	QStringList newC;
	for(QStringList::Iterator it = m_comp.begin(); it != m_comp.end(); ++it)
	{
		if((*it).startsWith(m_cview->text()))
			newC.append(*it);
	}
	m_cview->setCompletedItems(newC);
#endif
}

void
KexiInputTableEdit::completed(const QString &s)
{
	kdDebug() << "KexiInputTableEdit::completed(): " << s << endl;
	m_lineedit->setText(s);
}

bool KexiInputTableEdit::valueChanged()
{
	if (m_lineedit->text()!=m_origText)
		return true;
	return KexiTableEdit::valueChanged();
}

bool KexiInputTableEdit::valueIsNull()
{
	return m_lineedit->text().isNull();
}

bool KexiInputTableEdit::valueIsEmpty()
{
	return !m_lineedit->text().isNull() && m_lineedit->text().isEmpty();
}

QVariant KexiInputTableEdit::value(bool &ok)
{
	if (m_field->isFPNumericType()) {//==KexiDB::Field::Double || m_type==KexiDB::Field::Float) {
		//! js @todo PRESERVE PRECISION!
		QString txt = m_lineedit->text();
		if (m_decsym!=".")
			txt = txt.replace(m_decsym,".");//convert back
		return QVariant( txt.toDouble(&ok) );
	}
	else if (m_field->isIntegerType()) {
		//check constraints
		return QVariant( m_lineedit->text().toInt(&ok) );
	}
	//default: text
	ok = true;
	return QVariant( m_lineedit->text() );
}
#if 0
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
#endif


void
KexiInputTableEdit::clear()
{
	m_lineedit->clear();
}

bool KexiInputTableEdit::cursorAtStart()
{
	return m_lineedit->cursorPosition()==0;
}

bool KexiInputTableEdit::cursorAtEnd()
{
	return m_lineedit->cursorPosition()==(int)m_lineedit->text().length();
}

#if 0 //js: we've QValidator for validating!
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
#endif //0


//======================================================

KexiInputEditorFactoryItem::KexiInputEditorFactoryItem()
{
}

KexiInputEditorFactoryItem::~KexiInputEditorFactoryItem()
{
}

KexiTableEdit* KexiInputEditorFactoryItem::createEditor(
	KexiDB::Field &f, QWidget* parent)
{
	return new KexiInputTableEdit(f, parent);
}

#include "kexiinputtableedit.moc"
