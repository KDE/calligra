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
class KexiPropertyBuffer;

class KEXICORE_EXPORT KexiDialogBase : public KMdiChildView, public KexiActionProxy
{
	Q_OBJECT

	public:
		KexiDialogBase(KexiMainWindow *parent, const QString &caption = QString::null);
		virtual ~KexiDialogBase();

		bool isRegistered();

		//! \return currently seelcted view or 0 if there is no current view
		KexiViewBase *selectedView() const;

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

		//js todo: maybe remove this since it's often the same as partItem()->identifier()?:

		/*! This method sets internal identifier for the dialog, but
		 if there is a part item associated with this dialog (see partItem()),
		 partItem()->identifier() will be is used as identifier, so this method is noop. 
		 Thus, it's usable only for dialog types when no part item is assigned. */
		void setId(int id) { m_id = id; }

		/*! If there is a part item associated with this dialog (see partItem()),
		 partItem()->identifier() is returned, otherwise internal dialog's identifier
		 (previously set by setID()) is returned. */
		int	id() const;

		//! \return Kexi part used to create this window
		inline KexiPart::Part* part() const { return m_part; }

		//! \return Kexi part item used to create this window
		KexiPart::Item *partItem() const { return m_item; }

		//! Kexi part's gui client
		inline KexiPart::GUIClient* guiClient() const { return m_part ? m_part->instanceGuiClient() : 0; }

		/*! Tries to close the dialog. \return true if closing is accepted 
		 (sometimes, user may not want to close the dialog by pressing cancel). 
		 If \a dontSaveChanges if true, changes are not saved even if this dialog is dirty. */
//js removed		bool tryClose(bool dontSaveChanges);

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

		/*! Internal reimplementation. */
		virtual bool eventFilter(QObject *obj, QEvent *e);

		/*! Used by Main Window 
		 \todo js: PROBABLY REMOVE THESE TWO?
		*/
		virtual void attachToGUIClient();
		virtual void detachFromGUIClient();

		/*! True if contents (data) of the dialog is dirty and need to be saved
		 This may or not be used, depending if changes in the dialog 
		 are saved immediately (e.g. like in datatableview) or saved by hand (by user)
		 (e.g. like in alter-table dialog).
		 Default implementation always returns "dirty" flag retrieved from the current 
		 view (if present) ot just false;
		 Reimplement this if you e.g. want reuse "dirty" 
		 flag from internal structures that may be changed. */
		virtual bool dirty() const;

		/*! \return true, if this dialog's data were never saved.
		 If it's true we're usually try to ask a user if the dialog's 
		 data should be saved somewhere. After dialog construction,
		 "neverSaved" flag is set to appropriate value.
		 KexiPart::Item::neverSaved() is reused.
		*/
		bool neverSaved() const;

		/*! \return property buffer provided by a current view,
		 or NULL if there is no view set (or the view has no buffer assgned). */
		KexiPropertyBuffer *propertyBuffer();

		KexiDB::SchemaData* schemaData() const { return m_schemaData; }
		/*! Reimpelmented: "*" is added if for 'dirty' dialog's data. */
//		QString caption() const;

	public slots:
//		virtual void detach();
		virtual void setFocus();

		void updateCaption();

		/*! Tells this dialog to save changes of the existin object
		 to the backend. \sa storeNewData()
		 \return true on success. */
		bool storeData();

		/*! Tells this dialog to create and store data of the new object
		 to the backend.
		 Object's schema data has been never stored, 
		 so it is created automatically, using information obtained 
		 form part item. On success, part item's ID is updated to new value,
		 and m_schemaData is set. \sa schemaData().
		 \return true on success. */
		bool storeNewData();

	signals:
		void updateContextHelp();

		//! emitted when the window is about to close
		void closing();

		/*! Emited to inform the world that 'dirty' flag changes. 
		 Activated by KexiViewBase::setDirty(). */
		void dirtyChanged(KexiDialogBase*);

	protected:
		void registerDialog();
		virtual void closeEvent( QCloseEvent * e );

		//! Internal.
		void addView(KexiViewBase *view, int mode);

		int m_supportedViewModes;
		int m_currentViewMode;

		inline QWidgetStack * stack() const { return m_stack; }

		void dirtyChanged();

		/*! Stores large string data \a dataString, block (e.g. xml form's representation) 
		 at the backend. Block will be stored in "kexi__objectdata" table pointed by
		 this object's id and an optional \a dataID identifier. 
		 If there is already such record in the table, it's simply overwritten.
		 \return true on success
		*/
		bool storeDataBlock( const QString &dataString, const QString& dataID = QString::null );

		/*! Loads large string data \a dataString, block (e.g. xml form's representation).
		 \return true on success
		 \sa storeDataBlock(). */
		bool loadDataBlock( QString &dataString, const QString& dataID = QString::null);

	private:
		KexiMainWindow *m_parentWindow;
		bool m_isRegistered;
#ifdef KEXI_NO_CTXT_HELP
		KexiContextHelpInfo *m_contextHelpInfo;
#endif
		int m_id;
//		KInstance *m_instance;
		QGuardedPtr<KexiPart::Part> m_part;
		KexiPart::Item *m_item;
		QWidgetStack *m_stack;
		QString m_origCaption; //helper
		KexiDB::SchemaData* m_schemaData;
//		bool m_neverSaved : 1; //!< true, if this dialog's contents were never saved 
		bool m_destroying : 1; //!< true after entering to the dctor

		friend class KexiMainWindow;
//		friend class KexiMainWindowImpl;
		friend class KexiPart::Part;
		friend class KexiInternalPart;
		friend class KexiViewBase;
};

#endif

