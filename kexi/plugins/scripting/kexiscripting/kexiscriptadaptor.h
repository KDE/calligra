/* This file is part of the KDE project
   Copyright (C) 2006-2008 Sebastian Sauer <mail@dipe.org>

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

#ifndef KEXISCRIPTADAPTOR_H
#define KEXISCRIPTADAPTOR_H

#include <QObject>
#include <QMetaObject>
#include <QAction>
#include <kross/core/manager.h>
#include <kexi.h>
#include <kexipart.h>
#include <kexiproject.h>
//#include <kexidataawareview.h>
#include <KexiMainWindowIface.h>
#include <KexiWindow.h>
#include <KexiView.h>
#include <kexidb/connection.h>

/**
* Adaptor class that provides Kexi specific functionality to
* the scripting world.
*/
class KexiScriptAdaptor : public QObject
{
    Q_OBJECT
public:
    explicit KexiScriptAdaptor() : m_kexidbmodule(0) {
        setObjectName("Kexi");
    }
    virtual ~KexiScriptAdaptor() {}
public Q_SLOTS:

    /**
    * Returns the current KexiWindow widget.
    */
    QWidget* windowWidget() const {
        return currentWindow();
    }

    /**
    * Returns the current KexiView widget.
    */
    QWidget* viewWidget() const {
        return currentView();
    }

    /**
    * Returns a list of all QAction instances the Kexi main
    * window provides.
    *
    * Python sample that prints the list of all actions the
    * main window does provide.
    * \code
    * import Kexi
    * for a in Kexi.actions():
    *     print "name=%s text=%s" % (a.objectName,a.text)
    * \endcode
    */
    QVariantList actions() {
        QVariantList list;
        foreach(QAction* action, mainWindow()->allActions()) {
            QVariant v;
            v.setValue((QObject*) action);
            list << v;
        }
        return list;
    }

    /**
    * Returns the QAction instance the Kexi main window provides that
    * has the objectName \p name or NULL if there is no such action.
    */
    QObject* action(const QString& name) {
        foreach(QAction* action, mainWindow()->allActions())
        if (action->objectName() == name)
            return action;
        return 0;
    }

    /**
    * Returns true if we are connected with a project else false
    * is returned.
    */
    bool isConnected() {
        return project() ? project()->isConnected() : false;
    }

    /**
    * Returns the KexiDBConnection object that belongs to the opened
    * project or return NULL if there was no project opened (no
    * connection established).
    */
    QObject* getConnection() {
        if (! m_kexidbmodule)
            m_kexidbmodule = Kross::Manager::self().module("kexidb");
        ::KexiDB::Connection *connection = project() ? project()->dbConnection() : 0;
        if (m_kexidbmodule && connection) {
            QObject* result = 0;
            if (QMetaObject::invokeMethod(m_kexidbmodule, "connectionWrapper", Q_RETURN_ARG(QObject*, result), Q_ARG(QObject*, connection)))
                return result;
        }
        return 0;
    }

    /**
    * Returns a list of names of all items the mimetype provides. Possible
    * mimetypes are for example "table", "query", "form" or "script".
    *
    * Python sample that prints all tables within the current project.
    * \code
    * import Kexi
    * print Kexi.items("table")
    * \endcode
    */
    QStringList items(QString mimetype) {
        QStringList list;
        if (project()) {
            KexiPart::ItemList l;
            project()->getSortedItemsForMimeType(l, mimeType(mimetype).toUtf8());
            l.sort();
            foreach(KexiPart::Item* i, l)
            list << i->name();
        }
        return list;
    }

    /**
    * Returns the caption for the item defined with \p mimetype and \p name .
    */
    QString itemCaption(const QString& mimetype, const QString& name) const {
        KexiPart::Item *item = partItem(mimeType(mimetype), name);
        return item ? item->caption() : QString();
    }

    /**
    * Set the caption for the item defined with \p mimetype and \p name .
    */
    void setItemCaption(const QString& mimetype, const QString& name, const QString& caption) {
        if (KexiPart::Item *item = partItem(mimeType(mimetype), name))
            item->setCaption(caption);
    }

    /**
    * Returns the description for the item defined with \p mimetype and \p name .
    */
    QString itemDescription(const QString& mimetype, const QString& name) const {
        KexiPart::Item *item = partItem(mimeType(mimetype), name);
        return item ? item->description() : QString();
    }

    /**
    * Set the description for the item defined with \p mimetype and \p name .
    */
    void setItemDescription(const QString& mimetype, const QString& name, const QString& description) {
        if (KexiPart::Item *item = partItem(mimeType(mimetype), name))
            item->setDescription(description);
    }

