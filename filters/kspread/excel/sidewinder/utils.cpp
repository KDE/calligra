/* Swinder - Portable library for spreadsheet
   Copyright (C) 2003-2005 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2006,2009 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
   Copyright (C) 2009,2010 Sebastian Sauer <sebsauer@kdab.com>

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
   Boston, MA 02110-1301, USA
 */
#include "utils.h"
#include <string.h>
#include <iomanip>

namespace Swinder
{

QString readByteString(const void* p, unsigned length, unsigned maxSize, bool* error, unsigned* size)
{
    const unsigned char* data = reinterpret_cast<const unsigned char*>(p);

    if (size) *size = length;
    if (length > maxSize) {
        if (*error) *error = true;
        return QString::null;
    }

    char* buffer = new char[length+1];
    memcpy(buffer, data, length);
    buffer[length] = 0;
    QString str(buffer);
    delete[] buffer;

    return str;
}

QString readTerminatedUnicodeChars(const void* p, unsigned* pSize, unsigned maxSize, bool* error)
{
    const unsigned char* data = reinterpret_cast<const unsigned char*>(p);

    QString str;
    unsigned offset = 0;
    unsigned size = offset;
    while (true) {
        if (size+2 > maxSize) {
            if (*error) *error = true;
            return QString::null;
        }
        unsigned uchar = readU16(data + offset);
        size += 2;
        if (uchar == '\0') break;
        offset += 2;
        str.append(QChar(uchar));
    }

    if (pSize) *pSize = size;
    return str;
}

QString readUnicodeChars(const void* p, unsigned length, unsigned maxSize, bool* error, unsigned* pSize, unsigned continuePosition, unsigned offset, bool unicode, bool asianPhonetics, bool richText)
{
    const unsigned char* data = reinterpret_cast<const unsigned char*>(p);

    if (maxSize < 1) {
        if (*error) *error = true;
        return QString::null;
    }

    unsigned formatRuns = 0;
    unsigned asianPhoneticsSize = 0;

    if (richText) {
        if (offset + 2 > maxSize) {
            if (*error) *error = true;
            return QString::null;
        }
        formatRuns = readU16(data + offset);
        offset += 2;
    }

    if (asianPhonetics) {
        if (offset + 4 > maxSize) {
            if (*error) *error = true;
            return QString::null;
        }
        asianPhoneticsSize = readU32(data + offset);
        offset += 4;
    }

    // find out total bytes used in this string
    unsigned size = offset;
    if (richText) size += (formatRuns * 4);
    if (asianPhonetics) size += asianPhoneticsSize;
    if (size > maxSize) {
        if (*error) *error = true;
        return QString::null;
    }
    QString str;
    for (unsigned k = 0; k < length; k++) {
        unsigned uchar;
        if (unicode) {
            if (size + 2 > maxSize) {
                if (*error) *error = true;
                return QString::null;
            }
            uchar = readU16(data + offset);
            offset += 2;
            size += 2;
        } else {
            if (size + 1 > maxSize) {
                if (*error) *error = true;
                return QString::null;
            }
            uchar = data[offset++];
            size++;
        }
        str.append(QChar(uchar));
        if (offset == continuePosition && k < length - 1) {
            if (size + 1 > maxSize) {
                if (*error) *error = true;
                return QString::null;
            }
            unicode = data[offset] & 1;
            size++;
            offset++;
        }
    }

    if (pSize) *pSize = size;
    return str;
}

QString readUnicodeString(const void* p, unsigned length, unsigned maxSize, bool* error, unsigned* pSize, unsigned continuePosition)
{
    const unsigned char* data = reinterpret_cast<const unsigned char*>(p);

    if (maxSize < 1) {
        if (*error) *error = true;
        return QString::null;
    }

    unsigned char flags = data[0];
    unsigned offset = 1;
    bool unicode = flags & 0x01;
    bool asianPhonetics = flags & 0x04;
    bool richText = flags & 0x08;

    return readUnicodeChars(p, length, maxSize, error, pSize, continuePosition, offset, unicode, asianPhonetics, richText);
}

QString readUnicodeCharArray(const void* p, unsigned length, unsigned maxSize, bool* error, unsigned* pSize, unsigned continuePosition)
{
    if (length == unsigned(-1)) { // null terminated string
        return readTerminatedUnicodeChars(p, pSize, maxSize, error);
    } else {
        return readUnicodeChars(p, length, maxSize, error, pSize, continuePosition, 0, true, false, false);
    }
}

std::ostream& operator<<(std::ostream& s, const QString& ustring)
{
    s << qPrintable(ustring);
    return s;
}

std::ostream& operator<<(std::ostream& s, const QByteArray& d)
{
    s << std::hex << std::setfill('0');
    for (int i = 0; i < d.size(); i++) s << " " << std::setw(2) << int((unsigned char)d[i]);
    return s;
}

std::ostream& operator<<(std::ostream& s, const QUuid& uuid)
{
    return s << uuid.toString().toAscii().data();
}

Value errorAsValue(int errorCode)
{
    Value result(Value::Error);

    switch (errorCode) {
    case 0x00: result = Value::errorNULL();  break;
    case 0x07: result = Value::errorDIV0();  break;
    case 0x0f: result = Value::errorVALUE(); break;
    case 0x17: result = Value::errorREF();   break;
    case 0x1d: result = Value::errorNAME();  break;
    case 0x24: result = Value::errorNUM();   break;
    case 0x2A: result = Value::errorNA();    break;
    default: break;
    }

    return result;
}

// ========== base record ==========

const unsigned int Record::id = 0; // invalid of-course

Record::Record(Workbook *book)
{
    m_workbook = book;
    stream_position = 0;
    ver = Excel97;
    valid = true;
    m_size = 0;
}

Record::~Record()
{
}

Record* Record::create(unsigned type, Workbook *book)
{
    return RecordRegistry::createRecord(type, book);
}

void Record::setPosition(unsigned pos)
{
    stream_position = pos;
}

unsigned Record::position() const
{
    return stream_position;
}

void Record::setData(unsigned, const unsigned char*, const unsigned int*)
{
}

void Record::writeData(XlsRecordOutputStream &out) const
{
    Q_UNUSED(out);
    fprintf(stderr, "ERROR! writeData not implemented for record type %u\n", rtti());
}

void Record::dump(std::ostream&) const
{
    // nothing to dump
}

bool Record::isValid() const
{
    return valid;
}

void Record::setIsValid(bool isValid)
{
    valid = isValid;
}

void RecordRegistry::registerRecordClass(unsigned id, RecordFactory factory)
{
    instance()->records[id] = factory;
}

void RecordRegistry::registerRecordClass(unsigned id, RecordFactoryWithArgs factory, void* args)
{
    instance()->recordsWithArgs[id] = factory;
    instance()->recordArgs[id] = args;
}

void RecordRegistry::unregisterRecordClass(unsigned id)
{
    instance()->records.erase(id);
    instance()->recordsWithArgs.erase(id);
    instance()->recordArgs.erase(id);
}

Record* RecordRegistry::createRecord(unsigned id, Workbook *book)
{
    RecordRegistry* q = instance();

    std::map<unsigned, RecordFactory>::iterator it = q->records.find(id);
    if (it != q->records.end()) {
        return it->second(book);
    }

    std::map<unsigned, RecordFactoryWithArgs>::iterator it2 = q->recordsWithArgs.find(id);
    if (it2 != q->recordsWithArgs.end()) {
        return it2->second(book,  q->recordArgs[id]);
    }

    return 0;
}

RecordRegistry* RecordRegistry::instance()
{
    static RecordRegistry* sinstance = 0;
    if (!sinstance) sinstance = new RecordRegistry();
    return sinstance;
}

} // namespace Swinder
