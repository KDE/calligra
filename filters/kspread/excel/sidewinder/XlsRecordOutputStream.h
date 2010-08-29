/* This file is part of the KDE project
   Copyright (C) 2010 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

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

#ifndef XLSRECORDOUTPUTSTREAM_H
#define XLSRECORDOUTPUTSTREAM_H

#include <QDataStream>

class QBuffer;
class QIODevice;

namespace Swinder {

class Record;

class XlsRecordOutputStream
{
public:
    XlsRecordOutputStream(QIODevice* device);

    /**
     * Writes a record, updates the records position to the position it is saved to in the file.
     */
    void writeRecord(Record& record);
    void writeRecord(const Record& record);
    /**
     * Rewrites the given record at its recordPosition by first seeking there, and then going back to where the output device was first.
     */
    void rewriteRecord(const Record& record);

    void startRecord(unsigned recordType);
    void endRecord();

    void writeUnsigned(unsigned bits, unsigned value);
    void writeSigned(unsigned bits, signed value);
    void writeFloat(unsigned bits, double value);
    void writeUnicodeString(const QString& value);
    void writeUnicodeStringWithFlags(const QString& value);
    void writeUnicodeStringWithFlagsAndLength(const QString& value);
    void writeByteString(const QString& value);
    void writeBlob(const QByteArray& value);

    qint64 pos() const;
    qint64 recordPos() const;
private:
    QDataStream m_dataStream;
    unsigned m_currentRecord;
    QBuffer* m_buffer;
    unsigned char m_curByte;
    unsigned m_curBitOffset;
};

} // namespace Swinder

#endif // XLSRECORDOUTPUTSTREAM_H
