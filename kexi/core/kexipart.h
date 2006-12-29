/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIPART_H
#define KEXIPART_H

#include <qobject.h>
#include <qmap.h>
//Added by qt3to4:
#include <Q3CString>

#include <kexiutils/tristate.h>
#include "kexi.h"
#include "keximainwindow.h"

class KActionCollection;
class KexiDialogBase;
class KexiDialogTempData;
class KexiViewBase;
class KexiMainWindowImpl;
class KAction;
class KShortcut;
class KTabWidget;

namespace KexiPart
{
	class Info;
	class Item;
	class GUIClient;
	class PartPrivate;
	class StaticInfo;

/*! Official (registered) type IDs for objects like table, query, form... */
enum ObjectTypes {
	TableObjectType = KexiDB::TableObjectType, //!< 1, like in KexiDB::ObjectTypes
	QueryObjectType = KexiDB::QueryObjectType, //!< 2, like in KexiDB::ObjectTypes
	FormObjectType = 3,
	ReportObjectType = 4,
	ScriptObjectType = 5,
	WebObjectType = 6,
	MacroObjectType = 7,
	LastObjectType = 7, //ALWAYS UPDATE THIS

	UserObjectType = 100 //!< external types
};

/**
 * The main class for kexi frontend parts like tables, queries, forms and reports
 */
class KEXICORE_EXPORT Part : public QObject
{
	Q_OBJECT

	public:
		/*! Constructor. */
		Part(QObject *parent, const char *name, const QStringList &);
		/*! Destructor. */
		virtual ~Part();

//! @todo make it protected, outside world should use KexiProject
		/*! Try to execute the part. Implementations of this \a Part
		are able to overwrite this method to offer execution.
		\param item The \a KexiPart::Item that should be executed.
		\param sender The sender QObject which likes to execute this \a Part or
		NULL if there is no sender. The KFormDesigner uses this to pass
		the actual widget (e.g. the button that was pressed).
		\return true if execution was successfully else false.
                */
		virtual bool execute(KexiPart::Item* item, QObject* sender = 0) {
		    Q_UNUSED(item);
		    Q_UNUSED(sender);
		    return false;
		}

		/*! \return supported modes for dialogs created by this part, i.e. a combination
		 of Kexi::ViewMode enum elements.
		 Set this member in your KexiPart subclass' ctor, if you need to override the default value
		 that equals Kexi::DataViewMode | Kexi::DesignViewMode, 
		 or Kexi::DesignViewMode in case of Kexi::PartStaticPart object.
		 This information is used to set supported view modes for every 
		 KexiDialogBase-derived object created by this KexiPart. */
		inline int supportedViewModes() const { return m_supportedViewModes; }

		/*! \return supported modes for dialogs created by this part in "user mode", i.e. a combination
		 of Kexi::ViewMode enum elements.
		 Set this member in your KexiPart subclass' ctor, if you need to override the default value
		 that equals Kexi::DataViewMode. or 0 in case of Kexi::PartStaticPart object.
		 This information is used to set supported view modes for every 
		 KexiDialogBase-derived object created by this KexiPart. */
		inline int supportedUserViewModes() const { return m_supportedUserViewModes; }

//! @todo make it protected, outside world should use KexiProject
		/*! "Opens" an instance that the part provides, pointed by \a item in a mode \a viewMode. 
		 \a viewMode is one of Kexi::ViewMode enum. 
		 \a staticObjectArgs can be passed for static Kexi Parts. */
		KexiDialogBase* openInstance(KexiMainWindow *win, KexiPart::Item &item, 
			int viewMode = Kexi::DataViewMode, QMap<QString,QString>* staticObjectArgs = 0);

//! @todo make it protected, outside world should use KexiProject
		/*! Removes any stored data pointed by \a item (example: table is dropped for table part). 
		 From now this data is inaccesible, and \a item disappear.
		 You do not need to remove \a item, or remove object's schema stored in the database,
		 beacuse this will be done automatically by KexiProject after successful 
		 call of this method. All object's data blocks are also automatically removed from database 
		 (from "kexi__objectdata" table).
		 For this, a database connection associated with kexi project owned by \a win can be used.

		 Database transaction is started by KexiProject before calling this method, 
		 and it will be rolled back if you return false here.
		 You shouldn't use by hand transactions here.
		 
		 Default implementation just removes object from kexi__* system structures 
		 at the database backend using KexiDB::Connection::removeObject(). */
		virtual bool remove(KexiMainWindow *win, KexiPart::Item & item);

