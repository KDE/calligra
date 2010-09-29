/* This file is part of the KDE project
   Copyright (C) 2009,2010 KO GmbH <jos.van.den.oever@kogmbh.com>

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
#ifndef LEINPUTSTREAM_H
#define LEINPUTSTREAM_H

#include <QtCore/QIODevice>
#include <QtCore/QDataStream>
#include <QtCore/QDebug>
#include <exception>

class IOException : public std::exception {
public:
    const QString msg;
    IOException() {}
    IOException(const QString& m) :msg(m) {}
    ~IOException() throw() {}
};

class IncorrectValueException : public IOException {
public:
    IncorrectValueException(const QString& msg) :IOException(msg) {}
    IncorrectValueException(qint64 /*pos*/, const char* errMsg) :IOException(errMsg) {}
    ~IncorrectValueException() throw() {}
};

class EOFException : public IOException {
public:
    EOFException(const QString& msg = QString()) :IOException(msg) {}
    ~EOFException() throw() {}
};

class LEInputStream {
private:
    QIODevice* input;
    QDataStream data;

    qint64 maxPosition;

    qint8 bitfieldpos;
    quint8 bitfield;

    quint8 getBits(quint8 n) {
        if (bitfieldpos < 0) {
            bitfield = readuint8();
            bitfieldpos = 0;
        }
        quint8 v = bitfield >> bitfieldpos;
        bitfieldpos += n;
        if (bitfieldpos == 8) {
            bitfieldpos = -1;
        } else if (bitfieldpos > 8) {
            throw IOException("Bitfield does not have enough bits left.");
        }
        return v;
    }
    void checkForLeftOverBits() const {
        if (bitfieldpos >= 0) {
            throw IOException("Cannot read this type halfway through a bit operation.");
        }
    }
    void checkStatus() const {
        if (data.status() != QDataStream::Ok) {
            if (data.status() == QDataStream::ReadPastEnd) {
                throw EOFException("Stream claims to be at the end at position: " + QString::number(input->pos()) + "." );
            }
            throw IOException("Error reading data at position " + QString::number(input->pos()) + ".");
        }
    }

public:
    class Mark {
    friend class LEInputStream;
    private:
        QIODevice* input;
        qint64 pos;
        Mark(QIODevice* in) :input(in), pos((in) ?in->pos() :0) {} 
    public:
        Mark() :input(0), pos(0) {} 
    };

    LEInputStream(QIODevice* in) :input(in), data(in) {
        maxPosition = 0;
        bitfield = 0;
        bitfieldpos = -1;
        data.setByteOrder(QDataStream::LittleEndian);
    }

    Mark setMark() { return Mark(input); }
    void rewind(const Mark& m) {
        maxPosition = qMax(input->pos(), maxPosition);
        if (!m.input || !m.input->seek(m.pos)) {
            throw IOException("Cannot rewind.");
        }
        data.resetStatus();
    }

    bool readbit() {
        quint8 v = getBits(1) & 1;
        return v == 1;
    }

    quint8 readuint2() {
        return getBits(2) & 3;
    }

    quint8 readuint3() {
        return getBits(3) & 0x7;
    }

    quint8 readuint4() {
        return getBits(4) & 0xF;
    }

    quint8 readuint5() {
        return getBits(5) & 0x1F;
    }

    quint8 readuint6() {
        return getBits(6) & 0x3F;
    }

    quint8 readuint7() {
        return getBits(7) & 0x7F;
    }

    quint16 readuint9() {
        quint8 a = readuint8();
        quint8 b = getBits(1) & 0x1;
        return (b << 8) | a;
    }

    quint16 readuint12() {
        // we assume there are 4 bits left
        quint8 a = getBits(4) & 0xF;
        quint8 b = readuint8();
        return (b << 4) | a;
    }

    quint16 readuint13() {
        quint8 a = getBits(5) & 0x1F;
        quint8 b = readuint8();
        return (b << 5) | a;
    }

    quint16 readuint14() {
        quint16 v;
        if (bitfieldpos < 0) {
            quint8 a = readuint8();
            quint8 b = getBits(6) & 0x3F;
            v = (b << 8) | a;
        } else if (bitfieldpos == 2) {
            quint8 a = getBits(6) & 0x3F;
            quint8 b = readuint8();
            v = (b << 6) | a;
        } else {
            throw IOException("Cannot read this type halfway through a bit operation.");
        }
        return v;
    }

    quint16 readuint15() {
        // we assume there are 7 bits left
        quint8 a = getBits(7) & 0x7F;
        quint8 b = readuint8();
        return (b << 7) | a;
    }

    quint32 readuint20() {
        quint32 v;
        if (bitfieldpos < 0) {
            quint8 a = readuint8();
            quint8 b = readuint8();
            quint8 c = getBits(4) & 0xF;
            v = (c << 16) | (b << 8) | a;
        } else if (bitfieldpos == 4) {
            quint8 a = getBits(4) & 0xF;
            quint8 b = readuint8();
            quint8 c = readuint8();
            v = (c << 12) | (b << 4) | a;
        } else {
            throw IOException("Cannot read this type halfway through a bit operation.");
        }
        return v;
    }

    quint32 readuint30() {
        checkForLeftOverBits();
        quint8 a = readuint8();
        quint8 b = readuint8();
        quint8 c = readuint8();
        quint8 d = getBits(6) & 0x3F;
        return (d << 24) | (c << 16) | (b << 8) | a;
    }

    quint8 readuint8() {
        checkForLeftOverBits();
        quint8 a;
        data >> a;
        checkStatus();
        return a;
    }

    qint16 readint16() {
        checkForLeftOverBits();
        qint16 v;
        data >> v;
        checkStatus();
        return v;
    }

    quint16 readuint16() {
        checkForLeftOverBits();
        quint16 v;
        data >> v;
        checkStatus();
        return v;
    }

    quint32 readuint32() {
        checkForLeftOverBits();
        quint32 v;
        data >> v;
        checkStatus();
        return v;
    }

    qint32 readint32() {
        checkForLeftOverBits();
        qint32 v;
        data >> v;
        checkStatus();
        return v;
    }

    void readBytes(QByteArray& b) {
        int offset = 0;
        int todo = b.size();
        while (todo > 0) { // do not enter loop if array size is 0
            int nread = data.readRawData(b.data() + offset, todo);
            if (nread == -1 || nread == 0) {
                throw EOFException();// TODO: differentiate
            }
            todo -= nread;
            offset += nread;
        }
    }

    void skip(int len) {
        data.skipRawData(len);
    }

    qint64 getPosition() const { return input->pos(); }

    qint64 getMaxPosition() const { return qMax(input->pos(), maxPosition); }
    qint64 getSize() const { return input->size(); }
};

#endif
