/* This file is part of the KDE project
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIVIEWBASE_H
#define KEXIVIEWBASE_H

#include <qwidget.h>

#include "kexiactionproxy.h"

class KexiMainWindow;
class KexiDialogBase;

namespace KoProperty {
	class Set;
}

namespace KexiDB {
	class SchemaData;
}

//! Base class for single view embeddable of in KexiDialogBase.
/*! This class automatically works as a proxy for shared (application-wide) actions.
 KexiViewBase has 'dirty' flag to indicate that view's data has changed.
 This flag's state is reused by KexiDialogBase object that contain the view.
 KexiViewBase objects can be also nested, using addChildView(): any actions and 'dirty' flag
 are transmited to parent view in this case.

 KexiViewBase objects are usually allocated within KexiDialogBase objects by implementing
 KexiPart::createView() method. See query or table part code for examples.

 KexiViewBase object can be also allocated without attaching it KexiDialogBase,
 especially withinn dock window. see KexiMainWindowImpl::initNavigator() to see example
 how KexiBrowser does this.
*/
class KEXICORE_EXPORT KexiViewBase : public QWidget, public KexiActionProxy
{
	Q_OBJECT

	public:
		KexiViewBase(KexiMainWindow *mainWin, QWidget *parent, const char *name = 0);
		virtual ~KexiViewBase();

		//! \return kexi main window that contain this view
		inline KexiMainWindow *mainWin() const { return m_mainWin; }

		//! \return parent KexiDialogBase that contains this view, or 0 if no dialog contain this view
		KexiDialogBase* parentDialog() const { return m_dialog; }

		/*! Added for convenience.
		 \return KexiPart object that was used to create this view (with a dialog)
		 or 0 if this view is not created using KexiPart. \sa parentDialog() */
		KexiPart::Part* part() const;

		/*! \return preferred size hint, that can be used to resize the view.
		 It is computed using maximum of (a) \a otherSize and (b) current KMDI dock area's size,
		 so the view won't exceed this maximum size. The method is used e.g. in KexiDialogBase::sizeHint().
		 If you reimplement this method, do not forget to return value of
		 yoursize.boundedTo( KexiViewBase::preferredSizeHint(otherSize) ). */
		virtual QSize preferredSizeHint(const QSize& otherSize);

		virtual bool eventFilter( QObject *o, QEvent *e );

		void addChildView( KexiViewBase* childView );

		/*! True if contents (data) of the view is dirty and need to be saved
		 This may or not be used, depending if changes in the dialog
		 are saved immediately (e.g. like in datatableview) or saved by hand (by user)
		 (e.g. like in alter-table dialog).
		 "Dirty" flag is reused by KexiDialogBase::dirty().
		 Default implementation just uses internal m_dirty flag, that is false by default.
		 Reimplement this if you e.g. want reuse other "dirty"
		 flag from internal structures that may be changed. */
		virtual bool dirty() const { return m_dirty; }

		/*! \return the view mode for this view. */
		int viewMode() const { return m_viewMode; }

		/*! Reimpelmented from KexiActionProxy.
		 \return shared action with name \a action_name for this view.
		 If there's no such action declared in Kexi Part (part()),
		 global shared action is returned (if exists). */
		virtual KAction* sharedAction( const char *action_name );

		/*! Enables or disables shared action declared in Kexi Part (part()).
		 If there's no such action, global shared action is enabled or disabled (if exists). */
		virtual void setAvailable(const char* action_name, bool set);

	public slots:
		virtual void setFocus();

		/*! Call this in your view's implementation whenever current property set
		 (returned by propertySet()) is switched to other,
		 so property editor contents need to be completely replaced. */
		virtual void propertySetSwitched();

		/*! Sets dirty flag on or off. It the flag changes,
		 dirty(bool) signal is emitted by parent dialog (KexiDialog),
		 to inform the world about that. If this view has a parent view, setDirty()
		 is called also on parent view.
		 Always use this function to update 'dirty' flag information. */
		void setDirty(bool set);

		/*! Equal to setDirty(true). */
		void setDirty() { setDirty(true); }

	signals:
		//! emitted when the view is about to close
		void closing(bool& cancel);

		void focus(bool in);

	protected:
		/*! called by KexiDialogBase::switchToViewMode() right before dialog is switched to new mode
		 By default does nothing. Reimplement this if you need to do something
		 before switching to this view.
		 \return true if you accept or false if a error occupied and view shouldn't change
		 If there is no error but switching should be just cancelled
		 (probably after showing some info messages), you need to return cancelled.
		 Set \a dontStore to true (it's false by default) if you want to avoid data storing
		 by storeData() or storeNewData(). */
		virtual tristate beforeSwitchTo(int mode, bool &dontStore);

		/*! called by KexiDialogBase::switchToViewMode() right after dialog is switched to new mode
		 By default does nothing. Reimplement this if you need to do something
		 after switching to this view.
		 \return true if you accept or false if a error occupied and view shouldn't change
		 If there is no error but switching should be just cancelled
		 (probably after showing some info messages), you need to return cancelled. */
		virtual tristate afterSwitchFrom(int mode);

		virtual void closeEvent( QCloseEvent * e );

		/*! \return a property set for this view. For reimplementation. By default returns NULL. */
		virtual KoProperty::Set *propertySet();

