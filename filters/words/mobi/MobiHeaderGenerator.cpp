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
#include <kdebug.h>
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
  , textEncoding(1252)
  , uniqueId(123456789)
  , fileVersion(4)
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
  , minversion(4)
  , huffmanRecordOffset(0)
  , huffmanRecordCount(0)
  , huffmanTableOffset(0)
  , huffmanTableLength(0)
  , EXTH_Flags(0x40)
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
  exthRecordCount(2)
{
}
MobiHeaderGenerator::MobiHeaderGenerator()
{
}

MobiHeaderGenerator::~MobiHeaderGenerator()
{
}

void MobiHeaderGenerator::generateMobiHeaders(QHash<QString, QString> metaData, int mHtmlFileSize, QList<int> imagesSize)
{
    m_title = metaData.value("title").toUtf8();
    if (m_title.isEmpty()) {
        m_title = QString("Unknown").toUtf8();
    }
    m_author = metaData.value("creator").toUtf8();
    if (m_author.isEmpty()) {
        m_author = QString("Unknown").toUtf8();
    }

    m_rawTextSize = mHtmlFileSize;
    m_imgListSize = imagesSize;

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
    kDebug(30517) << date.toTime_t();

    m_dbHeader->creationDate = pdTime;
    m_dbHeader->modificationDate = pdTime;

    qint16 recordsCount = (qint16)calculateRecordsCount();

    m_dbHeader->uniqueIdSeed = recordsCount;
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
    // + EHTH header padding + book name (title) lenght + padding
    // *(padding are for to make it multiple of four bytes)
    // + 2052 bytes (I dont know what exactly it is for but i see it in
    // every file that i have made it by mobi packet creator)
    // + 1 ( to point the first character of text)
    m_dbHeader->recordOffset = m_dbHeader->headerLength + 16 + m_mobiHeader->mobiHeaderLength
                               +m_exthHeader->headerLength + m_exthHeader->pad
                               + m_title.size() + (4 - (m_title.size() % 4)) + 2052;

    if (m_rawTextSize != 0) {
        if (m_rawTextSize <= 4096) {
            // Our Text has just one record.
            m_dbHeader->recordUniqueId = recordId;
            m_dbHeader->recordsInfo.insert(m_dbHeader->recordOffset, m_dbHeader->recordUniqueId);
            m_dbHeader->recordOffset += m_rawTextSize;
            recordId++;
        }
        else {
            // Our Text has more than one record.
            int records = m_rawTextSize / 4096;

            // For the first record.
            m_dbHeader->recordUniqueId = recordId;
            m_dbHeader->recordsInfo.insert(m_dbHeader->recordOffset, m_dbHeader->recordUniqueId);
            recordId++;

            for (int i = 1; i < records; i++) {
                m_dbHeader->recordOffset += 4096;
                m_dbHeader->recordUniqueId = recordId;
                m_dbHeader->recordsInfo.insert(m_dbHeader->recordOffset, m_dbHeader->recordUniqueId);
                recordId++;
            }

            // For the remain of text.
            m_dbHeader->recordOffset += 4096;
            m_dbHeader->recordUniqueId = recordId;
            m_dbHeader->recordsInfo.insert(m_dbHeader->recordOffset, m_dbHeader->recordUniqueId);
            recordId++;

            m_dbHeader->recordOffset += m_rawTextSize % 4096;
        }
    }

    // Each image is just a record and images records can be more that 4096.
    if (!m_imgListSize.isEmpty()) {
        foreach (int imgSize, m_imgListSize) {
                // Our image has just one record.
                m_dbHeader->recordUniqueId = recordId;
                m_dbHeader->recordsInfo.insert(m_dbHeader->recordOffset, m_dbHeader->recordUniqueId);
                m_dbHeader->recordOffset += imgSize;
                recordId++;
            }
    }

//    if (m_imgListSize.isEmpty()) {
//        m_dbHeader->recordUniqueId = recordId;
//        m_dbHeader->recordsInfo.insert((m_dbHeader->recordOffset + 1), m_dbHeader->recordUniqueId);
//        m_dbHeader->recordOffset = m_dbHeader->recordOffset + 2;
//        m_mobiHeader->firstImageIndex = recordId;
//        recordId++;
//    }

    // FLIS record.
    m_dbHeader->recordUniqueId = recordId;
    m_dbHeader->recordsInfo.insert(m_dbHeader->recordOffset, m_dbHeader->recordUniqueId);
    m_dbHeader->recordOffset += 36; // FLIS record size.
    recordId++;

    // FCIS record.
    m_dbHeader->recordUniqueId = recordId;
    m_dbHeader->recordsInfo.insert(m_dbHeader->recordOffset, m_dbHeader->recordUniqueId);
    m_dbHeader->recordOffset += 44; // FCIS record size.
    recordId++;

    // End record.
    m_dbHeader->recordUniqueId = recordId;
    m_dbHeader->recordsInfo.insert(m_dbHeader->recordOffset, m_dbHeader->recordUniqueId);
}

