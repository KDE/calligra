/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MOBIFILE_H
#define MOBIFILE_H

#include "FileCollector.h"
#include "KoFilter.h"
#include "MobiHeaderGenerator.h"
#include <QObject>

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
