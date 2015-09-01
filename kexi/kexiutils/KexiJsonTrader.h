/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 2007 David Faure <faure@kde.org>
   Copyright (C) 2015 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXI_JSONTRADER_H
#define KEXI_JSONTRADER_H

#include "kexiutils_export.h"

#include <QList>
#include <QString>

class QPluginLoader;
class QJsonObject;

/**
 *  Support class to fetch a list of relevant plugins
 */
class KEXIUTILS_EXPORT KexiJsonTrader
{
public:
    //! Creates instance of the trader.
    //! @a subDir is a name of subdirectory in which plugin files of given type are stored, e.g. "kexi".
    explicit KexiJsonTrader(const QString& subDir);

    ~KexiJsonTrader();

    /**
     * The main function in the KexiJsonTrader class.
     *
     * It will return a list of QPluginLoader objects that match your
     * specifications.  The only required parameter is the @a servicetypes.
     * The @a mimetype parameter is used to limit the possible choices
     * returned based on the constraints you give it.
     *
     * The keys used in the query (Type, ServiceType, Exec) are all
     * fields found in the .desktop files.
     *
     * @param servicetypes A list of service types like 'KMyApp/Plugin' or 'KFilePlugin'.
     *                     At least one has to be found in a plugin.
     * @param mimetype    A mimetype constraint to limit the choices returned, QString() to
     *                    get all services of the given @p servicetypes.
     *
     * @return A list of QPluginLoader that satisfy the query
     * @see http://techbase.kde.org/Development/Tutorials/Services/Traders#The_KTrader_Query_Language
     */
     QList<QPluginLoader *> query(const QStringList &servicetypes, const QString &mimetype = QString());

     /**
      * @overload QList<QPluginLoader *> query(const QStringList &, const QString &);
      */
     QList<QPluginLoader *> query(const QString &servicetype, const QString &mimetype = QString());

     /**
      * @return root json object for @a pluginLoader
      */
     static QJsonObject rootObjectForPluginLoader(const QPluginLoader &pluginLoader);

private:
     Q_DISABLE_COPY(KexiJsonTrader)
     class Private;
     Private * const d;
};

#endif
