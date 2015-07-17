/* This file is part of the KDE project
   Copyright (C) 2005,2006 Jarosław Staniek <staniek@kde.org>

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

#include "kexicsvexport.h"
#include "kexicsvwidgets.h"
#include <core/KexiMainWindowIface.h>
#include <core/kexiproject.h>
#include <core/kexipartinfo.h>
#include <core/kexipartmanager.h>
#include <core/kexiguimsghandler.h>
#include <kexiutils/utils.h>
#include <widget/kexicharencodingcombobox.h>

#include <KDbCursor>
#include <KDbUtils>
#include <KDbTableOrQuerySchema>

#include <KLocalizedString>

#include <QApplication>
#include <QTextStream>
#include <QCheckBox>
#include <QClipboard>
#include <QDebug>
#include <QSaveFile>

using namespace KexiCSVExport;

Options::Options()
        : mode(File), itemId(0), addColumnNames(true)
{
}

bool Options::assign(QMap<QString, QString> *args)
{
    mode = (args->value("destinationType") == "file")
           ? KexiCSVExport::File : KexiCSVExport::Clipboard;

    if (args->contains("delimiter"))
        delimiter = args->value("delimiter");
    else
        delimiter = (mode == File) ? KEXICSV_DEFAULT_FILE_DELIMITER : KEXICSV_DEFAULT_CLIPBOARD_DELIMITER;

    if (args->contains("textQuote"))
        textQuote = args->value("textQuote");
    else
        textQuote = (mode == File) ? KEXICSV_DEFAULT_FILE_TEXT_QUOTE : KEXICSV_DEFAULT_CLIPBOARD_TEXT_QUOTE;

    bool ok;
    itemId = args->value("itemId").toInt(&ok);
    if (!ok || itemId == 0) //neverSaved items are supported
        return false;
    if (args->contains("forceDelimiter"))
        forceDelimiter = args->value("forceDelimiter");
    if (args->contains("addColumnNames"))
        addColumnNames = (args->value("addColumnNames") == "1");
    useTempQuery = (args->value("useTempQuery") == "1");
    return true;
}

//------------------------------------

bool KexiCSVExport::exportData(KDbTableOrQuerySchema *tableOrQuery,
                               const Options& options, int recordCount, QTextStream *predefinedTextStream)
{
    KDbConnection* conn = tableOrQuery->connection();
    if (!conn)
        return false;

    KDbQuerySchema* query = tableOrQuery->query();
    QList<QVariant> queryParams;
    if (!query) {
        query = tableOrQuery->table()->query();
    }
    else {
        queryParams = KexiMainWindowIface::global()->currentParametersForQuery(query->id());
    }

    if (recordCount == -1)
        recordCount = KDb::recordCount(tableOrQuery, queryParams);
    if (recordCount == -1)
        return false;

//! @todo move this to non-GUI location so it can be also used via command line
//! @todo add a "finish" page with a progressbar.
//! @todo look at recordCount whether the data is really large;
//!       if so: avoid copying to clipboard (or ask user) because of system memory

//! @todo OPTIMIZATION: use fieldsExpanded(true /*UNIQUE*/)
//! @todo OPTIMIZATION? (avoid multiple data retrieving) look for already fetched data within KexiProject..

    KDbQueryColumnInfo::Vector fields(query->fieldsExpanded(KDbQuerySchema::WithInternalFields));
    QString buffer;

    QScopedPointer<QSaveFile> kSaveFile;
    QTextStream *stream = 0;
    QScopedPointer<QTextStream> kSaveFileTextStream;

    const bool copyToClipboard = options.mode == Clipboard;
    if (copyToClipboard) {
//! @todo (during exporting): enlarge bufSize by factor of 2 when it became too small
        int bufSize = qMin((recordCount < 0 ? 10 : recordCount) * fields.count() * 20, 128000);
        buffer.reserve(bufSize);
        if (buffer.capacity() < bufSize) {
            qWarning() << "Cannot allocate memory for " << bufSize
            << " characters";
            return false;
        }
    } else {
        if (predefinedTextStream) {
            stream = predefinedTextStream;
        } else {
            if (options.fileName.isEmpty()) {//sanity
                qWarning() << "Fname is empty";
                return false;
            }
            kSaveFile.reset(new QSaveFile(options.fileName));

            qDebug() << "QSaveFile Filename:" << kSaveFile->fileName();

            if (kSaveFile->open(QIODevice::WriteOnly)) {
                kSaveFileTextStream.reset(new QTextStream(kSaveFile.data()));
                stream = kSaveFileTextStream.data();
                qDebug() << "have a stream";
            }
            if (QFileDevice::NoError != kSaveFile->error() || !stream) {//sanity
                qWarning() << "Status != 0 or stream == 0";
//! @todo show error
                return false;
            }
        }
    }

//! @todo escape strings

#define _ERR \
    if (kSaveFile) { kSaveFile->cancelWriting(); } \
    return false

#define APPEND(what) \
    if (copyToClipboard) buffer.append(what); else (*stream) << (what)

