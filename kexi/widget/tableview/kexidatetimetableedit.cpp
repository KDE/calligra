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

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kdatepicker.h>
#include <kdatetable.h>
#include <klineedit.h>
#include <kmenu.h>
#include <kdatewidget.h>

#include <kexiutils/utils.h>

KexiDateTimeTableEdit::KexiDateTimeTableEdit(KexiTableViewColumn &column, Q3ScrollView *parent)
 : KexiInputTableEdit(column, parent)
{
	setName("KexiDateTimeTableEdit");

//! @todo add QValidator so time like "99:88:77" cannot be even entered

	QString mask(m_dateFormatter.inputMask());
	mask.truncate(m_dateFormatter.inputMask().length()-2);
	m_lineedit->setInputMask( mask + " " + m_timeFormatter.inputMask() );
}

KexiDateTimeTableEdit::~KexiDateTimeTableEdit()
{
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
	if (m_origValue.isValid()) {
		m_lineedit->setText(
			m_dateFormatter.dateToString( m_origValue.toDateTime().date() ) + " " +
			m_timeFormatter.timeToString( m_origValue.toDateTime().time() )
		);
	}
	else {
		m_lineedit->setText( QString::null );
	}
	m_lineedit->setCursorPosition(0); //ok?
}

void KexiDateTimeTableEdit::setupContents( QPainter *p, bool focused, QVariant val, 
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
	return !dateTimeValue().isValid();
}

bool KexiDateTimeTableEdit::valueIsEmpty()
{
	return valueIsNull();//js OK? TODO (nonsense?)
}

QDateTime KexiDateTimeTableEdit::dateTimeValue()
{
	QString s = m_lineedit->text().trimmed();
	const int timepos = s.find(" ");
	const bool emptyTime = timepos >= 0 && s.mid(timepos+1).replace(':',"").trimmed().isEmpty();
	if (emptyTime)
		s = s.left(timepos);
	if (timepos>0 && !emptyTime) {
		return QDateTime(
			m_dateFormatter.stringToDate( s.left(timepos) ),
			m_timeFormatter.stringToTime( s.mid(timepos+1) )
		);
	}
	else {
		return QDateTime(
			m_dateFormatter.stringToDate( s ),
			QTime(0,0,0)
		);
	}
}

QVariant KexiDateTimeTableEdit::value()
{
	if (textIsEmpty())
		return QVariant();
	return dateTimeValue();
}

bool KexiDateTimeTableEdit::valueIsValid()
{
	QString s(m_lineedit->text());
	int timepos = s.find(" ");
	const bool emptyTime = timepos >= 0 && s.mid(timepos+1).replace(':',"").trimmed().isEmpty();
	if (timepos >= 0 && s.left(timepos).replace(m_dateFormatter.separator(), "").trimmed().isEmpty()
		&& emptyTime)
		//empty date/time is valid
		return true;
	return timepos>=0 && m_dateFormatter.stringToDate( s.left(timepos) ).isValid()
		&& (emptyTime /*date without time is also valid*/ || m_timeFormatter.stringToTime( s.mid(timepos+1) ).isValid());
}

bool KexiDateTimeTableEdit::textIsEmpty() const
{
	QString s(m_lineedit->text());
	int timepos = s.find(" ");
	const bool emptyTime = timepos >= 0 && s.mid(timepos+1).replace(':',"").trimmed().isEmpty();
	return (timepos >= 0 && s.left(timepos).replace(m_dateFormatter.separator(), "").trimmed().isEmpty()
		&& emptyTime);
}

KEXI_CELLEDITOR_FACTORY_ITEM_IMPL(KexiDateTimeEditorFactoryItem, KexiDateTimeTableEdit)

#include "kexidatetimetableedit.moc"
