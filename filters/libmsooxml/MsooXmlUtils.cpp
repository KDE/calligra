/*
 * This file is part of Office 2007 Filters for Calligra
 * Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
 * Copyright (c) 2003 Lukas Tinkl <lukas@kde.org>
 * Copyright (C) 2003 David Faure <faure@kde.org>
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
 * Contact: Suresh Chande suresh.chande@nokia.com
 * Copyright (C) 2011 Matus Uzak <matus.uzak@ixonos.com>
 *
 * Utils::columnName() based on Cell::columnName() from calligra/kspread/Utils.cpp:
 * Copyright 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 * Copyright 2004 Tomas Mecir <mecirt@gmail.com>
 * Copyright 1999-2002,2004 Laurent Montel <montel@kde.org>
 * Copyright 2002,2004 Ariya Hidayat <ariya@kde.org>
 * Copyright 2002-2003 Norbert Andres <nandres@web.de>
 * Copyright 2003 Stefan Hetzl <shetzl@chello.at>
 * Copyright 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
 * Copyright 2002 Harri Porten <porten@kde.org>
 * Copyright 2002 John Dailey <dailey@vt.edu>
 * Copyright 1999-2001 David Faure <faure@kde.org>
 * Copyright 2000-2001 Werner Trobin <trobin@kde.org>
 * Copyright 2000 Simon Hausmann <hausmann@kde.org
 * Copyright 1998-1999 Torben Weis <weis@kde.org>
 * Copyright 1999 Michael Reiher <michael.reiher@gmx.de>
 * Copyright 1999 Reginald Stadlbauer <reggie@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "MsooXmlUtils.h"
#include "MsooXmlUnits.h"
#include "MsooXmlContentTypes.h"
#include "MsooXmlSchemas.h"
#include "MsooXmlReader.h"
#include "MsooXmlDebug.h"

#include "ooxml_pole.h"

#include <styles/KoCharacterStyle.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoUnit.h>

#include <klocalizedstring.h>
#include <kzip.h>

#include <QGlobalStatic>
#include <QDomDocument>
#include <QColor>
#include <QBrush>
#include <QImage>
#include <QImageReader>
#include <QPalette>
#include <QRegExp>


#include <memory>

// common officedocument content types
const char MSOOXML::ContentTypes::coreProps[] =            "application/vnd.openxmlformats-package.core-properties+xml";
const char MSOOXML::ContentTypes::extProps[] =             "application/vnd.openxmlformats-officedocument.extended-properties+xml";
const char MSOOXML::ContentTypes::theme[] =                "application/vnd.openxmlformats-officedocument.theme+xml";

// wordprocessingml-specific content types
const char MSOOXML::ContentTypes::wordDocument[] =         "application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml";
const char MSOOXML::ContentTypes::wordSettings[] =         "application/vnd.openxmlformats-officedocument.wordprocessingml.settings+xml";
const char MSOOXML::ContentTypes::wordStyles[] =           "application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml";
const char MSOOXML::ContentTypes::wordHeader[] =           "application/vnd.openxmlformats-officedocument.wordprocessingml.header+xml";
const char MSOOXML::ContentTypes::wordFooter[] =           "application/vnd.openxmlformats-officedocument.wordprocessingml.footer+xml";
const char MSOOXML::ContentTypes::wordFootnotes[] =        "application/vnd.openxmlformats-officedocument.wordprocessingml.footnotes+xml";
const char MSOOXML::ContentTypes::wordEndnotes[] =         "application/vnd.openxmlformats-officedocument.wordprocessingml.endnotes+xml";
const char MSOOXML::ContentTypes::wordFontTable[] =        "application/vnd.openxmlformats-officedocument.wordprocessingml.fontTable+xml";
const char MSOOXML::ContentTypes::wordWebSettings[] =      "application/vnd.openxmlformats-officedocument.wordprocessingml.webSettings+xml";
const char MSOOXML::ContentTypes::wordTemplate[] =         "application/vnd.openxmlformats-officedocument.wordprocessingml.template.main+xml";
const char MSOOXML::ContentTypes::wordComments[] =         "application/vnd.openxmlformats-officedocument.wordprocessingml.comments+xml";

// presentationml-specific content types
const char MSOOXML::ContentTypes::presentationDocument[] =      "application/vnd.openxmlformats-officedocument.presentationml.presentation.main+xml";
const char MSOOXML::ContentTypes::presentationSlide[] =         "application/vnd.openxmlformats-officedocument.presentationml.slide+xml";
const char MSOOXML::ContentTypes::presentationSlideLayout[] =   "application/vnd.openxmlformats-officedocument.presentationml.slideLayout+xml";
const char MSOOXML::ContentTypes::presentationSlideShow[] =     "application/vnd.openxmlformats-officedocument.presentationml.slideshow.main+xml";
const char MSOOXML::ContentTypes::presentationTemplate[] =      "application/vnd.openxmlformats-officedocument.presentationml.template.main+xml";
const char MSOOXML::ContentTypes::presentationNotes[] =         "application/vnd.openxmlformats-officedocument.presentationml.notesMaster+xml";
const char MSOOXML::ContentTypes::presentationTableStyles[] =   "application/vnd.openxmlformats-officedocument.presentationml.tableStyles+xml";
const char MSOOXML::ContentTypes::presentationProps[] =         "application/vnd.openxmlformats-officedocument.presentationml.presProps+xml";
const char MSOOXML::ContentTypes::presentationViewProps[] =     "application/vnd.openxmlformats-officedocument.presentationml.viewProps+xml";
const char MSOOXML::ContentTypes::presentationComments[] =      "application/vnd.openxmlformats-officedocument.presentationml.comments+xml";

// spreadsheetml-specific content types
const char MSOOXML::ContentTypes::spreadsheetDocument[] =        "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml";
const char MSOOXML::ContentTypes::spreadsheetMacroDocument[] =   "application/vnd.ms-excel.sheet.macroEnabled.main+xml";
const char MSOOXML::ContentTypes::spreadsheetPrinterSettings[] = "application/vnd.openxmlformats-officedocument.spreadsheetml.printerSettings";
const char MSOOXML::ContentTypes::spreadsheetStyles[] =          "application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml";
const char MSOOXML::ContentTypes::spreadsheetWorksheet[] =       "application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml";
const char MSOOXML::ContentTypes::spreadsheetCalcChain[] =       "application/vnd.openxmlformats-officedocument.spreadsheetml.calcChain+xml";
const char MSOOXML::ContentTypes::spreadsheetSharedStrings[] =   "application/vnd.openxmlformats-officedocument.spreadsheetml.sharedStrings+xml";
const char MSOOXML::ContentTypes::spreadsheetTemplate[] =        "application/vnd.openxmlformats-officedocument.spreadsheetml.template.main+xml";
const char MSOOXML::ContentTypes::spreadsheetComments[] =        "application/vnd.openxmlformats-officedocument.spreadsheetml.comments+xml";

//generic namespaces
const char MSOOXML::Schemas::dublin_core[] =                                "http://purl.org/dc/elements/1.1/";

// common namespaces
const char MSOOXML::Schemas::contentTypes[] =                               "http://schemas.openxmlformats.org/package/2006/content-types";

const char MSOOXML::Schemas::relationships[] =                              "http://schemas.openxmlformats.org/package/2006/relationships";
const char MSOOXML::Schemas::core_properties[] =                            "http://schemas.openxmlformats.org/package/2006/metadata/core-properties";

// ISO/IEC 29500-1:2008(E), Annex A. (normative), p. 4355
// See also: specs/all.xsd
// A.1 WordprocessingML
const char MSOOXML::Schemas::wordprocessingml[] =                           "http://schemas.openxmlformats.org/wordprocessingml/2006/main";

// A.2 SpreadsheetML
const char MSOOXML::Schemas::spreadsheetml[] =                              "http://schemas.openxmlformats.org/spreadsheetml/2006/main";

// A.3 PresentationML
const char MSOOXML::Schemas::presentationml[] =                             "http://schemas.openxmlformats.org/presentationml/2006/main";

// A.4 DrawingML - Framework
const char MSOOXML::Schemas::drawingml::main[] =                            "http://schemas.openxmlformats.org/drawingml/2006/main";
const char MSOOXML::Schemas::drawingml::wordprocessingDrawing[] =           "http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing";
const char MSOOXML::Schemas::drawingml::spreadsheetDrawing[] =              "http://schemas.openxmlformats.org/drawingml/2006/spreadsheetDrawing";
const char MSOOXML::Schemas::drawingml::compatibility[] =                   "http://schemas.openxmlformats.org/drawingml/2006/compatibility";
const char MSOOXML::Schemas::drawingml::lockedCanvas[] =                    "http://schemas.openxmlformats.org/drawingml/2006/lockedCanvas";
const char MSOOXML::Schemas::drawingml::picture[] =                         "http://schemas.openxmlformats.org/drawingml/2006/picture";

// A.5 DrawingML - Components
const char MSOOXML::Schemas::drawingml::chart[] =                           "http://schemas.openxmlformats.org/drawingml/2006/chart";
const char MSOOXML::Schemas::drawingml::chartDrawing[] =                    "http://schemas.openxmlformats.org/drawingml/2006/chartDrawing";
const char MSOOXML::Schemas::drawingml::diagram[] =                         "http://schemas.openxmlformats.org/drawingml/2006/diagram";

// A.6 Shared MLs
const char MSOOXML::Schemas::officeDocument::math[] =                       "http://schemas.openxmlformats.org/officeDocument/2006/math";
const char MSOOXML::Schemas::officeDocument::bibliography[] =               "http://schemas.openxmlformats.org/officeDocument/2006/bibliography";
const char MSOOXML::Schemas::officeDocument::characteristics[] =            "http://schemas.openxmlformats.org/officeDocument/2006/characteristics";
const char MSOOXML::Schemas::officeDocument::customXml[] =                  "http://schemas.openxmlformats.org/officeDocument/2006/customXml";
const char MSOOXML::Schemas::officeDocument::custom_properties[] =          "http://schemas.openxmlformats.org/officeDocument/2006/custom-properties";
const char MSOOXML::Schemas::officeDocument::docPropsVTypes[] =             "http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes";
const char MSOOXML::Schemas::officeDocument::extended_properties[] =        "http://schemas.openxmlformats.org/officeDocument/2006/extended-properties";
const char MSOOXML::Schemas::officeDocument::relationships[] =              "http://schemas.openxmlformats.org/officeDocument/2006/relationships";
const char MSOOXML::Schemas::officeDocument::sharedTypes[] =                "http://schemas.openxmlformats.org/officeDocument/2006/sharedTypes";

// A.7 Custom XML Schema References
const char MSOOXML::Schemas::schemaLibrary[] =                              "http://schemas.openxmlformats.org/schemaLibrary/2006/main";

// Marks that the value has not been modified;
static const char UNUSED[] = "UNUSED";

using namespace MSOOXML;

//-----------------------------------------

KoFilter::ConversionStatus Utils::loadAndParse(QIODevice* io, KoXmlDocument& doc,
        QString& errorMessage, const QString & fileName)
{
    errorMessage.clear();

    QString errorMsg;
    int errorLine, errorColumn;
    bool ok = doc.setContent(io, true, &errorMsg, &errorLine, &errorColumn);
    if (!ok) {
        errorMsooXml << "Parsing error in " << fileName << ", aborting!" << endl
        << " In line: " << errorLine << ", column: " << errorColumn << endl
        << " Error message: " << errorMsg;
        errorMessage = i18n("Parsing error in the main document at line %1, column %2.\nError message: %3", errorLine , errorColumn , errorMsg);
        return KoFilter::ParsingError;
    }
    debugMsooXml << "File" << fileName << "loaded and parsed.";
    return KoFilter::OK;
}

KoFilter::ConversionStatus Utils::loadAndParse(KoXmlDocument& doc, const KZip* zip,
        QString& errorMessage, const QString& fileName)
{
    errorMessage.clear();
    KoFilter::ConversionStatus status;
    std::auto_ptr<QIODevice> device(openDeviceForFile(zip, errorMessage, fileName, status));
    if (!device.get())
        return status;
    return loadAndParse(device.get(), doc, errorMessage, fileName);
}

KoFilter::ConversionStatus Utils::loadAndParseDocument(MsooXmlReader* reader,
        const KZip* zip,
        KoOdfWriters *writers,
        QString& errorMessage,
        const QString& fileName,
        MsooXmlReaderContext* context)
{
    Q_UNUSED(writers)
    errorMessage.clear();
    KoFilter::ConversionStatus status;
    std::auto_ptr<QIODevice> device(openDeviceForFile(zip, errorMessage, fileName, status));
    if (!device.get())
        return status;
    reader->setDevice(device.get());
    reader->setFileName(fileName); // for error reporting
    status = reader->read(context);
    if (status != KoFilter::OK) {
        errorMessage = reader->errorString();
        return status;
    }
    debugMsooXml << "File" << fileName << "loaded and parsed.";
    return KoFilter::OK;
}

QIODevice* Utils::openDeviceForFile(const KZip* zip, QString& errorMessage, const QString& fileName,
                                    KoFilter::ConversionStatus& status)
{
    debugMsooXml << "Trying to open" << fileName;
    errorMessage.clear();
    const KArchiveEntry* entry = zip->directory()->entry(fileName);
    if (!entry) {
        errorMessage = i18n("Entry '%1' not found.", fileName);
        debugMsooXml << errorMessage;
        status = KoFilter::FileNotFound;
        return 0;
    }
    if (!entry->isFile()) {
        errorMessage = i18n("Entry '%1' is not a file.", fileName);
        debugMsooXml << errorMessage;
        status = KoFilter::WrongFormat;
        return 0;
    }
    const KZipFileEntry* f = static_cast<const KZipFileEntry *>(entry);
    debugMsooXml << "Entry" << fileName << "has size" << f->size();
    status = KoFilter::OK;
    // There seem to be some problems with kde/zlib when trying to read
    // multiple streams, this functionality is needed in the filter
    // Until there's another solution for this, this avoids the problem
    //return f->createDevice();
    QBuffer *device = new QBuffer();
    device->setData(f->data());
    device->open(QIODevice::ReadOnly);
    return device;
}

#define BLOCK_SIZE 4096
static KoFilter::ConversionStatus copyOle(QString& errorMessage,
                                    const QString sourceName, KoStore *outputStore,
                                    const QString& destinationName, const KZip* zip)
{
    KoFilter::ConversionStatus status = KoFilter::OK;

    QIODevice* inputDevice = Utils::openDeviceForFile(zip, errorMessage, sourceName, status);
    if (!inputDevice) {
        // Source did not exist
        return KoFilter::CreationError;
    }
    inputDevice->open(QIODevice::ReadOnly);

    OOXML_POLE::Storage storage(inputDevice);
    if (!storage.open()) {
        debugMsooXml << "Cannot open " << sourceName;
        return KoFilter::WrongFormat;
    }

    std::list<std::string> lista = storage.entries();
    std::string oleType = "Contents";

    for (std::list<std::string>::iterator it = lista.begin(); it != lista.end(); ++it)  {
        //debugMsooXml << "ENTRY " << (*it).c_str();
        if (QString((*it).c_str()).contains("Ole10Native")) {
            oleType = "Ole10Native";
        }
        else if (QString((*it).c_str()).contains("CONTENTS")) {
            oleType = "CONTENTS";
        }
    }

    OOXML_POLE::Stream stream(&storage, oleType);
    QByteArray array;
    array.resize(stream.size());

    unsigned long r = stream.read((unsigned char*)array.data(), stream.size());
    if (r != stream.size()) {
        errorMsooXml << "Error while reading from stream";
        return KoFilter::WrongFormat;
    }

    if (oleType == "Contents" || oleType == "Ole10Native") {
     // Removing first 4 bytes which are the size
        array = array.right(array.length() - 4);
    }

    // Uncomment to write any ole file for testing
    //POLE::Stream streamTemp(&storage, "Ole");
    //QByteArray arrayTemp;
    //arrayTemp.resize(streamTemp.size());
    //streamTemp.read((unsigned char*)arrayTemp.data(), streamTemp.size());
    //QFile file("olething.ole");
    //file.open(QIODevice::WriteOnly);
    //QDataStream out(&file);
    //out.writeRawData(arrayTemp.data(), arrayTemp.length());

    debugMsooXml << "mode:" << outputStore->mode();
    if (!outputStore->open(destinationName)) {
        errorMessage = i18n("Could not open entry \"%1\" for writing.", destinationName);
        return KoFilter::CreationError;
    }

    QByteArray array2;
    while (true) {
        array2 = array.left(BLOCK_SIZE);
        array = array.right(array.size() - array2.size());
        const qint64 in = array2.size();
        if (in <= 0) {
            break;
        }
        char *block = array2.data();
        if (in != outputStore->write(block, in)) {
            errorMessage = i18n("Could not write block");
            status = KoFilter::CreationError;
            break;
        }
    }
    outputStore->close();
    delete inputDevice;
    inputDevice = 0;
    return status;
}
#undef BLOCK_SIZE

#define BLOCK_SIZE 4096
KoFilter::ConversionStatus Utils::createImage(QString& errorMessage,
                                       const QImage& source, KoStore *outputStore,
                                       const QString& destinationName)
{
    if (outputStore->hasFile(destinationName)) {
        return KoFilter::OK;
    }

    KoFilter::ConversionStatus status = KoFilter::OK;
    QByteArray array;
    QBuffer inputDevice(&array);
    inputDevice.open(QIODevice::ReadWrite);
    QFileInfo info = QFileInfo(destinationName);
    source.save(&inputDevice, info.suffix().toUtf8());
    inputDevice.seek(0);

    if (!outputStore->open(destinationName)) {
        errorMessage = i18n("Could not open entry \"%1\" for writing.", destinationName);
        return KoFilter::CreationError;
    }
    char block[BLOCK_SIZE];
    while (true) {
        const qint64 in = inputDevice.read(block, BLOCK_SIZE);
        if (in <= 0) {
            break;
        }
        if (in != outputStore->write(block, in)) {
            errorMessage = i18n("Could not write block");
            status = KoFilter::CreationError;
            break;
        }
    }
    outputStore->close();
    return status;
}
#undef BLOCK_SIZE

#define BLOCK_SIZE 4096
KoFilter::ConversionStatus Utils::copyFile(const KZip* zip, QString& errorMessage,
                                           const QString& sourceName, KoStore *outputStore,
                                           const QString& destinationName, bool oleType)
{
    if (outputStore->hasFile(destinationName)) {
        return KoFilter::OK;
    }

    KoFilter::ConversionStatus status;
    if (oleType) {
        status = copyOle(errorMessage, sourceName, outputStore, destinationName, zip);
        return status;
    }

    std::auto_ptr<QIODevice> inputDevice = std::auto_ptr<QIODevice>(Utils::openDeviceForFile(zip, errorMessage, sourceName, status));

    if (!inputDevice.get()) {
        return status;
    }

    debugMsooXml << "mode:" << outputStore->mode();
    if (!outputStore->open(destinationName)) {
        errorMessage = i18n("Could not open entry \"%1\" for writing.", destinationName);
        return KoFilter::CreationError;
    }
    status = KoFilter::OK;
    char block[BLOCK_SIZE];
    while (true) {
        const qint64 in = inputDevice->read(block, BLOCK_SIZE);
//        debugMsooXml << "in:" << in;
        if (in <= 0)
            break;
        if (in != outputStore->write(block, in)) {
            errorMessage = i18n("Could not write block");
            status = KoFilter::CreationError;
            break;
        }
    }
    outputStore->close();
    return status;
}
#undef BLOCK_SIZE

KoFilter::ConversionStatus Utils::imageSize(const KZip* zip, QString& errorMessage, const QString& sourceName,
                                            QSize* size)
{
    Q_ASSERT(size);
    KoFilter::ConversionStatus status;
    std::auto_ptr<QIODevice> inputDevice(Utils::openDeviceForFile(zip, errorMessage, sourceName, status));
    if (!inputDevice.get()) {
        return status;
    }
    QImageReader r(inputDevice.get(), QFileInfo(sourceName).suffix().toLatin1());
    if (!r.canRead())
        return KoFilter::WrongFormat;
    *size = r.size();
    debugMsooXml << *size;
    return KoFilter::OK;
}

KoFilter::ConversionStatus Utils::loadThumbnail(QImage& thumbnail, KZip* zip)
{
//! @todo
    Q_UNUSED(thumbnail)
    Q_UNUSED(zip)
    return KoFilter::FileNotFound;
}

//! @return true if @a el has tag name is equal to @a expectedTag or false otherwise;
//!         on failure optional @a warningPrefix message is prepended to the warning
static bool checkTag(const KoXmlElement& el, const char* expectedTag, const char* warningPrefix = 0)
{
    if (el.tagName() != expectedTag) {
        warnMsooXml
        << (warningPrefix ? QString::fromLatin1(warningPrefix) + ":" : QString())
        << "tag name=" << el.tagName() << " expected:" << expectedTag;
        return false;
    }
    return true;
}

//! @return true if @a el has namespace URI is equal to @a expectedNSURI or false otherwise
static bool checkNsUri(const KoXmlElement& el, const char* expectedNsUri)
{
    if (el.namespaceURI() != expectedNsUri) {
        warnMsooXml << "Invalid namespace URI" << el.namespaceURI() << " expected:" << expectedNsUri;
        return false;
    }
    return true;
}

bool Utils::convertBooleanAttr(const QString& value, bool defaultValue)
{
    const QByteArray val(value.toLatin1());
    if (val.isEmpty()) {
        return defaultValue;
    }
    debugMsooXml << val;

    return val != MsooXmlReader::constOff && val != MsooXmlReader::constFalse && val != MsooXmlReader::const0;
}

KoFilter::ConversionStatus Utils::loadContentTypes(
    const KoXmlDocument& contentTypesXML, QMultiHash<QByteArray, QByteArray>& contentTypes)
{
    KoXmlElement typesEl(contentTypesXML.documentElement());
    if (!checkTag(typesEl, "Types", "documentElement")) {
        return KoFilter::WrongFormat;
    }
    if (!checkNsUri(typesEl, Schemas::contentTypes)) {
        return KoFilter::WrongFormat;
    }
    KoXmlElement e;
    forEachElement(e, typesEl) {
        const QString tagName(e.tagName());
        if (!checkNsUri(e, Schemas::contentTypes)) {
            return KoFilter::WrongFormat;
        }

        if (tagName == "Override") {
            //ContentType -> PartName mapping
            const QByteArray atrPartName(e.attribute("PartName").toLatin1());
            const QByteArray atrContentType(e.attribute("ContentType").toLatin1());
            if (atrPartName.isEmpty() || atrContentType.isEmpty()) {
                warnMsooXml << "Invalid data for" << tagName
                << "element: PartName=" << atrPartName << "ContentType=" << atrContentType;
                return KoFilter::WrongFormat;
            }
//debugMsooXml << atrContentType << "->" << atrPartName;
            contentTypes.insert(atrContentType, atrPartName);
        } else if (tagName == "Default") {
//! @todo
            // skip for now...
        }
    }
    return KoFilter::OK;
}

KoFilter::ConversionStatus Utils::loadDocumentProperties(const KoXmlDocument& appXML, QMap<QString, QVariant>& properties)
{
    KoXmlElement typesEl(appXML.documentElement());
    KoXmlElement e, elem, element;
    forEachElement(element, typesEl) {
        QVariant v;
        forEachElement(elem, element) {
            if(elem.tagName() == "vector") {
                QVariantList list;
                forEachElement(e, elem)
                    list.append(e.text());
                v = list;
            }
        }
        if(!v.isValid())
            v = element.text();
        properties[element.tagName()] = v;
    }
    return KoFilter::OK;
}

bool Utils::ST_Lang_to_languageAndCountry(const QString& value, QString& language, QString& country)
{
    int indexForCountry =  value.indexOf('-');
    if (indexForCountry <= 0)
        return false;
    indexForCountry++;
    language = value.left(indexForCountry - 1);
    country = value.mid(indexForCountry);
    return !country.isEmpty();
}

class ST_HighlightColorMapping : public QHash<QString, QColor>
{
public:
    ST_HighlightColorMapping() {
#define INSERT_HC(c, hex) insert(QLatin1String(c), QColor( QRgb( 0xff000000 | hex ) ) )
        INSERT_HC("black", 0x000000);
        INSERT_HC("blue", 0x0000ff);
        INSERT_HC("cyan", 0x00ffff);
        INSERT_HC("darkBlue", 0x000080);
        INSERT_HC("darkCyan", 0x008080);
        INSERT_HC("darkGray", 0x808080);
        INSERT_HC("darkGreen", 0x008000);
        INSERT_HC("darkMagenta", 0x800080);
        INSERT_HC("darkRed", 0x800000);
        INSERT_HC("darkYellow", 0x808000);
        INSERT_HC("green", 0x00ff00);
        INSERT_HC("lightGray", 0xc0c0c0);
        INSERT_HC("magenta", 0xff00ff);
        INSERT_HC("red", 0xff0000);
        INSERT_HC("yellow", 0xffff00);
        INSERT_HC("white", 0xffffff);
#undef INSERT_HC
    }
};

Q_GLOBAL_STATIC(ST_HighlightColorMapping, s_ST_HighlightColor_to_QColor)

QBrush Utils::ST_HighlightColor_to_QColor(const QString& colorName)
{
    const QColor c(s_ST_HighlightColor_to_QColor->value(colorName));
    if (c.isValid())
        return QBrush(c);
    return QBrush(); // for "none" or anything unsupported
}

qreal Utils::ST_Percentage_to_double(const QString& val, bool& ok)
{
    if (!val.endsWith('%')) {
        ok = false;
        return 0.0;
    }
    QString result(val);
    result.truncate(1);
    return result.toDouble(&ok);
}

qreal Utils::ST_Percentage_withMsooxmlFix_to_double(const QString& val, bool& ok)
{
    const qreal result = ST_Percentage_to_double(val, ok);
    if (ok)
        return result;
    // MSOOXML fix: the format is int({ST_Percentage}*1000)
    const int resultInt = val.toInt(&ok);
    if (!ok)
        return 0.0;
    return qreal(resultInt) / 1000.0;
}

QColor Utils::colorForLuminance(const QColor& color, const DoubleModifier& modulation, const DoubleModifier& offset)
{
    if (modulation.valid) {
        int r, g, b;
        color.getRgb(&r, &g, &b);
        if (offset.valid) {
            return QColor(
                       int(floor((255 - r) * (100.0 - modulation.value) / 100.0 + r)),
                       int(floor((255 - g) * offset.value / 100.0 + g)),
                       int(floor((255 - b) * offset.value / 100.0 + b)),
                       color.alpha());
        } else {
            return QColor(
                       int(floor(r * modulation.value / 100.0)),
                       int(floor(g * modulation.value / 100.0)),
                       int(floor(b * modulation.value / 100.0)),
                       color.alpha());
        }
    }
    return color;
}

KOMSOOXML_EXPORT void Utils::modifyColor(QColor& color, qreal tint, qreal shade, qreal satMod)
{
    int red = color.red();
    int green = color.green();
    int blue = color.blue();

    if (tint > 0) {
        red = tint * red + (1 - tint) * 255;
        green = tint * green + (1 - tint) * 255;
        blue = tint * blue + (1 - tint) * 255;
    }
    if (shade > 0) {
        red = shade * red;
        green = shade * green;
        blue = shade * blue;
    }

    // FIXME: This calculation for sure is incorrect,
    // According to MS forums, RGB should first be converted to linear RGB
    // Then to HSL and then multiply saturation value by satMod
    // SatMod can be for example 3.5 so converting RGB -> HSL is not an option
    // ADD INFO: MS document does not say that when calculating TINT and SHADE
    // That whether one should use normal RGB or linear RGB, check it!


    // This method is used temporarily, it seems to produce visually better results than the lower one.
    if (satMod > 0) {
        QColor temp = QColor(red, green, blue);
        qreal saturationFromFull = 1.0 - temp.saturationF();
        temp = QColor::fromHsvF(temp.hueF(), temp.saturationF() + saturationFromFull / 10 * satMod, temp.valueF());
        red = temp.red();
        green = temp.green();
        blue = temp.blue();
    }

    /*
    if (satMod > 0) {
        red = red * satMod;
        green = green * satMod;
        blue = blue * satMod;
        if (red > 255) {
            red = 255;
        }
        if (green > 255) {
            green = 255;
        }
        if (blue > 255) {
            blue = 255;
        }
    }
    */

    color = QColor(red, green, blue);
}

