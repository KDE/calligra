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

#include "kexidatetimetableedit.h"

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

KexiDateTimeTableEdit::KexiDateTimeTableEdit(KexiTableViewColumn &column, QWidget *parent)
 : KexiInputTableEdit(column, parent)
{
	setName("KexiDateTimeTableEdit");

//! @todo add QValidator so time like "99:88:77" cannot be even entered

	m_lineedit->setInputMask( 
		dateTimeInputMask( m_dateFormatter, m_timeFormatter ) );
}

KexiDateTimeTableEdit::~KexiDateTimeTableEdit()
{
}

void KexiDateTimeTableEdit::setValueInInternalEditor(const QVariant &value)
{
	if (value.isValid() && value.toDateTime().isValid()) 
		m_lineedit->setText( 
			m_dateFormatter.dateToString( value.toDateTime().date() ) + " " +
			m_timeFormatter.timeToString( value.toDateTime().time() ) );
	else
		m_lineedit->setText( QString() );
}

void KexiDateTimeTableEdit::setValueInternal(const QVariant& add_, bool removeOld)
{
	if (removeOld) {
		//new time entering... just fill the line edit
//! @todo cut string if too long..
		QString add(add_.toString());
		m_lineedit->setText(add);
		m_lineedit->setCursorPosition(add.length());
		return;
	}
	setValueInInternalEditor( m_origValue );
	m_lineedit->setCursorPosition(0); //ok?
}

void KexiDateTimeTableEdit::setupContents( QPainter *p, bool focused, const QVariant& val, 
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
	if (val.toDateTime().isValid())
		txt = m_dateFormatter.dateToString(val.toDateTime().date()) + " " 
			+ m_timeFormatter.timeToString(val.toDateTime().time());
	align |= Qt::AlignLeft;
}

bool KexiDateTimeTableEdit::valueIsNull()
{
	if (textIsEmpty())
		return true;
	return !stringToDateTime(m_dateFormatter, m_timeFormatter, m_lineedit->text()).isValid();
}

bool KexiDateTimeTableEdit::valueIsEmpty()
{
	return valueIsNull();//js OK? TODO (nonsense?)
}

QVariant KexiDateTimeTableEdit::value()
{
	if (textIsEmpty())
		return QVariant();
	return stringToDateTime(m_dateFormatter, m_timeFormatter, m_lineedit->text());
}

bool KexiDateTimeTableEdit::valueIsValid()
{
	return dateTimeIsValid( m_dateFormatter, m_timeFormatter, m_lineedit->text() );
}

bool KexiDateTimeTableEdit::textIsEmpty() const
{
	return dateTimeIsEmpty( m_dateFormatter, m_timeFormatter, m_lineedit->text() );
}

void KexiDateTimeTableEdit::handleCopyAction(const QVariant& value, const QVariant& visibleValue)
{
	Q_UNUSED(visibleValue);
	if (!value.isNull() && value.toDateTime().isValid())
		qApp->clipboard()->setText( m_dateFormatter.dateToString(value.toDateTime().date()) + " " 
			+ m_timeFormatter.timeToString(value.toDateTime().time()) );
	else
		qApp->clipboard()->setText( QString() );
}

void KexiDateTimeTableEdit::handleAction(const QString& actionName)
{
	const bool alreadyVisible = m_lineedit->isVisible();

	if (actionName=="edit_paste") {
		const QVariant newValue( stringToDateTime(m_dateFormatter, m_timeFormatter, qApp->clipboard()->text()) );
		if (!alreadyVisible) { //paste as the entire text if the cell was not in edit mode
			emit editRequested();
			m_lineedit->clear();
		}
		setValueInInternalEditor( newValue );
	}
	else
		KexiInputTableEdit::handleAction(actionName);
}

KEXI_CELLEDITOR_FACTORY_ITEM_IMPL(KexiDateTimeEditorFactoryItem, KexiDateTimeTableEdit)

#include "kexidatetimetableedit.moc"
