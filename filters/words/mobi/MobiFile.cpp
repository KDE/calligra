/* This file is part of the KDE project
   Copyright (C) 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

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
#include "MobiFile.h"

#include "MobiHeaderGenerator.h"
#include "MobiExportDebug.h"

#include <QFile>
#include <QDataStream>

MobiFile::MobiFile()
{
}

void MobiFile::addContentRawText(const QByteArray &content)
{
    m_textContent = content;
}

void MobiFile:: addContentImage(int id, const QByteArray &content)
{
    m_imageContent.insert(id, content);
}


KoFilter::ConversionStatus MobiFile::writeMobiFile(const QString &outputFile, MobiHeaderGenerator &headerGenerator)
{
    QFile mobi(outputFile);
    if (!mobi.open(QIODevice::WriteOnly)) {
        debugMobi << "Can not create the file";
        return KoFilter::CreationError;
    }

    QDataStream out(&mobi);

    writePalmDataBaseHeader(out, headerGenerator);

    writeRecord0(out, headerGenerator);

    out.device()->write(m_textContent);

    if (!m_imageContent.isEmpty()) {
        // After text blocks finished i need to add two zero bytes to kindle can open images.
        // I don't know why i have seen this in calibre ebooks.
        out << qint8(0);
        out << qint8(0);
        for (int imgId = 1; imgId <= m_imageContent.size(); imgId++) {
            out.device()->write(m_imageContent.value(imgId));
        }
    }

    writeFLISRecord(out, headerGenerator);
    writeFCISRecord(out, headerGenerator);
    writeEndOfFileRecord(out, headerGenerator);

    mobi.close();
    return KoFilter::OK;
}

void MobiFile::writeRecord0(QDataStream &out, MobiHeaderGenerator &headers)
{
    // Write palmDoc Header.
    out <<  headers.m_docHeader->compression;
    out <<  headers.m_docHeader->unused;
    out <<  headers.m_docHeader->textLength;
    out <<  headers.m_docHeader->pdbrecordCount;
    out <<  headers.m_docHeader->maxRecordSize;
    out <<  headers.m_docHeader->encryptionType;
    out <<  headers.m_docHeader->unknown;

    out.device()->write(headers.m_mobiHeader->identifier);

    out <<  headers.m_mobiHeader->mobiHeaderLength;
    out <<  headers.m_mobiHeader->mobiType;
    out <<  headers.m_mobiHeader->textEncoding;
    out <<  headers.m_mobiHeader->uniqueId;
    out <<  headers.m_mobiHeader->fileVersion;
    out <<  headers.m_mobiHeader->ortographicIndex;
    out <<  headers.m_mobiHeader->inflectionIndex;
    out <<  headers.m_mobiHeader->indexNames;
    out <<  headers.m_mobiHeader->indexkeys;
    out <<  headers.m_mobiHeader->extraIndex0;
    out <<  headers.m_mobiHeader->extraIndex1;
    out <<  headers.m_mobiHeader->extraIndex2;
    out <<  headers.m_mobiHeader->extraIndex3;
    out <<  headers.m_mobiHeader->extraIndex4;
    out <<  headers.m_mobiHeader->extraIndex5;
    out <<  headers.m_mobiHeader->firstNonBookIndex;
    out <<  headers.m_mobiHeader->fullNameOffset;
    out <<  headers.m_mobiHeader->fullNameLength;
    out <<  headers.m_mobiHeader->local;
    out <<  headers.m_mobiHeader->inputLanguage;
    out <<  headers.m_mobiHeader->outputLanguage;
    out <<  headers.m_mobiHeader->minversion;
    out <<  headers.m_mobiHeader->firstImageIndex;
    out <<  headers.m_mobiHeader->huffmanRecordOffset;
    out <<  headers.m_mobiHeader->huffmanRecordCount;
    out <<  headers.m_mobiHeader->huffmanTableOffset;
    out <<  headers.m_mobiHeader->huffmanTableLength;
    out <<  headers.m_mobiHeader->EXTH_Flags;
    out <<  headers.m_mobiHeader->unknown1;
    out <<  headers.m_mobiHeader->unknown1_1;
    out <<  headers.m_mobiHeader->unknown1_2;
    out <<  headers.m_mobiHeader->unknown1_3;
    out <<  headers.m_mobiHeader->drmOffset;
    out <<  headers.m_mobiHeader->drmCount;
    out <<  headers.m_mobiHeader->drmSize;
    out <<  headers.m_mobiHeader->drmFlags;
    out <<  headers.m_mobiHeader->unknown2;
    out <<  headers.m_mobiHeader->unknown2_1;
    out <<  headers.m_mobiHeader->firstContentRecordNumber;
    out <<  headers.m_mobiHeader->lastContentRecordNumber;
    out <<  headers.m_mobiHeader->unknown3;
    out <<  headers.m_mobiHeader->FCIS_recordNumber;
    out <<  headers.m_mobiHeader->unknown4;
    out <<  headers.m_mobiHeader->FLIS_recordNumber;
    out <<  headers.m_mobiHeader->unknown5;
    out <<  headers.m_mobiHeader->unknown6;
    out <<  headers.m_mobiHeader->unknown7;
    out <<  headers.m_mobiHeader->unknown8;
    out <<  headers.m_mobiHeader->unknown9;
    out <<  headers.m_mobiHeader->unknown10;
    out <<  headers.m_mobiHeader->extraRecordDataFlags;
    out <<  headers.m_mobiHeader->INDX_recordOffset;

    // EXTH header.
    out.device()->write(headers.m_exthHeader->identifier);

    out <<  qint32(headers.m_exthHeader->headerLength);
    out <<  headers.m_exthHeader->exthRecordCount;
    foreach (qint32 type, headers.m_exthHeader->exthRecord.keys()) {
        out <<  type;
        out <<  qint32((8 + headers.m_exthHeader->exthRecord.value(type).size()));
        out.device()->write(headers.m_exthHeader->exthRecord.value(type));
    }
    // These are extra EXTH records information about ebook.

    // Creator Software
    out << qint32(204);// Record type 204
    out << qint32(12);// Record length
    /* Known Values: 1=mobigen, 2=Mobipocket Creator, 200=kindlegen (Windows),
        201=kindlegen (Linux), 202=kindlegen (Mac).
        Warning: Calibre creates fake creator entries, pretending to be a Linux
        kindlegen 1.2 (201, 1, 2, 33307) for normal ebooks and a non-public Linux
        kindlegen 2.0 (201, 2, 0, 101) for periodicals.
        */
    out << qint32(201);

    // Creator Minor Version
    out << qint32(206); // Record type 206
    out << qint32(12); // Record length
    out << qint32(2); // Mobi Creator Minor version (Minor version of calligra)

    // Creator Major Version
    out << qint32(205); // Record type 205
    out << qint32(12); // Record length
    out << qint32(2); // Mobi Creator Major version (Major version of calligra)

    for (int i = 0; i < headers.m_exthHeader->pad; i++) {
        out << qint8(0);
    }

    // Reminder of record 0.
    out.device()->write(headers.m_dbHeader->title);

    // write pad to make title multiple of 4 bytes.
    int padding = 4 - (headers.m_title.size() % 4);
    for (int i = 0; i < padding; i++) {
        out << qint8(0);
    }

    // Write 2052 null bytes.
    for (int i = 0; i < 2052; i++) {
        out << (qint8)0;
    }
}