class ST_PlaceholderType_to_ODFMapping : public QHash<QByteArray, QByteArray>
{
public:
    ST_PlaceholderType_to_ODFMapping() {
        insert("body", "outline");
        insert("chart", "chart");
        insert("clipArt", "graphic");
        insert("ctrTitle", "title");
//! @todo dgm->orgchart?
        insert("dgm", "orgchart");
        insert("dt", "date-time");
        insert("ftr", "footer");
        insert("hdr", "header");
//! @todo media->object?
        insert("media", "object");
        insert("obj", "object");
        insert("pic", "graphic");
//! @todo sldImg->graphic?
        insert("sldImg", "graphic");
        insert("sldNum", "page-number");
        insert("subTitle", "subtitle");
        insert("tbl", "table");
        insert("title", "title");
    }
};

Q_GLOBAL_STATIC(ST_PlaceholderType_to_ODFMapping, s_ST_PlaceholderType_to_ODF)

QString Utils::ST_PlaceholderType_to_ODF(const QString& ecmaType)
{
    QHash<QByteArray, QByteArray>::ConstIterator it(s_ST_PlaceholderType_to_ODF->constFind(ecmaType.toLatin1()));
    if (it == s_ST_PlaceholderType_to_ODF->constEnd())
        return QLatin1String("text");
    return QString(it.value());
}

