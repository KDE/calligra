/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2002 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2001 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2001 Philipp Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 1999-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "MapAdaptor.h"

#include "core/Map.h"
#include "core/Sheet.h"
#include "engine/SheetsDebug.h"

using namespace Calligra::Sheets;

MapAdaptor::MapAdaptor(Map *map)
    : QDBusAbstractAdaptor(map)
{
    setAutoRelaySignals(true);
    m_map = map;
}

QString MapAdaptor::sheet(const QString &name)
{
    SheetBase *t = m_map->findSheet(name);
    if (!t)
        return QString();

    Sheet *sheet = dynamic_cast<Sheet *>(t);
    return sheet ? sheet->objectName() : QString();
}

QString MapAdaptor::sheetByIndex(int index)
{
    SheetBase *t = m_map->sheetList().at(index);
    if (!t) {
        debugSheets << "+++++ No table found at index" << index;
        return QString();
    }

    Sheet *sheet = dynamic_cast<Sheet *>(t);
    QString res = sheet ? sheet->objectName() : QString();
    debugSheets << "+++++++ Returning table" << res;
    return res;
}

int MapAdaptor::sheetCount() const
{
    return m_map->count();
}

QStringList MapAdaptor::sheetNames() const
{
    QStringList names;
    for (SheetBase *bsheet : m_map->sheetList()) {
        Sheet *sheet = dynamic_cast<Sheet *>(bsheet);
        names.append(sheet->objectName());
    }
    return names;
}

QStringList MapAdaptor::sheets()
{
    QStringList t;
    for (SheetBase *bsheet : m_map->sheetList()) {
        Sheet *sheet = dynamic_cast<Sheet *>(bsheet);
        t.append(sheet->objectName());
    }
    return t;
}

QString MapAdaptor::insertSheet(const QString &name)
{
    if (m_map->findSheet(name))
        return sheet(name);

    SheetBase *t = m_map->addNewSheet();
    t->setSheetName(name);

    return sheet(name);
}

// bool MapAdaptor::processDynamic(const DCOPCString &fun, const QByteArray &/*data*/,
//                                      DCOPCString& replyType, QByteArray &replyData)
// {
//     // Does the name follow the pattern "foobar()" ?
//     uint len = fun.length();
//     if ( len < 3 )
//         return false;
//
//     if ( fun[ len - 1 ] != ')' || fun[ len - 2 ] != '(' )
//         return false;
//
//     Sheet* t = m_map->findSheet( fun.left( len - 2 ).data() );
//     if ( !t )
//         return false;
//
//     replyType = "DCOPRef";
//     QDataStream out( &replyData,QIODevice::WriteOnly );
//     out.setVersion(QDataStream::Qt_3_1);
//     out << DCOPRef( kapp->dcopClient()->appId(), t->dcopObject()->objId() );
//     return true;
// }
