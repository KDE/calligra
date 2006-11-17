/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003   Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003-2004,2006 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "kexidatetableedit.h"

#include <qapplication.h>
#include <qpainter.h>
#include <qvariant.h>
#include <qrect.h>
#include <qpalette.h>
#include <qcolor.h>
#include <qfontmetrics.h>
#include <qdatetime.h>
#include <qcursor.h>
#include <qpoint.h>
#include <qlayout.h>
#include <qtoolbutton.h>
#include <q3datetimeedit.h>
#include <qclipboard.h>

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kdatepicker.h>
#include <kdatetable.h>
#include <klineedit.h>
#include <kmenu.h>
#include <kdatewidget.h>

#include <kexiutils/utils.h>


KexiDateTableEdit::KexiDateTableEdit(KexiTableViewColumn &column, QWidget *parent)
 : KexiInputTableEdit(column, parent)
{
	setName("KexiDateTableEdit");

//! @todo add QValidator so date like "2006-59-67" cannot be even entered

	m_lineedit->setInputMask( m_formatter.inputMask() );
}

KexiDateTableEdit::~KexiDateTableEdit()
{
}

void KexiDateTableEdit::setValueInInternalEditor(const QVariant &value)
{
	if (value.isValid() && value.toDate().isValid()) 
		m_lineedit->setText( m_formatter.dateToString( value.toDate() ) );
	else
		m_lineedit->setText( QString::null );
}

void KexiDateTableEdit::setValueInternal(const QVariant& add_, bool removeOld)
{
	if (removeOld) {
		//new date entering... just fill the line edit
//! @todo cut string if too long..
		QString add(add_.toString());
		m_lineedit->setText(add);
		m_lineedit->setCursorPosition(add.length());
		return;
	}
	setValueInInternalEditor( m_origValue );
	m_lineedit->setCursorPosition(0); //ok?
}

void KexiDateTableEdit::setupContents( QPainter *p, bool focused, const QVariant& val, 
	QString &txt, int &align, int &x, int &y_offset, int &w, int &h )
{
	Q_UNUSED(p);
	Q_UNUSED(focused);
	Q_UNUSED(x);
	Q_UNUSED(w);
	Q_UNUSED(h);
#ifdef Q_WS_WIN
	y_offset = -1;
#else
	y_offset = 0;
#endif
	if (val.toDate().isValid())
		txt = m_formatter.dateToString(val.toDate());
//		txt = val.toDate().toString(Qt::LocalDate);
	align |= Qt::AlignLeft;
}

bool KexiDateTableEdit::valueIsNull()
{
//	if (m_lineedit->text().replace(m_formatter.separator(),"").trimmed().isEmpty())
	if (m_formatter.isEmpty(m_lineedit->text())) //empty date is null
		return true;
	return dateValue().isNull();
}

bool KexiDateTableEdit::valueIsEmpty()
{
	return valueIsNull();//js OK? TODO (nonsense?)
}

QDate KexiDateTableEdit::dateValue() const
{
	return m_formatter.stringToDate( m_lineedit->text() );
}

QVariant KexiDateTableEdit::value()
{
	return m_formatter.stringToVariant( m_lineedit->text() );
}

bool KexiDateTableEdit::valueIsValid()
{
	if (m_formatter.isEmpty(m_lineedit->text())) //empty date is valid
		return true;
	return m_formatter.stringToDate( m_lineedit->text() ).isValid();
}

void KexiDateTableEdit::handleCopyAction(const QVariant& value, const QVariant& visibleValue)
{
	Q_UNUSED(visibleValue);
	if (!value.isNull() && value.toDate().isValid())
		qApp->clipboard()->setText( m_formatter.dateToString(value.toDate()) );
	else
		qApp->clipboard()->setText( QString::null );
}

void KexiDateTableEdit::handleAction(const QString& actionName)
{
	const bool alreadyVisible = m_lineedit->isVisible();

	if (actionName=="edit_paste") {
		const QVariant newValue( m_formatter.stringToDate(qApp->clipboard()->text()) );
		if (!alreadyVisible) { //paste as the entire text if the cell was not in edit mode
			emit editRequested();
			m_lineedit->clear();
		}
		setValueInInternalEditor( newValue );
	}
	else
		KexiInputTableEdit::handleAction(actionName);
}

