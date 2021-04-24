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

#include "SheetsDebug.h"
#include "Map.h"
#include "Sheet.h"

using namespace Calligra::Sheets;

MapAdaptor::MapAdaptor(Map* map)
        : QDBusAbstractAdaptor(map)
{
    setAutoRelaySignals(true);
    m_map = map;
}

QString MapAdaptor::sheet(const QString& name)
{
    Sheet* t = m_map->findSheet(name);
    if (!t)
        return QString();

    return t->objectName();
}

QString MapAdaptor::sheetByIndex(int index)
{
    Sheet* t = m_map->sheetList().at(index);
    if (!t) {
        debugSheets << "+++++ No table found at index" << index;
        return QString();
    }

    debugSheets << "+++++++ Returning table" << t->QObject::objectName();

    return t->objectName();
}

int MapAdaptor::sheetCount() const
{
    return m_map->count();
}

QStringList MapAdaptor::sheetNames() const
{
    QStringList names;
    foreach(Sheet* sheet, m_map->sheetList()) {
        names.append(sheet->objectName());
    }
    return names;
}

QStringList MapAdaptor::sheets()
{
    QStringList t;
    foreach(Sheet* sheet, m_map->sheetList()) {
        t.append(sheet->objectName());
    }
    return t;
}

QString MapAdaptor::insertSheet(const QString& name)
{
    if (m_map->findSheet(name))
        return sheet(name);

    Sheet* t = m_map->addNewSheet();
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