    /**
    * Open an item. A window for the item defined with \p mimetype and \p name will
    * be opened and we switch to it. The \p viewmode could be for example "data" (the
    * default), "design" or "text" while the \args are optional arguments passed
    * to the item.
    *
    * Python sample that opens the "cars" form in design view mode and sets then the
    * dirty state to mark the formular as modified.
    * \code
    * import Kexi
    * Kexi.openItem("form","cars","design")
    * Kexi.windowWidget().setDirty(True)
    * \endcode
    */
    bool openItem(const QString& mimetype, const QString& name, const QString& viewmode = QString(), QVariantMap args = QVariantMap()) {
        bool openingCancelled;
        KexiPart::Item *item = partItem(mimeType(mimetype), name);
        KexiWindow* window = item ? mainWindow()->openObject(item, stringToViewMode(viewmode), openingCancelled, args.isEmpty() ? 0 : &args) : 0;
        return (window && ! openingCancelled);
    }

    /**
    * Close an opened item. The window for the item defined with \p mimetype and \p name
    * will be closed.
    *
    * Python sample that opens the "table1" table and closes the window right after
    * being opened.
    * \code
    * import Kexi
    * Kexi.openItem("table","table1")
    * Kexi.closeItem("table","table1")
    * \endcode
    */
    bool closeItem(const QString& mimetype, const QString& name) {
        if (KexiPart::Item *item = partItem(mimeType(mimetype), name))
            return mainWindow()->closeObject(item) == true;
        return false;
    }

    /**
    * Print the item defined with \p mimetype and \p name .
    */
    bool printItem(const QString& mimetype, const QString& name, bool preview = false) {
        if (KexiPart::Item *item = partItem(mimeType(mimetype), name))
            return (preview ? mainWindow()->printPreviewForItem(item) : mainWindow()->printItem(item)) == true;
        return false;
    }

    /**
    * Executes custom action for the item defined with \p mimetype and \p name .
    */
    bool executeItem(const QString& mimetype, const QString& name, const QString& actionName) {
        if (KexiPart::Item *item = partItem(mimeType(mimetype), name))
            return mainWindow()->executeCustomActionForObject(item, actionName) == true;
        return false;
    }


    /**
    * Returns the name of the current viewmode. This could be for example "data",
    * "design", "text" or just an empty string if there is no view at the moment.
    */
    QString viewMode() const {
        return currentView() ? viewModeToString(currentView()->viewMode()) : QString();
    }

    /**
    * Returns a list of names of all available viewmodes the view supports.
    */
    QStringList viewModes() const {
        QStringList list;
        if (currentWindow()) {
            Kexi::ViewModes modes = currentWindow()->supportedViewModes();
            if (modes & Kexi::DataViewMode)
                list << "data";
            if (modes & Kexi::DesignViewMode)
                list << "design";
            if (modes & Kexi::TextViewMode)
                list << "text";
        }
        return list;
    }

    /**
    * Returns true if there is a current view and those current view is dirty aka
    * has the dirty-flag set that indicates that something changed.
    */
    bool viewIsDirty() const {
        return currentView() ? currentView()->isDirty() : false;
    }

private:
    QObject* m_kexidbmodule;

    KexiMainWindowIface* mainWindow() const {
        return KexiMainWindowIface::global();
    }
    KexiProject* project() const {
        return mainWindow()->project();
    }
    KexiWindow* currentWindow() const {
        return mainWindow()->currentWindow();
    }
    KexiView* currentView() const {
        return currentWindow() ? currentWindow()->selectedView() : 0;
    }
    KexiPart::Item* partItem(const QString& mimetype, const QString& name) const {
        return project() ? project()->itemForMimeType(mimetype, name) : 0;
    }
    QString mimeType(QString mimetype) const {
        return mimetype.startsWith("kexi/") ? mimetype : mimetype.prepend("kexi/");
    }
    QString viewModeToString(Kexi::ViewMode mode, const QString& defaultViewMode = QString()) const {
        switch (mode) {
        case Kexi::DataViewMode:
            return "data";
        case Kexi::DesignViewMode:
            return "design";
        case Kexi::TextViewMode:
            return "text";
        default:
            break;
        }
        return defaultViewMode;
    }
    Kexi::ViewMode stringToViewMode(const QString& mode, Kexi::ViewMode defaultViewMode = Kexi::DataViewMode) const {
        if (mode == "data")
            return Kexi::DataViewMode;
        if (mode == "design")
            return Kexi::DesignViewMode;
        if (mode == "text")
            return Kexi::TextViewMode;
        return defaultViewMode;
    }
};

#endif