//! Mapping for handling u element, used in setupUnderLineStyle()
struct UnderlineStyle {
    UnderlineStyle(
        KoCharacterStyle::LineStyle style_,
        KoCharacterStyle::LineType type_,
        KoCharacterStyle::LineWeight weight_,
        KoCharacterStyle::LineMode mode_ = KoCharacterStyle::ContinuousLineMode)
            : style(style_), type(type_), weight(weight_), mode(mode_) {
    }

    KoCharacterStyle::LineStyle style;
    KoCharacterStyle::LineType type;
    KoCharacterStyle::LineWeight weight;
    KoCharacterStyle::LineMode mode;
};

typedef QHash<QByteArray, UnderlineStyle*> UnderlineStylesHashBase;

class UnderlineStylesHash : public UnderlineStylesHashBase
{
public:
    UnderlineStylesHash() {
        // default:
        insert("-",
               new UnderlineStyle(KoCharacterStyle::SolidLine, KoCharacterStyle::SingleLine,
                                  KoCharacterStyle::AutoLineWeight)
              );
        // 17.18.99 ST_Underline (Underline Patterns), WML ECMA-376 p.1681:
        insert("single",
               new UnderlineStyle(KoCharacterStyle::SolidLine, KoCharacterStyle::SingleLine,
                                  KoCharacterStyle::AutoLineWeight)
              );
        insert("double",
               new UnderlineStyle(KoCharacterStyle::SolidLine, KoCharacterStyle::DoubleLine,
                                  KoCharacterStyle::AutoLineWeight)
              );
        insert("dbl",
               new UnderlineStyle(KoCharacterStyle::SolidLine, KoCharacterStyle::DoubleLine,
                                  KoCharacterStyle::AutoLineWeight)
              );
        insert("words",
               new UnderlineStyle(KoCharacterStyle::SolidLine, KoCharacterStyle::SingleLine,
                                  KoCharacterStyle::AutoLineWeight, KoCharacterStyle::SkipWhiteSpaceLineMode)
              );
        insert("thick",
               new UnderlineStyle(KoCharacterStyle::SolidLine, KoCharacterStyle::SingleLine,
                                  KoCharacterStyle::BoldLineWeight)
              );
        insert("dash",
               new UnderlineStyle(KoCharacterStyle::DashLine, KoCharacterStyle::SingleLine,
                                  KoCharacterStyle::AutoLineWeight)
              );
        insert("dashDotHeavy",
               new UnderlineStyle(KoCharacterStyle::DotDashLine, KoCharacterStyle::SingleLine,
                                  KoCharacterStyle::BoldLineWeight)
              );
        insert("dotted",
               new UnderlineStyle(KoCharacterStyle::DottedLine, KoCharacterStyle::SingleLine,
                                  KoCharacterStyle::AutoLineWeight)
              );
        insert("dotDash",
               new UnderlineStyle(KoCharacterStyle::DotDashLine, KoCharacterStyle::SingleLine,
                                  KoCharacterStyle::AutoLineWeight)
              );
        insert("dotDotDash",
               new UnderlineStyle(KoCharacterStyle::DotDotDashLine, KoCharacterStyle::SingleLine,
                                  KoCharacterStyle::AutoLineWeight)
              );
        insert("wave",
               new UnderlineStyle(KoCharacterStyle::WaveLine, KoCharacterStyle::SingleLine,
                                  KoCharacterStyle::AutoLineWeight)
              );
        insert("wavyDouble",
               new UnderlineStyle(KoCharacterStyle::WaveLine, KoCharacterStyle::DoubleLine,
                                  KoCharacterStyle::AutoLineWeight)
              );
        insert("wavyDbl",
               new UnderlineStyle(KoCharacterStyle::WaveLine, KoCharacterStyle::DoubleLine,
                                  KoCharacterStyle::AutoLineWeight)
              );
        insert("wavyHeavy",
               new UnderlineStyle(KoCharacterStyle::WaveLine, KoCharacterStyle::SingleLine,
                                  KoCharacterStyle::BoldLineWeight)
              );
//! @todo more styles

        // 20.1.10.82 ST_TextUnderlineType (Text Underline Types), DrawingML ECMA-376 p.3450:
        insert("none",
               new UnderlineStyle(KoCharacterStyle::NoLineStyle, KoCharacterStyle::NoLineType,
                                  KoCharacterStyle::AutoLineWeight)
              );
        insert("sng",
               new UnderlineStyle(KoCharacterStyle::SolidLine, KoCharacterStyle::SingleLine,
                                  KoCharacterStyle::AutoLineWeight)
              );
//! @todo more styles
    }

