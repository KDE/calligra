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

#include <kexi.h>
#include <kexipart.h>
#include <kexiproject.h>
#include <KexiMainWindowIface.h>
#include <KexiWindow.h>
//#include "../kexidb/kexidbconnection.h"

/**
* Adaptor class that provides Kexi specific functionality to
* the scripting world.
*/
class KexiScriptAdaptor : public QObject
{
        Q_OBJECT
    public:
        explicit KexiScriptAdaptor() { setObjectName("Kexi"); }
        virtual ~KexiScriptAdaptor() {}
    public Q_SLOTS:

        /**
        * Returns a list of all QAction instances the Kexi main
        * window provides.
        */
        QVariantList actions() {
            if( m_actions.count() == 0 ) {
                foreach(QAction* action, mainWindow()->allActions()) {
                    QVariant v;
                    v.setValue( (QObject*) action );
                    m_actions << v;
                }
            }
            return m_actions;
        }

        /**
        * Returns true if we are connected with a project else false
        * is returned.
        */
        bool isConnected() {
            return project() ? project()->isConnected() : false;
        }

#if 0
        /**
        * Returns the KexiDBConnection object that belongs to the opened
        * project or return NULL if there was no project opened (no
        * connection established).
        */
        QObject* getConnection() {
            //TODO
            //::KexiDB::Connection* connection = project() ? project()->dbConnection() : 0;
            //return connection ? new Scripting::KexiDBConnection(connection) : 0;
            return 0;
        }
#endif

        /**
        * Returns a list of names of all items the mimetype provides. Possible
        * mimetypes are for example "table", "query", "form" or "script".
        */
        QStringList items(QString mimetype) {
            QStringList list;
            if( project() ) {
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
            if( KexiPart::Item *item = partItem(mimeType(mimetype), name) )
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
            if( KexiPart::Item *item = partItem(mimeType(mimetype), name) )
                item->setDescription(description);
        }

        /**
        * Open an item. A window for the item defined with \p mimetype and \p name will
        * be opened and we switch to it.
        */
        bool openItem(const QString& mimetype, const QString& name) {
            bool openingCancelled;
            KexiPart::Item *item = partItem(mimeType(mimetype), name);
            KexiWindow* window = item ? mainWindow()->openObject(item, Kexi::DataViewMode, openingCancelled) : 0;
            return (window && ! openingCancelled);
        }

        /**
        * Close an opened item. The window for the item defined with \p mimetype and \p name
        * will be closed.
        */
        bool closeItem(const QString& mimetype, const QString& name) {
            if( KexiPart::Item *item = partItem(mimeType(mimetype), name) )
                return mainWindow()->closeObject(item) == true;
            return false;
        }

        /**
        * Print the item defined with \p mimetype and \p name .
        */
        bool printItem(const QString& mimetype, const QString& name, bool preview = false) {
            if( KexiPart::Item *item = partItem(mimeType(mimetype), name) )
                return (preview ? mainWindow()->printPreviewForItem(item) : mainWindow()->printItem(item)) == true;
            return false;
        }

        /**
        * Executes custom action for the item defined with \p mimetype and \p name .
        */
        bool executeItemAction(const QString& mimetype, const QString& name, const QString& actionName) {
            if( KexiPart::Item *item = partItem(mimeType(mimetype), name) )
                return mainWindow()->executeCustomActionForObject(item, actionName) == true;
            return false;
        }

    private:
        QVariantList m_actions;

        KexiMainWindowIface* mainWindow() const {
            return KexiMainWindowIface::global();
        }
        KexiProject* project() const {
            Q_ASSERT(mainWindow());
            return mainWindow()->project();
        }
        KexiWindow* currentWindow() const {
            Q_ASSERT(mainWindow());
            return mainWindow()->currentWindow();
        }
        KexiPart::Item* partItem(const QString& mimetype, const QString& name) const {
            return project() ? project()->itemForMimeType(mimetype, name) : 0;
        }
        QString mimeType(QString mimetype) const {
            return mimetype.startsWith("kexi/") ? mimetype : mimetype.prepend("kexi/");
        }
};

#endif

