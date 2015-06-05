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

#include <QDataStream>
#include <QStringList>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QDebug>

bool KexiFieldDrag::canDecode(QMimeSource *e)
{
    return e->provides("kexi/fields");
}

bool KexiFieldDrag::decode(QDropEvent* e, QString *sourceMimeType,
                           QString *sourceName, QStringList *fields)
{
    Q_ASSERT(sourceMimeType);
    Q_ASSERT(sourceName);
    Q_ASSERT(fields);

    QByteArray payload(e->encodedData("kexi/fields"));
    if (payload.isEmpty()) {//try single
        return false;
    }
    e->accept();
    QDataStream stream1(&payload, QIODevice::ReadOnly);

    stream1 >> *sourceMimeType;
    stream1 >> *sourceName;
    stream1 >> *fields;
// qDebug() << "decoded:" << sourceMimeType<<"/"<<sourceName<<"/"<<fields;
    return true;
}

// ----------

KexiDataProviderDrag::KexiDataProviderDrag(const QString& sourceMimeType, const QString& sourceName,
        QWidget *parent)
        : QDrag(parent)
{
    QMimeData *mimedata = new QMimeData();
    QByteArray data;
    QDataStream stream1(&data, QIODevice::WriteOnly);

    stream1 << sourceMimeType << sourceName;
    mimedata->setData("kexi/dataprovider", data);
    setMimeData(mimedata);
}

KexiDataProviderDrag::~KexiDataProviderDrag()
{
}

bool KexiDataProviderDrag::canDecode(QDragMoveEvent *e)
{
    return e->provides("kexi/dataprovider");
}

bool KexiDataProviderDrag::decode(QDropEvent* e, QString* sourceMimeType, QString *sourceName)
{
    Q_ASSERT(sourceMimeType);
    Q_ASSERT(sourceName);

    QByteArray payload = e->encodedData("kexidataprovider");
    if (payload.isEmpty()) {
        return false;
    }
    e->accept();
    QDataStream stream1(&payload, QIODevice::ReadOnly);
    stream1 >> *sourceMimeType;
    stream1 >> *sourceName;
//  qDebug() << "decoded:" << sourceMimeType <<"/"<<sourceName;
    return true;
}