    ~UnderlineStylesHash() {
        qDeleteAll(*this);
    }

    void setup(const QString& msooxmlName,
               KoCharacterStyle* textStyleProperties) {
        UnderlineStyle* style = value(msooxmlName.toLatin1());
        if (!style)
            style = value("-");
        textStyleProperties->setUnderlineStyle(style->style);
        // add style:text-underline-type if it is not "single"
        if (KoCharacterStyle::SingleLine != style->type) {
            textStyleProperties->setUnderlineType(style->type);
        }
        textStyleProperties->setUnderlineWidth(style->weight, 1.0);
        // add style:text-underline-mode if it is not "continuous"
        if (KoCharacterStyle::ContinuousLineMode != style->mode) {
            textStyleProperties->setUnderlineMode(style->mode);
        }
    }
};

void Utils::rotateString(const qreal rotation, const qreal width, const qreal height, qreal& angle, qreal& xDiff, qreal& yDiff)
{
    angle = -(qreal)rotation * ((qreal)(M_PI) / (qreal)180.0)/ (qreal)60000.0;
    //position change is calculated based on the fact that center point stays in the same location
    // Width/2 = Xnew + cos(angle)*Width/2 - sin(angle)*Height/2
    // Height/2 = Ynew + sin(angle)*Width/2 + cos(angle)*Height/2
    xDiff = width/2 - cos(-angle)*width/2 + sin(-angle)*height/2;
    yDiff = height/2 - sin(-angle)*width/2 - cos(-angle)*height/2;
}


