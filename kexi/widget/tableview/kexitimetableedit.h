/* This file is part of the KDE project
   Copyright (C) 2004,2006 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXITIMETABLEEDIT_H
#define KEXITIMETABLEEDIT_H

#include <qregexp.h>

#include "kexiinputtableedit.h"

//! @short Time formatter used by KexiTimeTableEdit and KexiDateTimeTableEdit
//! Following time formats are allowed: HH:MM:SS (24h), HH:MM (24h), HH:MM AM/PM (12h)
//! Separator MUST be ":"
class KEXIDATATABLE_EXPORT KexiTimeFormatter
{
	public:
		//! Creates new formatter with KDE setting for time
		KexiTimeFormatter();

		//! Creates new formatter with given settings
//! @todo		KexiDateFormatter(... settings ...);

		~KexiTimeFormatter();

		//! converts string \a str to time using predefined settings
		//! \return invalid time if the conversion is impossible
		QTime stringToTime( const QString& str );

		//! converts \a time to string using predefined settings
		//! \return null string if \a time is invalid
		QString timeToString( const QTime& time ) const;

		//! \return Input mask generated using the formatter settings. 
		//! Can be used in QLineEdit::setInputMask().
		QString inputMask() const { return m_inputMask; }

	protected:
		//! Input mask generated using the formatter settings. Can be used in QLineEdit::setInputMask().
		QString m_inputMask;

//		//! Order of date sections
//		QDateEdit::Order m_order;

		//! 12 or 12h
		bool m_24h;

		bool m_hoursWithLeadingZero;

		//! Time format used in timeToString(). Notation from KLocale::setTimeFormat() is used.
		QString m_outputFormat;

		//! Used in stringToTime() to convert string back to QTime
		int m_hourpos, m_minpos, m_secpos, m_ampmpos;

		QRegExp m_hmsRegExp, m_hmRegExp;
};

/*! @short Editor class for Time type.
 It is a replacement QTimeEdit due to usability problems: 
 people are accustomed to use single-character cursor.
 Time format is retrieved from the KDE global settings
 and input/output is performed using KLineEdit (from KexiInputTableEdit).
*/
class KEXIDATATABLE_EXPORT KexiTimeTableEdit : public KexiInputTableEdit
{
	Q_OBJECT

	public:
		KexiTimeTableEdit(KexiTableViewColumn &column, Q3ScrollView *parent=0);
		virtual ~KexiTimeTableEdit();
		virtual void setupContents( QPainter *p, bool focused, QVariant val, 
			QString &txt, int &align, int &x, int &y_offset, int &w, int &h );
		virtual QVariant value();
		virtual bool valueIsNull();
		virtual bool valueIsEmpty();
		virtual bool valueIsValid();

	protected:
		virtual void setValueInternal(const QVariant& add, bool removeOld);
		QTime timeValue();

		//! Used to format and convert time values
		KexiTimeFormatter m_formatter;
};

KEXI_DECLARE_CELLEDITOR_FACTORY_ITEM(KexiTimeEditorFactoryItem)

#endif
