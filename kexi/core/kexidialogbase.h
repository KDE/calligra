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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIDIALOGBASE_H
#define KEXIDIALOGBASE_H

#include "kexipartguiclient.h"
#include "kexiactionproxy.h"
#include "kexi.h"
#include "kexipart.h"

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

namespace KoProperty {
	class Set;
}

//! Privides temporary data shared between KexiDialogBase's views (KexiView's)
/*! Designed for reimplementation, if needed. */
class KEXICORE_EXPORT KexiDialogTempData : public QObject
{
	public:
	KexiDialogTempData(QObject* parent)
	 : QObject(parent, "KexiDialogTempData")
	 , proposeOpeningInTextViewModeBecauseOfProblems(false)
	{}
	/*! Initially false, KexiPart::Part implementation can set this to true
	 on data loading (e.g. in loadSchemaData()), to indicate that TextView mode
	 could be used instead of DataView or DesignView, because there are problems
	 with opening object.

	 For example, in KexiQueryPart::loadSchemaData() query statement can be invalid,
	 and thus could not be displayed in DesignView mode or executed for DataView.
	 So, this flag is set to true and user is asked for confirmation for switching
	 to TextView (SQL Editor).

	 After switching to TextView, this flag is cleared.
	 */
	bool proposeOpeningInTextViewModeBecauseOfProblems : 1;
};