Q_GLOBAL_STATIC(UnderlineStylesHash, s_underLineStyles)

void Utils::setupUnderLineStyle(const QString& msooxmlName, KoCharacterStyle* textStyleProperties)
{
    s_underLineStyles->setup(msooxmlName, textStyleProperties);
}

//-----------------------------------------
// Marker styles
//-----------------------------------------

namespace
{
    static const char* const markerStyles[6] = {
        "", "msArrowEnd_20_5", "msArrowStealthEnd_20_5", "msArrowDiamondEnd_20_5",
        "msArrowOvalEnd_20_5", "msArrowOpenEnd_20_5"
    };

    // trying to maintain compatibility with libmso
    enum MSOLINEEND_CUSTOM {
        msolineNoEnd,
        msolineArrowEnd,
        msolineArrowStealthEnd,
        msolineArrowDiamondEnd,
        msolineArrowOvalEnd,
        msolineArrowOpenEnd
    };
}

QString Utils::defineMarkerStyle(KoGenStyles& mainStyles, const QString& type)
{
    uint id;

    if (type == "arrow") {
        id = msolineArrowOpenEnd;
    } else if (type == "stealth") {
        id = msolineArrowStealthEnd;
    } else if (type == "diamond") {
        id = msolineArrowDiamondEnd;
    } else if (type == "oval") {
        id = msolineArrowOvalEnd;
    } else if (type == "triangle") {
        id = msolineArrowEnd;
    } else {
        return QString();
    }

    const QString name(markerStyles[id]);

    if (mainStyles.style(name, "")) {
        return name;
    }

    KoGenStyle marker(KoGenStyle::MarkerStyle);
    marker.addAttribute("draw:display-name",  QString(markerStyles[id]).replace("_20_", " "));

    // sync with LO
    switch (id) {
    case msolineArrowStealthEnd:
        marker.addAttribute("svg:viewBox", "0 0 318 318");
        marker.addAttribute("svg:d", "m159 0 159 318-159-127-159 127z");
        break;
    case msolineArrowDiamondEnd:
        marker.addAttribute("svg:viewBox", "0 0 318 318");
        marker.addAttribute("svg:d", "m159 0 159 159-159 159-159-159z");
        break;
    case msolineArrowOvalEnd:
        marker.addAttribute("svg:viewBox", "0 0 318 318");
        marker.addAttribute("svg:d", "m318 0c0-87-72-159-159-159s-159 72-159 159 72 159 159 159 159-72 159-159z");
        break;
    case msolineArrowOpenEnd:
        marker.addAttribute("svg:viewBox", "0 0 477 477");
        marker.addAttribute("svg:d", "m239 0 238 434-72 43-166-305-167 305-72-43z");
        break;
    case msolineArrowEnd:
    default:
        marker.addAttribute("svg:viewBox", "0 0 318 318");
        marker.addAttribute("svg:d", "m159 0 159 318h-318z");
        break;
    }
    return mainStyles.insert(marker, name, KoGenStyles::DontAddNumberToName);
}

qreal Utils::defineMarkerWidth(const QString &markerWidth, const qreal lineWidth)
{
    int c = 0;

    if (markerWidth == "lg") {
        c = 3;
    } else if (markerWidth == "med" || markerWidth.isEmpty()) {
        c = 2; //MSOOXML default = "med"
    } else if (markerWidth == "sm") {
        c = 1;
    }
    return ( lineWidth * c );
}

//-----------------------------------------
// XmlWriteBuffer
//-----------------------------------------

