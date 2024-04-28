/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012-2014 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DOCXFILE_H
#define DOCXFILE_H

// Qt
#include <QFlags>
#include <QHash>

// Calligra
#include <KoFilter.h>

// This filter
#include "FileCollector.h"

class QString;
class QByteArray;

class KoStore;

class OdtTraverserDocxContext;
class OdfReaderDocxContext;

class DocxFile : public FileCollector
{
public:
    DocxFile();
    ~DocxFile() override;

    /** When you have created all the content and added it using
     * addContentFile(), call this function once and it will write the
     * docx to the disk.
     */
    KoFilter::ConversionStatus
    writeDocx(const QString &fileName, const QByteArray &appIdentification, const OdtTraverserDocxContext &context, bool commentsExist);

    /** When you have created all the content and added it using
     * addContentFile(), call this function once and it will write the
     * docx to the disk. This function is overloaded.
     */
    KoFilter::ConversionStatus writeDocx(const QString &fileName, const QByteArray &appIdentification, const OdfReaderDocxContext &context, bool commentsExist);

private:
    // Private functions
    KoFilter::ConversionStatus writeTopLevelRels(KoStore *docxStore);
    KoFilter::ConversionStatus writeDocumentRels(KoStore *docxStore);

private:
    // data
    bool m_commentsExist;
};

#endif // DOCXFILE_H
