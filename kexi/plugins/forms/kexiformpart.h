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

#ifndef KEXITABLEPART_H
#define KEXITABLEPART_H

#include <qdom.h>
#include <qcstring.h>

#include <kexi.h>
#include <kexipart.h>
#include <kexidialogbase.h>

namespace KFormDesigner
{
	class FormManager;
	class Form;
}

namespace KexiDB
{
	class FieldList;
}

class KexiFormPart : public KexiPart::Part
{
	Q_OBJECT

	public:
		KexiFormPart(QObject *parent, const char *name, const QStringList &);
		virtual ~KexiFormPart();

		virtual bool remove(KexiMainWindow *win, KexiPart::Item &item);

		virtual KexiViewBase* createView(QWidget *parent, KexiDialogBase* dialog,
			KexiPart::Item &item, int viewMode = Kexi::DataViewMode);

		KFormDesigner::FormManager *manager() { return m_manager; }

		void generateForm(KexiDB::FieldList *list, QDomDocument &domDoc);

		//QByteArray loadForm(KexiDB::Connection *, const KexiPart::Item &item);
		//void saveForm(KexiDB::Connection *, const KexiPart::Item &item, const QByteArray &data);

		class TempData : public KexiDialogTempData
		{
			public:
				TempData(QObject* parent);
				QGuardedPtr<KFormDesigner::Form> form;
				QGuardedPtr<KFormDesigner::Form> previewForm;
				QByteArray  tempForm;
		};

	protected:
//		virtual void initPartActions( KActionCollection *col );
//		virtual void initInstanceActions( int mode, KActionCollection *col );
		virtual void initActions();

	private:
		QGuardedPtr<KFormDesigner::FormManager> m_manager;
};

#endif