Utils::XmlWriteBuffer::XmlWriteBuffer()
        : m_origWriter(0), m_newWriter(0)
{
}

Utils::XmlWriteBuffer::~XmlWriteBuffer()
{
    releaseWriterInternal();
}

KoXmlWriter* Utils::XmlWriteBuffer::setWriter(KoXmlWriter* writer)
{
    Q_ASSERT(!m_origWriter && !m_newWriter);
    if (m_origWriter || m_newWriter) {
        return 0;
    }
    m_origWriter = writer; // remember
    m_newWriter = new KoXmlWriter(&m_buffer, m_origWriter->indentLevel() + 1);
    return m_newWriter;
}

KoXmlWriter* Utils::XmlWriteBuffer::releaseWriter()
{
    Q_ASSERT(m_newWriter && m_origWriter);
    if (!m_newWriter || !m_origWriter) {
        return 0;
    }
    m_origWriter->addCompleteElement(&m_buffer);
    return releaseWriterInternal();
}

KoXmlWriter* Utils::XmlWriteBuffer::releaseWriter(QString& bkpXmlSnippet)
{
    Q_ASSERT(m_newWriter && m_origWriter);
    if (!m_newWriter || !m_origWriter) {
        return 0;
    }
    bkpXmlSnippet = QString::fromUtf8(m_buffer.buffer(), m_buffer.buffer().size());
    return releaseWriterInternal();
}

KoXmlWriter* Utils::XmlWriteBuffer::releaseWriterInternal()
{
    if (!m_newWriter || !m_origWriter) {
        return 0;
    }
    delete m_newWriter;
    m_newWriter = 0;
    KoXmlWriter* tmp = m_origWriter;
    m_origWriter = 0;
    return tmp;
}

void Utils::XmlWriteBuffer::clear()
{
    delete m_newWriter;
    m_newWriter = 0;
    m_origWriter = 0;
}

QString Utils::columnName(uint column)
{
    uint digits = 1;
    uint offset = 0;

    for (uint limit = 26; column >= limit + offset; limit *= 26, digits++)
        offset += limit;

    QString str;
    for (uint col = column - offset; digits > 0; --digits, col /= 26)
        str.prepend(QChar('A' + (col % 26)));

    return str;
}

void Utils::splitPathAndFile(const QString& pathAndFile, QString* path, QString* file)
{
    Q_ASSERT(path);
    Q_ASSERT(file);
    *path = pathAndFile.left(pathAndFile.lastIndexOf('/'));
    *file = pathAndFile.mid(pathAndFile.lastIndexOf('/') + 1);
}

// <units> -------------------

QString Utils::EMU_to_ODF(const QString& twipValue)
{
    if (twipValue.isEmpty())
        return QLatin1String("0cm");
    bool ok;
    const int emu = twipValue.toInt(&ok);
    if (!ok)
        return QString();
    if (emu == 0)
        return QLatin1String("0cm");
    return EMU_TO_CM_STRING(emu);
}

QString Utils::TWIP_to_ODF(const QString& twipValue)
{
    if (twipValue.isEmpty())
        return QLatin1String("0cm");
    bool ok;
    const int twip = twipValue.toInt(&ok);
    if (!ok)
        return QString();
    if (twip == 0)
        return QLatin1String("0cm");
    return cmString(TWIP_TO_CM(qreal(twip)));
}

QString Utils::ST_EighthPointMeasure_to_ODF(const QString& value)
{
    if (value.isEmpty())
        return QString();
    bool ok;
    const qreal point = qreal(value.toFloat(&ok)) / 8.0;
    if (!ok)
        return QString();
    return QString::number(point, 'g', 2) + QLatin1String("pt");
}

//! @return true if @a string is non-negative integer number
static bool isPositiveIntegerNumber(const QString& string)
{
    for (const QChar *c = string.constData(); !c->isNull(); c++) {
        if (!c->isNumber())
            return false;
    }
    return !string.isEmpty();
}

//! Splits number and unit
static bool splitNumberAndUnit(const QString& _string, qreal *number, QString* unit)
{
    int unitIndex = 0;
    QString string(_string);
    for (const QChar *c = string.constData(); !c->isNull(); c++, unitIndex++) {
        if (!c->isNumber() && *c != '.')
            break;
    }
    *unit = string.mid(unitIndex);
    string.truncate(unitIndex);
    if (string.isEmpty()) {
        warnMsooXml << "No unit found in" << _string;
        return false;
    }
    bool ok;
    *number = string.toFloat(&ok);
    if (!ok)
        warnMsooXml << "Invalid number in" << _string;
    return ok;
}

//! @return true is @a unit is one of these mentioned in 22.9.2.15 ST_UniversalMeasure (Universal Measurement)
static bool isUnitAcceptable(const QString& unit)
{
    if (unit.length() != 2)
        return false;
    return unit == QString::fromLatin1("cm")
           || unit == QString::fromLatin1("mm")
           || unit == QString::fromLatin1("in")
           || unit == QString::fromLatin1("pt")
           || unit == QString::fromLatin1("pc")
           || unit == QString::fromLatin1("pi");
}

static QString ST_TwipsMeasure_to_ODF_with_unit(const QString& value,
                                                qreal (*convertFromTwips)(qreal), const char* unit)
{
    if (value.isEmpty())
        return QString();
    if (isPositiveIntegerNumber(value)) {
        // a positive number in twips (twentieths of a point, equivalent to 1/1440th of an inch)
        bool ok;
        const qreal point = convertFromTwips( qreal(value.toFloat(&ok)) );
        if (!ok)
            return QString();
        return QString::number(point, 'g', 2) + QLatin1String(unit);
    }
    return Utils::ST_PositiveUniversalMeasure_to_ODF(value);
}

qreal twipToPt(qreal v)
{
    return TWIP_TO_POINT(v);
}

KOMSOOXML_EXPORT QString Utils::ST_TwipsMeasure_to_pt(const QString& value)
{
    return ST_TwipsMeasure_to_ODF_with_unit(value, twipToPt, "pt");
}

qreal twipToCm(qreal v)
{
    return TWIP_TO_CM(v);
}

KOMSOOXML_EXPORT QString Utils::ST_TwipsMeasure_to_cm(const QString& value)
{
    return ST_TwipsMeasure_to_ODF_with_unit(value, twipToCm, "cm");
}

KOMSOOXML_EXPORT QString Utils::ST_PositiveUniversalMeasure_to_ODF(const QString& value)
{
    // a positive decimal number immediately following by a unit identifier.
    qreal number(0.0);
    QString unit;
    if (!splitNumberAndUnit(value, &number, &unit))
        return QString();
    // special case: pc is another name for pica
    if (unit == QString::fromLatin1("pc")) {
        return QString::number(number) + QLatin1String("pi");
    }
    if (!isUnitAcceptable(unit)) {
        warnMsooXml << "Unit" << unit << "not supported. Expected cm/mm/in/pt/pc/pi.";
        return QString();
    }
    return value; // the original is OK
}

KOMSOOXML_EXPORT QString Utils::ST_PositiveUniversalMeasure_to_cm(const QString& value)
{
    QString v(ST_PositiveUniversalMeasure_to_ODF(value));
    if (v.isEmpty())
        return QString();
    return cmString(POINT_TO_CM(KoUnit::parseValue(v)));
}

// </units> -------------------

Utils::ParagraphBulletProperties::ParagraphBulletProperties()
{
    clear();
}

void Utils::ParagraphBulletProperties::clear()
{
    m_level = -1;
    m_type = ParagraphBulletProperties::DefaultType;
    m_startValue = "1"; //ECMA-376, p.4575
    m_bulletFont = UNUSED;
    m_bulletChar = UNUSED;
    m_numFormat = UNUSED;
    m_prefix = UNUSED;
    m_suffix = UNUSED;
    m_align = UNUSED;
    m_indent = UNUSED;
    m_margin = UNUSED;
    m_picturePath = UNUSED;
    m_bulletColor = UNUSED;
    m_followingChar = UNUSED;
    m_bulletRelativeSize = UNUSED;
    m_bulletSize = UNUSED;
    m_startOverride = false;
}

