/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIREPORTVIEW_H
#define KEXIREPORTVIEW_H

#include <qscrollview.h>
#include <qtimer.h>

#include <kexiviewbase.h>

#include "kexiscrollview.h"
#include "kexireportpart.h"

class KexiReportForm;

class KexiReportScrollView : public KexiScrollView
{
	Q_OBJECT

	public:
		KexiReportScrollView(QWidget *parent, bool preview);
		virtual ~KexiReportScrollView();

		void setForm(KFormDesigner::Form *form) { m_form = form; }

	public slots:
		/*! Reimplemented to update resize policy. */
		virtual void show();

	protected slots:
		void slotResizingStarted();

	private:
		KFormDesigner::Form *m_form;
};


//! The FormPart's view
/*! This class presents a single view used inside KexiDialogBase.
 It takes care of saving/loading report, of enabling actions when needed.
 One KexiReportView object is instantiated for data view mode (preview == true in constructor),
 and second KexiReportView object is instantiated for design view mode
 (preview == false in constructor). */
class KexiReportView : public KexiViewBase
{
	Q_OBJECT

	public:
		enum ResizeMode {
			ResizeAuto = 0,
			ResizeDefault = ResizeAuto,
			ResizeFixed = 1,
			NoResize = 2 /*! @todo */
		};

		KexiReportView(KexiMainWindow *win, QWidget *parent, const char *name, KexiDB::Connection *conn);
		virtual ~KexiReportView();

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

		KexiReportPart::TempData* tempData() const {
			return static_cast<KexiReportPart::TempData*>(parentDialog()->tempData()); }
		KexiReportPart* reportPart() const { return static_cast<KexiReportPart*>(part()); }

		void disableWidgetActions();
		void enableFormActions();

		KFormDesigner::Form* form() const;
		void setForm(KFormDesigner::Form *f);

		void initForm();
		void loadForm();

		virtual void resizeEvent ( QResizeEvent * );

	private:
		KexiReportForm *m_reportform;
		KexiReportScrollView *m_scrollView;
		KexiPropertyBuffer *m_buffer;
		KexiDB::Connection *m_conn;
		int m_resizeMode;
};

#endif