// use native line ending for copying, RFC 4180 one for saving to file
#define APPEND_EOLN \
    if (copyToClipboard) { APPEND('\n'); } else { APPEND("\r\n"); }

    qDebug() << 0 << "Columns: " << query->fieldsExpanded().count();
    // 0. Cache information
    const int fieldsCount = query->fieldsExpanded().count(); //real fields count without internals
    const QByteArray delimiter(options.delimiter.left(1).toLatin1());
    const bool hasTextQuote = !options.textQuote.isEmpty();
    const QString textQuote(options.textQuote.left(1));
    const QByteArray escapedTextQuote((textQuote + textQuote).toLatin1());   //ok?
    //cache for faster checks
    QScopedArrayPointer<bool> isText(new bool[fieldsCount]);
    QScopedArrayPointer<bool> isDateTime(new bool[fieldsCount]);
    QScopedArrayPointer<bool> isTime(new bool[fieldsCount]);
    QScopedArrayPointer<bool> isBLOB(new bool[fieldsCount]);
    QScopedArrayPointer<int> visibleFieldIndex(new int[fieldsCount]);
// bool isInteger[fieldsCount]; //cache for faster checks
// bool isFloatingPoint[fieldsCount]; //cache for faster checks
    for (int i = 0; i < fieldsCount; i++) {
        KDbQueryColumnInfo* ci;
        const int indexForVisibleLookupValue = fields[i]->indexForVisibleLookupValue();
        if (-1 != indexForVisibleLookupValue) {
            ci = query->expandedOrInternalField(indexForVisibleLookupValue);
            visibleFieldIndex[i] = indexForVisibleLookupValue;
        } else {
            ci = fields[i];
            visibleFieldIndex[i] = i;
        }

        isText[i] = ci->field->isTextType();
        isDateTime[i] = ci->field->type() == KDbField::DateTime;
        isTime[i] = ci->field->type() == KDbField::Time;
        isBLOB[i] = ci->field->type() == KDbField::BLOB;
//  isInteger[i] = fields[i]->field->isIntegerType()
//   || fields[i]->field->type()==KDbField::Boolean;
//  isFloatingPoint[i] = fields[i]->field->isFPNumericType();
    }

    // 1. Output column names
    if (options.addColumnNames) {
        for (int i = 0; i < fieldsCount; i++) {
            qDebug() << "Adding column names";
            if (i > 0) {
                APPEND(delimiter);
            }

            if (hasTextQuote) {
                APPEND(textQuote + fields[i]->captionOrAliasOrName().replace(textQuote, escapedTextQuote) + textQuote);
            } else {
                APPEND(fields[i]->captionOrAliasOrName());
            }
        }
        APPEND_EOLN
    }

    KexiGUIMessageHandler handler;
    KDbCursor *cursor = conn->executeQuery(query, queryParams);
    if (!cursor) {
        handler.showErrorMessage(conn->result());
        _ERR;
    }
    for (cursor->moveFirst(); !cursor->eof() && !cursor->result().isError(); cursor->moveNext()) {
        //qDebug() << "Adding records";
        const int realFieldCount = qMin(cursor->fieldCount(), fieldsCount);
        for (int i = 0; i < realFieldCount; i++) {
            const int real_i = visibleFieldIndex[i];
            if (i > 0) {
                APPEND(delimiter);
            }

            if (cursor->value(real_i).isNull()) {
                continue;
            }

            if (isText[real_i]) {
                if (hasTextQuote)
                    APPEND(textQuote + QString(cursor->value(real_i).toString()).replace(textQuote, escapedTextQuote) + textQuote);
                else
                    APPEND(cursor->value(real_i).toString());
            } else if (isDateTime[real_i]) { //avoid "T" in ISO DateTime
                APPEND(cursor->value(real_i).toDateTime().date().toString(Qt::ISODate) + " "
                       + cursor->value(real_i).toDateTime().time().toString(Qt::ISODate));
            } else if (isTime[real_i]) { //time is temporarily stored as null date + time...
                APPEND(cursor->value(real_i).toTime().toString(Qt::ISODate));
            } else if (isBLOB[real_i]) { //BLOB is escaped in a special way
                if (hasTextQuote)
//! @todo add options to suppport other types from KDbBLOBEscapingType enum...
                    APPEND(textQuote + KDb::escapeBLOB(cursor->value(real_i).toByteArray(), KDb::BLOBEscapeHex) + textQuote);
                else
                    APPEND(KDb::escapeBLOB(cursor->value(real_i).toByteArray(), KDb::BLOBEscapeHex));
            } else {//other types
                APPEND(cursor->value(real_i).toString());
            }
        }
        APPEND_EOLN
    }

    if (copyToClipboard)
        buffer.squeeze();

    if (!conn->deleteCursor(cursor)) {
        handler.showErrorMessage(conn->result());
        _ERR;
    }

    if (copyToClipboard)
        QApplication::clipboard()->setText(buffer, QClipboard::Clipboard);

    qDebug() << "Done";

    if (kSaveFile) {
        stream->flush();
        if (!kSaveFile->commit()) {
                qDebug() << "Error commiting the file" << kSaveFile->fileName();
        }
    }
    return true;
}
