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

#ifndef KEXIDATETABLEEDIT_H
#define KEXIDATETABLEEDIT_H

#include <q3datetimeedit.h>

#include "kexiinputtableedit.h"

//! @short Date formatter used by KexiDateTableEdit and KexiDateTimeTableEdit
class KEXIDATATABLE_EXPORT KexiDateFormatter
{
	public:
		//! Creates new formatter with KDE setting for "short date"
		KexiDateFormatter();

		//! Creates new formatter with given settings
//! @todo		KexiDateFormatter(... settings ...);

		~KexiDateFormatter();

		//! converts string \a str to date using predefined settings
		//! \return invalid date if the conversion is impossible
		QDate stringToDate( const QString& str ) const;

		//! converts \a date to string using predefined settings
		//! \return null string if \a date is invalid
		QString dateToString( const QDate& date ) const;

		//! \return Input mask generated using the formatter settings. 
		//! Can be used in QLineEdit::setInputMask().
		QString inputMask() const { return m_inputMask; }

		QString separator() const { return m_separator; }

	protected:
		//! Input mask generated using the formatter settings. Can be used in QLineEdit::setInputMask().
		QString m_inputMask;

		//! Order of date sections
		Q3DateEdit::Order m_order;

		//! 4 or 2 digits
		bool m_longYear;

		bool m_monthWithLeadingZero, m_dayWithLeadingZero;

		//! Date format used in dateToString()
		QString m_qtFormat;

		//! Used in stringToDate() to convert string back to QDate
		int m_yearpos, m_monthpos, m_daypos;

		QString m_separator;
};

/*! @short Editor class for Date type.
 It is a replacement QDateEdit due to usability problems: 
 people are accustomed to use single-character cursor.
 Date format is retrieved from the KDE global settings.
 and input/output is performed using KLineEdit (from KexiInputTableEdit).
*/
class KEXIDATATABLE_EXPORT KexiDateTableEdit : public KexiInputTableEdit
{
	Q_OBJECT

	public:
		KexiDateTableEdit(KexiTableViewColumn &column, Q3ScrollView *parent=0);
		virtual ~KexiDateTableEdit();
		virtual void setupContents( QPainter *p, bool focused, QVariant val, 
			QString &txt, int &align, int &x, int &y_offset, int &w, int &h );
		virtual QVariant value();
		virtual bool valueIsNull();
		virtual bool valueIsEmpty();
		virtual bool valueIsValid();

	protected:
		virtual void setValueInternal(const QVariant& add, bool removeOld);
		QDate dateValue() const;

		//! Used to format and convert date values
		KexiDateFormatter m_formatter;
};

KEXI_DECLARE_CELLEDITOR_FACTORY_ITEM(KexiDateEditorFactoryItem)

#endif