bool Utils::ParagraphBulletProperties::isEmpty() const
{
    if (m_type == ParagraphBulletProperties::DefaultType) {
        return true;
    }
    return false;
}

void Utils::ParagraphBulletProperties::setAlign(const QString& align)
{
    m_align = align;
}

void Utils::ParagraphBulletProperties::setBulletChar(const QString& bulletChar)
{
    m_bulletChar = bulletChar;
    m_type = ParagraphBulletProperties::BulletType;
}

void Utils::ParagraphBulletProperties::setStartValue(const QString& value)
{
    m_startValue = value;
}

void Utils::ParagraphBulletProperties::setMargin(const qreal margin)
{
    m_margin = QString("%1").arg(margin);
}

void Utils::ParagraphBulletProperties::setIndent(const qreal indent)
{
    m_indent = QString("%1").arg(indent);
}

void Utils::ParagraphBulletProperties::setPrefix(const QString& prefixChar)
{
    m_prefix = prefixChar;
}

void Utils::ParagraphBulletProperties::setSuffix(const QString& suffixChar)
{
    m_suffix = suffixChar;
}

void Utils::ParagraphBulletProperties::setNumFormat(const QString& numFormat)
{
    m_numFormat = numFormat;
    m_type = ParagraphBulletProperties::NumberType;
}

void Utils::ParagraphBulletProperties::setPicturePath(const QString& picturePath)
{
    m_picturePath = picturePath;
    m_type = ParagraphBulletProperties::PictureType;
}

void Utils::ParagraphBulletProperties::setBulletRelativeSize(const int size)
{
    m_bulletRelativeSize = QString("%1").arg(size);
}

void Utils::ParagraphBulletProperties::setBulletSizePt(const qreal size)
{
    m_bulletSize = QString("%1").arg(size);
}

void Utils::ParagraphBulletProperties::setBulletFont(const QString& font)
{
    m_bulletFont = font;
}

void Utils::ParagraphBulletProperties::setBulletColor(const QString& bulletColor)
{
    m_bulletColor = bulletColor;
}

void Utils::ParagraphBulletProperties::setFollowingChar(const QString& followingChar)
{
    m_followingChar = followingChar;
}

void Utils::ParagraphBulletProperties::setTextStyle(const KoGenStyle& textStyle)
{
    m_textStyle = textStyle;

    //m_bulletFont
    if (!(m_textStyle.property("fo:font-family")).isEmpty()) {
        m_bulletFont = m_textStyle.property("fo:font-family");
    }
    if (!(m_textStyle.property("style:font-name")).isEmpty()) {
        m_bulletFont = m_textStyle.property("style:font-name");
    }
    //m_bulletColor
    if (!(m_textStyle.property("fo:color")).isEmpty()) {
        m_bulletColor = m_textStyle.property("fo:color");
    }
    //m_bulletRelativeSize
    //m_bulletSize
    if (!m_textStyle.property("fo:font-size").isEmpty()) {
        QString bulletSize = m_textStyle.property("fo:font-size");
        if (bulletSize.endsWith(QLatin1Char('%'))) {
            bulletSize.chop(1);
            m_bulletRelativeSize = bulletSize;
        } else if (bulletSize.endsWith(QLatin1String("pt"))) {
            bulletSize.chop(2);
            m_bulletSize = bulletSize;
        } else {
            debugMsooXml << "Unit of font-size NOT supported!";
        }
    }
}

void Utils::ParagraphBulletProperties::setStartOverride(const bool startOverride)
{
    m_startOverride = startOverride;
}

QString Utils::ParagraphBulletProperties::startValue() const
{
    return m_startValue;
}

QString Utils::ParagraphBulletProperties::bulletColor() const
{
    return m_bulletColor;
}

QString Utils::ParagraphBulletProperties::bulletChar() const
{
    return m_bulletChar;
}

QString Utils::ParagraphBulletProperties::bulletFont() const
{
    return m_bulletFont;
}

QString Utils::ParagraphBulletProperties::margin() const
{
    return m_margin;
}

QString Utils::ParagraphBulletProperties::indent() const
{
    return m_indent;
}

QString Utils::ParagraphBulletProperties::bulletRelativeSize() const
{
    return m_bulletRelativeSize;
}

QString Utils::ParagraphBulletProperties::bulletSizePt() const
{
    return m_bulletSize;
}

QString Utils::ParagraphBulletProperties::followingChar() const
{
    return m_followingChar;
}

KoGenStyle Utils::ParagraphBulletProperties::textStyle() const
{
    return m_textStyle;
}

bool Utils::ParagraphBulletProperties::startOverride() const
{
    return m_startOverride;
}

void Utils::ParagraphBulletProperties::addInheritedValues(const ParagraphBulletProperties& properties)
{
    // This function is intented for helping to inherit some values from other properties
    if (m_level == -1) {
        m_level = properties.m_level;
    }
    if (properties.m_type != ParagraphBulletProperties::DefaultType) {
        m_type = properties.m_type;
    }
    if (properties.m_startValue != "1") {
        m_startValue = properties.m_startValue;
    }
    if (properties.m_bulletFont != UNUSED) {
        m_bulletFont = properties.m_bulletFont;
    }
    if (properties.m_bulletChar != UNUSED) {
        m_bulletChar = properties.m_bulletChar;
    }
    if (properties.m_numFormat != UNUSED) {
        m_numFormat = properties.m_numFormat;
    }
    if (properties.m_prefix != UNUSED) {
        m_prefix = properties.m_prefix;
    }
    if (properties.m_suffix != UNUSED) {
        m_suffix = properties.m_suffix;
    }
    if (properties.m_align != UNUSED) {
        m_align = properties.m_align;
    }
    if (properties.m_indent != UNUSED) {
        m_indent = properties.m_indent;
    }
    if (properties.m_margin != UNUSED) {
        m_margin = properties.m_margin;
    }
    if (properties.m_picturePath != UNUSED) {
        m_picturePath = properties.m_picturePath;
    }
    if (properties.m_bulletColor != UNUSED) {
        m_bulletColor = properties.m_bulletColor;
    }
    if (properties.m_bulletRelativeSize != UNUSED) {
        m_bulletRelativeSize = properties.m_bulletRelativeSize;
    }
    if (properties.m_bulletSize != UNUSED) {
        m_bulletSize = properties.m_bulletSize;
    }
    if (properties.m_followingChar != UNUSED) {
        m_followingChar = properties.m_followingChar;
    }
    if (!(properties.m_textStyle == m_textStyle)) {
        KoGenStyle::copyPropertiesFromStyle(properties.m_textStyle, m_textStyle, KoGenStyle::TextType);
    }
}

