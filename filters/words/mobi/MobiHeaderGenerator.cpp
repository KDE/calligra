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

#include "MobiHeaderGenerator.h"

#include <QDateTime>
#include <QChar>
#include <QBuffer>
#include <QIODevice>
#include <QDataStream>
#include <QHash>

palmDBHeader::palmDBHeader():
    attributes(0)
  , version(0)
  , lastBackupDate(0)
  , modificationNumber(0)
  , appInfoId(0)
  , sortInfoId(0)
  , nextRecordIdList(0)

{
}

palmDocHeader::palmDocHeader():
    compression(2)
  , unused(0)
  , maxRecordSize(4096)
  , encryptionType(0)
  , unknown(0)
{
}

mobiHeader::mobiHeader():
    mobiHeaderLength(232)
  , mobiType(2)
  , textEncoding(65001)
  , uniqueId(123456789)
  , fileVersion(6)
  , ortographicIndex(0xFFFFFFFF)
  , inflectionIndex(0xFFFFFFFF)
  , indexNames(0xFFFFFFFF)
  , indexkeys(0xFFFFFFFF)
  , extraIndex0(0xFFFFFFFF)
  , extraIndex1(0xFFFFFFFF)
  , extraIndex2(0xFFFFFFFF)
  , extraIndex3(0xFFFFFFFF)
  , extraIndex4(0xFFFFFFFF)
  , extraIndex5(0xFFFFFFFF)
  , local(9)
  , inputLanguage(0)
  , outputLanguage(0)
  , minversion(6)
  , huffmanRecordOffset(0)
  , huffmanRecordCount(0)
  , huffmanTableOffset(0)
  , huffmanTableLength(0)
  , EXTH_Flags(0x50)
  , unknown1(0)
  , unknown1_1(0)
  , unknown1_2(0)
  , unknown1_3(0)
  , drmOffset(0xFFFFFFFF)
  , drmCount(0xFFFFFFFF)
  , drmSize(0)
  , drmFlags(0)
  , unknown2(0)
  , unknown2_1(0)
  , firstContentRecordNumber(1)
  , unknown3(1)
  , unknown4(1)
  , unknown5(1)
  , unknown6(0)
  , unknown7(0xFFFFFFFF)
  , unknown8(0)
  , unknown9(0xFFFFFFFF)
  , unknown10(0xFFFFFFFF)
  , extraRecordDataFlags(0)
  , INDX_recordOffset(0xFFFFFFFF)
{
}

exthHeader::exthHeader():
  exthRecordCount(5)
{
}

MobiHeaderGenerator::MobiHeaderGenerator()
{
}

MobiHeaderGenerator::~MobiHeaderGenerator()
{
}

void MobiHeaderGenerator::generateMobiHeaders(QHash<QString, QString> metaData, int compressedTextSize,
                                              int uncompressedTextSize, QList<int> imagesSize,
                                              QList<qint32> textRecordsOffset)
{
    m_title = metaData.value("title").toUtf8();
    if (m_title.isEmpty()) {
        m_title = QString("Unknown").toUtf8();
    }
    m_author = metaData.value("creator").toUtf8();
    if (m_author.isEmpty()) {
        m_author = QString("Unknown").toUtf8();
    }

    m_rawTextSize = compressedTextSize;
    m_uncompressedTextSize = uncompressedTextSize;
    m_imgListSize = imagesSize;
    m_textRecordsOffset = textRecordsOffset;

    m_exthHeader = new exthHeader;
    m_mobiHeader = new mobiHeader;
    m_dbHeader = new palmDBHeader;
    m_docHeader = new palmDocHeader;

    generateEXTH();
    generatePalmDataBase();
    generatePalmDocHeader();
    generateMobiHeader();
}

