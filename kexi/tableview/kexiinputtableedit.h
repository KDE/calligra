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

#ifndef KEXIINPUTTABLEEDIT_H
#define KEXIINPUTTABLEEDIT_H

#include <klineedit.h>
#include <qvariant.h>

#include "kexitableedit.h"

class KEXIDATATABLE_EXPORT KexiInputTableEdit : public KexiTableEdit
{
	Q_OBJECT

	public:
//		KexiInputTableEdit(QVariant value, int type, QString ov=QString::null, bool mark=false,
//		KexiInputTableEdit(QVariant value, int type, const QString& add=QString::null,
//		 QWidget *parent=0);
		KexiInputTableEdit(QVariant value, KexiDB::Field &f, const QString& add=QString::null,
		 QWidget *parent=0);

		virtual bool valueChanged();

		//! \return true is editor's value is null (not empty)
		virtual bool valueIsNull();
		//! \return true is editor's value is empty (not null). 
		//! Only few field types can accept "EMPTY" property 
		//! (check this with KexiDB::Field::hasEmptyProperty()), 
		virtual bool valueIsEmpty();

		virtual QVariant value(bool &ok);

//		virtual bool eventFilter(QObject* watched, QEvent* e);
		void end(bool mark);
		void backspace();
		void clear();

	protected slots:
		void setRestrictedCompletion();
		void completed(const QString &);

	protected:
		void showHintButton();
		void init(const QString& add);

		bool		m_calculatedCell;
//js		QStringList	m_comp;

		QString m_decsym; //! decimal symbol
		QString m_origText; //! orig. Line Edit's text after conversion - for easy comparing


	private:
		KLineEdit	*m_cview;

	signals:
		void hintClicked();
};

#endif
