/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIDATASOURCEWIZARD_H
#define KEXIDATASOURCEWIZARD_H

#include <qlabel.h>
#include <kwizard.h>
#include <kexipartitem.h>

class KexiMainWindow;

namespace KexiPart
{
	class DataSource;
};

namespace KexiDB
{
	class FieldList;
};

typedef QPair<KexiPart::DataSource*, KexiPart::Item> DSInfo;

/**
 * this is a wizard which helps the user
 * creating documents (like forms and reports)
 * where fields should be displayed by default
 */
class KexiDataSourceWizard : public KWizard
{
	Q_OBJECT

	public:
		KexiDataSourceWizard(KexiMainWindow *win, QWidget *parent, const char *name=0);
		~KexiDataSourceWizard();

		/**
		 * @returns the chosen datasource
		 */
		DSInfo dataSource() { return m_ds; }

		/**
		 * @reutrns the fields chosen to be displayed
		 */
		KexiDB::FieldList *fields() { return m_fields; }

		/**
		 * sets the fieldlist
		 */
		void setFieldList(KexiDB::FieldList *fl);

		/**
		 * returns the wizard's left pixmpap
		 */
		static QPixmap &pixmap();

		/**
		 * sets if the wizard was finished with choosing to create a document using the wizard
		 */
		void setWizardUsed(bool used) { m_used = used; }

		/**
		 * makes the next page accept() the dialog
		 */
		void finishNext(bool f) { m_finishNext = f; }

	public slots:
		/**
		 * sets the datasource
		 */
		void setDataSource(const DSInfo &ds);

	protected slots:
		void next();

	private:
		KexiMainWindow *m_win;
		KexiDB::FieldList *m_fields;
		DSInfo m_ds;
		bool m_finishNext : 1;
		bool m_used : 1;
};

class KexiDSPixmap : public QLabel
{
	Q_OBJECT

	public:
		KexiDSPixmap(QWidget *parent);
		~KexiDSPixmap();
};


#endif

