/* This file is part of the KDE project
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIFORMVIEW_H
#define KEXIFORMVIEW_H

#include <qscrollview.h>
#include <kexiviewbase.h>

#include "kexiformpart.h"

class KexiFormPart;
class KexiMainWindow;
class KexiDBForm;

using KFormDesigner::Form;

class KexiFormScrollView : public QScrollView
{
	Q_OBJECT

	public:
		KexiFormScrollView(QWidget *parent, const char *name="formpart_kexiformview");
		~KexiFormScrollView();

		void  setWidget(QWidget *w);
		void  setResizingEnabled(bool enabled) { m_enableResizing = enabled; }

	protected:
		virtual void contentsMousePressEvent(QMouseEvent * ev);
		virtual void contentsMouseReleaseEvent(QMouseEvent * ev);
		virtual void contentsMouseMoveEvent(QMouseEvent * ev);

	private:
		bool    m_resizing;
		bool    m_enableResizing;
		QWidget *m_widget;
};

class KexiFormView : public KexiViewBase
{
	Q_OBJECT

	public:
		KexiFormView(KexiMainWindow *win, QWidget *parent, const char *name, bool preview/*, KexiDB::Connection *conn*/);
		~KexiFormView();

	protected slots:
		void managerPropertyChanged(KexiPropertyBuffer *b);
		void slotDirty(KFormDesigner::Form *f, bool isDirty);

		void slotWidgetSelected(Form *form, bool multiple);
		void slotFormWidgetSelected(Form *form);
		void slotNoFormSelected();

		void setUndoEnabled(bool enabled);
		void setRedoEnabled(bool enabled);

	protected:
		virtual bool beforeSwitchTo(int mode, bool &cancelled, bool &dontStore);
		virtual bool afterSwitchFrom(int mode, bool &cancelled);
		virtual KexiPropertyBuffer* propertyBuffer();

		virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata, bool &cancel);
		virtual bool storeData(bool &cancel);

		KexiFormPart::TempData* tempData() const {
			return static_cast<KexiFormPart::TempData*>(parentDialog()->tempData()); }
		KexiFormPart* formPart() const { return static_cast<KexiFormPart*>(part()); }

		void disableWidgetActions();
		void enableFormActions();

		KFormDesigner::Form* form() const;
		void setForm(KFormDesigner::Form *f);

		void initForm();
		void loadForm();

	private:
		KexiDBForm   *m_dbform;
		KexiFormScrollView   *m_scrollView;
		bool   m_preview;
};

#endif
