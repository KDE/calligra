/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2005 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIMAINWINDOWIFACE_H
#define KEXIMAINWINDOWIFACE_H

#include <QMap>
#include <Q3CString>

#include <kexi_global.h>
#include <kmainwindow.h>
#include <kexiutils/tristate.h>

#include "kexisharedactionhost.h"
#include "kexi.h"
//#include "keximdi.h"

class KexiWindow;
class KexiProject;
class KActionCollection;
class KXMLGUIClient;
class KXMLGUIFactory;
namespace KexiPart
{
class Item;
}

/**
 * @short Kexi's main window interface
 * This interface is implemented by KexiMainWindow class.
 * KexiMainWindow offers simple features what lowers cross-dependency (and also avoids
 * circular dependencies between Kexi modules).
 */
class KEXICORE_EXPORT KexiMainWindowIface : /*public KexiMdiMainFrm,*/ public KexiSharedActionHost
{
// Q_OBJECT
public:
    //! Used by printActionForItem()
    enum PrintActionType {
        PrintItem,
        PreviewItem,
        PageSetupForItem
    };

    KexiMainWindowIface();
    virtual ~KexiMainWindowIface();

    //! \return KexiMainWindowImpl global singleton (if it is instantiated)
    static KexiMainWindowIface* global();

    QWidget* thisWidget() {
        return dynamic_cast<QWidget*>(this);
    }

    //! Project data of currently opened project or NULL if no project here yet.
    virtual KexiProject *project() = 0;

#ifdef __GNUC__
#warning TODO virtual KActionCollection* actionCollection() const = 0;
#else
#pragma WARNING( TODO virtual KActionCollection* actionCollection() const = 0; )
#endif
    virtual KActionCollection* actionCollection() const = 0;

#ifdef __GNUC__
#warning TODO virtual QWidget* focusWidget() const = 0;
#else
#pragma WARNING( TODO virtual QWidget* focusWidget() const = 0; )
#endif
    virtual QWidget* focusWidget() const = 0;

    //! Implemented by KXMLGUIClient
#ifdef __GNUC__
#warning TODO virtual void plugActionList(const QString& name, const QList<KAction *>& actionList) = 0;
#else
#pragma WARNING( TODO virtual void plugActionList(const QString& name, const QList<KAction *>& actionList) = 0; )
#endif
    virtual void plugActionList(const QString& name,
                                const QList<KAction *>& actionList) = 0;

#ifdef __GNUC__
#warning TODO KXMLGUIClient* guiClient() const = 0;
#else
#pragma WARNING( TODO KXMLGUIClient* guiClient() const = 0; )
#endif
    virtual KXMLGUIClient* guiClient() const = 0;

    //! Implemented by KXMLGUIClient
#ifdef __GNUC__
#warning TODO virtual void unplugActionList (const QString &name) = 0;
#else
#pragma WARNING( TODO virtual void unplugActionList (const QString &name) = 0; )
#endif
    virtual void unplugActionList(const QString &name) = 0;

    //! Implemented by KMainWindow
#ifdef __GNUC__
#warning TODO virtual KXMLGUIFactory * KMainWindow::guiFactory() = 0;
#else
#pragma WARNING( TODO virtual KXMLGUIFactory * KMainWindow::guiFactory() = 0; )
#endif
    virtual KXMLGUIFactory * guiFactory() = 0;

    /*! Registers window \a window for watching and adds it to the main window's stack. */
    virtual void registerChild(KexiWindow *window) = 0;

//2.0 disabled  virtual Q3PopupMenu* findPopupMenu(const char *popupName) = 0;

    /*! Generates ID for private "document" like Relations window.
     Private IDs are negative numbers (while ID regular part instance's IDs are >0)
     Private means that the object is not stored as-is in the project but is somewhat
     generated and in most cases there is at most one unique instance document of such type (part).
     To generate this ID, just app-wide internal counter is used. */
//moved to KexiProject  virtual int generatePrivateID() = 0;

    /*! \return a list of all actions defined by application.
     Not all of them are shared. Don't use plug these actions
     in your windows by hand but user methods from KexiView! */
    virtual QList<QAction*> allActions() const = 0;

    /*! \return currently active window or 0 if there is no active window. */
    virtual KexiWindow* currentWindow() const = 0;

    /*! Switches \a window to view \a mode.
     Activates the window if it is not the current window. */
    virtual tristate switchToViewMode(KexiWindow& window, Kexi::ViewMode viewMode) = 0;

    /*! \return true if this window is in the User Mode. */
    virtual bool userMode() const = 0;

// signals:
    //! Emitted to make sure the project can be close.
    //! Connect a slot here and set \a cancel to true to cancel the closing.
    virtual void acceptProjectClosingRequested(bool& cancel) = 0;

