/* This file is part of the KDE project

   Copyright 1999-2000 Torben Weis <weis@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_MAP_ADAPTOR
#define KSPREAD_MAP_ADAPTOR

#include <QStringList>

#include <QtDBus/QtDBus>
#include "kspread_export.h"

namespace KSpread
{
class Map;

/**
* The MapAdaptor class provides additional functionality to deal with sheets.
*/
class KSPREAD_EXPORT MapAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.koffice.spreadsheet.map")
public:
    explicit MapAdaptor(Map*);

//     virtual bool processDynamic(const DCOPCString &fun, const QByteArray &data,
//     DCOPCString& replyType, QByteArray &replyData);

public Q_SLOTS: // METHODS

    /** Return the objectName of the sheet with the defined sheet name. */
    virtual QString sheet(const QString& name);
    /** Return the objectName of the sheet with the defined index. */
    virtual QString sheetByIndex(int index);
    /** Return the number of available sheets. */
    virtual int sheetCount() const;
    /** Return a list of available sheet names. */
    virtual QStringList sheetNames() const;
    /** Return a list of available sheet objectName's. */
    virtual QStringList sheets();
    /** Insert a new sheet with the defined sheet name. */
    virtual QString insertSheet(const QString& name);

private:
    Map* m_map;
};

} // namespace KSpread

#endif