/*
void
KexiDateTableEdit::slotDateChanged(QDate date)
{
	m_edit->setDate(date);
	repaint();
}

void
KexiDateTableEdit::slotShowDatePicker()
{
	QDate date = m_edit->date();

	m_datePicker->setDate(date);
	m_datePicker->setFocus();
	m_datePicker->show();
	m_datePicker->setFocus();
}

//! @internal helper
void KexiDateTableEdit::moveToFirstSection()
{
	if (!m_dte_date_obj)
		return;
#ifdef QDateTimeEditor_HACK
	if (m_dte_date)
		m_dte_date->setFocusSection(0);
#else
#ifdef Q_WS_WIN //tmp
	QKeyEvent ke_left(QEvent::KeyPress, Qt::Key_Left, 0, 0);
	for (int i=0; i<8; i++)
		QApplication::sendEvent( m_dte_date_obj, &ke_left );
#endif
#endif
}

bool KexiDateTableEdit::eventFilter( QObject *o, QEvent *e )
{
	if (o==m_datePicker) {
		kDebug() << e->type() << endl;
		switch (e->type()) {
		case QEvent::Hide:
			m_datePickerPopupMenu->hide();
			break;
		case QEvent::KeyPress:
		case QEvent::KeyRelease: {
			kDebug() << "ok!" << endl;
			QKeyEvent *ke = (QKeyEvent *)e;
			if (ke->key()==Qt::Key_Enter || ke->key()==Qt::Key_Return) {
				//accepting picker
				acceptDate();
				return true;
			}
			else if (ke->key()==Qt::Key_Escape) {
				//cancelling picker
				m_datePickerPopupMenu->hide();
				kDebug() << "reject" << endl;
				return true;
			}
			else m_datePickerPopupMenu->setFocus();
			break;
			}
		default:
			break;
		}
	}
#ifdef Q_WS_WIN //tmp
	else if (e->type()==QEvent::FocusIn && o->parent() && o->parent()->parent()==m_edit
		&& m_setNumberOnFocus >= 0 && m_dte_date_obj)
	{
		// there was a number character passed as 'add' parameter in init():
		moveToFirstSection();
		QKeyEvent ke(QEvent::KeyPress, int(Qt::Key_0)+m_setNumberOnFocus, 
			'0'+m_setNumberOnFocus, 0, QString::number(m_setNumberOnFocus));
		QApplication::sendEvent( m_dte_date_obj, &ke );
		m_setNumberOnFocus = -1;
	}
#endif
#ifdef QDateTimeEditor_HACK
	else if (e->type()==QEvent::KeyPress && m_dte_date) {
		QKeyEvent *ke = static_cast<QKeyEvent*>(e);
		if ((ke->key()==Qt::Key_Right && !m_sentEvent && cursorAtEnd())
			|| (ke->key()==Qt::Key_Left && !m_sentEvent && cursorAtStart()))
		{
			//the editor should send this key event:
			m_sentEvent = true; //avoid recursion
			QApplication::sendEvent( this, ke );
			m_sentEvent = false;
			ke->ignore();
			return true;
		}
	}
#endif
	return false;
}

void KexiDateTableEdit::acceptDate()
{
	m_edit->setDate(m_datePicker->date());
	m_datePickerPopupMenu->hide();
	kDebug() << "accept" << endl;
}

bool KexiDateTableEdit::cursorAtStart()
{
#ifdef QDateTimeEditor_HACK
	return m_dte_date && m_edit->hasFocus() && m_dte_date->focusSection()==0;
#else
	return false;
#endif
}

bool KexiDateTableEdit::cursorAtEnd()
{
#ifdef QDateTimeEditor_HACK
	return m_dte_date && m_edit->hasFocus() 
		&& m_dte_date->focusSection()==int(m_dte_date->sectionCount()-1);
#else
	return false;
#endif
}

void KexiDateTableEdit::clear()
{
	m_edit->setDate(QDate());
}*/

KEXI_CELLEDITOR_FACTORY_ITEM_IMPL(KexiDateEditorFactoryItem, KexiDateTableEdit)

#include "kexidatetableedit.moc"
