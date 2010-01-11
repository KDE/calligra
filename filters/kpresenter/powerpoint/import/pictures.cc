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
#include <zlib.h>
#include <cstdio>
#include <iostream>

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
void
saveStream(POLE::Stream& stream, quint32 size, KoStore* out) {
    const quint16 bufferSize = 1024;
    unsigned char buffer[bufferSize];
    unsigned long nread = stream.read(buffer,
                                      (bufferSize < size) ? bufferSize : size);
    while (nread > 0) {
        out->write((char*)buffer, nread);
        size -= nread;
        nread = stream.read(buffer, (bufferSize < size) ? bufferSize : size);
    }
}
bool
saveDecompressedStream(POLE::Stream& stream, quint32 size, KoStore* out) {
    const quint16 bufferSize = 1024;
    unsigned char bufin[bufferSize];
    unsigned char bufout[bufferSize];

    // initialize for decompressing ZLIB format
    z_stream_s zstream;
    zstream.zalloc = Z_NULL;
    zstream.zfree = Z_NULL;
    zstream.opaque = Z_NULL;
    zstream.avail_in = 0;
    zstream.next_in = Z_NULL;
    int r = inflateInit(&zstream);
    if (r != Z_OK) {
        inflateEnd(&zstream);
        return false;
    }

    unsigned long nread = stream.read(bufin,
            (bufferSize < size) ? bufferSize : size);
    while (nread > 0) { // loop over the available data
        size -= nread;
        zstream.next_in = (Bytef*)bufin;
        zstream.avail_in = nread;
        do { // loop until the data in bufin has all been decompressed
            zstream.next_out = (Bytef*)bufout;
            zstream.avail_out = bufferSize;
            int r = inflate(&zstream, Z_SYNC_FLUSH);
            int32_t nwritten = bufferSize - zstream.avail_out;
            if (r != Z_STREAM_END && r != Z_OK) {
                inflateEnd(&zstream);
                return false;
            }
            out->write((char*)bufout, nwritten);
            if (r == Z_STREAM_END) {
                inflateEnd(&zstream);
                return true; // successfully reached the end
            }
        } while (zstream.avail_in > 0);
        nread = stream.read(bufin, (bufferSize < size) ? bufferSize : size);
    }

    inflateEnd(&zstream);
    return false; // the stream was incomplete
}
PictureReference
savePicture(POLE::Stream& stream, KoStore* out)
{
    PictureReference ref;
    const quint16 bufferSize = 1024;
    unsigned char buffer[bufferSize];
    if (stream.read(buffer, 8) != 8) return ref;


    quint16 instance = readU16(buffer) >> 4;
    quint16 type = readU16(buffer + 2);
    quint32 size = readU32(buffer + 4);

    if (type == 0xF007) { // OfficeArtFBSE
        if (stream.read(buffer, 36) != 36) return ref;
        quint16 cbName = *(buffer + 33);
        if (cbName > bufferSize || stream.read(buffer, cbName) != cbName) {
            return ref;
        }
        size = size - 36 - cbName;
        // read embedded BLIP
        if (stream.read(buffer, 8) != 8) return ref;
        instance = readU16(buffer) >> 4;
        type = readU16(buffer + 2);
        size = readU32(buffer + 4);
    }

    // Image data is stored raw in the Pictures stream
    // The offset to the data differs per image type.
    quint16 offset;
    const char* namesuffix;
    switch (type) {
    case 0xF01A:
        offset = (instance == 0x3D4) ? 50 : 66;
        namesuffix = ".emf";
        ref.mimetype = "application/octet-stream";
        break;
    case 0xF01B:
        offset = (instance == 0x216) ? 50 : 66;
        namesuffix = ".wmf";
        ref.mimetype = "application/octet-stream";
        break;
    case 0xF01C:
        offset = (instance == 0x542) ? 50 : 66;
        namesuffix = ".pict";
        ref.mimetype = "image/pict";
        break;
    case 0xF01D:
        offset = (instance == 0x46A) ? 17 : 33;
        namesuffix = ".jpg";
        ref.mimetype = "image/jpeg";
        break;
    case 0xF01E:
        offset = (instance == 0x6E0) ? 17 : 33;
        namesuffix = ".png";
        ref.mimetype = "image/png";
        break;
    case 0xF01F:
        offset = (instance == 0x7A8) ? 17 : 33;
        namesuffix = ".dib";
        ref.mimetype = "application/octet-stream";
        break;
    case 0xF029:
        offset = (instance == 0x6E4) ? 17 : 33;
        namesuffix = ".tiff";
        ref.mimetype = "image/tiff";
        break;
    case 0xF02A:
        offset = (instance == 0x46A) ? 17 : 33;
        namesuffix = ".jpg";
        ref.mimetype = "image/jpeg";
        break;
    default:
        offset = 0;
        namesuffix = "";
        ref.mimetype = "application/octet-stream";
        break;
    }

    // skip offset
    if (offset != 0 && stream.read(buffer, offset) != offset) return ref;
    size -= offset;

    bool compressed = false;
    if (type == 0xF01A || type == 0xF01B || type == 0xF01C) {
        // read the compressed field from the OfficeArtMetafileHeader
        compressed = buffer[offset-2] == 0;
    }

    ref.uid = QByteArray((const char*)buffer, 16);
    ref.name = ref.uid.toHex() + namesuffix;
    if (!out->open(ref.name.toLocal8Bit())) {
        ref.name.clear();
        ref.uid.clear();
        return ref; // empty name reports an error
    }
    unsigned long next = stream.tell() + size;
    if (compressed) {
        saveDecompressedStream(stream, size, out);
    } else {
        saveStream(stream, size, out);
    }
    stream.seek(next);
    out->close();

    return ref;
}
