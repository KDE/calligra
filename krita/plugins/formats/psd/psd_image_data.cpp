/*
 *   Copyright (C) 2011 by Siddharth Sharma <siddharth.kde@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
<<<<<<< HEAD
 *   This program is distributed in the hope that it will be useful,  
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of   
=======
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
>>>>>>> master
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
<<<<<<< HEAD
 *   along with this program.  If not, see <http://www.gnu.org/licenses/> 
=======
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>
>>>>>>> master
 */

#include <netinet/in.h> // htonl

#include <QFile>
#include <QDebug>
#include <QVector>
#include <QByteArray>
#include <QBuffer>

<<<<<<< HEAD

=======
#include <KoChannelInfo.h>
>>>>>>> master
#include <KoColorSpace.h>
#include <KoColorSpaceMaths.h>
#include <KoColorSpaceTraits.h>

#include <psd_image_data.h>
#include "psd_utils.h"
#include "compression.h"

<<<<<<< HEAD
=======
#include "kis_iterator_ng.h"
#include "kis_paint_device.h"

>>>>>>> master
PSDImageData::PSDImageData(PSDHeader *header)
{
    m_header = header;
}

PSDImageData::~PSDImageData() {

}

<<<<<<< HEAD
bool PSDImageData::read(KisPaintDeviceSP dev ,QIODevice *io) {

    qDebug() << "Position before read " << io->pos();
    psdread(io, &m_compression);
    qDebug() << "COMPRESSION TYPE " << m_compression;
    quint64 start = io->pos();
    m_channelSize = m_header->channelDepth/8;
    m_channelDataLength = m_header->height * m_header->width * m_channelSize;
    qDebug()<<"Height" << m_header->height << "Width:"<< m_header->width;
=======
bool PSDImageData::read(QIODevice *io, KisPaintDeviceSP dev ) {

    psdread(io, &m_compression);
    quint64 start = io->pos();
    m_channelSize = m_header->channelDepth/8;
    m_channelDataLength = m_header->height * m_header->width * m_channelSize;

>>>>>>> master
    switch (m_compression) {

    case 0: // Uncompressed

        for (int channel = 0; channel < m_header->nChannels; channel++) {
            m_channelOffsets << 0;
            ChannelInfo channelInfo;
            channelInfo.channelId = channel;
            channelInfo.compressionType = Compression::Uncompressed;
            channelInfo.channelDataStart = start;
            channelInfo.channelDataLength = m_header->width * m_header->height * m_channelSize;
            start += channelInfo.channelDataLength;
            m_channelInfoRecords.append(channelInfo);

        }

<<<<<<< HEAD
        for (int channel = 0; channel < m_header->nChannels; channel++) {

            qDebug() << "Channel ID: " << m_channelInfoRecords[channel].channelId;
            qDebug() << "Channel Compression Type: " << m_channelInfoRecords[channel].compressionType;
            qDebug() << "Channel Data Start: " << m_channelInfoRecords[channel].channelDataStart;
            qDebug() << "Channel Data Length: " << m_channelInfoRecords[channel].channelDataLength;
            qDebug() << "---------------------------------------------------";

        }

        switch (m_header->colormode) {
=======
    switch (m_header->colormode) {
>>>>>>> master
        case Bitmap:
            break;
        case Grayscale:
            break;
        case Indexed:
            break;
        case RGB:
<<<<<<< HEAD
            qDebug()<<"RGB";
            doRGB(dev, io);
            break;
        case CMYK:
            doCMYK(dev,io);
=======
            readRGB(io, dev);
            break;
        case CMYK:
            readCMYK(io, dev);
>>>>>>> master
            break;
        case MultiChannel:
            break;
        case DuoTone:
            break;
        case Lab:
<<<<<<< HEAD
            doLAB(dev, io);
=======
            readLAB(io, dev);
>>>>>>> master
            break;
        case UNKNOWN:
            break;
        default:
            break;
        }

        break;

    case 1: // RLE
    {
<<<<<<< HEAD
        qDebug()<<"RLE ENCODED";
        quint32 rlelength = 0;        
        
=======

        quint32 rlelength = 0;

>>>>>>> master
        // The start of the actual channel data is _after_ the RLE rowlengths block
        if (m_header->version == 1) {
            start += m_header->nChannels * m_header->height * 2;
        }
        else if (m_header->version == 2) {
            start += m_header->nChannels * m_header->height * 4;
        }
<<<<<<< HEAD
        
=======

>>>>>>> master
        for (int channel = 0; channel < m_header->nChannels; channel++) {
            m_channelOffsets << 0;
            quint32 sumrlelength = 0;
            ChannelInfo channelInfo;
            channelInfo.channelId = channel;
            channelInfo.channelDataStart = start;
            channelInfo.compressionType = Compression::RLE;
<<<<<<< HEAD
            for (int row = 0; row < m_header->height; row++ ) {
=======
            for (quint32 row = 0; row < m_header->height; row++ ) {
>>>>>>> master
                if (m_header->version == 1) {
                    psdread(io,(quint16*)&rlelength);
                }
                else if (m_header->version == 2) {
                    psdread(io,&rlelength);
                }
                channelInfo.rleRowLengths.append(rlelength);
                sumrlelength += rlelength;
            }
            channelInfo.channelDataLength = sumrlelength;
            start += channelInfo.channelDataLength;
            m_channelInfoRecords.append(channelInfo);
        }

<<<<<<< HEAD
        for (int channel = 0; channel < m_header->nChannels; channel++) {
            qDebug() << "Channel offset" << m_channelOffsets[channel];
            qDebug() << "Channel ID: " << m_channelInfoRecords[channel].channelId;
            qDebug() << "Channel Compression Type: " << m_channelInfoRecords[channel].compressionType;
            qDebug() << "Channel Data Start: " << m_channelInfoRecords[channel].channelDataStart;
            qDebug() << "Channel Data Length: " << m_channelInfoRecords[channel].channelDataLength;
            qDebug() << "Found " << m_channelInfoRecords[channel].rleRowLengths.size() << "rows";
            qDebug() << "---------------------------------------------------";
        }

=======
>>>>>>> master
        switch (m_header->colormode) {
        case Bitmap:
            break;
        case Grayscale:
            break;
        case Indexed:
            break;
        case RGB:
<<<<<<< HEAD
            doRGB(dev,io);
            break;
        case CMYK:
=======
            readRGB(io, dev);
            break;
        case CMYK:
            readCMYK(io, dev);
>>>>>>> master
            break;
        case MultiChannel:
            break;
        case DuoTone:
            break;
        case Lab:
<<<<<<< HEAD
=======
            readLAB(io, dev);
>>>>>>> master
            break;
        case UNKNOWN:
            break;
        default:
            break;
        }

        break;
    }
    case 2: // ZIP without prediction
<<<<<<< HEAD
        qDebug()<<"ZIP without prediction";
=======
>>>>>>> master

        switch (m_header->colormode) {
        case Bitmap:
            break;
        case Grayscale:
            break;
        case Indexed:
            break;
        case RGB:
            break;
        case CMYK:
            break;
        case MultiChannel:
            break;
        case DuoTone:
            break;
        case Lab:
            break;
        case UNKNOWN:
            break;
        default:
            break;
        }
        break;

    case 3: // ZIP with prediction
<<<<<<< HEAD
        qDebug()<<"ZIP with prediction";

=======
>>>>>>> master
        switch (m_header->colormode) {
        case Bitmap:
            break;
        case Grayscale:
            break;
        case Indexed:
            break;
        case RGB:
            break;
        case CMYK:
            break;
        case MultiChannel:
            break;
        case DuoTone:
            break;
        case Lab:
            break;
        case UNKNOWN:
            break;
        default:
            break;
        }

        break;
    default:
        break;
    }

    return true;
}

<<<<<<< HEAD
bool PSDImageData::doRGB(KisPaintDeviceSP dev, QIODevice *io) {

    int channelid = 0;

    for (int row = 0; row < m_header->height; row++) {
        
        KisHLineIterator it = dev->createHLineIterator(0, row, m_header->width);
=======
bool PSDImageData::write(QIODevice *io, KisPaintDeviceSP dev)
{
    // XXX: make the compression settting configurable. For now, always use RLE.
    psdwrite(io, (quint16)Compression::RLE);

    // now write all the channels in display order
    // fill in the channel chooser, in the display order, but store the pixel index as well.
    QRect rc = dev->exactBounds();
    QVector<quint8* > planes = dev->readPlanarBytes(rc.x(), rc.y(), rc.width(), rc.height());

    quint64 channelLengthPos = io->pos();
    // write zero's for the channel lengths section
    for (uint i = 0; i < dev->colorSpace()->channelCount() * rc.height(); ++i) {
        psdwrite(io, (quint16)0);
    }
    // here the actual channel data starts
    quint64 channelStartPos = io->pos();

    foreach (KoChannelInfo *channelInfo, KoChannelInfo::displayOrderSorted(dev->colorSpace()->channels())) {

        dbgFile << "Writing channel" << channelInfo->name() << "to image section, Display position" << channelInfo->displayPosition() << "channel index" << KoChannelInfo::displayPositionToChannelIndex(channelInfo->displayPosition(), dev->colorSpace()->channels());

        quint8 *plane = planes[KoChannelInfo::displayPositionToChannelIndex(channelInfo->displayPosition(), dev->colorSpace()->channels())];
        quint32 stride = channelInfo->size() * rc.width();
        for (qint32 row = 0; row < rc.height(); ++row) {

            QByteArray uncompressed = QByteArray::fromRawData((const char*)plane + row * stride, stride);
            QByteArray compressed = Compression::compress(uncompressed, Compression::RLE);

            io->seek(channelLengthPos);
            psdwrite(io, (quint16)compressed.size());
            channelLengthPos +=2;
            io->seek(channelStartPos);

            if (!io->write(compressed) == compressed.size()) {
                error = "Could not write image data";
                return false;
            }

            channelStartPos += compressed.size();
        }
    }

    return true;
}

bool PSDImageData::readRGB(QIODevice *io, KisPaintDeviceSP dev) {

    int channelid = 0;

    for (quint32 row = 0; row < m_header->height; row++) {

        KisHLineIteratorSP it = dev->createHLineIteratorNG(0, row, m_header->width);
>>>>>>> master
        QVector<QByteArray> channelBytes;

        for (int channel = 0; channel < m_header->nChannels; channel++) {

<<<<<<< HEAD

            switch (m_compression) {

            case Compression::Uncompressed:

            {
                io->seek(m_channelInfoRecords[channel].channelDataStart + m_channelOffsets[0]);
                channelBytes.append(io->read(m_header->width*m_channelSize));
                // Debug
                qDebug() << "channel: " << m_channelInfoRecords[channel].channelId << "is at position " << io->pos();
            }
                break;

=======
            switch (m_compression) {
            case Compression::Uncompressed:
            {
                io->seek(m_channelInfoRecords[channel].channelDataStart + m_channelOffsets[0]);
                channelBytes.append(io->read(m_header->width*m_channelSize));
            }
                break;
>>>>>>> master
            case Compression::RLE:
            {
                io->seek(m_channelInfoRecords[channel].channelDataStart + m_channelOffsets[channel]);
                int uncompressedLength = m_header->width * m_header->channelDepth / 8;
<<<<<<< HEAD

                qDebug() << "channel" << channel << "row" << row << "rle length" << m_channelInfoRecords[channel].rleRowLengths[row] << "uncompressed length" << uncompressedLength;

                QByteArray compressedBytes = io->read(m_channelInfoRecords[channel].rleRowLengths[row]);
                QByteArray uncompressedBytes = Compression::uncompress(uncompressedLength, compressedBytes, m_channelInfoRecords[channel].compressionType);
                qDebug() << "uncompressedBytes" << uncompressedBytes.length();
                channelBytes.append(uncompressedBytes);

                m_channelOffsets[channel] +=  m_channelInfoRecords[channel].rleRowLengths[row];
                qDebug() << "channel: " << m_channelInfoRecords[channel].channelId << "is at position " << io->pos();

            }
                break;

            case Compression::ZIP:
                break;

=======
                QByteArray compressedBytes = io->read(m_channelInfoRecords[channel].rleRowLengths[row]);
                QByteArray uncompressedBytes = Compression::uncompress(uncompressedLength, compressedBytes, m_channelInfoRecords[channel].compressionType);
                channelBytes.append(uncompressedBytes);
                m_channelOffsets[channel] +=  m_channelInfoRecords[channel].rleRowLengths[row];

            }
                break;
            case Compression::ZIP:
                break;
>>>>>>> master
            case Compression::ZIPWithPrediction:
                break;

            default:
                break;
            }

        }

        if (m_channelInfoRecords[channelid].compressionType == 0){
            m_channelOffsets[channelid] += (m_header->width * m_channelSize);
        }

<<<<<<< HEAD
        qDebug() << "------------------------------------------";
        qDebug() << "channel offset:"<< m_channelOffsets[channelid] <<": "<<  channelid;

        for (int col = 0; col < m_header->width; col++) {
=======
        for (quint32 col = 0; col < m_header->width; col++) {
>>>>>>> master

            if (m_channelSize == 1) {

                quint8 red = channelBytes[0].constData()[col];
<<<<<<< HEAD
                KoRgbU8Traits::setRed(it.rawData(), red);

                quint8 green = channelBytes[1].constData()[col];
                KoRgbU8Traits::setGreen(it.rawData(), green);

                quint8 blue = channelBytes[2].constData()[col];
                KoRgbU8Traits::setBlue(it.rawData(), blue);
=======
                KoBgrU8Traits::setRed(it->rawData(), red);

                quint8 green = channelBytes[1].constData()[col];
                KoBgrU8Traits::setGreen(it->rawData(), green);

                quint8 blue = channelBytes[2].constData()[col];
                KoBgrU8Traits::setBlue(it->rawData(), blue);
>>>>>>> master

            }

            else if (m_channelSize == 2) {

                quint16 red = ntohs(reinterpret_cast<const quint16 *>(channelBytes[0].constData())[col]);
<<<<<<< HEAD
                KoRgbU16Traits::setRed(it.rawData(), red);

                quint16 green = ntohs(reinterpret_cast<const quint16 *>(channelBytes[1].constData())[col]);
                KoRgbU16Traits::setGreen(it.rawData(), green);

                quint16 blue = ntohs(reinterpret_cast<const quint16 *>(channelBytes[2].constData())[col]);
                KoRgbU16Traits::setBlue(it.rawData(), blue);
=======
                KoBgrU16Traits::setRed(it->rawData(), red);

                quint16 green = ntohs(reinterpret_cast<const quint16 *>(channelBytes[1].constData())[col]);
                KoBgrU16Traits::setGreen(it->rawData(), green);

                quint16 blue = ntohs(reinterpret_cast<const quint16 *>(channelBytes[2].constData())[col]);
                KoBgrU16Traits::setBlue(it->rawData(), blue);
>>>>>>> master

            }

            // XXX see implementation Openexr
            else if (m_channelSize == 4) {

                quint16 red = ntohs(reinterpret_cast<const quint16 *>(channelBytes.constData())[col]);
<<<<<<< HEAD
                KoRgbU16Traits::setRed(it.rawData(), red);

                quint16 green = ntohs(reinterpret_cast<const quint16 *>(channelBytes.constData())[col]);
                KoRgbU16Traits::setGreen(it.rawData(), green);

                quint16 blue = ntohs(reinterpret_cast<const quint16 *>(channelBytes.constData())[col]);
                KoRgbU16Traits::setBlue(it.rawData(), blue);

            }

            dev->colorSpace()->setOpacity(it.rawData(), OPACITY_OPAQUE_U8, 1);
            ++it;
=======
                KoBgrU16Traits::setRed(it->rawData(), red);

                quint16 green = ntohs(reinterpret_cast<const quint16 *>(channelBytes.constData())[col]);
                KoBgrU16Traits::setGreen(it->rawData(), green);

                quint16 blue = ntohs(reinterpret_cast<const quint16 *>(channelBytes.constData())[col]);
                KoBgrU16Traits::setBlue(it->rawData(), blue);

            }

            dev->colorSpace()->setOpacity(it->rawData(), OPACITY_OPAQUE_U8, 1);
            it->nextPixel();
>>>>>>> master
        }

    }

<<<<<<< HEAD
    qDebug()<<"IO Position: "<<io->pos();
=======
>>>>>>> master
    return true;
}


<<<<<<< HEAD
bool PSDImageData::doCMYK(KisPaintDeviceSP dev, QIODevice *io) {
    int m_channelOffsets = 0;

    for (int row = 0; row <m_header->height; row++) {

        KisHLineIterator it = dev->createHLineIterator(0, row, m_header->width);
=======
bool PSDImageData::readCMYK(QIODevice *io, KisPaintDeviceSP dev) {

    int channelid = 0;

    for (quint32 row = 0; row < m_header->height; row++) {

        KisHLineIteratorSP it = dev->createHLineIteratorNG(0, row, m_header->width);
>>>>>>> master
        QVector<QByteArray> channelBytes;

        for (int channel = 0; channel < m_header->nChannels; channel++) {

<<<<<<< HEAD
            io->seek(m_channelInfoRecords[channel].channelDataStart + m_channelOffsets);
            channelBytes.append(io->read(m_header->width*m_channelSize));
            qDebug() << "channel: " << m_channelInfoRecords[channel].channelId << "is at position " << io->pos();

        }
        m_channelOffsets += (m_header->width * m_channelSize);

        qDebug() << "------------------------------------------";
        qDebug() << "channel offset:"<< m_channelOffsets ;

        for (int col = 0; col < m_header->width; col++) {

            if (m_channelSize == 1) {

                quint8 C = ntohs(reinterpret_cast<const quint8 *>(channelBytes[0].constData())[col]);
                KoCmykTraits<quint8>::setC(it.rawData(),C);

                quint8 M = ntohs(reinterpret_cast<const quint8 *>(channelBytes[1].constData())[col]);
                KoCmykTraits<quint8>::setM(it.rawData(),M);

                quint8 Y = ntohs(reinterpret_cast<const quint8 *>(channelBytes[2].constData())[col]);
                KoCmykTraits<quint8>::setY(it.rawData(),Y);

                quint8 K = ntohs(reinterpret_cast<const quint8 *>(channelBytes[3].constData())[col]);
                KoCmykTraits<quint8>::setK(it.rawData(),K);

            }

            else if (m_channelSize == 2) {

                quint16 C = ntohs(reinterpret_cast<const quint16 *>(channelBytes[0].constData())[col]);
                KoCmykTraits<quint16>::setC(it.rawData(),C);

                quint16 M = ntohs(reinterpret_cast<const quint16 *>(channelBytes[1].constData())[col]);
                KoCmykTraits<quint16>::setM(it.rawData(),M);

                quint16 Y = ntohs(reinterpret_cast<const quint16 *>(channelBytes[2].constData())[col]);
                KoCmykTraits<quint16>::setY(it.rawData(),Y);

                quint16 K = ntohs(reinterpret_cast<const quint16 *>(channelBytes[3].constData())[col]);
                KoCmykTraits<quint16>::setK(it.rawData(),K);

            }

            else if (m_channelSize == 4) {

                quint32 C = ntohs(reinterpret_cast<const quint32 *>(channelBytes[0].constData())[col]);
                KoCmykTraits<quint32>::setC(it.rawData(),C);

                quint32 M = ntohs(reinterpret_cast<const quint32 *>(channelBytes[1].constData())[col]);
                KoCmykTraits<quint32>::setM(it.rawData(),M);

                quint32 Y = ntohs(reinterpret_cast<const quint32 *>(channelBytes[2].constData())[col]);
                KoCmykTraits<quint32>::setY(it.rawData(),Y);

                quint32 K = ntohs(reinterpret_cast<const quint32 *>(channelBytes[3].constData())[col]);
                KoCmykTraits<quint32>::setK(it.rawData(),K);

            }

            dev->colorSpace()->setOpacity(it.rawData(), OPACITY_OPAQUE_U8, 1);
            ++it;
        }

    }
    return true;
}

bool PSDImageData::doLAB(KisPaintDeviceSP dev, QIODevice *io) {
    int m_channelOffsets = 0;

    for (int row = 0; row <m_header->height; row++) {

        KisHLineIterator it = dev->createHLineIterator(0, row, m_header->width);
=======

            switch (m_compression) {

            case Compression::Uncompressed:

            {
                io->seek(m_channelInfoRecords[channel].channelDataStart + m_channelOffsets[0]);
                channelBytes.append(io->read(m_header->width*m_channelSize));

            }
                break;

            case Compression::RLE:
            {
                io->seek(m_channelInfoRecords[channel].channelDataStart + m_channelOffsets[channel]);
                int uncompressedLength = m_header->width * m_header->channelDepth / 8;
                QByteArray compressedBytes = io->read(m_channelInfoRecords[channel].rleRowLengths[row]);
                QByteArray uncompressedBytes = Compression::uncompress(uncompressedLength, compressedBytes, m_channelInfoRecords[channel].compressionType);
                channelBytes.append(uncompressedBytes);
                m_channelOffsets[channel] +=  m_channelInfoRecords[channel].rleRowLengths[row];
            }
                break;

            case Compression::ZIP:
                break;

            case Compression::ZIPWithPrediction:
                break;

            default:
                break;
            }

        }

        if (m_channelInfoRecords[channelid].compressionType == 0){
            m_channelOffsets[channelid] += (m_header->width * m_channelSize);
        }

        for (quint32 col = 0; col < m_header->width; col++) {

            if (m_channelSize == 1) {

                quint8 *pixel = new quint8[5];
                memset(pixel, 0, 5);
                dev->colorSpace()->setOpacity(pixel, OPACITY_OPAQUE_U8, 1);

                memset(pixel, 255 - channelBytes[0].constData()[col], 1);
                memset(pixel + 1, 255 - channelBytes[1].constData()[col], 1);
                memset(pixel + 2, 255 - channelBytes[2].constData()[col], 1);
                memset(pixel + 3, 255 - channelBytes[3].constData()[col], 1);
                dbgFile << "C" << pixel[0] << "M" << pixel[1] << "Y" << pixel[2] << "K" << pixel[3] << "A" << pixel[4];
                memcpy(it->rawData(), pixel, 5);


            }
            else if (m_channelSize == 2) {

                quint16 C = ntohs(reinterpret_cast<const quint16 *>(channelBytes[0].constData())[col]);
                KoCmykTraits<quint16>::setC(it->rawData(),C);

                quint16 M = ntohs(reinterpret_cast<const quint16 *>(channelBytes[1].constData())[col]);
                KoCmykTraits<quint16>::setM(it->rawData(),M);

                quint16 Y = ntohs(reinterpret_cast<const quint16 *>(channelBytes[2].constData())[col]);
                KoCmykTraits<quint16>::setY(it->rawData(),Y);

                quint16 K = ntohs(reinterpret_cast<const quint16 *>(channelBytes[3].constData())[col]);
               KoCmykTraits<quint16>::setK(it->rawData(),K);

            }
            else if (m_channelSize == 4) {

                quint32 C = ntohs(reinterpret_cast<const quint32 *>(channelBytes[0].constData())[col]);
                KoCmykTraits<quint32>::setC(it->rawData(),C);

                quint32 M = ntohs(reinterpret_cast<const quint32 *>(channelBytes[1].constData())[col]);
                KoCmykTraits<quint32>::setM(it->rawData(),M);

                quint32 Y = ntohs(reinterpret_cast<const quint32 *>(channelBytes[2].constData())[col]);
                KoCmykTraits<quint32>::setY(it->rawData(),Y);

                quint32 K = ntohs(reinterpret_cast<const quint32 *>(channelBytes[3].constData())[col]);
                KoCmykTraits<quint32>::setK(it->rawData(),K);

            }

            dev->colorSpace()->setOpacity(it->rawData(), OPACITY_OPAQUE_U8, 1);
            it->nextPixel();;
        }

    }

    return true;

}

bool PSDImageData::readLAB(QIODevice *io, KisPaintDeviceSP dev) {

    int channelid = 0;

    for (quint32 row = 0; row < m_header->height; row++) {

        KisHLineIteratorSP it = dev->createHLineIteratorNG(0, row, m_header->width);
>>>>>>> master
        QVector<QByteArray> channelBytes;

        for (int channel = 0; channel < m_header->nChannels; channel++) {

<<<<<<< HEAD
            io->seek(m_channelInfoRecords[channel].channelDataStart + m_channelOffsets);
            channelBytes.append(io->read(m_header->width*m_channelSize));
            qDebug() << "channel: " << m_channelInfoRecords[channel].channelId << "is at position " << io->pos();

        }
        m_channelOffsets += (m_header->width * m_channelSize);

        qDebug() << "------------------------------------------";
        qDebug() << "channel offset:"<< m_channelOffsets ;
        qDebug() << "------------------------------------------";

        for (int col = 0; col < m_header->width; col++) {
=======

            switch (m_compression) {

            case Compression::Uncompressed:

            {
                io->seek(m_channelInfoRecords[channel].channelDataStart + m_channelOffsets[0]);
                channelBytes.append(io->read(m_header->width*m_channelSize));
            }
                break;

            case Compression::RLE:
            {
                io->seek(m_channelInfoRecords[channel].channelDataStart + m_channelOffsets[channel]);
                int uncompressedLength = m_header->width * m_header->channelDepth / 8;
                QByteArray compressedBytes = io->read(m_channelInfoRecords[channel].rleRowLengths[row]);
                QByteArray uncompressedBytes = Compression::uncompress(uncompressedLength, compressedBytes, m_channelInfoRecords[channel].compressionType);
                channelBytes.append(uncompressedBytes);
                m_channelOffsets[channel] +=  m_channelInfoRecords[channel].rleRowLengths[row];

            }
                break;

            case Compression::ZIP:
                break;

            case Compression::ZIPWithPrediction:
                break;

            default:
                break;
            }

        }

        if (m_channelInfoRecords[channelid].compressionType == 0){
            m_channelOffsets[channelid] += (m_header->width * m_channelSize);
        }

        for (quint32 col = 0; col < m_header->width; col++) {
>>>>>>> master

            if (m_channelSize == 1) {

                quint8 L = ntohs(reinterpret_cast<const quint8 *>(channelBytes[0].constData())[col]);
<<<<<<< HEAD
                KoLabTraits<quint8>::setL(it.rawData(),L);

                quint8 A = ntohs(reinterpret_cast<const quint8 *>(channelBytes[1].constData())[col]);
                KoLabTraits<quint8>::setA(it.rawData(),A);

                quint8 B = ntohs(reinterpret_cast<const quint8 *>(channelBytes[2].constData())[col]);
                KoLabTraits<quint8>::setB(it.rawData(),B);
=======
                KoLabTraits<quint16>::setL(it->rawData(),KoColorSpaceMaths<quint8, quint16 >::scaleToA(L));

                quint8 A = ntohs(reinterpret_cast<const quint8 *>(channelBytes[1].constData())[col]);
                KoLabTraits<quint16>::setA(it->rawData(),KoColorSpaceMaths<quint8, quint16 >::scaleToA(A));

                quint8 B = ntohs(reinterpret_cast<const quint8 *>(channelBytes[2].constData())[col]);
                KoLabTraits<quint16>::setB(it->rawData(),KoColorSpaceMaths<quint8, quint16 >::scaleToA(B));
>>>>>>> master

            }

            else if (m_channelSize == 2) {

                quint16 L = ntohs(reinterpret_cast<const quint16 *>(channelBytes[0].constData())[col]);
<<<<<<< HEAD
                KoLabTraits<quint16>::setL(it.rawData(),L);

                quint16 A = ntohs(reinterpret_cast<const quint16 *>(channelBytes[1].constData())[col]);
                KoLabTraits<quint16>::setA(it.rawData(),A);

                quint16 B = ntohs(reinterpret_cast<const quint16 *>(channelBytes[2].constData())[col]);
                KoLabTraits<quint16>::setB(it.rawData(),B);
=======
                KoLabU16Traits::setL(it->rawData(),L);

                quint16 A = ntohs(reinterpret_cast<const quint16 *>(channelBytes[1].constData())[col]);
                KoLabU16Traits::setA(it->rawData(),A);

                quint16 B = ntohs(reinterpret_cast<const quint16 *>(channelBytes[2].constData())[col]);
                KoLabU16Traits::setB(it->rawData(),B);
>>>>>>> master

            }

            else if (m_channelSize == 4) {

                quint32 L = ntohs(reinterpret_cast<const quint32 *>(channelBytes[0].constData())[col]);
<<<<<<< HEAD
                KoLabTraits<quint32>::setL(it.rawData(),L);

                quint32 A = ntohs(reinterpret_cast<const quint32 *>(channelBytes[1].constData())[col]);
                KoLabTraits<quint32>::setA(it.rawData(),A);

                quint32 B = ntohs(reinterpret_cast<const quint32 *>(channelBytes[2].constData())[col]);
                KoLabTraits<quint32>::setB(it.rawData(),B);

            }

            dev->colorSpace()->setOpacity(it.rawData(), OPACITY_OPAQUE_U8, 1);
            ++it;
=======
                KoLabTraits<quint32>::setL(it->rawData(),L);

                quint32 A = ntohs(reinterpret_cast<const quint32 *>(channelBytes[1].constData())[col]);
                KoLabTraits<quint32>::setA(it->rawData(),A);

                quint32 B = ntohs(reinterpret_cast<const quint32 *>(channelBytes[2].constData())[col]);
                KoLabTraits<quint32>::setB(it->rawData(),B);

            }

            dev->colorSpace()->setOpacity(it->rawData(), OPACITY_OPAQUE_U8, 1);
            it->nextPixel();;
>>>>>>> master
        }

    }

    return true;
<<<<<<< HEAD

=======
>>>>>>> master
}
