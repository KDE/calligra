/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDIALOGBASE_H
#define KEXIDIALOGBASE_H

#include "kexipartguiclient.h"
#include "kexiactionproxy.h"
#include "kexi.h"

#include <qguardedptr.h>

#include <kmdichildview.h>
#include <kxmlguiclient.h>

class QWidgetStack;
class KexiMainWindow;
class KexiViewBase;
class KActionCollection;
class KexiContextHelpInfo;
namespace KexiPart {
	class Part;
}

class KEXICORE_EXPORT KexiDialogBase : public KMdiChildView, public KexiActionProxy
{
	Q_OBJECT

	public:
		KexiDialogBase(KexiMainWindow *parent, const QString &caption);
		virtual ~KexiDialogBase();
		bool isRegistered();

		//! Adds \a view for the dialog. It will be the _only_ view (of unspecified mode) for the dialog
		void addView(KexiViewBase *view);

		/*! \return main (top level) widget inside this dialog.
		 This widget is used for e.g. determining minimum size hint and size hint. */
//		virtual QWidget* mainWidget() = 0;

		/*! reimplemented: minimum size hint is inherited from currently visible view. */
		virtual QSize minimumSizeHint() const;

		/*! reimplemented: size hint is inherited from currently visible view. */
		virtual QSize sizeHint() const;

		KexiMainWindow	*mainWin() { return m_parentWindow; }

		void	setDocID(int id);
		int	docID() { return m_docID; }
//		KInstance *instance();

		//! Kexi part used to create this window
		inline KexiPart::Part* part() const { return m_part; }

		//! Kexi part's gui client
		inline KexiPart::GUIClient* guiClient() const { return m_part ? m_part->instanceGuiClient() : 0; }

		/*! Tries to close the dialog. \return true if closing is accepted 
		 (sometimes, user may not want to close the dialog by pressing cancel). 
		 If \a dontSaveChanges if true, changes are not saved iven if this dialog is dirty. */
		bool tryClose(bool dontSaveChanges);

		/*! \return name of icon provided by part that created this dialog.
		 The name is used by KexiMainWindow to set/reset icon for this dialog. */
		virtual QString itemIcon();

		/*! \return true if this dialog supports switching to \a mode. 
		 \a mode is one of Kexi::ViewMode enum elements.
		 The flags are used e.g. for testing by KexiMainWindow, if actions 
		 of switching to given view mode is available. 
		 This member is intialised in KexiPart that creates this KexiDialogBase object. */
		bool supportsViewMode( int mode ) const { return m_supportedViewModes & mode; }

		/*! \return current view mode for this dialog. */
		int currentViewMode() const { return m_currentViewMode; }

		/*! Switches this dialog to \a viewMode.
		 \a viewMode is one of Kexi::ViewMode enum elements. */
		bool switchToViewMode( int viewMode );

		void setContextHelp(const QString& caption, const QString& text, const QString& iconName);

	public slots:
//		virtual void detach();
		virtual void setFocus();

	signals:
		void updateContextHelp();

		//! emitted when the window is about to close
		void closing();

	protected:
		void registerDialog();
		virtual void attachToGUIClient();
		virtual void detachFromGUIClient();
		virtual void closeEvent( QCloseEvent * e );

		/*! True if contents (data) of the dialog is dirty and need to be saved
		 This may or not be used, depending if changes in the dialog 
		 are saved immediately (e.g. like in datatableview) or saved by hand (by user)
		 (e.g. like in alter-table dialog).
		 Default implementation always return false. Reimplement this if you e.g. want reuse "dirty" 
		 flag from internal structures that may be changed. */
		virtual bool dirty();

		int m_supportedViewModes;
		int m_currentViewMode;

		inline QWidgetStack * stack() const { return m_stack; }

	private:
		KexiMainWindow *m_parentWindow;
		bool m_isRegistered;
#ifdef KEXI_NO_CTXT_HELP
		KexiContextHelpInfo *m_contextHelpInfo;
#endif
		int m_docID;
//		KInstance *m_instance;
		QGuardedPtr<KexiPart::Part> m_part;
		const KexiPart::Item *m_item;
		QWidgetStack *m_stack;

		friend class KexiMainWindow;
		friend class KexiMainWindowImpl;
		friend class KexiPart::Part;
		friend class KexiInternalPart;
};

#endif