		/*! Renames stored data pointed by \a item to \a newName 
		 (example: table name is altered in the database). 
		 For this, a database connection associated with kexi project owned by \a win can be used. 
		 You do not need to change \a item, and change object's schema stored in the database,
		 beacuse this is automatically handled by KexiProject. 

		 Database transaction is started by KexiProject before calling this method, 
		 and it will be rolled back if you return false here.
		 You shouldn't use by hand transactions here.

		 Default implementation does nothing and returns true. */
		virtual tristate rename(KexiMainWindow *win, KexiPart::Item &item, const QString& newName);

		/*! Creates and returns a new temporary data for a dialog \a dialog. 
		 This method is called on openInstance() once per dialog.
		 Reimplement this to return KexiDialogTempData subclass instance.
		 Default implemention just returns empty KexiDialogTempData object. */
		virtual KexiDialogTempData* createTempData(KexiDialogBase* dialog);

		/*! Creates a new view for mode \a viewMode, \a item and \a parent. The view will be 
		 used inside \a dialog. */
		virtual KexiViewBase* createView(QWidget *parent, KexiDialogBase* dialog, 
			KexiPart::Item &item, int viewMode = Kexi::DataViewMode, QMap<QString,QString>* staticObjectArgs = 0) = 0;

		/*! i18n'd instance name usable for displaying in gui as object's name.
		 The name is valid identifier - contains latin1 lowercase characters only.
		 @todo move this to Info class when the name could be moved as localized property 
		 to service's .desktop file. */
		Q3CString instanceName() const;

		/*! i18n'd instance name usable for displaying in gui as object's caption.
		 @todo move this to Info class when the name could be moved as localized property 
		 to service's .desktop file. */
		QString instanceCaption() const;

		inline Info *info() const { return m_info; }

		/*! \return part's GUI Client, so you can 
		 create part-wide actions using this client. */
		inline GUIClient *guiClient() const { return m_guiClient; }

		/*! \return part's GUI Client, so you can 
		 create instance-wide actions using this client. */
		inline GUIClient *instanceGuiClient(int mode = 0) const
			{ return m_instanceGuiClients[mode]; }

#if 0
		/**
		 * @returns the datasource object of this part
		 * reeimplement it to make a part work as dataprovider ;)
		 */
		virtual DataSource *dataSource() { return 0; }
#endif

		/*! \return action collection for mode \a viewMode. */
		KActionCollection* actionCollectionForMode(int viewMode) const;

		const Kexi::ObjectStatus& lastOperationStatus() const { return m_status; }

		/*! \return i18n'd message translated from \a englishMessage.
		 This method is useful for messages like: 
		 "<p>Table \"%1\" has been modified.</p>",
		 -- such messages can be accurately translated, 
		 while this could not: "<p>%1 \"%2\" has been modified.</p>".
		 See implementation of this method in KexiTablePart to see 
		 what strings are needed for translation.

		 Default implementation returns generic \a englishMessage. 
		 In special cases, \a englishMessage can start with ":", 
		 to indicate that empty string will be generated if 
		 a part does not offer a message for such \a englishMessage.
		 This is used e.g. in KexiMainWindowImpl::closeDialog().
		 */
		virtual QString i18nMessage(const Q3CString& englishMessage, 
			KexiDialogBase* dlg) const;

	signals: 
		void newObjectRequest( KexiPart::Info *info );

	protected slots:
		void slotCreate();

	protected:
		//! Used by StaticPart
		Part(QObject* parent, StaticInfo *info);

//		virtual KexiDialogBase* createInstance(KexiMainWindow *win, const KexiPart::Item &item, int viewMode = Kexi::DataViewMode) = 0;

		//! Creates GUICLients for this part, attached to \a win
		//! This method is called from KexiMainWindow
		void createGUIClients(KexiMainWindow *win);

#if 0
		/*! For reimplementation. Create here all part actions (KAction or similar). 
		 "Part action" is an action that is bound to given part, not for dialogs 
		 created with this part, eg. "Open external project" action for Form part.
		 Default implementation does nothing.
		*/
		virtual void initPartActions( KActionCollection * ) {};