QString Utils::ParagraphBulletProperties::convertToListProperties(KoGenStyles& mainStyles, Utils::MSOOXMLFilter currentFilter)
{
    QBuffer buf;
    buf.open(QIODevice::WriteOnly);
    KoXmlWriter out(&buf);

    //---------------------------------------------
    // list-level-style-*
    //---------------------------------------------
    if (m_type == ParagraphBulletProperties::NumberType) {
        out.startElement("text:list-level-style-number");
        if (m_numFormat != UNUSED) {
            out.addAttribute("style:num-format", m_numFormat);
        }
        if (m_prefix != UNUSED) {
            out.addAttribute("style:num-prefix", m_prefix);
        }
        if (m_suffix != UNUSED) {
            out.addAttribute("style:num-suffix", m_suffix);
        }
        out.addAttribute("text:start-value", m_startValue);
    }
    else if (m_type == ParagraphBulletProperties::PictureType) {
        out.startElement("text:list-level-style-image");
        out.addAttribute("xlink:href", m_picturePath);
        out.addAttribute("xlink:type", "simple");
        out.addAttribute("xlink:show", "embed");
        out.addAttribute("xlink:actuate", "onLoad");
    }
    else {
        out.startElement("text:list-level-style-bullet");
        if (m_bulletChar.length() != 1) {
            // TODO: if there is no bullet char this should not be
            // saved as list but as normal paragraph. Both LO and MSO
            // do export it just as paragraph and no list until there
            // is a fix available that change that we use a Zero Width
            // Space to not generate invalid xml
            out.addAttribute("text:bullet-char", QChar(0x200B));
        } else {
            out.addAttribute("text:bullet-char", m_bulletChar);
        }
    }
    out.addAttribute("text:level", m_level);

    //---------------------------------------------
    // text-properties
    //---------------------------------------------
    //
    // NOTE: Setting a num. of text-properties to default values if
    // not provided for the list style to maintain compatibility with
    // both ODF and MSOffice.

    QString bulletSize;
    if (m_bulletRelativeSize != UNUSED) {
        bulletSize = QString(m_bulletRelativeSize).append("%");
    } else if (m_bulletSize != UNUSED) {
        bulletSize = QString(m_bulletSize).append("pt");
    } else {
        bulletSize = "100%";
    }

    // MSWord: A label does NOT inherit Underline from text-properties
    // of the paragraph style.  A bullet does not inherit {Italics, Bold}.
    if (currentFilter == Utils::DocxFilter && m_type != ParagraphBulletProperties::PictureType) {
        if (m_type != ParagraphBulletProperties::NumberType) {
            if ((m_textStyle.property("fo:font-style")).isEmpty()) {
                m_textStyle.addProperty("fo:font-style", "normal");
            }
            if ((m_textStyle.property("fo:font-weight")).isEmpty()) {
                m_textStyle.addProperty("fo:font-weight", "normal");
            }
        }
        if ((m_textStyle.property("style:text-underline-style")).isEmpty()) {
            m_textStyle.addProperty("style:text-underline-style", "none");
        }
        //fo:font-size
        if ((m_textStyle.property("fo:font-size")).isEmpty()) {
            m_textStyle.addProperty("fo:font-size", bulletSize);
        }
        out.addAttribute("text:style-name", mainStyles.insert(m_textStyle, "T"));
    }
    //---------------------------------------------
    // list-level-properties
    //---------------------------------------------
    out.startElement("style:list-level-properties");
    if (m_align != UNUSED) {
        out.addAttribute("fo:text-align", m_align);
    }
    if ((m_type == ParagraphBulletProperties::PictureType) && (m_bulletSize != UNUSED)) {
        QString size = QString(m_bulletSize).append("pt");
        out.addAttribute("fo:width", size);
        out.addAttribute("fo:height", size);
    }

    out.addAttribute("text:list-level-position-and-space-mode", "label-alignment");
    // NOTE: DrawingML: If indent and marL were not provided by a master slide
    // or defaults, then according to the spec. a value of -342900 is implied
    // for indent and a value of 347663 is implied for marL (no matter which
    // level and which type of text).  However the result is not compliant with
    // MS PowerPoint => using ZERO values as in the ppt filter.
    double margin = 0;
    double indent = 0;
    bool ok = false;

    if (m_margin != UNUSED) {
        margin = m_margin.toDouble(&ok);
        if (!ok) {
            debugMsooXml << "STRING_TO_DOUBLE: error converting" << m_margin << "(attribute \"marL\")";
        }
    }
    if (m_indent != UNUSED) {
        indent = m_indent.toDouble(&ok);
        if (!ok) {
            debugMsooXml << "STRING_TO_DOUBLE: error converting" << m_indent << "(attribute \"indent\")";
        }
    }
    out.startElement("style:list-level-label-alignment");

    if (currentFilter == Utils::PptxFilter) {
        //fo:margin-left
        out.addAttributePt("fo:margin-left", margin);

        if (((m_type == ParagraphBulletProperties::BulletType) && m_bulletChar.isEmpty()) ||
            (m_type == ParagraphBulletProperties::DefaultType))
        {
            //hanging:
            if (indent < 0) {
                if (qAbs(indent) > margin) {
                    out.addAttributePt("fo:text-indent", -margin);
                } else {
                    out.addAttributePt("fo:text-indent", indent);
                }
            }
            //first-line and none:
            else {
                out.addAttributePt("fo:text-indent", indent);
            }
            out.addAttribute("text:label-followed-by", "nothing");
        } else {
            //hanging:
            if (indent < 0) {
                if (qAbs(indent) > margin) {
                    out.addAttributePt("fo:text-indent", -margin);
                    out.addAttribute("text:label-followed-by", "listtab");
                    out.addAttributePt("text:list-tab-stop-position", qAbs(indent));
                } else {
                    out.addAttributePt("fo:text-indent", indent);
                    out.addAttribute("text:label-followed-by", "listtab");
                    out.addAttributePt("text:list-tab-stop-position", margin);
                }
            }
            //first-line:
            else if (indent > 0) {
                out.addAttribute("fo:text-indent", "0pt");
                out.addAttribute("text:label-followed-by", "listtab");
                out.addAttributePt("text:list-tab-stop-position", margin + indent);
            }
            //none
            else {
                out.addAttribute("fo:text-indent", "0pt");
                out.addAttribute("text:label-followed-by", "nothing");
            }
        }
    } else {
        //fo:margin-left
        out.addAttributePt("fo:margin-left", margin);
        //fo:text-indent
        out.addAttributePt("fo:text-indent", indent);
        //text:label-followed-by
        if ((m_followingChar == "tab") || (m_followingChar == UNUSED)) {
            out.addAttribute("text:label-followed-by", "listtab");
            // Layout hints: none/first-line/hanging are values from the
            // Special field of the Paragraph dialog in MS Word.
            //
            // first-line:
            // IF (indent > 0) and (margin > 0), THEN use default tab stop OR a custom tab stop if defined.
            // IF (indent > 0) and (margin == 0), THEN use default tab stop OR a custom tab stop if defined.
            // IF (indent > 0) and (margin < 0), THEN use default tab stop OR a custom tab stop if defined.
            //
            // none:
            // IF (indent == 0) and (margin > 0), THEN use default tab stop OR a custom tab stop if defined.
            // IF (indent == 0) and (margin == 0), THEN use default tab stop OR a custom tab stop if defined.
            // IF (indent == 0) and (margin < 0), THEN use default tab stop OR a custom tab stop if defined.
            //
            // hanging:
            // 1. the tab should be placed at the margin position
            // 2. bullet_position = margin - indent; (that's the indentation
            // left value that can be seen in Paragraph dialog in MS Word)
        }
        //space and nothing are same in OOXML and ODF
        else {
            out.addAttribute("text:label-followed-by", m_followingChar);
        }
    }
    out.endElement(); //style:list-level-label-alignment
    out.endElement(); //style:list-level-properties
    if (currentFilter != Utils::DocxFilter && m_type != ParagraphBulletProperties::PictureType) {
        out.startElement("style:text-properties");
        if (m_bulletColor != UNUSED) {
            out.addAttribute("fo:color", m_bulletColor);
        }
        out.addAttribute("fo:font-size", bulletSize);

        //MSPowerPoint: UI does not enable to change font of a numbered lists.
        if (m_bulletFont != UNUSED) {
            if ((currentFilter != Utils::PptxFilter) || (m_type == ParagraphBulletProperties::BulletType)) {
                out.addAttribute("fo:font-family", m_bulletFont);
            }
        }
        //MSPowerPoint: A label does NOT inherit Underline from text-properties
        //of the 1st text chunk.  A bullet does NOT inherit {Italics, Bold}.
        if (currentFilter == Utils::PptxFilter) {
            if (m_type != ParagraphBulletProperties::NumberType) {
                out.addAttribute("fo:font-style", "normal");
                out.addAttribute("fo:font-weight", "normal");
            }
            out.addAttribute("style:text-underline-style", "none");
        }
        out.endElement(); //style:text-properties
    }
    out.endElement(); //text:list-level-style-*

    return QString::fromUtf8(buf.buffer(), buf.buffer().size());
}

