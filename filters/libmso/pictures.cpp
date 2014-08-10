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

#include <KoStore.h>
#include <KoXmlWriter.h>

#include <zlib.h>
#include <cstdio>
#include <iostream>
#include <QDebug>
#include <QImage>
#include <QBuffer>

//#define DEBUG_PICTURES

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

void saveStream(POLE::Stream& stream, quint32 size, KoStore* out)
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

bool saveDecompressedStream(POLE::Stream& stream, quint32 size, KoStore* out)
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

const char* getMimetype(quint16 type)
{
    switch (type) {
    case officeArtBlipEMF: return "image/x-emf";
    case officeArtBlipWMF: return "image/x-wmf";
    case officeArtBlipPICT: return "image/pict";
    case officeArtBlipJPEG: return "image/jpeg";
    case officeArtBlipPNG: return "image/png";
    case officeArtBlipDIB: return "application/octet-stream";
    case officeArtBlipTIFF: return "image/tiff";
    case officeArtBlipJPEG2: return "image/jpeg";
    }
    return "";
}

const char* getSuffix(quint16 type)
{
    switch (type) {
    case officeArtBlipEMF: return ".emf";
    case officeArtBlipWMF: return ".wmf";
    case officeArtBlipPICT: return ".pict";
    case officeArtBlipJPEG: return ".jpg";
    case officeArtBlipPNG: return ".png";
    case officeArtBlipDIB: return ".dib";
    case officeArtBlipTIFF: return ".tiff";
    case officeArtBlipJPEG2: return ".jpg";
    }
    return "";
}

template<class T> void savePicture(PictureReference& ref, const T* a, KoStore* out)
{
    if (!a) return;
    ref.uid = a->rgbUid1 + a->rgbUid2;
    ref.name.clear();

    QByteArray imagePixelBytes = a->BLIPFileData;
    if (a->rh.recType == officeArtBlipDIB) {
        // convert to QImage
        QImage image;
        bool result = dibToBmp(image, imagePixelBytes.data(), imagePixelBytes.size());
        if (!result) {
            return; // empty name reports an error
        }

        // writes image into ba in PNG format
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        result = image.save(&buffer, "PNG");
        if (!result) {
            return; // empty name reports an error
        }

        imagePixelBytes = ba;
        // save as png
        ref.name = ref.uid.toHex() + getSuffix(officeArtBlipPNG);
        ref.mimetype = getMimetype(officeArtBlipPNG);
    } else {
        ref.name = ref.uid.toHex() + getSuffix(a->rh.recType);
        ref.mimetype = getMimetype(a->rh.recType);
    }

    if (!out->open(ref.name.toLocal8Bit())) {
        ref.name.clear();
        ref.uid.clear();
        return; // empty name reports an error
    }

    out->write(imagePixelBytes.data(), imagePixelBytes.size());
    out->close();
}

template<class T> void saveDecompressedPicture(PictureReference& ref, const T* a, KoStore* store)
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

        if ((uint)buff.size() != cbSize) {
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

PictureReference savePicture(const MSO::OfficeArtBlip& a, KoStore* store)
{
    PictureReference ref;
    store->setCompressionEnabled(true);
    // only one of these calls will actually save a picture
    saveDecompressedPicture(ref, a.anon.get<MSO::OfficeArtBlipEMF>(), store);
    saveDecompressedPicture(ref, a.anon.get<MSO::OfficeArtBlipWMF>(), store);
    saveDecompressedPicture(ref, a.anon.get<MSO::OfficeArtBlipPICT>(), store);
    // formats below are typically not very compressible
    store->setCompressionEnabled(false);
    savePicture(ref, a.anon.get<MSO::OfficeArtBlipJPEG>(), store);
    savePicture(ref, a.anon.get<MSO::OfficeArtBlipPNG>(), store);
    savePicture(ref, a.anon.get<MSO::OfficeArtBlipDIB>(), store);
    savePicture(ref, a.anon.get<MSO::OfficeArtBlipTIFF>(), store);
    return ref;
}
} //namespace