		/*! Call this in your view's implementation whenever current property set
		 is changed that few properties are now visible and/or few other are invisible,
		 so property editor operating on this property set should be completely reloaded.
		 If \a preservePrevSelection is true and there was a property set
		 assigned before call, previously selected item will be preselected
		 in the editor (if found). */
		void propertySetReloaded(bool preservePrevSelection = false, const QCString& propertyToSelect = QCString());

		/*! Tells this dialog to create and store data of the new object
		 pointed by \a sdata on the backend.
		 Called by KexiDialogBase::storeNewData().
		 Default implementation:
		 - makes a deep copy of \a sdata
		 - stores object schema data \a sdata in 'kexi__objects' internal table
		   using Connection::storeObjectSchemaData().
		 Reimpelment this for your needs.
		 Requirements:
		 - deep copy of \a sdata should be made
		 - schema data should be created at the backend
		   (by calling KexiViewBase::storeNewData(const KexiDB::SchemaData& sdata)),
		   or using Connection::storeObjectSchemaData() or more specialized
		   method. For example, KexiAlterTableDialog
		   uses Connection::createTable(TableSchema) for this
		   (tableschema is SchemaData subclass) to store more information than
		   just a schem adata. You should use such subclasses if needed.
		 Should return newly created schema data object on success.
		 In this case, do not store schema object yourself (make deep copy if needed). */
		virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata, bool &cancel);

		/*! Loads large string data \a dataString block (e.g. xml form's representation),
		 indexed with optional \a dataID, from the database backend. 
		 If \a canBeEmpty is true and there is no data block for dataID, true is returned
		 and \a dataString is set to null string. The default is false.
		 \return true on success
		 \sa storeDataBlock(). */
		bool loadDataBlock( QString &dataString, const QString& dataID = QString::null, bool canBeEmpty = false );

		/*! Tells this view to store data changes on the backend.
		 Called by KexiDialogBase::storeData().
		 Default implementation:
		 - makes a deep copy of \a sdata
		 - stores object schema data \a sdata in 'kexi__objects' internal table
		   using Connection::storeObjectSchemaData().
		 If \a dontAsk is true, no question dialog will 
		 be shown to the user. The default is false.

		 Reimpelment this for your needs. Should return true on success 
		 or cancelled when the task should be cancelled.
		 \sa storeNewData() */
		virtual tristate storeData(bool dontAsk = false);

		/*! Stores (potentially large) string data \a dataString, block (e.g. xml form's representation),
		 at the database backend. Block will be stored in "kexi__objectdata" table pointed by
		 this object's id and an optional \a dataID identifier.

		 If dialog's id is not available (KexiDialogBase::id()),
		 then ID that was just created in storeNewData() is used
		 (see description of m_newlyAssignedID member).
		 If there is already such record in the table, it's simply overwritten.
		 \return true on success
		*/
		bool storeDataBlock( const QString &dataString, const QString &dataID = QString::null );

		/*! Removes (potentially large) string data (e.g. xml form's representation),
		 pointed by optional \a dataID, from the database backend.
		 \return true on success. Does not fail if the block doe not exists.
		 Note that if \a dataID is not specified, all data blocks for this view will be removed.
		 \sa storeDataBlock(). */
		bool removeDataBlock( const QString& dataID = QString::null);

		void setViewWidget(QWidget* w, bool focusProxy = false);

		/*! Updates actions (e.g. availability). Reimplement it, if needed (you must
		 call superclass impelmentation at the end!).
		 This implementation does nothing for this view but calls updateActions()
		 for every child-view of this view.
		 called by KexiDialogBase on dialog's activation (\a activated is true)
		 or deactivation. */
		virtual void updateActions(bool activated);

		virtual void setFocusInternal() { QWidget::setFocus(); }

		/*! Allows to react on parent dialog's detaching (only for KMDI's ChildFrame mode)
		 - it is called by KexiDialogBase::youAreDetached().
		 Default implementation does nothing.
		 Implement it if you want to perform some appropriate actions. */
		virtual void parentDialogDetached() {};

		/*! Allows to react on parent dialog's attaching (only for KMDI's ChildFrame mode)
		 - it is called by KexiDialogBase::youAreAttached().
		 Default implementation does nothing.
		 Implement it if you want to perform some appropriate actions. */
		virtual void parentDialogAttached() {};

		QString m_defaultIconName;

		KexiMainWindow *m_mainWin;

		KexiDialogBase *m_dialog;

		QWidget *m_viewWidget;

		KexiViewBase *m_parentView;

		QGuardedPtr<QWidget> m_lastFocusedChildBeforeFocusOut;

	private:
		/*! Member set to newly assigned object's ID in storeNewData()
		 and used in storeDataBlock(). This is needed because usually,
		 storeDataBlock() can be called from storeNewData() and in this case
		 dialog has not yet assigned valid identifier (it has just negative temp. number).
		 \sa KexiDialogBase::id()
		 */
		int m_newlyAssignedID;

		/*! Mode for this view. Initialized by KexiDialogBase::switchToViewMode().
		 Can be useful when single class is used for more than one view (e.g. KexiDBForm). */
		int m_viewMode;

		QPtrList<KexiViewBase> m_children;

		bool m_dirty : 1;

	friend class KexiDialogBase;
};

#endif

