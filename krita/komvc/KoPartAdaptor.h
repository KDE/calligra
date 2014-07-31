/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>
   Copyright (C) 2006 Fredrik Edemar <f_edemar@linux.se>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KOPARTADAPTOR_H
#define KOPARTADAPTOR_H

#ifndef QT_NO_DBUS

#include <QMap>

#include <QObject>
#include <QDBusAbstractAdaptor>
#include <QList>
#include "komvc_export.h"
class KoPart;

/**
 * DBUS interface for any Calligra document
 * Use KoApplicationIface to get hold of an existing document's interface,
 * or to create a document.
 */
class KOMVC_EXPORT KoPartAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.calligra.part")
public:

    explicit KoPartAdaptor(KoPart* part);
    ~KoPartAdaptor();

public slots: // METHODS


    /**
     * @return the number of documents this part managers
     */
    Q_SCRIPTABLE int documentCount();

    /**
     * @return a representing the document with index @p idx
     */
    Q_SCRIPTABLE QString document(int idx);


    /**
     * @return the number of views part manages
     */
    Q_SCRIPTABLE int viewCount();

    /**
     * @return a representing the view with index @p idx
     */
    Q_SCRIPTABLE QString view(int idx);

protected:
    KoPart* m_part;
};

#endif // QT_NO_DBUS

#endif

