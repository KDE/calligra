/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2005 Jarosław Staniek <staniek@kde.org>

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

#include "kexidragobjects.h"

#include <q3cstring.h>
#include <qdatastream.h>
//Added by qt3to4:
#include <QDragMoveEvent>
#include <QDropEvent>
#include <kdebug.h>

/// implementation of KexiFieldDrag

KexiFieldDrag::KexiFieldDrag(const QString& sourceMimeType, const QString& sourceName,
                             const QString& field, QWidget *parent, const char *name)
        : Q3StoredDrag("kexi/field", parent, name)
{
    QByteArray data;
    QDataStream stream1(&data, QIODevice::WriteOnly);
//    stream1.setVersion(QDataStream::Qt_3_1);
    stream1 << sourceMimeType << sourceName << field;
    setEncodedData(data);
}

KexiFieldDrag::KexiFieldDrag(const QString& sourceMimeType, const QString& sourceName,
                             const QStringList& fields, QWidget *parent, const char *name)
        : Q3StoredDrag((fields.count() > 1) ? "kexi/fields" : "kexi/field", parent, name)
{
    QByteArray data;
    QDataStream stream1(&data, QIODevice::WriteOnly);
    //stream1.setVersion(QDataStream::Qt_3_1);
    if (fields.count() > 1)
        stream1 << sourceMimeType << sourceName << fields;
    else {
        QString field;
        if (fields.count() == 1)
            field = fields.first();
        else
            kDebug() << "fields list is empty!";
        stream1 << sourceMimeType << sourceName << field;
    }
    setEncodedData(data);
}

KexiFieldDrag::~KexiFieldDrag()
{
}

bool
KexiFieldDrag::canDecodeSingle(QMimeSource *e)
{
    return e->provides("kexi/field");
}

bool
KexiFieldDrag::canDecodeMultiple(QMimeSource *e)
{
    return e->provides("kexi/field") || e->provides("kexi/fields");
}

bool
KexiFieldDrag::decodeSingle(QDropEvent* e, QString& sourceMimeType,
                            QString& sourceName, QString& field)
{
    QByteArray payload(e->encodedData("kexi/field"));
    if (payload.isEmpty())
        return false;
    e->accept();
    QDataStream stream1(&payload, QIODevice::ReadOnly);
//    stream1.setVersion(QDataStream::Qt_3_1);
    stream1 >> sourceMimeType;
    stream1 >> sourceName;
    stream1 >> field;
// kDebug() << sourceMimeType<<"/"<<sourceName<<"/"<<field;
    return true;
}

bool
KexiFieldDrag::decodeMultiple(QDropEvent* e, QString& sourceMimeType,
                              QString& sourceName, QStringList& fields)
{
    QByteArray payload(e->encodedData("kexi/fields"));
    if (payload.isEmpty()) {//try single
        QString field;
        bool res = KexiFieldDrag::decodeSingle(e, sourceMimeType, sourceName, field);
        if (!res)
            return false;
        fields.append(field);
        return true;
    }
    e->accept();
    QDataStream stream1(&payload, QIODevice::ReadOnly);
//    stream1.setVersion(QDataStream::Qt_3_1);
    stream1 >> sourceMimeType;
    stream1 >> sourceName;
    stream1 >> fields;
// kDebug() << "decoded:" << sourceMimeType<<"/"<<sourceName<<"/"<<fields;
    return true;
}

/// implementation of KexiDataProviderDrag

KexiDataProviderDrag::KexiDataProviderDrag(const QString& sourceMimeType, const QString& sourceName,
        QWidget *parent, const char *name)
        : Q3StoredDrag("kexi/dataprovider", parent, name)
{
    QByteArray data;
    QDataStream stream1(&data, QIODevice::WriteOnly);
//    stream1.setVersion(QDataStream::Qt_3_1);
    stream1 << sourceMimeType << sourceName;
    setEncodedData(data);
}


bool
KexiDataProviderDrag::canDecode(QDragMoveEvent *e)
{
    return e->provides("kexi/dataprovider");
}

bool
KexiDataProviderDrag::decode(QDropEvent* e, QString& sourceMimeType, QString& sourceName)
{
    Q3CString tmp;
    QByteArray payload = e->encodedData("kexidataprovider");
    if (payload.size()) {
        e->accept();
        QDataStream stream1(&payload, QIODevice::ReadOnly);
//        stream1.setVersion(QDataStream::Qt_3_1);
        stream1 >> sourceMimeType;
        stream1 >> sourceName;
//  kDebug() << "decoded:" << sourceMimeType <<"/"<<sourceName;
        return true;
    }
    return false;
}
