/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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

#ifndef KEXIREPORTPART_H
#define KEXIREPORTPART_H

#include <kexi.h>
#include <kexipart.h>
#include <Q3CString>

namespace KFormDesigner
{
	class FormManager;
	class WidgetLibrary;
	class Form;
}

namespace KexiDB
{
	class FieldList;
}

/*! @short Kexi Report Plugin
 It just creates a \ref KexiReportView. See there for most of code. */
class KEXIREPORTUTILS_EXPORT KexiReportPart : public KexiPart::Part
{
	Q_OBJECT

	public:
		KexiReportPart(QObject *parent, const QStringList &);
		virtual ~KexiReportPart();

		//! \return a pointer to Reports Widget Library.
		static KFormDesigner::WidgetLibrary* library();

//		KFormDesigner::FormManager *manager() { return m_manager; }

		void generateForm(KexiDB::FieldList *list, QDomDocument &domDoc);

		class TempData : public KexiWindowData
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

		virtual KLocalizedString i18nMessage(const QString& englishMessage, 
			KexiWindow* window) const;

	protected:
		virtual KexiWindowData* createWindowData(KexiWindow* window);
		
		virtual KexiView* createView(QWidget *parent, KexiWindow* window,
			KexiPart::Item &item, Kexi::ViewMode viewMode = Kexi::DataViewMode,
			QMap<QString,QVariant>* staticObjectArgs = 0);

		virtual void initPartActions();
		virtual void initInstanceActions();

		static KFormDesigner::WidgetLibrary* static_reportsLibrary;

	private:
//		QPointer<KFormDesigner::FormManager> m_manager;
};

#endif