void MobiHeaderGenerator::generatePalmDataBase()
{
    m_dbHeader->title = m_title;
    m_dbHeader->type = "BOOK";
    m_dbHeader->creator = "MOBI";

    // set creation date
    // seconds since start of January 1, 1970
    QDateTime date = QDateTime::currentDateTime();

    qint32 pdTime = date.toTime_t();
    m_dbHeader->creationDate = pdTime;
    m_dbHeader->modificationDate = pdTime;

    qint16 recordsCount = qint16(calculateRecordsCount());

    m_dbHeader->uniqueIdSeed = (2 * recordsCount) - 1;
    m_dbHeader->nextRecordIdList = 0;
    m_dbHeader->numberOfRecords = recordsCount;

    m_dbHeader->headerLength = (78 + (calculateRecordsCount() * 8)) + 2; // 2 gap zero to make
                                                                        //a multiple of 4

    // I want to set offset but first i need the sizes.
    int recordId = 0;
    // record 0
    m_dbHeader->recordOffset = m_dbHeader->headerLength;
    m_dbHeader->recordUniqueId = recordId;
    m_dbHeader->recordsInfo.insert(m_dbHeader->recordOffset, m_dbHeader->recordUniqueId);
    recordId++;
    // text record.

    // palmDBHeader length + palmDoc header length (16) + MobiHeader length + EXTH header length
    // + EHTH header padding + book name (title) length + padding
    // *(padding are for to make it multiple of four bytes)
    // + 2052 bytes (I don't know what exactly it is for but i see it in
    // every file that i have made it by mobi packet creator)
    // + 1 ( to point the first character of text)

    m_dbHeader->recordOffset = qint32((m_dbHeader->headerLength + 16
                                       + m_mobiHeader->mobiHeaderLength
                                       + m_exthHeader->headerLength + m_exthHeader->pad
                                       + m_title.size() + (4 - (m_title.size() % 4)) + 2052));


    m_dbHeader->recordsInfo.insert(m_dbHeader->recordOffset, recordId);
    qint32 temp = m_dbHeader->recordOffset;
    recordId++;
    // "i" ?: I have added a zero byte between block texts, so record offset has gone to forward
    // after each insert.
    int i;
    for (i = 1; i < m_textRecordsOffset.size(); i++) {
        m_dbHeader->recordOffset = m_textRecordsOffset.at(i) + temp;
        m_dbHeader->recordOffset += qint32(i);
        m_dbHeader->recordsInfo.insert(m_dbHeader->recordOffset, recordId);
        recordId++;
    }
    m_dbHeader->recordOffset = (qint32(m_rawTextSize)) + temp + qint32(i - 1);
    // Each image is just a record and images records can be more that 4096.
    if (!m_imgListSize.isEmpty()) {
        m_dbHeader->recordOffset += qint32(1);
        m_dbHeader->recordUniqueId = recordId;
        m_dbHeader->recordsInfo.insert(m_dbHeader->recordOffset, m_dbHeader->recordUniqueId);
        m_dbHeader->recordOffset += qint32(1);
        recordId++;
        foreach (int imgSize, m_imgListSize) {
                // Our image has just one record.
                m_dbHeader->recordUniqueId = recordId;
                m_dbHeader->recordsInfo.insert(m_dbHeader->recordOffset, m_dbHeader->recordUniqueId);
                m_dbHeader->recordOffset += qint32(imgSize);
                recordId++;
            }
    }
    // FLIS record.
    m_dbHeader->recordUniqueId = recordId;
    m_dbHeader->recordsInfo.insert(m_dbHeader->recordOffset, m_dbHeader->recordUniqueId);
    m_dbHeader->recordOffset += qint32(36); // FLIS record size.
    recordId++;
    // FCIS record.
    m_dbHeader->recordUniqueId = recordId;
    m_dbHeader->recordsInfo.insert(m_dbHeader->recordOffset, m_dbHeader->recordUniqueId);
    m_dbHeader->recordOffset += (qint32)44; // FCIS record size.
    recordId++;
    // End record.
    m_dbHeader->recordUniqueId = recordId;
    m_dbHeader->recordsInfo.insert(m_dbHeader->recordOffset, m_dbHeader->recordUniqueId);
}

void MobiHeaderGenerator::generatePalmDocHeader()
{
    m_docHeader->textLength = m_uncompressedTextSize;
    m_docHeader->pdbrecordCount = qint16(m_textRecordsOffset.size());
}

void MobiHeaderGenerator::generateEXTH()
{
    m_exthHeader->identifier = "EXTH";
    // Record type aouthpr 100 <dc:creator>
    m_exthHeader->exthRecord.insert(100, m_author);
    // Record type Contributor 108
    QByteArray contributor = QString("Calligra Author [http://calligra.org]").toUtf8();
    m_exthHeader->exthRecord.insert(108, contributor);
    // Record type Source 112
    QDateTime dateTime = QDateTime::currentDateTime();
    QDate date = dateTime.date();
    QByteArray source = date.toString("yyyy-MM-dd").toUtf8() +
                       dateTime.toUTC().time().toString("hh:mm:ss").toUtf8();
    m_exthHeader->exthRecord.insert(112, source);
    // 4 bytes identifier, 4 bytes header length,
    // 4 bytes record count, two record type each one 4 bytes (2 * 4)
    // two record length (2 * 4)
    // publisher size, author size
    m_exthHeader->headerLength = 4 + 4 + 4 + (6 * 2 * 4) + contributor.size() + source.size() +
                                 4 + 4 + 4 + m_author.size();
    // Null bytes to pad the EXTH header to a multiple of four bytes (none if the header is already
    //a multiple of four). This padding is not included in the EXTH header length.
    m_exthHeader->pad = 4 - (m_exthHeader->headerLength % 4);
}

void MobiHeaderGenerator::generateMobiHeader()
{
    m_mobiHeader->identifier =  "MOBI";
    if (!m_imgListSize.isEmpty()) {
        // 2 ( record 0 and first text block)
        m_mobiHeader->firstNonBookIndex = 2 + m_textRecordsOffset.size();
        m_mobiHeader->firstImageIndex = 2 + m_textRecordsOffset.size();
    }
    else {
        // Point to FLIS record
        m_mobiHeader->firstNonBookIndex = calculateRecordsCount() - 3;
        m_mobiHeader->firstImageIndex = calculateRecordsCount() - 3;
    }


    m_mobiHeader->fullNameOffset = 16 + m_mobiHeader->mobiHeaderLength
                                    + m_exthHeader->headerLength + m_exthHeader->pad;
    m_mobiHeader->fullNameLength = m_title.size();
    // calculateRecordsCount() - 3 (FLIS, FCIS, end of file)
    m_mobiHeader->lastContentRecordNumber = calculateRecordsCount() - 4;
    m_mobiHeader->FLIS_recordNumber = calculateRecordsCount() - 3;
    m_mobiHeader->FCIS_recordNumber = calculateRecordsCount() - 2;
}

int MobiHeaderGenerator::calculateRecordsCount()
{
    // the first record is record 0 include MOBI header and EXTH header
    int recordsCount = 1;
    recordsCount += m_textRecordsOffset.size();
    // Images records
    recordsCount += m_imgListSize.size();
    // Before first we add a record include two bytes zero.
    if (!m_imgListSize.isEmpty()) {
        recordsCount ++;
    }
    // FLIS record and FCIS and end of file record
    recordsCount += 3;
    return recordsCount;
}
