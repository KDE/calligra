/* This file is part of the KDE project
 * Copyright (C) 2015-2016 MultiRacio Ltd. <multiracio@multiracio.com> (S.Schliszka, F.Novak, P.Rakyta)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "ExtractFile.h"

#include <QtCore/QFile>

bool copyTo(const QString &dest, const QString &name, const KZipFileEntry * zipEntry)
{
    QFile f(dest + QLatin1Char('/')  + name);
    if (f.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        QIODevice *inputDev = zipEntry->createDevice();

        // Read and write data in chunks to minimize memory usage
        const qint64 chunkSize = 1024 * 1024;
        qint64 remainingSize = zipEntry->size();
        QByteArray array;
        array.resize(int(qMin(chunkSize, remainingSize)));

        while (remainingSize > 0) {
            const qint64 currentChunkSize = qMin(chunkSize, remainingSize);
            const qint64 n = inputDev->read(array.data(), currentChunkSize);
            Q_UNUSED(n) // except in Q_ASSERT
            Q_ASSERT(n == currentChunkSize);
            f.write(array.data(), currentChunkSize);
            remainingSize -= currentChunkSize;
        }
        f.close();

        delete inputDev;
        return true;
    }
    return false;
}
