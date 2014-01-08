/* This file is part of the KDE project
 *   Copyright (C) 2006-2008 Sebastian Sauer <mail@dipe.org>
 * 
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 * 
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 * 
 *   You should have received a copy of the GNU Library General Public License
 *   along with this library; see the file COPYING.LIB.  If not, write to
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KEXISCRIPTADAPTORQ_H
#define KEXISCRIPTADAPTORQ_H

#include <QObject>
#include <QMetaObject>
#include <QAction>
//#include <kross/core/manager.h>
#include <kexi.h>
#include <kexipart.h>
#include <kexiproject.h>
//#include <kexidataawareview.h>
#include <KexiMainWindowIface.h>
#include <KexiWindow.h>
#include <KexiView.h>
#include <db/connection.h>

/**
 * Adaptor class that provides Kexi specific functionality to
 * the scripting world.
 */
class KexiScriptAdaptorQ : public QObject
{
    Q_OBJECT
public:
    explicit KexiScriptAdaptorQ();
    virtual ~KexiScriptAdaptorQ();
public Q_SLOTS:
    
    /**
     * Returns the current KexiWindow widget.
     */
    QWidget* windowWidget() const;
    
    /**
     * Returns the current KexiView widget.
     */
    QWidget* viewWidget() const;
    
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
    QVariantList actions();
    
    /**
     * Returns the QAction instance the Kexi main window provides that
     * has the objectName \p name or NULL if there is no such action.
     */
    QObject* action(const QString& name);
    
    /**
     * Returns true if we are connected with a project else false
     * is returned.
     */
    bool isConnected();
    
    #if 0
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
#endif

/**
 * Returns a list of names of all items the part class provides. Possible
 * classes are for example "org.kexi-project.table", "org.kexi-project.query", 
 * "org.kexi-project.form" or "org.kexi-project.script".
 *
 * Python sample that prints all tables within the current project.
 * \code
 * import Kexi
 * print Kexi.items("table")
 * \endcode
 */
QStringList items(const QString& className);

/**
 * Returns the caption for the item defined with \p className and \p name .
 */
QString itemCaption(const QString& className, const QString& name) const;

/**
 * Set the caption for the item defined with \p className and \p name .
 */
void setItemCaption(const QString& className, const QString& name, const QString& caption);

/**
 * Returns the description for the item defined with \p className and \p name .
 */
QString itemDescription(const QString& className, const QString& name) const;

/**
 * Set the description for the item defined with \p className and \p name .
 */
void setItemDescription(const QString& className, const QString& name, const QString& description);

/**
 * Open an item. A window for the item defined with \p className and \p name will
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
bool openItem(const QString& className, const QString& name, const QString& viewmode = QString(), QVariantMap args = QVariantMap());

/**
 * Close an opened item. The window for the item defined with \p className and \p name
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
bool closeItem(const QString& className, const QString& name);

/**
 * Print the item defined with \p className and \p name .
 */
bool printItem(const QString& className, const QString& name, bool preview = false);
/**
 * Executes custom action for the item defined with \p className and \p name .
 */
bool executeItem(const QString& className, const QString& name, const QString& actionName);


/**
 * Returns the name of the current viewmode. This could be for example "data",
 * "design", "text" or just an empty string if there is no view at the moment.
 */
QString viewMode() const;

/**
 * Returns a list of names of all available viewmodes the view supports.
 */
QStringList viewModes() const;

/**
 * Returns true if there is a current view and those current view is dirty aka
 * has the dirty-flag set that indicates that something changed.
 */
bool viewIsDirty() const;

private:
    #if 0
    QObject* m_kexidbmodule;
    #endif
    
    KexiMainWindowIface* mainWindow() const;
    KexiProject* project() const;
    KexiWindow* currentWindow() const;
    KexiView* currentView() const;
    KexiPart::Item* partItem(const QString& className, const QString& name) const;
    QString partClass(const QString& partClass) const;
    QString viewModeToString(Kexi::ViewMode mode, const QString& defaultViewMode = QString()) const;
    Kexi::ViewMode stringToViewMode(const QString& mode, Kexi::ViewMode defaultViewMode = Kexi::DataViewMode) const;
};

#endif

