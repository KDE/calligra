/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2006 Fredrik Edemar <f_edemar@linux.se>

   SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef __KoApplicationAdaptor_h__
#define __KoApplicationAdaptor_h__

#ifndef QT_NO_DBUS

#include <QDBusAbstractAdaptor>
#include "komain_export.h"
template<class T> class QList;
template<class Key, class Value> class QMap;

class QString;
class KoApplication;

/**
 * D-BUS interface for any Calligra application (entry point)
 */
class KOMAIN_EXPORT KoApplicationAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.calligra.application")
public:
    explicit KoApplicationAdaptor(KoApplication *parent);
    ~KoApplicationAdaptor() override;

public Q_SLOTS: // METHODS
//    /**
//     * Creates a new document for the given native mimetype
//     * Use it to create a main window and to load an existing file, if any
//     */
//    Q_SCRIPTABLE QString createDocument(const QString &nativeFormat);

    /**
     * @return a list of references to all the documents
     * (see KoDocumentIface)
     */
    Q_SCRIPTABLE QStringList getDocuments();

    /**
     * @return a list of references to all the views
     * (see KoViewIface)
     * Convenience method to avoid iterating over all documents to get all the views.
     */
    Q_SCRIPTABLE QStringList getViews();

    /**
     * @return a list of references to all the windows
     * (see KoMainWindowIface)
     */
    Q_SCRIPTABLE QStringList getWindows();

Q_SIGNALS:
    /**
     * emitted when a new document is opened.
     */
    void documentOpened(const QString &ref);

    /**
     * emitted when an old document is closed.
     */
    void documentClosed(const QString &ref);
private:

    KoApplication *m_application;
};

#endif // QT_NO_DBUS

#endif

