/* This file is part of the KDE project
   Copyright (C) 2009 KO GmbH <jos.van.den.oever@kogmbh.com>

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
   Boston, MA 02110-1301, USA.
*/
#include "pictures.h"
#include <stdio.h>

// Use anonymous namespace to cover following functions
namespace
{

static inline quint16 readU16(const void* p)
{
    const unsigned char* ptr = (const unsigned char*) p;
    return ptr[0] + (ptr[1] << 8);
}

static inline quint32 readU32(const void* p)
{
    const unsigned char* ptr = (const unsigned char*) p;
    return ptr[0] + (ptr[1] << 8) + (ptr[2] << 16) + (ptr[3] << 24);
}

}

std::string
savePicture(POLE::Stream& stream, int position, KoStore* out, QString& mimetype)
{
    const quint16 bufferSize = 1024;
    unsigned char buffer[bufferSize];
    if (stream.read(buffer, 8) != 8) return "";

    quint16 instance = readU16(buffer) >> 4;
    quint16 type = readU16(buffer + 2);
    quint32 size = readU32(buffer + 4);

    if (type == 0xF007) { // OfficeArtFBSE
        if (stream.read(buffer, 36) != 36) return "";
        quint16 cbName = *(buffer + 33);
        if (cbName > bufferSize || stream.read(buffer, cbName) != cbName)
            return "";
        size = size - 36 - cbName;
        // read embedded BLIP
        if (stream.read(buffer, 8) != 8) return "";
        instance = readU16(buffer) >> 4;
        type = readU16(buffer + 2);
        size = readU32(buffer + 4);
    }

    // Image data is stored raw in the Pictures stream
    // The offset to the data differs per image type.
    quint16 offset;
    const char* nametemplate;
    switch (type) {
    case 0xF01A:
        offset = (instance == 0x3D4) ?50 :66;
        nametemplate = "%06i.emf";
        mimetype = "application/octet-stream";
        break;
    case 0xF01B:
        offset = (instance == 0x216) ?50 :66;
        nametemplate = "%06i.wmf";
        mimetype = "application/octet-stream";
        break;
    case 0xF01C:
        offset = (instance == 0x542) ?50 :66;
        nametemplate = "%06i.pict";
        mimetype = "image/pict";
        break;
    case 0xF01D:
        offset = (instance == 0x46A) ?17 :33;
        nametemplate = "%06i.jpg";
        mimetype = "image/jpeg";
        break;
    case 0xF01E:
        offset = (instance == 0x6E0) ?17 :33;
        nametemplate = "%06i.png";
        mimetype = "image/png";
        break;
    case 0xF01F:
        offset = (instance == 0x7A8) ?17 :33;
        nametemplate = "%06i.dib";
        mimetype = "application/octet-stream";
        break;
    case 0xF029:
        offset = (instance == 0x6E4) ?17 :33;
        nametemplate = "%06i.tiff";
        mimetype = "image/tiff";
        break;
    case 0xF02A:
        offset = (instance == 0x46A) ?17 :33;
        nametemplate = "%06i.jpg";
        mimetype = "image/jpeg";
        break;
    default:
        offset = 0;
        nametemplate = "%06i";
        mimetype = "application/octet-stream";
        break;
    }

    // skip offset
    if (offset != 0 && stream.read(buffer, offset) != offset) return "";
    size -= offset;

    int n = sprintf((char*)buffer, nametemplate, position);
    std::string filename((char*)buffer, n);

    if (!out->open(filename.c_str())) {
        return ""; // empty name reports an error
    }
    unsigned long nread = stream.read(buffer,
                                      (bufferSize < size) ? bufferSize : size);
    while (nread > 0) {
        out->write((char*)buffer, nread);
        size -= nread;
        nread = stream.read(buffer, (bufferSize < size) ? bufferSize : size);
    }
    out->close();

    return filename;
}
