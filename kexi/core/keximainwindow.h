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

#ifndef KEXIMAINWINDOW_H
#define KEXIMAINWINDOW_H

#include <qmap.h>
#include <qintdict.h>

#include <kmdimainfrm.h>
#include <kexiutils/tristate.h>

#include "kexisharedactionhost.h"
#include "kexi.h"

class KexiDialogBase;
class KexiProject;
namespace KexiPart {
	class Item;
}

/**
 * @short Kexi's main window interface
 * This interface is implemented by KexiMainWindowImpl class.
 * KexiMainWindow offers simple features what lowers cross-dependency (and also avoids
 * circular dependencies between Kexi modules).
 */
class KEXICORE_EXPORT KexiMainWindow : public KMdiMainFrm, public KexiSharedActionHost
{
	Q_OBJECT
	public:
		//! Used by printActionForItem()
		enum PrintActionType {
			PrintItem,
			PreviewItem,
			PageSetupForItem
		};

		KexiMainWindow();
		virtual ~KexiMainWindow();

		//! Project data of currently opened project or NULL if no project here yet.
		virtual KexiProject *project() = 0;

		/*! Registers dialog \a dlg for watching and adds it to the main window's stack. */
		virtual void registerChild(KexiDialogBase *dlg) = 0;

		virtual QPopupMenu* findPopupMenu(const char *popupName) = 0;

		/*! Generates ID for private "document" like Relations window.
		 Private IDs are negative numbers (while ID regular part instance's IDs are >0)
		 Private means that the object is not stored as-is in the project but is somewhat
		 generated and in most cases there is at most one unique instance document of such type (part).
		 To generate this ID, just app-wide internal counter is used. */
		virtual int generatePrivateID() = 0;

		/*! \return a list of all actions defined by application.
		 Not all of them are shared. Don't use plug these actions
		 in your windows by hand but user methods from KexiViewBase! */
		virtual KActionPtrList allActions() const = 0;

		/*! \return currently active dialog (window) od 0 if there is no active dialog. */
		virtual KexiDialogBase* currentDialog() const = 0;

	signals:
		//! Emitted to make sure the project can be close. 
		//! Connect a slot here and set \a cancel to true to cancel the closing.
		void acceptProjectClosingRequested(bool& cancel);

		//! Emitted before closing the project (and destroying all it's data members).
		//! You can do you cleanup of your structures here. 
		void beforeProjectClosing();

		//! Emitted after closing the project. 
		void projectClosed();

	public slots:
		//! Opens object pointed by \a item in a view \a viewMode
		virtual KexiDialogBase * openObject(KexiPart::Item *item, int viewMode,
			bool &openingCancelled, QMap<QString,QString>* staticObjectArgs = 0) = 0;

		//! For convenience
		virtual KexiDialogBase * openObject(const QCString& mime, const QString& name, 
			int viewMode, bool &openingCancelled, QMap<QString,QString>* staticObjectArgs = 0) = 0;

		/*! Called to accept property butter editing. */
		virtual void acceptPropertySetEditing() = 0;

		/*! Received information from active view that \a dlg has switched
		its property set, so property editor contents should be reloaded.
		 If \a force is true, property editor's data is reloaded even
		 if the currently pointed property set is the same as before.
		 If \a preservePrevSelection is true and there was a property set
		 set before call, previously selected item will be preselected
		 in the editor (if found). */
		virtual void propertySetSwitched(KexiDialogBase *dlg, bool force=false,
			bool preservePrevSelection = true, const QCString& propertyToSelect = QCString()) = 0;

		/*! Saves dialog's \a dlg data. If dialog's data is never saved,
		 user is asked for name and title, before saving (see getNewObjectInfo()).
		 \return true on successul saving or false on error.
		 If saving was cancelled by user, cancelled is returned.
		 \a messageWhenAskingForName is a i18n'ed text that will be visible
		 within name/caption dialog (see KexiNameDialog), which is popped
		 up for never saved objects. */
		virtual tristate saveObject( KexiDialogBase *dlg,
			const QString& messageWhenAskingForName = QString::null, bool dontAsk = false ) = 0;

		/*! Closes dialog \a dlg. If dialog's data (see KexiDialoBase::dirty()) is unsaved,
		 used will be asked if saving should be perforemed.
		 \return true on successull closing or false on closing error.
		 If closing was cancelled by user, cancelled is returned. */
		virtual tristate closeDialog(KexiDialogBase *dlg) = 0;

		/*! Displays a dialog for entering object's name and title.
		 Used on new object saving.
		 \return true on successul closing or cancelled on cancel returned.
		 It's unlikely to have false returned here.
		 \a messageWhenAskingForName is a i18n'ed text that will be visible
		 within name/caption dialog (see KexiNameDialog).
		 If \a allowOverwriting is true, user will be asked for existing
		 object's overwriting, else it will be impossible to enter
		 a name of exisiting object.
		 You can check \a allowOverwriting after calling this method.
		 If it's true, user agreed on overwriting, if it's false, user picked
		 nonexisting name, so no overwrite will be needed. */
		virtual tristate getNewObjectInfo( KexiPart::Item *partItem, KexiPart::Part *part,
			bool& allowOverwriting, const QString& messageWhenAskingForName = QString::null ) = 0;

		/*! Highlights object of mime \a mime and name \a name.
		 This can be done in the Project Navigator or so. 
		 If a window for the object is opened (in any mode), it should be raised. */
		virtual void highlightObject(const QCString& mime, const QCString& name) = 0;

		//! Shows "print" dialog for \a item.
		//! \return true on success.
		virtual tristate printItem(KexiPart::Item* item) = 0;

		//! Shows "print preview" dialog. 
		//! \return true on success.
		virtual tristate printPreviewForItem(KexiPart::Item* item) = 0;

		//! Shows "page setup" dialog for \a item.
		//! \return true on success and cancelled when the action was cancelled.
		virtual tristate showPageSetupForItem(KexiPart::Item* item) = 0;

		/*! Executes custom action for the main window, usually provided by a plugin. 
		 Also used by KexiFormEventAction. */
		virtual tristate executeCustomActionForObject(KexiPart::Item* item, const QString& actionName) = 0;

	protected slots:
		virtual void slotObjectRenamed(const KexiPart::Item &item, const QCString& oldName) = 0;

};


#endif