void MobiHeaderGenerator::generatePalmDocHeader()
{
    m_docHeader->textLength = m_rawTextSize;

    int textRecordCount = 0;
    if (m_rawTextSize <= 4096) {
        textRecordCount ++;
    }
    else {
        textRecordCount += m_rawTextSize / 4096;
        if ((m_rawTextSize % 4096) > 0) {
            textRecordCount ++;
        }
    }
    m_docHeader->recordCount =(qint16)textRecordCount;
}

void MobiHeaderGenerator::generateEXTH()
{
    m_exthHeader->identifier = "EXTH";

    // Record type aouthpr 100 <dc:creator>
    m_exthHeader->exthRecord.insert(100, m_author);
    // Record type publisher 101 <dc:publisher>
    QByteArray publisher = QString("Calligra Author").toUtf8();
    m_exthHeader->exthRecord.insert(101, publisher);

    // 4 bytes identifier, 4 bytes header length,
    // 4 bytes record count, two record type each one 4 bytes (2 * 4)
    // two record lenght (2 * 4)
    // publisher size, author size
    m_exthHeader->headerLength = 4 + 4 + 4 + (2 * 4) + (2 * 4) + publisher.size() + m_author.size();

    // Null bytes to pad the EXTH header to a multiple of four bytes (none if the header is already
    //a multiple of four). This padding is not included in the EXTH header length.
    m_exthHeader->pad = 4 - (m_exthHeader->headerLength % 4);
}

void MobiHeaderGenerator::generateMobiHeader()
{
    m_mobiHeader->identifier =  "MOBI";

    if (!m_imgListSize.isEmpty()) {
        m_mobiHeader->firstNonBookIndex = 2;
        m_mobiHeader->firstImageIndex = 2;
    }
    else {
        m_mobiHeader->firstNonBookIndex = calculateRecordsCount() - 3;
        m_mobiHeader->firstImageIndex = calculateRecordsCount() - 3;
    }

    //
    m_mobiHeader->fullNameOffset = 16 + m_mobiHeader->mobiHeaderLength
                                    + m_exthHeader->headerLength + m_exthHeader->pad;
    m_mobiHeader->fullNameLength = m_title.size();

    // calculateRecordsCount() - 3 (FLIS, FCIS, end of file)
    m_mobiHeader->lastContentRecordNumber = calculateRecordsCount() - 3;
    m_mobiHeader->FLIS_recordNumber = calculateRecordsCount() - 2;
    m_mobiHeader->FCIS_recordNumber = calculateRecordsCount() - 1;
}

int MobiHeaderGenerator::calculateRecordsCount()
{
    // the first record is record 0 include MOBI header and EXTH header
    int recordsCount = 1;
    // Then there is text but you should care that the max record size is 4096
    if (m_rawTextSize <= 4096) {
        recordsCount ++;
    }
    else {
        recordsCount += m_rawTextSize / 4096;
        if ((m_rawTextSize % 4096) > 0) {
            recordsCount ++;
        }
    }

    // Images records
    recordsCount += m_imgListSize.size();

    // FLIS record and FCIS and end of file record
    recordsCount += 3;

    return recordsCount;
}