void MobiFile::writePalmDataBaseHeader(QDataStream &out, MobiHeaderGenerator &headers)
{
    out.device()->write(headers.m_title);

    for (int i = 0; i < (32 - headers.m_title.size()); i++) {
        out << qint8(0);
    }

    out << headers.m_dbHeader->attributes;
    out << headers.m_dbHeader->version;
    out << headers.m_dbHeader->creationDate;
    out << headers.m_dbHeader->modificationDate;
    out << headers.m_dbHeader->lastBackupDate;
    out << headers.m_dbHeader->modificationNumber;
    out << headers.m_dbHeader->appInfoId;
    out << headers.m_dbHeader->sortInfoId;
    out.device()->write(headers.m_dbHeader->type);
    out.device()->write(headers.m_dbHeader->creator);
    out << headers.m_dbHeader->uniqueIdSeed;
    out << headers.m_dbHeader->nextRecordIdList;
    out << headers.m_dbHeader->numberOfRecords;

    for(int offset = 0; offset < headers.m_dbHeader->recordsInfo.size(); offset++) {
        out << qint32(headers.m_dbHeader->recordsInfo.key(offset));
        out << qint32((2 * offset));
    }

    out << qint16(0);
}

void MobiFile::writeFLISRecord(QDataStream &out, MobiHeaderGenerator &headers)
{
    // The FLIS record appears to have a fixed value. The meaning of the values is not known.
    Q_UNUSED(headers);
    QByteArray id = "FLIS";
    out.device()->write(id);
    out << qint32(8);
    out << qint16(65);
    out << qint16(0);
    out << qint32(0);
    out << qint32(0xFFFFFFFF);
    out << qint16(1);
    out << qint16(3);
    out << qint32(3);
    out << qint32(1);
    out << qint32(0xFFFFFFFF);
}

void MobiFile::writeFCISRecord(QDataStream &out, MobiHeaderGenerator &headers)
{
    // The FCIS record appears to have mostly fixed values.
    QByteArray id = "FCIS";
    out.device()->write(id);
    out << qint32(20);
    out << qint32(16);
    out << qint32(1);
    out << qint32(0);
    out << headers.m_docHeader->textLength;
    out << qint32(0);
    out << qint32(32);
    out << qint32(8);
    out << qint16(1);
    out << qint16(1);
    out << qint32(0);
}

void MobiFile::writeEndOfFileRecord(QDataStream &out, MobiHeaderGenerator &headers)
{
    // The end-of-file record is a fixed 4-byte record.
    Q_UNUSED(headers);
    out << qint8(233);
    out << qint8(142);
    out << qint8(13);
    out << qint8(10);
}
