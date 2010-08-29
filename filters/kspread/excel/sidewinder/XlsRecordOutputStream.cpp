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
#include "XlsRecordOutputStream.h"

#include <QBuffer>
#include <QDebug>

#include "utils.h"

using namespace Swinder;

static const unsigned NORECORD = 0xFFFFFFFF;

XlsRecordOutputStream::XlsRecordOutputStream(QIODevice* device)
    : m_dataStream(device), m_currentRecord(NORECORD), m_buffer(0)
{
    Q_ASSERT(device->isOpen());
    Q_ASSERT(device->isWritable());
    m_dataStream.setByteOrder(QDataStream::LittleEndian);
}

void XlsRecordOutputStream::writeRecord(const Record& record)
{
    startRecord(record.rtti());
    record.writeData(*this);
    endRecord();
}

void XlsRecordOutputStream::startRecord(unsigned recordType)
{
    Q_ASSERT(m_currentRecord == NORECORD);

    m_currentRecord = recordType;
    m_buffer = new QBuffer();
    m_buffer->open(QIODevice::WriteOnly);
    m_curByte = 0;
    m_curBitOffset = 0;
}

void XlsRecordOutputStream::endRecord()
{
    Q_ASSERT(m_currentRecord != NORECORD);
    Q_ASSERT(m_curBitOffset == 0);

    // TODO: add support for larger-than-8224 records
    Q_ASSERT(m_buffer->data().size() <= 8224);

    m_dataStream << quint16(m_currentRecord);
    m_dataStream << quint16(m_buffer->data().size());
    m_dataStream.writeRawData(m_buffer->data().data(), m_buffer->data().size());

    delete m_buffer;

    m_currentRecord = NORECORD;
}

void XlsRecordOutputStream::writeUnsigned(unsigned bits, unsigned value)
{
    Q_ASSERT(m_currentRecord != NORECORD);
    unsigned mask = (unsigned(1) << bits) - 1;
    value &= mask;
    if (m_curBitOffset) {
        if (bits < 8-m_curBitOffset) {
            m_curBitOffset += bits;
            m_curByte |= value << (m_curBitOffset - 1);
            return;
        } else if (bits == 8-m_curBitOffset) {
            m_curBitOffset += bits;
            m_curByte |= value << (m_curBitOffset - 1);
            m_buffer->write(reinterpret_cast<char*>(&m_curByte), 1);
            m_curByte = 0;
            m_curBitOffset = 0;
            return;
        } else {
            unsigned bitsLeft = 8-m_curBitOffset;
            unsigned maskVal = value & ((1 << bitsLeft) - 1);
            m_curByte |= maskVal << 7;
            m_buffer->write(reinterpret_cast<char*>(&m_curByte), 1);
            m_curByte = 0;
            m_curBitOffset = 0;

            bits -= bitsLeft;
            value >>= bitsLeft;
            mask >>= bitsLeft;
        }
    }
    while (bits >= 8) {
        m_buffer->write(reinterpret_cast<char*>(&value), 1);
        bits -= 8;
        value >>= 8;
        mask >>= 8;
    }
    m_curByte = value;
    m_curBitOffset = bits;
}

void XlsRecordOutputStream::writeSigned(unsigned bits, signed value)
{
    writeUnsigned(bits, value);
}

void XlsRecordOutputStream::writeUnicodeString(const QString& value)
{
    QBuffer b;
    b.open(QIODevice::WriteOnly);
    QDataStream ds(&b);
    ds.setByteOrder(QDataStream::LittleEndian);
    const ushort* d = value.utf16();
    while (*d) {
        ds << quint16(*d);
        d++;
    }
    writeBlob(b.data());
}

void XlsRecordOutputStream::writeUnicodeStringWithFlags(const QString& value)
{
    char flags = 1; // just unicode, nothing else
    m_buffer->write(&flags, 1);
    writeUnicodeString(value);
}


void XlsRecordOutputStream::writeByteString(const QString& value)
{
    writeBlob(value.toAscii());
}

void XlsRecordOutputStream::writeBlob(const QByteArray& value)
{
    Q_ASSERT(m_currentRecord != NORECORD);
    Q_ASSERT(m_curBitOffset == 0);

    m_buffer->write(value);
}
