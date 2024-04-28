/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef XLSRECORDOUTPUTSTREAM_H
#define XLSRECORDOUTPUTSTREAM_H

#include <QDataStream>

class QBuffer;
class QIODevice;

namespace Swinder
{

class Record;

class XlsRecordOutputStream
{
public:
    explicit XlsRecordOutputStream(QIODevice *device);

    /**
     * Writes a record, updates the records position to the position it is saved to in the file.
     */
    void writeRecord(Record &record);
    void writeRecord(const Record &record);
    /**
     * Rewrites the given record at its recordPosition by first seeking there, and then going back to where the output device was first.
     */
    void rewriteRecord(const Record &record);

    void startRecord(unsigned recordType);
    void endRecord();

    void writeUnsigned(unsigned bits, unsigned value);
    void writeSigned(unsigned bits, signed value);
    void writeFloat(unsigned bits, double value);
    void writeUnicodeString(const QString &value);
    void writeUnicodeStringWithFlags(const QString &value);
    void writeUnicodeStringWithFlagsAndLength(const QString &value);
    void writeByteString(const QString &value);
    void writeBlob(const QByteArray &value);

    qint64 pos() const;
    qint64 recordPos() const;

private:
    QDataStream m_dataStream;
    unsigned m_currentRecord;
    QBuffer *m_buffer;
    unsigned char m_curByte;
    unsigned m_curBitOffset;
};

} // namespace Swinder

#endif // XLSRECORDOUTPUTSTREAM_H
