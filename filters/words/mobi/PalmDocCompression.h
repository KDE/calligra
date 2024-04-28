/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PALMDOCCOMPRESSION_H
#define PALMDOCCOMPRESSION_H

#include <QObject>

class QByteArray;

class PalmDocCompression
{
public:
    PalmDocCompression();
    ~PalmDocCompression();

    void compressContent(QByteArray input, QByteArray &output, QList<qint32> &recordOffset);

private:
    void startCompressing(QByteArray input, QDataStream &out, QList<qint32> &recordOffset);

private:
    const int m_winSize;
    const int m_buffSize;
    const int m_maxBlockSize;
};

#endif // PALMDOCCOMPRESSION_H
