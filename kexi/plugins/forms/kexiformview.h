/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIFORMVIEW_H
#define KEXIFORMVIEW_H

#include <qscrollview.h>
#include <qtimer.h>

#include <kexiviewbase.h>
#include <kexidataiteminterface.h>
#include <widget/utils/kexirecordnavigator.h>

#include "kexiscrollview.h"
#include "kexiformpart.h"

class KexiFormPart;
class KexiMainWindow;
class KexiDBForm;
class KexiTableItem;
class KexiTableViewData;

//! The KexiDataProvider class is a data provider for Kexi Forms
/*! This provider collects data-aware widgets using setMainWidget().
 Then, usedDataSources() unique list of required field names is available.
 On every call of fillDataItems() method, thew provider will fill data items 
 with appropriate data from a database cursor.

 Field names are collected effectively, so eg. having widgets using data sources:
 ("name", "surname", "surname", "name") - "name" and "surname" repeated - will only 
 return ("name", "surname") list, so the cursor's query can be simplified 
 and thus more effective.
*/
class KexiDataProvider : public KexiDataItemChangesListener
{
	public:
		KexiDataProvider();
		~KexiDataProvider();

		/*! sets \a mainWidget to be a main widget for this data provider.
		 Also find widgets whose will work as data items 
		 (all of them must implement KexiDataItemInterface), so these could be 
		 filled with data on demand. */
		void setMainWidget(QWidget* mainWidget);

		QStringList usedDataSources() const { return m_usedDataSources; }

		const QPtrList<KexiDataItemInterface>& dataItems() const { return m_dataItems; }

		/*! Fills data items with appropriate data fetched from \a cursor. */
		void fillDataItems(KexiTableItem& row);// KexiDB::Cursor& cursor);

		//! Reaction for change of \a item.
		virtual void valueChanged(KexiDataItemInterface* item);

	protected:
		QWidget *m_mainWidget;
		QPtrList<KexiDataItemInterface> m_dataItems;
		QStringList m_usedDataSources;
		QMap<KexiDataItemInterface*,uint> m_fieldNumbersForDataItems;
};

class KexiFormScrollView : public KexiScrollView
{
	Q_OBJECT

	public:
		KexiFormScrollView(QWidget *parent, bool preview);
		virtual ~KexiFormScrollView();

		void setForm(KFormDesigner::Form *form) { m_form = form; }

	public slots:
		/*! Reimplemented to update resize policy. */
		virtual void show();

	protected slots:
		void slotResizingStarted();

	protected:

		KFormDesigner::Form *m_form;
};


//! The FormPart's view
/*! This class provides a single view used inside KexiDialogBase.
 It takes care of saving/loading form, of enabling actions when needed. 
 One KexiFormView object is instantiated for data view mode (preview == true in constructor),
 and second KexiFormView object is instantiated for design view mode 
 (preview == false in constructor).
*/
class KexiFormView : public KexiViewBase, public KexiRecordNavigatorHandler
{
	Q_OBJECT

	public:
		enum ResizeMode { 
			ResizeAuto = 0, 
			ResizeDefault = ResizeAuto, 
			ResizeFixed = 1, 
			NoResize = 2 /*! @todo */
		};

		KexiFormView(KexiMainWindow *win, QWidget *parent, const char *name, KexiDB::Connection *conn);
		virtual ~KexiFormView();

		KexiDB::Connection* connection() { return m_conn; }

		virtual QSize preferredSizeHint(const QSize& otherSize);

		int resizeMode() const { return m_resizeMode; }

	protected slots:
		void managerPropertyChanged(KexiPropertyBuffer *b);
		void slotDirty(KFormDesigner::Form *f, bool isDirty);

		void slotWidgetSelected(KFormDesigner::Form *form, bool multiple);
		void slotFormWidgetSelected(KFormDesigner::Form *form);
		void slotNoFormSelected();

		void setUndoEnabled(bool enabled);
		void setRedoEnabled(bool enabled);

	protected:
		virtual tristate beforeSwitchTo(int mode, bool &dontStore);
		virtual tristate afterSwitchFrom(int mode);
		virtual KexiPropertyBuffer* propertyBuffer() { return m_buffer; }

		virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata, bool &cancel);
		virtual tristate storeData();

		KexiFormPart::TempData* tempData() const {
			return static_cast<KexiFormPart::TempData*>(parentDialog()->tempData()); }
		KexiFormPart* formPart() const { return static_cast<KexiFormPart*>(part()); }

		void disableWidgetActions();
		void enableFormActions();

		KFormDesigner::Form* form() const;
		void setForm(KFormDesigner::Form *f);

		void initForm();
		void loadForm();

		virtual void resizeEvent ( QResizeEvent * );

		// for navigator
		virtual void moveToRecordRequested(uint r);
		virtual void moveToLastRecordRequested();
		virtual void moveToPreviousRecordRequested();
		virtual void moveToNextRecordRequested();
		virtual void moveToFirstRecordRequested();
		virtual void addNewRecordRequested();

		KexiDBForm *m_dbform;
		KexiFormScrollView *m_scrollView;
		KexiPropertyBuffer *m_buffer;
		KexiDB::Connection *m_conn;

		/*! Database cursor used for data retrieving. 
		 It is shared between subsequent Data view sessions (just reopened on switch), 
		 but deleted and recreated from scratch when form's "dataSource" property changed
		 since last form viewing (m_previousDataSourceString is used for that).
		*/
		QString m_previousDataSourceString;
		int m_resizeMode;

		KexiDataProvider* m_provider;
//		KexiDB::Cursor *m_cursor;
		KexiDB::QuerySchema* m_query;
		KexiTableViewData *m_data;
		KexiTableItem *m_currentRow;
		int m_currentRowNumber;
};

#endif