    //! Emitted before closing the project (and destroying all it's data members).
    //! You can do you cleanup of your structures here.
    virtual void beforeProjectClosing() = 0;

    //! Emitted after closing the project.
    virtual void projectClosed() = 0;
//#endif

// public slots:
    /*! Creates new object of type defined by \a info part info.
     \a openingCancelled is set to true is opening has been cancelled.
     \return true on success. */
    virtual bool newObject(KexiPart::Info *info, bool& openingCancelled) = 0;

    //! Opens object pointed by \a item in a view \a viewMode
    virtual KexiWindow* openObject(KexiPart::Item *item, Kexi::ViewMode viewMode,
                                   bool &openingCancelled, QMap<QString, QVariant>* staticObjectArgs = 0,
                                   QString* errorMessage = 0) = 0;

    //! For convenience
    virtual KexiWindow* openObject(const Q3CString& mime, const QString& name,
                                   Kexi::ViewMode viewMode, bool &openingCancelled, QMap<QString, QVariant>* staticObjectArgs = 0) = 0;

    /*! Closes the object for \a item.
     \return true on success (closing can be dealyed though), false on failure and cancelled
     if the object has "opening" job assigned. */
    virtual tristate closeObject(KexiPart::Item* item) = 0;

    /*! Called to accept property butter editing. */
    virtual void acceptPropertySetEditing() = 0;

    /*! Received information from active view that \a window has switched
    its property set, so property editor contents should be reloaded.
     If \a force is true, property editor's data is reloaded even
     if the currently pointed property set is the same as before.
     If \a preservePrevSelection is true and there was a property set
     set before call, previously selected item will be preselected
     in the editor (if found). */
    virtual void propertySetSwitched(KexiWindow *window, bool force = false,
                                     bool preservePrevSelection = true, const QByteArray& propertyToSelect = QByteArray()) = 0;

    /*! Saves window's \a window data. If window's data is never saved,
     user is asked for name and title, before saving (see getNewObjectInfo()).
     \return true on successul saving or false on error.
     If saving was cancelled by user, cancelled is returned.
     \a messageWhenAskingForName is a i18n'ed text that will be visible
     within name/caption dialog (see KexiNameDialog), which is popped
     up for never saved objects. */
    virtual tristate saveObject(KexiWindow *window,
                                const QString& messageWhenAskingForName = QString(), bool dontAsk = false) = 0;

    /*! Closes window \a window. If window's data (see KexiWindow::isDirty()) is unsaved,
     used will be asked if saving should be perforemed.
     \return true on successull closing or false on closing error.
     If closing was cancelled by user, cancelled is returned.
     If \a window is 0, the current one will be closed. */
    virtual tristate closeWindow(KexiWindow *window) = 0;

    /*! Displays a window for entering object's name and title.
     Used on new object saving.
     \return true on successul closing or cancelled on cancel returned.
     It's unlikely to have false returned here.
     \a messageWhenAskingForName is a i18n'ed text that will be visible
     within name/caption dialog (see KexiNameDialog).
     If \a allowOverwriting is true, user will be asked for existing
     object's overwriting, else it will be impossible to enter
     a name of existing object.
     You can check \a allowOverwriting after calling this method.
     If it's true, user agreed on overwriting, if it's false, user picked
     nonexisting name, so no overwrite will be needed. */
    virtual tristate getNewObjectInfo(KexiPart::Item *partItem, KexiPart::Part *part,
                                      bool& allowOverwriting, const QString& messageWhenAskingForName = QString()) = 0;

    /*! Highlights object of mime \a mime and name \a name.
     This can be done in the Project Navigator or so.
     If a window for the object is opened (in any mode), it should be raised. */
    virtual void highlightObject(const Q3CString& mime, const Q3CString& name) = 0;

    //! Shows "print" dialog for \a item.
    //! \return true on success.
    virtual tristate printItem(KexiPart::Item* item) = 0;

    //! Shows "print preview" window.
    //! \return true on success.
    virtual tristate printPreviewForItem(KexiPart::Item* item) = 0;

    //! Shows "page setup" window for \a item.
    //! \return true on success and cancelled when the action was cancelled.
    virtual tristate showPageSetupForItem(KexiPart::Item* item) = 0;

    /*! Executes custom action for the main window, usually provided by a plugin.
     Also used by KexiFormEventAction. */
    virtual tristate executeCustomActionForObject(KexiPart::Item* item, const QString& actionName) = 0;

    /*! Shows application's startup widget, setups it if necessary. */
    virtual void showStartupMainWidget() = 0;

protected: // slots:
    virtual void slotObjectRenamed(const KexiPart::Item &item, const QString& oldName) = 0;

};


#endif

