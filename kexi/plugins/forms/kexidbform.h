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

#ifndef KEXIDBFORM_H
#define KEXIDBFORM_H

#include <kexiviewbase.h>
#include <kexipartitem.h>

class KexiFormPart;
class KexiFormPartItem;
class KexiMainWindow;
class KexiPropertyBuffer;

namespace KexiDB
{
	class Connection;
}

namespace KFormDesigner
{
	class FormManager;
	class Form;
}

/**
 * this class is a data-aware form
 */
class KexiDBForm : public KexiViewBase
{
	Q_OBJECT
	Q_PROPERTY(QString DataSource READ datasource WRITE setDatasource DESIGNABLE true)
	Q_PROPERTY(bool RecordNavigator READ navigatorShown WRITE showRecordNavigator DESIGNABLE true)

	public:
		KexiDBForm(KexiFormPart *m, KexiFormPartItem &i, KexiMainWindow *win, QWidget *parent, const char *name, KexiDB::Connection *conn);
		~KexiDBForm();

		QString datasource() const { return m_ds; }
		bool navigatorShown() const { return m_nav; }
		void setDatasource(const QString &s) { m_ds = s; }
		void showRecordNavigator(bool s) { m_nav = s; }

		void initForm();
		void preview();
	
	protected slots:
		void managerPropertyChanged(KexiPropertyBuffer *b);
		void slotDirty(KFormDesigner::Form *f);

	protected:
		virtual KexiPropertyBuffer *propertyBuffer() { return m_buffer; }
		virtual bool beforeSwitchTo(int mode);
		virtual bool afterSwitchFrom(int mode);

		virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata);
		virtual bool storeData();

	private:
		QString m_ds;
		KexiPropertyBuffer *m_buffer;
		bool m_nav;
		KexiFormPart *m_part;
		KexiDB::Connection *m_conn;
		KexiFormPartItem m_item;
		QWidget *m_preview;
		int m_id;
};

#endif

