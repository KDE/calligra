/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright     2007       David Faure <faure@kde.org>

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

#include "KoDocumentEntry.h"

#include "KoPart.h"
#include "KoDocument.h"
#include "KoFilter.h"

#include <kservicetype.h>
#include <kpluginfactory.h>

#include <kdebug.h>
#include <KoJsonTrader.h>

#include <QPluginLoader>

#include <limits.h> // UINT_MAX

KoDocumentEntry::KoDocumentEntry()
        : m_loader(0)
{
}

KoDocumentEntry::KoDocumentEntry(QPluginLoader *loader)
        : m_loader(loader)
{
}

KoDocumentEntry::~KoDocumentEntry()
{
}

/**
 *  @return TRUE if the document can handle the requested mimetype.
 */
bool KoDocumentEntry::supportsMimeType(const QString & _mimetype) const {
    return mimeTypes().contains(_mimetype);
}

KoPart *KoDocumentEntry::createKoPart(QString* errorMsg) const
{
    QString error;
    QObject *obj = m_loader->instance();
    KPluginFactory *factory = qobject_cast<KPluginFactory *>(obj);
    KoPart *part = factory->create<KoPart>(0, QVariantList());

    if (!part) {
        if (errorMsg)
            *errorMsg = m_loader->errorString();
        return 0;
    }

    return part;
}

KoDocumentEntry KoDocumentEntry::queryByMimeType(const QString &mimetype)
{
    QList<KoDocumentEntry> vec = queryAllByMimeType(mimetype);
    if (vec.isEmpty()) {
        // Still no match. Either the mimetype itself is unknown, or we have no service for it.
        // Help the user debugging stuff by providing some more diagnostics
        if (KServiceType::serviceType(mimetype).isNull()) {
            kError(30003) << "Unknown Calligra MimeType " << mimetype << "." << endl;
            kError(30003) << "Check your installation (for instance, run 'kde4-config --path mime' and check the result)." << endl;
        } else {
            kError(30003) << "Found no Calligra part able to handle " << mimetype << "!" << endl;
            kError(30003) << "Check your installation (does the desktop file have X-KDE-NativeMimeType and Calligra/Part, did you install Calligra in a different prefix than KDE, without adding the prefix to /etc/kderc ?)" << endl;
        }
        return KoDocumentEntry();
    }

    return KoDocumentEntry(vec[0]);
}

QList<KoDocumentEntry> KoDocumentEntry::queryAllByMimeType(const QString &mimetype)
{

    QList<KoDocumentEntry> lst;
    // Query the trader
    const QList<QPluginLoader *> offers = KoJsonTrader::self()->query("Calligra/Part", mimetype);

    QList<QPluginLoader *>::ConstIterator it = offers.begin();
    unsigned int max = offers.count();
    for (unsigned int i = 0; i < max; i++, ++it) {
        lst.append(KoDocumentEntry(*it));
    }

    return lst;
}
