/* This file is part of the KDE project

   SPDX-FileCopyrightText: 1999-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_MAP_ADAPTOR
#define CALLIGRA_SHEETS_MAP_ADAPTOR

#ifdef WITH_QTDBUS
#include <QStringList>

#include "sheets_part_export.h"
#include <QDBusAbstractAdaptor>

namespace Calligra
{
namespace Sheets
{
class Map;

/**
 * The MapAdaptor class provides additional functionality to deal with sheets.
 */
class CALLIGRA_SHEETS_PART_EXPORT MapAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.calligra.spreadsheet.map")
public:
    explicit MapAdaptor(Map *);

    //     virtual bool processDynamic(const DCOPCString &fun, const QByteArray &data,
    //     DCOPCString& replyType, QByteArray &replyData);

public Q_SLOTS: // METHODS

    /** Return the objectName of the sheet with the defined sheet name. */
    virtual QString sheet(const QString &name);
    /** Return the objectName of the sheet with the defined index. */
    virtual QString sheetByIndex(int index);
    /** Return the number of available sheets. */
    virtual int sheetCount() const;
    /** Return a list of available sheet names. */
    virtual QStringList sheetNames() const;
    /** Return a list of available sheet objectName's. */
    virtual QStringList sheets();
    /** Insert a new sheet with the defined sheet name. */
    virtual QString insertSheet(const QString &name);

private:
    Map *m_map;
};

} // namespace Sheets
} // namespace Calligra
#endif // WITH_QTDBUS

#endif
