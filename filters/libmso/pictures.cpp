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
#include <QtCore/QDebug>

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

void
saveStream(POLE::Stream& stream, quint32 size, KoStore* out)
{
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
saveDecompressedStream(POLE::Stream& stream, quint32 size, KoStore* out)
{
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
            qint32 nwritten = bufferSize - zstream.avail_out;
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
const char*
getMimetype(quint16 type)
{
    switch (type) {
    case 0xF01A: return "application/octet-stream";
    case 0xF01B: return "application/octet-stream";
    case 0xF01C: return "image/pict";
    case 0xF01D: return "image/jpeg";
    case 0xF01E: return "image/png";
    case 0xF01F: return "application/octet-stream";
    case 0xF029: return "image/tiff";
    case 0xF02A: return "image/jpeg";
    }
    return "";
}
const char*
getSuffix(quint16 type)
{
    switch (type) {
    case 0xF01A: return ".emf";
    case 0xF01B: return ".wmf";
    case 0xF01C: return ".pict";
    case 0xF01D: return ".jpg";
    case 0xF01E: return ".png";
    case 0xF01F: return ".dib";
    case 0xF029: return ".tiff";
    case 0xF02A: return ".jpg";
    }
    return "";
}
template<class T>
void
savePicture(PictureReference& ref, const T* a, KoStore* out)
{
    if (!a) return;
    ref.uid = a->rgbUid1 + a->rgbUid2;
    ref.name = ref.uid.toHex() + getSuffix(a->rh.recType);
    if (!out->open(ref.name.toLocal8Bit())) {
        ref.name.clear();
        ref.uid.clear();
        return; // empty name reports an error
    }
    out->write(a->BLIPFileData.data(), a->BLIPFileData.size());
    ref.mimetype = getMimetype(a->rh.recType);
    out->close();
}
template<class T>
void
saveDecompressedPicture(PictureReference& ref, const T* a, KoStore* store)
{
    if (!a) return;

    QByteArray buff = a->BLIPFileData;
    bool compressed = a->metafileHeader.compression == 0;

    if (compressed) {
        quint32 cbSize = a->metafileHeader.cbSize;
        char tmp[4];

        //big-endian byte order required
        tmp[3] = (cbSize & 0x000000ff);
        tmp[2] = ((cbSize >> 8) & 0x0000ff);
        tmp[1] = ((cbSize >> 16) & 0x00ff);
        tmp[0] = (cbSize >> 24);
        buff.prepend((char*) tmp, 4);
        buff = qUncompress(buff);

        if (buff.size() != cbSize) {
            qDebug() << "Warning: uncompressed size of the metafile differs";
        }
    }
    //reuse the savePicture code
    ref.uid = a->rgbUid1 + a->rgbUid2;
    ref.name = ref.uid.toHex() + getSuffix(a->rh.recType);
    if (!store->open(ref.name.toLocal8Bit())) {
        ref.name.clear();
        ref.uid.clear();
        return; // empty name reports an error
    }
    store->write(buff.data(), buff.size());
    ref.mimetype = getMimetype(a->rh.recType);
    store->close();
}
PictureReference
savePicture(const MSO::OfficeArtBlip& a, KoStore* store)
{
    PictureReference ref;
    // only one of these calls will actually save a picture
    saveDecompressedPicture(ref, a.anon.get<MSO::OfficeArtBlipEMF>(), store);
    saveDecompressedPicture(ref, a.anon.get<MSO::OfficeArtBlipWMF>(), store);
    saveDecompressedPicture(ref, a.anon.get<MSO::OfficeArtBlipPICT>(), store);
    savePicture(ref, a.anon.get<MSO::OfficeArtBlipJPEG>(), store);
    savePicture(ref, a.anon.get<MSO::OfficeArtBlipPNG>(), store);
    savePicture(ref, a.anon.get<MSO::OfficeArtBlipDIB>(), store);
    savePicture(ref, a.anon.get<MSO::OfficeArtBlipTIFF>(), store);
    return ref;
}
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
    switch (type) {
    case 0xF01A: offset = (instance == 0x3D4) ? 50 : 66; break;
    case 0xF01B: offset = (instance == 0x216) ? 50 : 66; break;
    case 0xF01C: offset = (instance == 0x542) ? 50 : 66; break;
    case 0xF01D: offset = (instance == 0x46A) ? 17 : 33; break;
    case 0xF01E: offset = (instance == 0x6E0) ? 17 : 33; break;
    case 0xF01F: offset = (instance == 0x7A8) ? 17 : 33; break;
    case 0xF029: offset = (instance == 0x6E4) ? 17 : 33; break;
    case 0xF02A: offset = (instance == 0x46A) ? 17 : 33; break;
    default: return ref;
    }
    const char* namesuffix = getSuffix(type);
    ref.mimetype = getMimetype(type);

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
PictureReference
savePicture(const MSO::OfficeArtBStoreContainerFileBlock& a, KoStore* store)
{
    const MSO::OfficeArtBlip* blip = a.anon.get<MSO::OfficeArtBlip>();
    const MSO::OfficeArtFBSE* fbse = a.anon.get<MSO::OfficeArtFBSE>();
    if (blip) {
        return savePicture(*blip, store);
    }
    if (fbse && fbse->embeddedBlip) {
        return savePicture(*fbse->embeddedBlip, store);
    }
    return PictureReference();
}
