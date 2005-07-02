/* This file is part of the KDE project
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifndef KEXIDATASOURCEPAGE_H
#define KEXIDATASOURCEPAGE_H

#include <qwidget.h>

class KexiObjectInfoLabel;
class KexiDataSourceComboBox;
class KexiFieldComboBox;
class KexiFieldListView;
class KexiProject;
class QToolButton;
class QLabel;
class QFrame;
namespace KoProperty {
	class Set;
}

//! A page within form's property tabbed pane, providing data source information
class KEXIFORMUTILS_EXPORT KexiDataSourcePage : public QWidget
{
	Q_OBJECT

	public:
		KexiDataSourcePage(QWidget *parent, const char *name = 0);
		virtual ~KexiDataSourcePage();

		KexiDataSourceComboBox* dataSourceCombo() const { return m_dataSourceCombo; }
		KexiObjectInfoLabel* objectInfoLabel() const { return m_objectInfoLabel; }

	public slots:
		void setProject(KexiProject *prj);
		void clearDataSourceSelection();
		void clearWidgetDataSourceSelection();

		//! Sets data source of a currently selected form. 
		//! This is performed on form initialization and on activating.
		void setDataSource(const QCString& mimeType, const QCString& name);

		//! Receives a pointer to a new property \a set (from KexiFormView::managerPropertyChanged())
		void assignPropertySet(KoProperty::Set* propertySet);

	signals:
		//! Signal emitted when helper button 'go to selected data source' is clicked.
		void jumpToObjectRequested(const QCString& mime, const QCString& name);

		//! Signal emitted when form's data source has been changed. It's connected to the Form Manager.
		void formDataSourceChanged(const QCString& mime, const QCString& name);

		//! Signal emitted when current widget's data source (field/expression)
		//! has been changed. It's connected to the Form Manager.
		void dataSourceFieldOrExpressionChanged(const QString& string);

	protected slots:
		void slotDataSourceTextChanged(const QString & string);
		void slotDataSourceSelected();
		void slotFieldSelected();
		void slotGotoSelected();
		void slotAddSelectedField();

	protected:
		KexiFieldComboBox *m_sourceFieldCombo;
		KexiObjectInfoLabel *m_objectInfoLabel;
		KexiDataSourceComboBox* m_dataSourceCombo;
		KexiFieldListView* m_fieldListView;
		QLabel *m_noDataSourceAvailableLabel, *m_widgetDSLabel, *m_availableFieldsLabel;
		QToolButton *m_clearWidgetDSButton, *m_clearDSButton, *m_gotoButton, *m_addField;
		QFrame *m_dataSourceSeparator;
		QString m_noDataSourceAvailableSingleText, m_noDataSourceAvailableMultiText;
		bool m_insideClearDataSourceSelection : 1;

//		class Private;
//		Private *d;
};

#endif