		/*! For reimplementation. You should here create all instance actions (KAction or similar)
		 for \a mode (this method called for every value given by Kexi::ViewMode enum, 
		 and in special cases, in the future - for user-defined part-specific modes). 
		 Actions should be bound to action collection \a col. 
		 "Instance action" is an action that is bound to given dialog instance (created with a part), 
		 for specific view. \a mo; eg. "Filter data" action for DataViewMode of Table part. 
		 By creating actions here, you can ensure that after switching to other view mode (eg. from
		 Design view to Data view), appropriate actions will be switched/hidden.
		 \a mode equal Kexi::AllViewModes means that given actions will be available for 
		 all supported views.
		 Default implementation does nothing.
		*/
		virtual void initInstanceActions( int mode, KActionCollection *col ) {};
#endif

		virtual void initPartActions();
		virtual void initInstanceActions();

		virtual KexiDB::SchemaData* loadSchemaData(KexiDialogBase *dlg, 
			const KexiDB::SchemaData& sdata, int viewMode);

		bool loadDataBlock( KexiDialogBase *dlg, QString &dataString, const QString& dataID = QString::null);

		/*! Creates shared action for action collection declared 
		 for 'instance actions' of this part.
		 See KexiSharedActionHost::createSharedAction() for details.
		 Pass desired KAction subclass with \a subclassName (e.g. "KToggleAction") to have
		 that subclass allocated instead just KAction (what is the default). */
		KAction* createSharedAction(int mode, const QString &text, 
			const QString &pix_name, const KShortcut &cut, const char *name, 
			const char *subclassName = 0);

		/*! Convenience version of above method - creates shared toggle action. */
		KAction* createSharedToggleAction(int mode, const QString &text,
			const QString &pix_name, const KShortcut &cut, const char *name);

		/*! Creates shared action for action collection declared 
		 for 'part actions' of this part.
		 See KexiSharedActionHost::createSharedAction() for details.
		 Pass desired KAction subclass with \a subclassName (e.g. "KToggleAction") to have
		 that subclass allocated instead just KAction (what is the default). */
		KAction* createSharedPartAction(const QString &text, 
			const QString &pix_name, const KShortcut &cut, const char *name,
			const char *subclassName = 0);

		/*! Convenience version of above method - creates shared toggle action 
		 for 'part actions' of this part. */
		KAction* createSharedPartToggleAction(const QString &text,
			const QString &pix_name, const KShortcut &cut, const char *name);

		void setActionAvailable(const char *action_name, bool avail);

		inline void setInfo(Info *info) { m_info = info; }

		/*! This method can be reimplemented to setup additional tabs 
		 in the property editor panel. Default implementation does nothing. 
		 This method is called whenever current dialog (KexiDialogBase) is switched and
		 type (mime type) of its contents differs from previous one. 
		 For example, if a user switched from Table Designer to Form Designer,
		 additional tab containing Form Designer's object tree should be shown. */
		virtual void setupCustomPropertyPanelTabs(KTabWidget *tab, KexiMainWindow* mainWin);

		//! Set of i18n'd action names for, initialised on KexiPart::Part subclass ctor
		//! The names are useful because the same action can have other name for each part
		//! E.g. "New table" vs "New query" can have different forms for some languages...
		QMap<QString,QString> m_names;

		/*! Supported modes for dialogs created by this part.
		 @see supportedViewModes() */
		int m_supportedViewModes;

		/*! Supported modes for dialogs created by this part in "user mode".
		 The default is Kexi::DataViewMode. It is altered in classes like KexiSimplePrintingPart.
		 @see supportedUserViewModes() */
		int m_supportedUserViewModes;

		Info *m_info;
		GUIClient *m_guiClient;
		QMap<int, GUIClient*> m_instanceGuiClients;
		KexiMainWindow* m_mainWin;
		Kexi::ObjectStatus m_status;

		/*! If you're implementing a new part, set this to value >0 in your ctor 
		 if you have well known (ie registered ID) for your part.
		 So far, table, query, form, report and script part have defined their IDs 
		 (see KexiPart::ObjectTypes). */
		int m_registeredPartID;

		/*! True if newwly created, unsaved objects are dirty. False by default.
		 You can change it in your subclass' constructor.	*/
		bool m_newObjectsAreDirty : 1;

		PartPrivate *d;

	friend class Manager;
	friend class ::KexiMainWindow;
	friend class ::KexiMainWindowImpl;
	friend class GUIClient;
};

}

#endif