PictureReference savePicture(POLE::Stream& stream, KoStore* out)
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
    case officeArtBlipEMF: offset = (instance == 0x3D4) ? 50 : 66; break;
    case officeArtBlipWMF: offset = (instance == 0x216) ? 50 : 66; break;
    case officeArtBlipPICT: offset = (instance == 0x542) ? 50 : 66; break;
    case officeArtBlipJPEG: offset = (instance == 0x46A) ? 17 : 33; break;
    case officeArtBlipPNG: offset = (instance == 0x6E0) ? 17 : 33; break;
    case officeArtBlipDIB: offset = (instance == 0x7A8) ? 17 : 33; break;
    case officeArtBlipTIFF: offset = (instance == 0x6E4) ? 17 : 33; break;
    case officeArtBlipJPEG2: offset = (instance == 0x46A) ? 17 : 33; break;
    default: return ref;
    }
    const char* namesuffix = getSuffix(type);
    ref.mimetype = getMimetype(type);

    // skip offset
    if (offset != 0 && stream.read(buffer, offset) != offset) return ref;
    size -= offset;

    bool compressed = false;
    if (type == officeArtBlipEMF || type == officeArtBlipWMF || type == officeArtBlipPICT) {
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

PictureReference savePicture(const MSO::OfficeArtBStoreContainerFileBlock& a, KoStore* store)
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

QByteArray getRgbUid(const MSO::OfficeArtDggContainer& dgg, quint32 pib, quint32& offset)
{
    quint32 n = pib - 1;
    // return 16 byte rgbuid for this given blip id
    if (dgg.blipStore) {
        const MSO::OfficeArtBStoreContainer* b = dgg.blipStore.data();
        if (n < (quint32) b->rgfb.size() &&
            b->rgfb[n].anon.is<MSO::OfficeArtFBSE>())
        {
            const MSO::OfficeArtFBSE* fbse = b->rgfb[n].anon.get<MSO::OfficeArtFBSE>();
#ifdef DEBUG_PICTURES
            qDebug() << "rgfb.size():" << b->rgfb.size();
            qDebug() << "pib:" << pib;
            qDebug() << "OfficeArtFBSE: DEBUG";
            qDebug() << "rgbUid:" << fbse->rgbUid.toHex();
            qDebug() << "tag:" << fbse->tag;
            qDebug() << "cRef:" << fbse->cRef;
            qDebug() << "foDelay:" << fbse->foDelay;
            qDebug() << "embeddeBlip:" << fbse->embeddedBlip;
#endif
            offset = fbse->foDelay;
            return fbse->rgbUid;
        }
    }
    if (pib != 0xFFFF && pib != 0) {
#ifdef DEBUG_PICTURES
        qDebug() << "Could not find image for pib " << pib;
#endif
    }
    return QByteArray();
}

QMap<QByteArray, QString> createPictures(KoStore* store, KoXmlWriter* manifest, const QList<MSO::OfficeArtBStoreContainerFileBlock>* rgfb)
{
    PictureReference ref;
    QMap<QByteArray, QString> fileNames;

    if (!rgfb) return fileNames;

    foreach (const MSO::OfficeArtBStoreContainerFileBlock& block, *rgfb) {
        ref = savePicture(block, store);

        if (ref.name.length() == 0) {
#ifdef DEBUG_PICTURES
            qDebug() << "Empty picture reference, probably an empty slot";
#endif
            continue;
        }
        //check if the MD4 digest is up2date
        if (block.anon.is<MSO::OfficeArtFBSE>()) {
            const MSO::OfficeArtFBSE* fbse = block.anon.get<MSO::OfficeArtFBSE>();
            if (fbse->rgbUid != ref.uid) {
                ref.uid = fbse->rgbUid;
            }
        }

        if (manifest) {
            manifest->addManifestEntry("Pictures/" + ref.name, ref.mimetype);
        }

        fileNames[ref.uid] = ref.name;
    }
#ifdef DEBUG_PICTURES
    qDebug() << "fileNames: DEBUG";
    QMap<QByteArray, QString>::const_iterator i = fileNames.constBegin();
    while (i != fileNames.constEnd()) {
        qDebug() << i.key().toHex() << ": " << i.value();
        ++i;
    }
#endif
    return fileNames;
}

// NOTE: copied from qwmf.cc, I just changed WORD -> short and DWORD -> int,
// removed some commented code, and changed the kWarning to qDebug here
bool dibToBmp(QImage& bmp, const char* dib, long int size)
{
    typedef struct _BMPFILEHEADER {
        short bmType;
        int bmSize;
        short bmReserved1;
        short bmReserved2;
        int bmOffBits;
    }  BMPFILEHEADER;

    int sizeBmp = size + 14;
    QByteArray pattern;       // BMP header and DIB data
    pattern.fill(0, sizeBmp);    //resize and fill
    pattern.insert(14, QByteArray::fromRawData(dib, size));

    // add BMP header
    BMPFILEHEADER* bmpHeader;
    bmpHeader = (BMPFILEHEADER*)((const char*)pattern);
    bmpHeader->bmType = 0x4D42;
    bmpHeader->bmSize = sizeBmp;

    if (!bmp.loadFromData((const uchar*)bmpHeader, pattern.size(), "BMP")) {
        qDebug() << "dibToBmp: invalid bitmap";
        return false;
    } else {
        return true;
    }
}
