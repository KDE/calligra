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

#ifndef KEXIDATETIMETABLEEDIT_H
#define KEXIDATETIMETABLEEDIT_H

#include "kexidatetableedit.h"
#include "kexitimetableedit.h"

/*! @short Editor class for Date/Time type.
 It is a replacement QDateTimeEdit due to usability problems: 
 people are accustomed to use single-character cursor.
 Date and Time format is retrieved from the KDE global settings
 and input/output is performed using KLineEdit (from KexiInputTableEdit).
*/
class KexiDateTimeTableEdit : public KexiInputTableEdit
{
	Q_OBJECT

	public:
		KexiDateTimeTableEdit(KexiTableViewColumn &column, QWidget *parent=0);
		virtual ~KexiDateTimeTableEdit();
		virtual void setupContents( QPainter *p, bool focused, const QVariant& val, 
			QString &txt, int &align, int &x, int &y_offset, int &w, int &h );
		virtual QVariant value();
		virtual bool valueIsNull();
		virtual bool valueIsEmpty();
		virtual bool valueIsValid();

		/*! Reimplemented after KexiInputTableEdit. */
		virtual void handleAction(const QString& actionName);

		/*! Reimplemented after KexiInputTableEdit. */
		virtual void handleCopyAction(const QVariant& value, const QVariant& visibleValue);

	protected:
		//! helper
		void setValueInInternalEditor(const QVariant &value);
		virtual void setValueInternal(const QVariant& add, bool removeOld);
		bool textIsEmpty() const;

		//! Used to format and convert date values
		KexiDateFormatter m_dateFormatter;

		//! Used to format and convert time values
		KexiTimeFormatter m_timeFormatter;
};

KEXI_DECLARE_CELLEDITOR_FACTORY_ITEM(KexiDateTimeEditorFactoryItem)

#endif