//! Base class for child window of Kexi's main application window.
/*! This class can contain a number of configurable views, switchable using toggle action.
 It also automatically works as a proxy for shared (application-wide) actions.
*/
class KEXICORE_EXPORT KexiDialogBase :
	public KMdiChildView,
	public KexiActionProxy,
	public Kexi::ObjectStatus
{
	Q_OBJECT

	public:
		KexiDialogBase(KexiMainWindow *parent, const QString &caption = QString::null);
		virtual ~KexiDialogBase();

		bool isRegistered();

		//! \return currently selected view or 0 if there is no current view
		KexiViewBase *selectedView() const;

		/*! \return a view for a given \a mode or 0 if there's no such mode available (or opened).
		 This does not open mode if it's not opened. */
		KexiViewBase *viewForMode(int mode) const;

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

		//! Kexi dialog's gui COMMON client.
		//! It's obtained by querying part object for this dialog.
		KexiPart::GUIClient* commonGUIClient() const;

		//! Kexi dialog's gui client for currently selected view.
		//! It's obtained by querying part object for this dialog.
		KexiPart::GUIClient* guiClient() const;

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

		/*! Switches this dialog to \a newViewMode.
		 \a viewMode is one of Kexi::ViewMode enum elements.
		 \return true for successful switching
		 True is returned also if user has cancelled switching
		 (rarely, but for any reason) - cancelled is returned.
		 */
		tristate switchToViewMode( int newViewMode );

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
		 \return true if at least on "dirty" flag is set for one of the dialog's view. */
		bool dirty() const;

		/*! \return a pointer to view that has recently set dirty flag.
		 This value is cleared when dirty flag is set to false (i.e. upon saving changes). */
		KexiViewBase* viewThatRecentlySetDirtyFlag() const { return m_viewThatRecentlySetDirtyFlag; }

		/*! \return true, if this dialog's data were never saved.
		 If it's true we're usually try to ask a user if the dialog's
		 data should be saved somewhere. After dialog construction,
		 "neverSaved" flag is set to appropriate value.
		 KexiPart::Item::neverSaved() is reused.
		*/
		bool neverSaved() const;

		/*! \return property set provided by a current view,
		 or NULL if there is no view set (or the view has no set assigned). */
		KoProperty::Set *propertySet();

		KexiDB::SchemaData* schemaData() const { return m_schemaData; }
		/*! Reimpelmented: "*" is added if for 'dirty' dialog's data. */
//		QString caption() const;

		/*! Used by KexiViewBase subclasses. \return temporary data shared between views */
		KexiDialogTempData *tempData() const { return m_tempData; }

//		/*! Used by KexiViewBase subclasses. Sets temporary data shared between views. */
//		void setTempData( KexiDialogTempData* data ) { m_tempData = data; }

		/*! Called primarily by KexiMainWindowImpl to activate dialog.
		 Selected view (if present) is also informed about activation. */
		void activate();

		/*! Called primarily by KexiMainWindowImpl to deactivate dialog.
		 Selected view (if present) is also informed about deactivation. */
		void deactivate();

	public slots:
		virtual void setFocus();

		void updateCaption();

		/*! Internal. Called by KexiMainWindowImpl::saveObject().
		 Tells this dialog to save changes of the existing object
		 to the backend. If \a dontAsk is true, no question dialog will 
		 be shown to the user. The default is false.
		 \sa storeNewData()
		 \return true on success, false on failure and cancelled when storing has been cancelled. */
		tristate storeData(bool dontAsk = false);

		/*! Internal. Called by KexiMainWindowImpl::saveObject().
		 Tells this dialog to create and store data of the new object
		 to the backend.
		 Object's schema data has been never stored,
		 so it is created automatically, using information obtained
		 form part item. On success, part item's ID is updated to new value,
		 and m_schemaData is set. \sa schemaData().
		 \return true on success, false on failure and cancelled when storing has been cancelled. */
		tristate storeNewData();

		/*! Reimplemented - we're informing the current view about performed detaching by calling
		 KexiViewBase::parentDialogDetached(), so the view can react on this event
		 (by default KexiViewBase::parentDialogDetached() does nothing, you can reimplement it). */
		void sendDetachedStateToCurrentView();

		/*! W're informing the current view about performed atttaching by calling
		 KexiViewBase::parentDialogAttached(), so the view can react on this event
		 (by default KexiViewBase::parentDialogAttached() does nothing, you can reimplement it). */
		void sendAttachedStateToCurrentView();

	signals:
		void updateContextHelp();

		//! emitted when the window is about to close
		void closing();

		/*! Emited to inform the world that 'dirty' flag changes.
		 Activated by KexiViewBase::setDirty(). */
		void dirtyChanged(KexiDialogBase*);

	protected slots:
		/*!  Sets 'dirty' flag on every dialog's view. */
		void setDirty(bool dirty);

	protected:
		/*! Used by Part::openInstance(), 
		 like switchToViewMode( int newViewMode ), but passed \a staticObjectArgs.
		 Only used for parts of class KexiPart::StaticPart. */
		tristate switchToViewMode( int newViewMode, QMap<QString,QString>* staticObjectArgs,
			bool& proposeOpeningInTextViewModeBecauseOfProblems);

		void registerDialog();

		virtual void closeEvent( QCloseEvent * e );

		//! \internal
		void addView(KexiViewBase *view, int mode);

		//! \internal
		void removeView(int mode);

		int m_supportedViewModes;
		int m_openedViewModes;
		int m_currentViewMode;

		inline QWidgetStack * stack() const { return m_stack; }

		//! Used by \a view to inform the dialog about changing state of the "dirty" flag.
		void dirtyChanged(KexiViewBase* view);
#if 0
		/*! Loads large string data \a dataString block (e.g. xml form's representation),
		 indexed with optional \a dataID, from the database backend.
		 \return true on success
		 \sa storeDataBlock(). */
		bool loadDataBlock( QString &dataString, const QString& dataID = QString::null);

		/*! Stores (potentially large) string data \a dataString, block (e.g. xml form's representation),
		 at the database backend. Block will be stored in "kexi__objectdata" table pointed by
		 this object's id and an optional \a dataID identifier.
		 If there is already such record in the table, it's simply overwritten.
		 \return true on success
		*/
		bool storeDataBlock( const QString &dataString, const QString& dataID = QString::null );

		/*! Removes (potentially large) string data (e.g. xml form's representation),
		 pointed by optional \a dataID, from the database backend.
		 \return true on success. Does not fail if the block doe not exists.
		 Note that if \a dataID is not specified, all data blocks for this dialog will be removed.
		 \sa storeDataBlock(). */
		bool removeDataBlock( QString &dataString, const QString& dataID = QString::null);

		/*! @internal
		 Used by KexiDialogBase::storeDataBlock() and by KexiViewBase::storeDataBlock().
		*/
		bool storeDataBlock_internal( const QString &dataString, int o_id, const QString& dataID );
#endif
//		void setError(const QString& message, const QString& details);

		bool isDesignModePreloadedForTextModeHackUsed(int newViewMode) const;

	private:
		KexiMainWindow *m_parentWindow;
		bool m_isRegistered;
#ifdef KEXI_NO_CTXT_HELP
		KexiContextHelpInfo *m_contextHelpInfo;
#endif
		int m_id;
		QGuardedPtr<KexiPart::Part> m_part;
		KexiPart::Item *m_item;
		QWidgetStack *m_stack;
		QString m_origCaption; //!< helper
		KexiDB::SchemaData* m_schemaData;
		QGuardedPtr<KexiViewBase> m_newlySelectedView; //!< Used in dirty(), temporary set in switchToViewMode()
		                                   //!< during view setup, when a new view is not yet raised.
		//! Used in viewThatRecentlySetDirtyFlag(), modified in dirtyChanged().
		QGuardedPtr<KexiViewBase> m_viewThatRecentlySetDirtyFlag; 
		QGuardedPtr<KexiDialogTempData> m_tempData; //!< temporary data shared between views

		/*! Created view's mode - helper for switchToViewMode(),
		 KexiViewBase ctor uses that info. >0 values are useful. */
		int m_creatingViewsMode;

		bool m_destroying : 1; //!< true after entering to the dctor
		bool m_disableDirtyChanged; //!< used in setDirty(), affects dirtyChanged()

		friend class KexiMainWindow;
//		friend class KexiMainWindowImpl;
		friend class KexiPart::Part;
		friend class KexiInternalPart;
		friend class KexiViewBase;
};

#endif

