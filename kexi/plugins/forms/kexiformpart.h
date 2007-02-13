/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIFORMPART_H
#define KEXIFORMPART_H

#include <qdom.h>
#include <q3cstring.h>

#include <kexi.h>
#include <kexipart.h>
#include <kexidialogbase.h>

namespace KFormDesigner
{
	class WidgetLibrary;
	class FormManager;
	class Form;
}

namespace KexiDB
{
	class FieldList;
}

class KexiDataSourcePage;

//! Kexi Form Plugin
/*! It just creates a \ref KexiFormView. See there for most of code. */
class KEXIFORMUTILS_EXPORT KexiFormPart : public KexiPart::Part
{
	Q_OBJECT

	public:
		KexiFormPart(QObject *parent, const char *name, const QStringList &);
		virtual ~KexiFormPart();

		//! \return a pointer to Forms Widget Library.
		static KFormDesigner::WidgetLibrary* library();

		KexiDataSourcePage* dataSourcePage() const;

		void generateForm(KexiDB::FieldList *list, QDomDocument &domDoc);

		class TempData : public KexiDialogTempData
		{
			public:
				TempData(QObject* parent);
				~TempData();
				QPointer<KFormDesigner::Form> form;
				QPointer<KFormDesigner::Form> previewForm;
				QString tempForm;
				QPoint scrollViewContentsPos; //!< to preserve contents pos after switching to other view
				int resizeMode; //!< form's window's resize mode -one of KexiFormView::ResizeMode items
		};

		virtual QString i18nMessage(const Q3CString& englishMessage, 
			KexiDialogBase* dlg) const;

	protected:
		virtual KexiDialogTempData* createTempData(KexiDialogBase* dialog);

		virtual KexiViewBase* createView(QWidget *parent, KexiDialogBase* dialog,
			KexiPart::Item &item, int viewMode = Kexi::DataViewMode, QMap<QString,QString>* staticObjectArgs = 0);

		virtual void initPartActions();
		virtual void initInstanceActions();
		virtual void setupCustomPropertyPanelTabs(KTabWidget *tab, KexiMainWindow* mainWin);

		static KFormDesigner::WidgetLibrary* static_formsLibrary;

	protected slots:
		void slotAutoTabStopsSet(KFormDesigner::Form *form, bool set);
		void slotAssignAction();
		void slotPropertyChanged(QWidget *widget, const Q3CString &name, const QVariant &value);
		void slotWidgetCreatedByFormsLibrary(QWidget* widget);

	private:
		class Private;
		Private* d;
};

#endif

