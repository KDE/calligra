/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexitimetableedit.h"
#include "kexidatetimeeditor_p.h"

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
#include <qdatetimeedit.h>

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kdatepicker.h>
#include <kdatetbl.h>
#include <klineedit.h>
#include <kpopupmenu.h>
#include <kdatewidget.h>

#include "kexi_utils.h"

KexiTimeTableEdit::KexiTimeTableEdit(KexiTableViewColumn &column, QScrollView *parent)
 : KexiTableEdit(column, parent,"KexiTimeTableEdit")
{
	m_sentEvent = false;
	setViewWidget( m_edit = new QTimeEdit(this) );
	m_edit->setAutoAdvance(true);
	m_edit->installEventFilter(this);
	m_cleared = false;
	m_setNumberOnFocus = -1;

	m_dte_time_obj = Kexi::findFirstChild<QObject>(m_edit, "QDateTimeEditor");
	if (m_dte_time_obj)
		m_dte_time_obj->installEventFilter(this);
	
#ifdef QDateTimeEditor_HACK
	m_dte_time = dynamic_cast<QDateTimeEditor*>(m_dte_time_obj);
#else
	m_dte_time = 0;
#endif

	connect(m_edit,SIGNAL(valueChanged(const QTime&)), this, SLOT(slotValueChanged(const QTime&)));

	setFocusProxy(m_edit);

	m_acceptEditorAfterDeleteContents = true;
}

void KexiTimeTableEdit::setValueInternal(const QVariant& add_, bool removeOld)
{
	m_cleared = !m_origValue.isValid();

	m_setNumberOnFocus = -1;
	QTime t;
	QString add(add_.toString());
	if (removeOld) {
		if (!add.isEmpty() && add[0].latin1()>='0' && add[0].latin1() <='9') {
			m_setNumberOnFocus = add[0].latin1()-'0';
			t = QTime(m_setNumberOnFocus, 0, 0);
		}
	}
	else {
		t = m_origValue.toTime();
	}
	m_edit->setTime(t);
	moveToFirstSection();
}

//! \return true is editor's value is null (not empty)
bool KexiTimeTableEdit::valueIsNull()
{
	return m_cleared;
}

bool KexiTimeTableEdit::valueIsEmpty()
{
	return valueIsNull();// OK? TODO (nonsense?)
}

QVariant
KexiTimeTableEdit::value()
{
//	ok = true;

	//QDateTime - a hack needed because QVariant(QTime) has broken isNull()
	return QVariant(QDateTime( m_cleared ? QDate() : QDate(0,1,2)/*nevermind*/, m_edit->time()));
}

bool KexiTimeTableEdit::cursorAtStart()
{
#ifdef QDateTimeEditor_HACK
	return m_dte_time && m_edit->hasFocus() && m_dte_time->focusSection()==0;
#else
	return false;
#endif
}

bool KexiTimeTableEdit::cursorAtEnd()
{
#ifdef QDateTimeEditor_HACK
	return m_dte_time && m_edit->hasFocus() 
		&& m_dte_time->focusSection()==int(m_dte_time->sectionCount()-1);
#else
	return false;
#endif
}

void KexiTimeTableEdit::clear()
{
	m_edit->setTime(QTime());
	m_cleared = true;
}

void KexiTimeTableEdit::slotValueChanged(const QTime& /*t*/)
{
	m_cleared = false;
}

//! @internal helper
void KexiTimeTableEdit::moveToFirstSection()
{
#ifdef QDateTimeEditor_HACK
	m_dte_time->setFocusSection(0);
#else
	QKeyEvent ke_left(QEvent::KeyPress, Qt::Key_Left, 0, 0);
	for (int i=0; i<8; i++)
		QApplication::sendEvent( m_dte_time_obj, &ke_left );
#endif
}

bool KexiTimeTableEdit::eventFilter( QObject *o, QEvent *e )
{
	if (e->type()==QEvent::FocusIn && o->parent() && o->parent()->parent()==m_edit) {
		if (m_setNumberOnFocus >= 0 && m_dte_time_obj) {
			// there was a number character passed as 'add' parameter in init():
			moveToFirstSection();
			QKeyEvent ke(QEvent::KeyPress, int(Qt::Key_0)+m_setNumberOnFocus, 
				'0'+m_setNumberOnFocus, 0, QString::number(m_setNumberOnFocus));
			QApplication::sendEvent( m_dte_time_obj, &ke );
			m_setNumberOnFocus = -1;
		}
	}
#ifdef QDateTimeEditor_HACK
	else if (e->type()==QEvent::KeyPress && m_dte_time) {
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

//======================================================

KexiTimeEditorFactoryItem::KexiTimeEditorFactoryItem()
{
}

KexiTimeEditorFactoryItem::~KexiTimeEditorFactoryItem()
{
}

KexiTableEdit* KexiTimeEditorFactoryItem::createEditor(
	KexiTableViewColumn &column, QScrollView* parent)
{
	return new KexiTimeTableEdit(column, parent);
}

#include "kexitimetableedit.moc"
