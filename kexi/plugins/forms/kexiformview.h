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

#ifndef KEXIFORMVIEW_H
#define KEXIFORMVIEW_H

#include <qscrollview.h>
#include <qtimer.h>

#include <kexiviewbase.h>

#include "kexiformpart.h"

class KexiFormPart;
class KexiMainWindow;
class KexiDBForm;
class QColor;
class QFont;
class KexiRecordNavigator;

//! The scrollview which inlcudes KexiDBForm
/*! It allows to resize its m_widget, following snapToGrid setting. 
 Its contents is resized so the widget can always be resized. */
class KexiFormScrollView : public QScrollView
{
	Q_OBJECT

	public:
		KexiFormScrollView(QWidget *parent, bool preview);
		virtual ~KexiFormScrollView();

		void setFormWidget(KexiDBForm *w);
		void setResizingEnabled(bool enabled) { m_enableResizing = enabled; }

		void setForm(KFormDesigner::Form *form) { m_form = form; }

		void refreshContentsSizeLater(bool horizontal, bool vertical);

		void updateNavPanelGeometry();

	public slots:
		/*! Make sure there is a 300px margin around the form contents to allow resizing. */
		void refreshContentsSize();

		/*! Reimplemented to update resize policy. */
		virtual void show();

	protected:
		virtual void contentsMousePressEvent(QMouseEvent * ev);
		virtual void contentsMouseReleaseEvent(QMouseEvent * ev);
		virtual void contentsMouseMoveEvent(QMouseEvent * ev);
		virtual void drawContents( QPainter * p, int clipx, int clipy, int clipw, int cliph );
		virtual void leaveEvent( QEvent *e );
		virtual void setHBarGeometry( QScrollBar & hbar, int x, int y, int w, int h );

		bool m_resizing;
		bool m_enableResizing;
		QWidget *m_widget;

		KFormDesigner::Form *m_form;
		int m_gridX, m_gridY;
		QFont m_helpFont;
		QColor m_helpColor;
		QTimer m_delayedResize;
		//! for refreshContentsSizeLater()
		QScrollView::ScrollBarMode m_vsmode, m_hsmode;
		bool m_snapToGrid : 1;
		bool m_preview : 1;
		bool m_smodeSet : 1;
		KexiRecordNavigator* m_navPanel;
};

//! The FormPart's view
/*! This class presents a siungle view used inside KexiDialogBase.
 It takes care of saving/loading form, of enabling actions when needed. 
 One KexiFormView object is instantiated for data view mode (preview == true in constructor),
 and second KexiFormView object is instantiated for design view mode 
 (preview == false in constructor). */
class KexiFormView : public KexiViewBase
{
	Q_OBJECT

	public:
		enum ResizeMode { 
			ResizeAuto = 0, 
			ResizeDefault = ResizeAuto, 
			ResizeFixed = 1, 
			NoResize = 2 /*! @todo */
		};

		KexiFormView(KexiMainWindow *win, QWidget *parent, const char *name, bool preview, KexiDB::Connection *conn);
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
	private:
		KexiDBForm *m_dbform;
		KexiFormScrollView *m_scrollView;
		KexiPropertyBuffer *m_buffer;
		KexiDB::Connection *m_conn;
		int m_resizeMode;
};

#endif
