/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDATETABLEEDIT_H
#define KEXIDATETABLEEDIT_H

#include <kexitableedit.h>

#include <kdatepicker.h>


//class KDatePicker;

class KDatePicker;
class KLineEdit;
class KDateWidget;
class QDateEdit;
class QDate;
class KPopupMenu;

/*!
  this class represents an editor for QVariant::Date

  uhm, thanks to tronical, who added me a second constructor
  for KDatePicker i dont't have to type so much JIPPIE
  update (2002-09-02 (00:30))
*/
class KEXIDATATABLE_EXPORT KexiDateTableEdit : public KexiTableEdit
{

	Q_OBJECT

	public:
		KexiDateTableEdit(QVariant value, KexiDB::Field &f, const QString& add=QString::null,
			QWidget *parent=0);
//		KexiDateTableEdit(QVariant v=0, QWidget *parent=0, const char *name=0);

		virtual QVariant value(bool &ok);
	
	protected:
		virtual bool eventFilter( QObject *o, QEvent *e );

	protected slots:
		void slotDateChanged(QDate);
		void slotShowDatePicker();

	protected:
		KDatePicker	*m_datePicker;
		/*KLineEdit*/QDateEdit* m_edit;
		
		QDate m_oldVal;
		KPopupMenu *m_datePickerPopupMenu;
};

class KEXIDATATABLE_EXPORT KexiDatePicker : public KDatePicker
{
	Q_OBJECT

	public:
		KexiDatePicker(QWidget *parent, QDate date, const char *name, WFlags f);
		~KexiDatePicker();
};

#endif
