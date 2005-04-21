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

#include <qtimer.h>

#include <kexiviewbase.h>
#include <widget/kexidataawareview.h>

#include "kexiformpart.h"

class KexiFormPart;
class KexiMainWindow;
class KexiDBForm;
class KexiTableItem;
class KexiTableViewData;
class KexiFormScrollView;

//! The KexiFormView lass provides a data-driven (record-based) form view .
/*! The KexiFormView can display data provided "by hand"
 or from KexiDB-compatible database source.

 This class provides a single view used inside KexiDialogBase.
 It takes care of saving/loading form, of enabling actions when needed.
 One KexiFormView object is instantiated for data view mode
 and a second KexiFormView object is instantiated for design view mode.

 @see KexiDataTable
*/
class KexiFormView : public KexiDataAwareView
{
	Q_OBJECT

	public:
		enum ResizeMode {
			ResizeAuto = 0,
			ResizeDefault = ResizeAuto,
			ResizeFixed = 1,
			NoResize = 2 /*! @todo */
		};

//		KexiFormView(KexiMainWindow *win, QWidget *parent, const char *name, KexiDB::Connection *conn);
		KexiFormView(KexiMainWindow *mainWin, QWidget *parent, const char *name = 0,
			bool dbAware = true);
		virtual ~KexiFormView();

//		KexiDB::Connection* connection() { return m_conn; }

		virtual QSize preferredSizeHint(const QSize& otherSize);

		int resizeMode() const { return m_resizeMode; }

		KFormDesigner::Form* form() const;

	public slots:
		/*! Reimplemented to update resize policy. */
		virtual void show();

	protected slots:
		void managerPropertyChanged(KexiPropertyBuffer *b);
		void slotDirty(KFormDesigner::Form *f, bool isDirty);
		void slotFocus(bool in);

//moved to formmanager		void slotWidgetSelected(KFormDesigner::Form *form, bool multiple);
//moved to formmanager		void slotFormWidgetSelected(KFormDesigner::Form *form);
//moved to formmanager		void slotNoFormSelected();

//moved to formmanager		void setUndoEnabled(bool enabled);
//moved to formmanager		void setRedoEnabled(bool enabled);

	protected:
		virtual tristate beforeSwitchTo(int mode, bool &dontStore);
		virtual tristate afterSwitchFrom(int mode);
		virtual KexiPropertyBuffer* propertyBuffer() { return m_buffer; }

		virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata, bool &cancel);
		virtual tristate storeData();

		KexiFormPart::TempData* tempData() const {
			return static_cast<KexiFormPart::TempData*>(parentDialog()->tempData()); }
		KexiFormPart* formPart() const { return static_cast<KexiFormPart*>(part()); }

//moved to formmanager		void disableWidgetActions();
//moved to formmanager		void enableFormActions();

		void setForm(KFormDesigner::Form *f);

		void initForm();
		void loadForm();

		virtual void resizeEvent ( QResizeEvent * );

		void initDataSource();

		virtual void setFocusInternal();

/*		// for navigator
		virtual void moveToRecordRequested(uint r);
		virtual void moveToLastRecordRequested();
		virtual void moveToPreviousRecordRequested();
		virtual void moveToNextRecordRequested();
		virtual void moveToFirstRecordRequested();
		virtual void addNewRecordRequested();*/

		/*! Called after loading the form contents (before showing it).
		 Also called when the form window (KexiDialogBase) is detached
		 (in KMDI's Child Frame mode), because otherwise tabstop ordering can get broken. */
		void updateTabStopsOrder();

		/*! @internal */
		void deleteQuery();

		/*! Reimplemented after KexiViewBase.
		 Updates actions (e.g. availability). */
// todo		virtual void updateActions(bool activated);

		KexiDBForm *m_dbform;
		KexiFormScrollView *m_scrollView;
		KexiPropertyBuffer *m_buffer;

		/*! Database cursor used for data retrieving.
		 It is shared between subsequent Data view sessions (just reopened on switch),
		 but deleted and recreated from scratch when form's "dataSource" property changed
		 since last form viewing (m_previousDataSourceString is used for that). */
		QString m_previousDataSourceString;

		int m_resizeMode;

		KexiDB::QuerySchema* m_query;

		/*! True, if m_query is created as temporary object within this form.
		 If user selected an existing, predefined (stored) query, m_queryIsOwned will be false,
		 so the query object will not be destroyed. */
		bool m_queryIsOwned;

		KexiDB::Cursor *m_cursor;

		/*! For new (empty) forms only:
		 Our form's area will be resized more than once.
		 We will resize form widget itself later (in resizeEvent()). */
		bool m_delayedFormContentsResizeOnShow : 1;
};

#endif
