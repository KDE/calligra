/* This file is part of the KDE project
   Copyright (C) 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

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

#ifndef MOBIFILE_H
#define MOBIFILE_H

#include <QObject>
#include "KoFilter.h"
#include "MobiHeaderGenerator.h"
#include "FileCollector.h"

class MobiFile : public FileCollector
{
public:
    MobiFile();

    void addContentRawText(const QByteArray &content);
    void addContentImage(int id, const QByteArray &content);

    KoFilter::ConversionStatus writeMobiFile(const QString &filename, MobiHeaderGenerator &headerGenerator);

private:
    void writeRecord0(QDataStream &out, MobiHeaderGenerator &headers);
    void writePalmDataBaseHeader(QDataStream &out, MobiHeaderGenerator &headers);
    void writeFLISRecord(QDataStream &out, MobiHeaderGenerator &headers);
    void writeFCISRecord(QDataStream &out, MobiHeaderGenerator &headers);
    void writeEndOfFileRecord(QDataStream &out, MobiHeaderGenerator &headers);

private:
    QByteArray m_textContent;
    QHash<int, QByteArray> m_imageContent;
};

#endif // MOBIFILE_H
