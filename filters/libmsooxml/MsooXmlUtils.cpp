/*
 * This file is part of Office 2007 Filters for KOffice
 * Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
 * Copyright (c) 2003 Lukas Tinkl <lukas@kde.org>
 * Copyright (C) 2003 David Faure <faure@kde.org>
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * Utils::columnName() based on Cell::columnName() from koffice/kspread/Utils.cpp:
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

#include "pole.h"

#include <KoOdfReadStore.h>
#include <styles/KoCharacterStyle.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoUnit.h>

#include <qdom.h>
#include <QColor>
#include <QBrush>
#include <QImage>
#include <QImageReader>
#include <QPalette>
#include <QRegExp>
#include <QtXml>

#include <KGlobal>
#include <KDebug>
#include <KZip>

#include <memory>

// common officedocument content types
const char* const MSOOXML::ContentTypes::coreProps =            "application/vnd.openxmlformats-package.core-properties+xml";
const char* const MSOOXML::ContentTypes::extProps =             "application/vnd.openxmlformats-officedocument.extended-properties+xml";
const char* const MSOOXML::ContentTypes::theme =                "application/vnd.openxmlformats-officedocument.theme+xml";

// wordprocessingml-specific content types
const char* const MSOOXML::ContentTypes::wordDocument =         "application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml";
const char* const MSOOXML::ContentTypes::wordSettings =         "application/vnd.openxmlformats-officedocument.wordprocessingml.settings+xml";
const char* const MSOOXML::ContentTypes::wordStyles =           "application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml";
const char* const MSOOXML::ContentTypes::wordHeader =           "application/vnd.openxmlformats-officedocument.wordprocessingml.header+xml";
const char* const MSOOXML::ContentTypes::wordFooter =           "application/vnd.openxmlformats-officedocument.wordprocessingml.footer+xml";
const char* const MSOOXML::ContentTypes::wordFootnotes =        "application/vnd.openxmlformats-officedocument.wordprocessingml.footnotes+xml";
const char* const MSOOXML::ContentTypes::wordEndnotes =         "application/vnd.openxmlformats-officedocument.wordprocessingml.endnotes+xml";
const char* const MSOOXML::ContentTypes::wordFontTable =        "application/vnd.openxmlformats-officedocument.wordprocessingml.fontTable+xml";
const char* const MSOOXML::ContentTypes::wordWebSettings =      "application/vnd.openxmlformats-officedocument.wordprocessingml.webSettings+xml";
const char* const MSOOXML::ContentTypes::wordTemplate =         "application/vnd.openxmlformats-officedocument.wordprocessingml.template.main+xml";
const char* const MSOOXML::ContentTypes::wordComments =         "application/vnd.openxmlformats-officedocument.wordprocessingml.comments+xml";

// presentationml-specific content types
const char* const MSOOXML::ContentTypes::presentationDocument =      "application/vnd.openxmlformats-officedocument.presentationml.presentation.main+xml";
const char* const MSOOXML::ContentTypes::presentationSlide =         "application/vnd.openxmlformats-officedocument.presentationml.slide+xml";
const char* const MSOOXML::ContentTypes::presentationSlideLayout =   "application/vnd.openxmlformats-officedocument.presentationml.slideLayout+xml";
const char* const MSOOXML::ContentTypes::presentationSlideShow =     "application/vnd.openxmlformats-officedocument.presentationml.slideshow.main+xml";
const char* const MSOOXML::ContentTypes::presentationTemplate =      "application/vnd.openxmlformats-officedocument.presentationml.template.main+xml";
const char* const MSOOXML::ContentTypes::presentationNotes =         "application/vnd.openxmlformats-officedocument.presentationml.notesMaster+xml";
const char* const MSOOXML::ContentTypes::presentationTableStyles =   "application/vnd.openxmlformats-officedocument.presentationml.tableStyles+xml";
const char* const MSOOXML::ContentTypes::presentationProps =         "application/vnd.openxmlformats-officedocument.presentationml.presProps+xml";
const char* const MSOOXML::ContentTypes::presentationViewProps =     "application/vnd.openxmlformats-officedocument.presentationml.viewProps+xml";
const char* const MSOOXML::ContentTypes::presentationComments =      "application/vnd.openxmlformats-officedocument.presentationml.comments+xml";

// spreadsheetml-specific content types
const char* const MSOOXML::ContentTypes::spreadsheetDocument =        "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml";
const char* const MSOOXML::ContentTypes::spreadsheetMacroDocument =   "application/vnd.ms-excel.sheet.macroEnabled.main+xml";
const char* const MSOOXML::ContentTypes::spreadsheetPrinterSettings = "application/vnd.openxmlformats-officedocument.spreadsheetml.printerSettings";
const char* const MSOOXML::ContentTypes::spreadsheetStyles =          "application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml";
const char* const MSOOXML::ContentTypes::spreadsheetWorksheet =       "application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml";
const char* const MSOOXML::ContentTypes::spreadsheetCalcChain =       "application/vnd.openxmlformats-officedocument.spreadsheetml.calcChain+xml";
const char* const MSOOXML::ContentTypes::spreadsheetSharedStrings =   "application/vnd.openxmlformats-officedocument.spreadsheetml.sharedStrings+xml";
const char* const MSOOXML::ContentTypes::spreadsheetTemplate =        "application/vnd.openxmlformats-officedocument.spreadsheetml.template.main+xml";
const char* const MSOOXML::ContentTypes::spreadsheetComments =        "application/vnd.openxmlformats-officedocument.spreadsheetml.comments+xml";

//generic namespaces
const char* const MSOOXML::Schemas::dublin_core =                                "http://purl.org/dc/elements/1.1/";

// common namespaces
const char* const MSOOXML::Schemas::contentTypes =                               "http://schemas.openxmlformats.org/package/2006/content-types";

const char* const MSOOXML::Schemas::relationships =                              "http://schemas.openxmlformats.org/package/2006/relationships";
const char* const MSOOXML::Schemas::core_properties =                            "http://schemas.openxmlformats.org/package/2006/metadata/core-properties";

// ISO/IEC 29500-1:2008(E), Annex A. (normative), p. 4355
// See also: specs/all.xsd
// A.1 WordprocessingML
const char* const MSOOXML::Schemas::wordprocessingml =                           "http://schemas.openxmlformats.org/wordprocessingml/2006/main";

// A.2 SpreadsheetML
const char* const MSOOXML::Schemas::spreadsheetml =                              "http://schemas.openxmlformats.org/spreadsheetml/2006/main";

// A.3 PresentationML
const char* const MSOOXML::Schemas::presentationml =                             "http://schemas.openxmlformats.org/presentationml/2006/main";

// A.4 DrawingML - Framework
const char* const MSOOXML::Schemas::drawingml::main =                            "http://schemas.openxmlformats.org/drawingml/2006/main";
const char* const MSOOXML::Schemas::drawingml::wordprocessingDrawing =           "http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing";
const char* const MSOOXML::Schemas::drawingml::spreadsheetDrawing =              "http://schemas.openxmlformats.org/drawingml/2006/spreadsheetDrawing";
const char* const MSOOXML::Schemas::drawingml::compatibility =                   "http://schemas.openxmlformats.org/drawingml/2006/compatibility";
const char* const MSOOXML::Schemas::drawingml::lockedCanvas =                    "http://schemas.openxmlformats.org/drawingml/2006/lockedCanvas";
const char* const MSOOXML::Schemas::drawingml::picture =                         "http://schemas.openxmlformats.org/drawingml/2006/picture";

// A.5 DrawingML - Components
const char* const MSOOXML::Schemas::drawingml::chart =                           "http://schemas.openxmlformats.org/drawingml/2006/chart";
const char* const MSOOXML::Schemas::drawingml::chartDrawing =                    "http://schemas.openxmlformats.org/drawingml/2006/chartDrawing";
const char* const MSOOXML::Schemas::drawingml::diagram =                         "http://schemas.openxmlformats.org/drawingml/2006/diagram";

// A.6 Shared MLs
const char* const MSOOXML::Schemas::officeDocument::math =                       "http://schemas.openxmlformats.org/officeDocument/2006/math";
const char* const MSOOXML::Schemas::officeDocument::bibliography =               "http://schemas.openxmlformats.org/officeDocument/2006/bibliography";
const char* const MSOOXML::Schemas::officeDocument::characteristics =            "http://schemas.openxmlformats.org/officeDocument/2006/characteristics";
const char* const MSOOXML::Schemas::officeDocument::customXml =                  "http://schemas.openxmlformats.org/officeDocument/2006/customXml";
const char* const MSOOXML::Schemas::officeDocument::custom_properties =          "http://schemas.openxmlformats.org/officeDocument/2006/custom-properties";
const char* const MSOOXML::Schemas::officeDocument::docPropsVTypes =             "http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes";
const char* const MSOOXML::Schemas::officeDocument::extended_properties =        "http://schemas.openxmlformats.org/officeDocument/2006/extended-properties";
const char* const MSOOXML::Schemas::officeDocument::relationships =              "http://schemas.openxmlformats.org/officeDocument/2006/relationships";
const char* const MSOOXML::Schemas::officeDocument::sharedTypes =                "http://schemas.openxmlformats.org/officeDocument/2006/sharedTypes";

// A.7 Custom XML Schema References
const char* const MSOOXML::Schemas::schemaLibrary =                              "http://schemas.openxmlformats.org/schemaLibrary/2006/main";

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
        kError() << "Parsing error in " << fileName << ", aborting!" << endl
        << " In line: " << errorLine << ", column: " << errorColumn << endl
        << " Error message: " << errorMsg;
        errorMessage = i18n("Parsing error in the main document at line %1, column %2.\n"
                            "Error message: %3", errorLine , errorColumn , i18n("QXml", errorMsg));
        return KoFilter::ParsingError;
    }
    kDebug() << "File" << fileName << "loaded and parsed.";
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
    kDebug() << "File" << fileName << "loaded and parsed.";
    return KoFilter::OK;
}

QIODevice* Utils::openDeviceForFile(const KZip* zip, QString& errorMessage, const QString& fileName,
                                    KoFilter::ConversionStatus& status)
{
    kDebug() << "Trying to open" << fileName;
    errorMessage.clear();
    const KArchiveEntry* entry = zip->directory()->entry(fileName);
    if (!entry) {
        errorMessage = i18n("Entry '%1' not found.", fileName);
        kDebug() << errorMessage;
        status = KoFilter::FileNotFound;
        return 0;
    }
    if (!entry->isFile()) {
        errorMessage = i18n("Entry '%1' is not a file.", fileName);
        kDebug() << errorMessage;
        status = KoFilter::WrongFormat;
        return 0;
    }
    const KZipFileEntry* f = static_cast<const KZipFileEntry *>(entry);
    kDebug() << "Entry" << fileName << "has size" << f->size();
    status = KoFilter::OK;
    return f->createDevice();
}

#define BLOCK_SIZE 4096
static KoFilter::ConversionStatus copyOle(QString& errorMessage,
                                    const QString sourceName, KoStore *outputStore,
                                    const QString& destinationName, const KZip* zip)
{
    KoFilter::ConversionStatus status = KoFilter::OK;

    QIODevice* inputDevice = Utils::openDeviceForFile(zip, errorMessage, sourceName, status);
    inputDevice->open(QIODevice::ReadOnly);

    POLE::Storage storage(inputDevice);
    if (!storage.open()) {
        kDebug(30513) << "Cannot open " << sourceName;
        return KoFilter::WrongFormat;
    }

    std::list<std::string> lista = storage.entries();
    std::string oleType = "Contents";

    for (std::list<std::string>::iterator it = lista.begin(); it != lista.end(); ++it)  {
        //qDebug() << "ENTRY " << (*it).c_str();
        if (QString((*it).c_str()).contains("Ole10Native")) {
            oleType = "Ole10Native";
        }
        else if (QString((*it).c_str()).contains("CONTENTS")) {
            oleType = "CONTENTS";
        }
    }

    POLE::Stream stream(&storage, oleType);
    QByteArray array;
    array.resize(stream.size());

    unsigned long r = stream.read((unsigned char*)array.data(), stream.size());
    if (r != stream.size()) {
        kError(30513) << "Error while reading from stream";
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

    kDebug() << "mode:" << outputStore->mode();
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

    kDebug() << "mode:" << outputStore->mode();
    if (!outputStore->open(destinationName)) {
        errorMessage = i18n("Could not open entry \"%1\" for writing.", destinationName);
        return KoFilter::CreationError;
    }
    status = KoFilter::OK;
    char block[BLOCK_SIZE];
    while (true) {
        const qint64 in = inputDevice->read(block, BLOCK_SIZE);
//        kDebug() << "in:" << in;
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
    kDebug() << *size;
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
        kWarning()
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
        kWarning() << "Invalid namespace URI" << el.namespaceURI() << " expected:" << expectedNsUri;
        return false;
    }
    return true;
}

bool Utils::convertBooleanAttr(const QString& value, bool defaultValue)
{
    const QByteArray val(value.toLatin1());
    if (val.isEmpty())
        return defaultValue;
kDebug() << val;
    
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
                kWarning() << "Invalid data for" << tagName
                << "element: PartName=" << atrPartName << "ContentType=" << atrContentType;
                return KoFilter::WrongFormat;
            }
//kDebug() << atrContentType << "->" << atrPartName;
            contentTypes.insert(atrContentType, atrPartName);
        } else if (tagName == "Default") {
//! @todo
            // skip for now...
        }
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

QBrush Utils::ST_HighlightColor_to_QColor(const QString& colorName)
{
    K_GLOBAL_STATIC(ST_HighlightColorMapping, s_ST_HighlightColor_to_QColor)
    const QColor c(s_ST_HighlightColor_to_QColor->value(colorName));
    if (c.isValid())
        return QBrush(c);
    return QBrush(); // for "none" or anything unsupported
}

class DefaultIndexedColors : public QList< QColor >
{
public:
    DefaultIndexedColors()
    {
        push_back( QColor( 0, 0, 0 ) );
        push_back( QColor( 255, 255, 255 ) );
        push_back( QColor( 255, 0, 0 ) );
        push_back( QColor( 0, 255, 0 ) );
        push_back( QColor( 0, 0, 255 ) );
        push_back( QColor( 255, 255, 0 ) );
        push_back( QColor( 255, 0, 255 ) );
        push_back( QColor( 0, 255, 255 ) );
        push_back( QColor( 0, 0, 0 ) );
        push_back( QColor( 255, 255, 255 ) );
        push_back( QColor( 255, 0, 0 ) );
        push_back( QColor( 0, 255, 0 ) );
        push_back( QColor( 0, 0, 255 ) );
        push_back( QColor( 255, 255, 0 ) );
        push_back( QColor( 255, 0, 255 ) );
        push_back( QColor( 0, 255, 255 ) );
        push_back( QColor( 128, 0, 0 ) );
        push_back( QColor( 0, 128, 0 ) );
        push_back( QColor( 0, 0, 128 ) );
        push_back( QColor( 128, 128, 0 ) );
        push_back( QColor( 128, 0, 128 ) );
        push_back( QColor( 0, 128, 128 ) );
        push_back( QColor( 192, 192, 192 ) );
        push_back( QColor( 128, 128, 128 ) );
        push_back( QColor( 153, 153, 255 ) );
        push_back( QColor( 153, 51, 102 ) );
        push_back( QColor( 255, 255, 204 ) );
        push_back( QColor( 204, 255, 255 ) );
        push_back( QColor( 102, 0, 102 ) );
        push_back( QColor( 255, 128, 128 ) );
        push_back( QColor( 0, 102, 204 ) );
        push_back( QColor( 204, 204, 255 ) );
        push_back( QColor( 0, 0, 128 ) );
        push_back( QColor( 255, 0, 255 ) );
        push_back( QColor( 255, 255, 0 ) );
        push_back( QColor( 0, 255, 255 ) );
        push_back( QColor( 128, 0, 128 ) );
        push_back( QColor( 128, 0, 0 ) );
        push_back( QColor( 0, 128, 128 ) );
        push_back( QColor( 0, 0, 255 ) );
        push_back( QColor( 0, 204, 255 ) );
        push_back( QColor( 204, 255, 255 ) );
        push_back( QColor( 204, 255, 204 ) );
        push_back( QColor( 255, 255, 153 ) );
        push_back( QColor( 153, 204, 255 ) );
        push_back( QColor( 255, 153, 204 ) );
        push_back( QColor( 204, 153, 255 ) );
        push_back( QColor( 255, 204, 153 ) );
        push_back( QColor( 51, 102, 255 ) );
        push_back( QColor( 51, 204, 204 ) );
        push_back( QColor( 153, 204, 0 ) );
        push_back( QColor( 255, 204, 0 ) );
        push_back( QColor( 255, 153, 0 ) );
        push_back( QColor( 255, 102, 0 ) );
        push_back( QColor( 102, 102, 153 ) );
        push_back( QColor( 150, 150, 150 ) );
        push_back( QColor( 0, 51, 102 ) );
        push_back( QColor( 51, 102, 153 ) );
        push_back( QColor( 0, 51, 0 ) );
        push_back( QColor( 51, 51, 0 ) );
        push_back( QColor( 153, 51, 0 ) );
        push_back( QColor( 153, 51, 102 ) );
        push_back( QColor( 51, 51, 153 ) );
        push_back( QColor( 51, 51, 51 ) );
        push_back( QPalette().color( QPalette::Active, QPalette::WindowText ) );
        push_back( QPalette().color( QPalette::Active, QPalette::Window ) );
     }
};

QColor Utils::defaultIndexedColor( int index )
{
    K_GLOBAL_STATIC( DefaultIndexedColors, s_defaultIndexedColors )
    if( index < 0 || s_defaultIndexedColors->count() <= index )
        return QColor();
    return s_defaultIndexedColors->at( index );
}

class LangIdToLocaleMapping : public QMap< int, QString >
{
public:
    LangIdToLocaleMapping()
    {
#define DEFINELOCALE( ID, CODE ) insert( ID, QLatin1String( CODE ) );
        DEFINELOCALE( 0x0436, "af-ZA" ) //  Afrikaans   South Africa
        DEFINELOCALE( 0x041c, "sq-AL" ) //  Albanian    Albania
        DEFINELOCALE( 0x0484, "gsw-FR" ) //  Alsatian    France
        DEFINELOCALE( 0x045e, "am-ET" ) //   Amharic     Ethiopia
        DEFINELOCALE( 0x0401, "ar-SA" ) //   Arabic  Saudi Arabia
        DEFINELOCALE( 0x0801, "ar-IQ" ) //   Arabic  Iraq
        DEFINELOCALE( 0x0c01, "ar-EG" ) //   Arabic  Egypt
        DEFINELOCALE( 0x1001, "ar-LY" ) //   Arabic  Libya
        DEFINELOCALE( 0x1401, "ar-DZ" ) //   Arabic  Algeria
        DEFINELOCALE( 0x1801, "ar-MA" ) //   Arabic  Morocco
        DEFINELOCALE( 0x1c01, "ar-TN" ) //   Arabic  Tunisia
        DEFINELOCALE( 0x2001, "ar-OM" ) //   Arabic  Oman
        DEFINELOCALE( 0x2401, "ar-YE" ) //   Arabic  Yemen
        DEFINELOCALE( 0x2801, "ar-SY" ) //   Arabic  Syria
        DEFINELOCALE( 0x2c01, "ar-JO" ) //   Arabic  Jordan
        DEFINELOCALE( 0x3001, "ar-LB" ) //   Arabic  Lebanon
        DEFINELOCALE( 0x3401, "ar-KW" ) //   Arabic  Kuwait
        DEFINELOCALE( 0x3801, "ar-AE" ) //   Arabic  U.A.E.
        DEFINELOCALE( 0x3c01, "ar-BH" ) //   Arabic  Bahrain
        DEFINELOCALE( 0x4001, "ar-QA" ) //   Arabic  Qatar
        DEFINELOCALE( 0x042b, "hy-AM" ) //   Armenian    Armenia
        DEFINELOCALE( 0x044d, "as-IN" ) //   Assamese    India
        DEFINELOCALE( 0x082c, "az-Cyrl-AZ" ) //  Azeri (Cyrillic)    Azerbaijan
        DEFINELOCALE( 0x042c, "az-Latn-AZ" ) //  Azeri (Latin)   Azerbaijan
        DEFINELOCALE( 0x046d, "ba-RU" ) //   Bashkir     Russia
        DEFINELOCALE( 0x042d, "eu-ES" ) //   Basque  Basque
        DEFINELOCALE( 0x0423, "be-BY" ) //   Belarusian  Belarus
        DEFINELOCALE( 0x0445, "bn-IN" ) //   Bengali     India
        DEFINELOCALE( 0x0845, "bn-BD" ) //   Bengali     Bangladesh
        DEFINELOCALE( 0x201a, "bs-Cyrl-BA" ) //  Bosnian (Cyrillic)  Bosnia and Herzegovina
        DEFINELOCALE( 0x141a, "bs-Latn-BA" ) //  Bosnian (Latin)     Bosnia and Herzegovina
        DEFINELOCALE( 0x047e, "br-FR" ) //   Breton  France
        DEFINELOCALE( 0x0402, "bg-BG" ) //   Bulgarian   Bulgaria
        DEFINELOCALE( 0x0403, "ca-ES" ) //   Catalan     Catalan
        DEFINELOCALE( 0x0404, "zh-TW" ) //   Chinese     Taiwan
        DEFINELOCALE( 0x0804, "zh-CN" ) //   Chinese     PRC
        DEFINELOCALE( 0x0c04, "zh-HK" ) //   Chinese     Hong Kong SAR
        DEFINELOCALE( 0x1004, "zh-SG" ) //   Chinese     Singapore
        DEFINELOCALE( 0x1404, "zh-MO" ) //   Chinese     Macao SAR
        DEFINELOCALE( 0x0483, "co-FR" ) //   Corsican    France
        DEFINELOCALE( 0x041a, "hr-HR" ) //   Croatian    Croatia
        DEFINELOCALE( 0x101a, "hr-BA" ) //   Croatian (Latin)    Bosnia and Herzegovina
        DEFINELOCALE( 0x0405, "cs-CZ" ) //   Czech   Czech Republic
        DEFINELOCALE( 0x0406, "da-DK" ) //   Danish  Denmark
        DEFINELOCALE( 0x048c, "prs-AF" ) //  Dari    Afghanistan
        DEFINELOCALE( 0x0465, "dv-MV" ) //   Divehi  Maldives
        DEFINELOCALE( 0x0813, "nl-BE" ) //   Dutch   Belgium
        DEFINELOCALE( 0x0413, "nl-NL" ) //   Dutch   Netherlands
        DEFINELOCALE( 0x1009, "en-CA" ) //   English     Canada
        DEFINELOCALE( 0x2009, "en-JM" ) //   English     Jamaica
        DEFINELOCALE( 0x2409, "en-029" ) //  English     Caribbean
        DEFINELOCALE( 0x2809, "en-BZ" ) //   English     Belize
        DEFINELOCALE( 0x2c09, "en-TT" ) //   English     Trinidad
        DEFINELOCALE( 0x0809, "en-GB" ) //   English     United Kingdom
        DEFINELOCALE( 0x1809, "en-IE" ) //   English     Ireland
        DEFINELOCALE( 0x4009, "en-IN" ) //   English     India
        DEFINELOCALE( 0x1c09, "en-ZA" ) //   English     South Africa
        DEFINELOCALE( 0x3009, "en-ZW" ) //   English     Zimbabwe
        DEFINELOCALE( 0x0c09, "en-AU" ) //   English     Australia
        DEFINELOCALE( 0x1409, "en-NZ" ) //   English     New Zealand
        DEFINELOCALE( 0x3409, "en-PH" ) //   English     Philippines
        DEFINELOCALE( 0x0409, "en-US" ) //   English     United States
        DEFINELOCALE( 0x4409, "en-MY" ) //   English     Malaysia
        DEFINELOCALE( 0x4809, "en-SG" ) //   English     Singapore
        DEFINELOCALE( 0x0425, "et-EE" ) //   Estonian    Estonia
        DEFINELOCALE( 0x0438, "fo-FO" ) //   Faroese     Faroe Islands
        DEFINELOCALE( 0x0464, "fil-PH" ) //  Filipino    Philippines
        DEFINELOCALE( 0x040b, "fi-FI" ) //   Finnish     Finland
        DEFINELOCALE( 0x0c0c, "fr-CA" ) //   French  Canada
        DEFINELOCALE( 0x040c, "fr-FR" ) //   French  France
        DEFINELOCALE( 0x180c, "fr-MC" ) //   French  Monaco
        DEFINELOCALE( 0x100c, "fr-CH" ) //   French  Switzerland
        DEFINELOCALE( 0x080c, "fr-BE" ) //   French  Belgium
        DEFINELOCALE( 0x140c, "fr-LU" ) //   French  Luxembourg
        DEFINELOCALE( 0x0462, "fy-NL" ) //   Frisian     Netherlands
        DEFINELOCALE( 0x0456, "gl-ES" ) //   Galician    Galician
        DEFINELOCALE( 0x0437, "ka-GE" ) //   Georgian    Georgia
        DEFINELOCALE( 0x0407, "de-DE" ) //   German  Germany
        DEFINELOCALE( 0x0807, "de-CH" ) //   German  Switzerland
        DEFINELOCALE( 0x0c07, "de-AT" ) //   German  Austria
        DEFINELOCALE( 0x1407, "de-LI" ) //   German  Liechtenstein
        DEFINELOCALE( 0x1007, "de-LU" ) //   German  Luxembourg
        DEFINELOCALE( 0x0408, "el-GR" ) //   Greek   Greece
        DEFINELOCALE( 0x046f, "kl-GL" ) //   Greenlandic     Greenland
        DEFINELOCALE( 0x0447, "gu-IN" ) //   Gujarati    India
        DEFINELOCALE( 0x0468, "ha-Latn-NG" ) //  Hausa   Nigeria
        DEFINELOCALE( 0x040d, "he-IL" ) //   Hebrew  Israel
        DEFINELOCALE( 0x0439, "hi-IN" ) //   Hindi   India
        DEFINELOCALE( 0x040e, "hu-HU" ) //   Hungarian   Hungary
        DEFINELOCALE( 0x040f, "is-IS" ) //   Icelandic   Iceland
        DEFINELOCALE( 0x0470, "ig-NG" ) //   Igbo    Nigeria
        DEFINELOCALE( 0x0421, "id-ID" ) //   Indonesian  Indonesia
        DEFINELOCALE( 0x045d, "iu-Cans-CA" ) //  Inukitut (Syllabics)    Canada
        DEFINELOCALE( 0x085d, "iu-Latn-CA" ) //  Inuktitut (Latin)   Canada
        DEFINELOCALE( 0x083c, "ga-IE" ) //   Irish   Ireland
        DEFINELOCALE( 0x0434, "xh-ZA" ) //   isiXhosa    South Africa
        DEFINELOCALE( 0x0435, "zu-ZA" ) //   isiZulu     South Africa
        DEFINELOCALE( 0x0410, "it-IT" ) //   Italian     Italy
        DEFINELOCALE( 0x0810, "it-CH" ) //   Italian     Switzerland
        DEFINELOCALE( 0x0411, "ja-JP" ) //   Japanese    Japan
        DEFINELOCALE( 0x044b, "kn-IN" ) //   Kannada     India
        DEFINELOCALE( 0x043f, "kk-KZ" ) //   Kazakh  Kazakhstan
        DEFINELOCALE( 0x0453, "km-KH" ) //   Khmer   Cambodia
        DEFINELOCALE( 0x0486, "qut-GT" ) //  K'iche  Guatemala
        DEFINELOCALE( 0x0487, "rw-RW" ) //   Kinyarwanda     Rwanda
        DEFINELOCALE( 0x0441, "sw-KE" ) //   Kiswahili   Kenya
        DEFINELOCALE( 0x0457, "kok-IN" ) //  Konkani     India
        DEFINELOCALE( 0x0412, "ko-KR" ) //   Korean  Korea
        DEFINELOCALE( 0x0440, "ky-KG" ) //   Kyrgyz  Kyrgyzistan
        DEFINELOCALE( 0x0454, "lo-LA" ) //   Lao     Lao P.D.R.
        DEFINELOCALE( 0x0426, "lv-LV" ) //   Latvian     Latvia
        DEFINELOCALE( 0x0427, "lt-LT" ) //   Lithuanian  Lithuania
        DEFINELOCALE( 0x082e, "dsb-DE" ) //  Lower Sorbian   Germany
        DEFINELOCALE( 0x046e, "lb-LU" ) //   Luxembourgish   Luxembourg
        DEFINELOCALE( 0x042f, "mk-MK" ) //   Macedonian (FYROM)  Macedonia (FYROM)
        DEFINELOCALE( 0x043e, "ms-MY" ) //   Malay   Malaysia
        DEFINELOCALE( 0x083e, "ms-BN" ) //   Malay   Brunei Darussalam
        DEFINELOCALE( 0x044c, "ml-IN" ) //   Malayalam   India
        DEFINELOCALE( 0x043a, "mt-MT" ) //   Maltese     Malta
        DEFINELOCALE( 0x0481, "mi-NZ" ) //   Maori   New Zealand
        DEFINELOCALE( 0x047a, "arn-CL" ) //  Mapudungun  Chile
        DEFINELOCALE( 0x044e, "mr-IN" ) //   Marathi     India
        DEFINELOCALE( 0x047c, "moh-CA" ) //  Mohawk  Mohawk
        DEFINELOCALE( 0x0450, "mn-MN" ) //   Mongolian (Cyrillic)    Mongolia
        DEFINELOCALE( 0x0850, "mn-Mong-CN" ) //  Mongolian (Mongolian)   PRC
        DEFINELOCALE( 0x0461, "ne-NP" ) //   Nepali  Nepal
        DEFINELOCALE( 0x0414, "nb-NO" ) //   Norwegian (Bokmål)  Norway
        DEFINELOCALE( 0x0814, "nn-NO" ) //   Norwegian (Nynorsk)     Norway
        DEFINELOCALE( 0x0482, "oc-FR" ) //   Occitan     France
        DEFINELOCALE( 0x0448, "or-IN" ) //   Oriya   India
        DEFINELOCALE( 0x0463, "ps-AF" ) //   Pashto  Afghanistan
        DEFINELOCALE( 0x0429, "fa-IR" ) //   Persian     Iran
        DEFINELOCALE( 0x0415, "pl-PL" ) //   Polish  Poland
        DEFINELOCALE( 0x0416, "pt-BR" ) //   Portuguese  Brazil
        DEFINELOCALE( 0x0816, "pt-PT" ) //   Portuguese  Portugal
        DEFINELOCALE( 0x0446, "pa-IN" ) //   Punjabi (Gurmukhi)  India
        DEFINELOCALE( 0x046b, "quz-BO" ) //  Quechua     Bolivia
        DEFINELOCALE( 0x086b, "quz-EC" ) //  Quechua     Ecuador
        DEFINELOCALE( 0x0c6b, "quz-PE" ) //  Quechua     Peru
        DEFINELOCALE( 0x0418, "ro-RO" ) //   Romanian    Romania
        DEFINELOCALE( 0x0417, "rm-CH" ) //   Romansh     Switzerland
        DEFINELOCALE( 0x0419, "ru-RU" ) //   Russian     Russia
        DEFINELOCALE( 0x243b, "smn-FI" ) //  Sami, Inari     Finland
        DEFINELOCALE( 0x143b, "smj-SE" ) //  Sami, Lule  Sweden
        DEFINELOCALE( 0x103b, "smj-NO" ) //  Sami, Lule  Norway
        DEFINELOCALE( 0x043b, "se-NO" ) //   Sami, Northern  Norway
        DEFINELOCALE( 0x083b, "se-SE" ) //   Sami, Northern  Sweden
        DEFINELOCALE( 0x0c3b, "se-FI" ) //   Sami, Northern  Finland
        DEFINELOCALE( 0x203b, "sms-FI" ) //  Sami, Skolt     Finland
        DEFINELOCALE( 0x183b, "sma-NO" ) //  Sami, Southern  Norway
        DEFINELOCALE( 0x1c3b, "sma-SE" ) //  Sami, Southern  Sweden
        DEFINELOCALE( 0x044f, "sa-IN" ) //   Sanskrit    India
        DEFINELOCALE( 0x0c1a, "sr-Cyrl-CS" ) //  Serbian (Cyrillic)  Serbia
        DEFINELOCALE( 0x1c1a, "sr-Cyrl-BA" ) //  Serbian (Cyrillic)  Bosnia and Herzegovina
        DEFINELOCALE( 0x081a, "sr-Latn-CS" ) //  Serbian (Latin)     Serbia
        DEFINELOCALE( 0x181a, "sr-Latn-BA" ) //  Serbian (Latin)     Bosnia and Herzegovina
        DEFINELOCALE( 0x046c, "nso-ZA" ) //  Sesotho sa Leboa    South Africa
        DEFINELOCALE( 0x0432, "tn-ZA" ) //   Setswana    South Africa
        DEFINELOCALE( 0x045b, "si-LK" ) //   Sinhala     Sri Lanka
        DEFINELOCALE( 0x041b, "sk-SK" ) //   Slovak  Slovakia
        DEFINELOCALE( 0x0424, "sl-SI" ) //   Slovenian   Slovenia
        DEFINELOCALE( 0x080a, "es-MX" ) //   Spanish     Mexico
        DEFINELOCALE( 0x100a, "es-GT" ) //   Spanish     Guatemala
        DEFINELOCALE( 0x140a, "es-CR" ) //   Spanish     Costa Rica
        DEFINELOCALE( 0x180a, "es-PA" ) //   Spanish     Panama
        DEFINELOCALE( 0x1c0a, "es-DO" ) //   Spanish     Dominican Republic
        DEFINELOCALE( 0x200a, "es-VE" ) //   Spanish     Venezuela
        DEFINELOCALE( 0x240a, "es-CO" ) //   Spanish     Colombia
        DEFINELOCALE( 0x280a, "es-PE" ) //   Spanish     Peru
        DEFINELOCALE( 0x2c0a, "es-AR" ) //   Spanish     Argentina
        DEFINELOCALE( 0x300a, "es-EC" ) //   Spanish     Ecuador
        DEFINELOCALE( 0x340a, "es-CL" ) //   Spanish     Chile
        DEFINELOCALE( 0x3c0a, "es-PY" ) //   Spanish     Paraguay
        DEFINELOCALE( 0x400a, "es-BO" ) //   Spanish     Bolivia
        DEFINELOCALE( 0x440a, "es-SV" ) //   Spanish     El Salvador
        DEFINELOCALE( 0x480a, "es-HN" ) //   Spanish     Honduras
        DEFINELOCALE( 0x4c0a, "es-NI" ) //   Spanish     Nicaragua
        DEFINELOCALE( 0x500a, "es-PR" ) //   Spanish     Puerto Rico
        DEFINELOCALE( 0x540a, "es-US" ) //   Spanish     United States
        DEFINELOCALE( 0x380a, "es-UY" ) //   Spanish     Uruguay
        DEFINELOCALE( 0x0c0a, "es-ES" ) //   Spanish (International Sort)    Spain
        DEFINELOCALE( 0x040a, "es-ES_tradnl" ) //    Spanish (Traditional Sort)  Spain
        DEFINELOCALE( 0x041d, "sv-SE" ) //   Swedish     Sweden
        DEFINELOCALE( 0x081d, "sv-FI" ) //   Swedish     Finland
        DEFINELOCALE( 0x045a, "syr-SY" ) //  Syriac  Syria
        DEFINELOCALE( 0x0428, "tg-Cyrl-TJ" ) //  Tajik   Tajikistan
        DEFINELOCALE( 0x085f, "tzm-Latn-DZ" ) //     Tamazight (Latin)   Algeria
        DEFINELOCALE( 0x0449, "ta-IN" ) //   Tamil   India
        DEFINELOCALE( 0x0444, "tt-RU" ) //   Tatar   Russia
        DEFINELOCALE( 0x044a, "te-IN" ) //   Telugu  India
        DEFINELOCALE( 0x041e, "th-TH" ) //   Thai    Thailand
        DEFINELOCALE( 0x0451, "bo-CN" ) //   Tibetan     PRC
        DEFINELOCALE( 0x041f, "tr-TR" ) //   Turkish     Turkey
        DEFINELOCALE( 0x0442, "tk-TM" ) //   Turkmen     Turkmenistan
        DEFINELOCALE( 0x0480, "ug-CN" ) //   Uighur  PRC
        DEFINELOCALE( 0x0422, "uk-UA" ) //   Ukrainian   Ukraine
        DEFINELOCALE( 0x042e, "wen-DE" ) //  Upper Sorbian   Germany
        DEFINELOCALE( 0x0420, "ur-PK" ) //   Urdu    Pakistan
        DEFINELOCALE( 0x0843, "uz-Cyrl-UZ" ) //  Uzbek (Cyrillic)    Uzbekistan
        DEFINELOCALE( 0x0443, "uz-Latn-UZ" ) //  Uzbek (Latin)   Uzbekistan
        DEFINELOCALE( 0x042a, "vi-VN" ) //   Vietnamese  Vietnam
        DEFINELOCALE( 0x0452, "cy-GB" ) //   Welsh   United Kingdom
        DEFINELOCALE( 0x0488, "wo-SN" ) //   Wolof   Senegal
        DEFINELOCALE( 0x0485, "sah-RU" ) //  Yakut   Russia
        DEFINELOCALE( 0x0478, "ii-CN" ) //   Yi  PRC
        DEFINELOCALE( 0x046a, "yo-NG" ) //   Yoruba  Nigeria

#undef DEFINELOCALE
    }
};

QLocale Utils::localeForLangId( int langid )
{
    K_GLOBAL_STATIC( LangIdToLocaleMapping, s_LangIdToLocaleMapping )
    return QLocale( s_LangIdToLocaleMapping->value( langid ) );
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

class ST_PlaceholderType_to_ODFMapping : public QHash<QByteArray, QByteArray>
{
public:
    ST_PlaceholderType_to_ODFMapping() {
        insert("body", "text");
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

QString Utils::ST_PlaceholderType_to_ODF(const QString& ecmaType)
{
    K_GLOBAL_STATIC(ST_PlaceholderType_to_ODFMapping, s_ST_PlaceholderType_to_ODF)
    QHash<QByteArray, QByteArray>::ConstIterator it(s_ST_PlaceholderType_to_ODF->constFind(ecmaType.toLatin1()));
    if (it == s_ST_PlaceholderType_to_ODF->constEnd())
        return QLatin1String("outline");
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

void Utils::rotateString(const qreal rotation, const qreal width, const qreal height, qreal& angle, qreal& xDiff, qreal& yDiff,
    bool flipH, bool flipV)
{
    angle = -(qreal)rotation * ((qreal)(M_PI) / (qreal)180.0)/ (qreal)60000.0;
    // Angle seems to be negative if one of the flips is active
    if ((flipH && !flipV) || (!flipH && flipV)) {
        angle = -angle;
    }
    //position change is calculated based on the fact that center point stays in the same location
    // Width/2 = Xnew + cos(angle)*Width/2 - sin(angle)*Height/2
    // Height/2 = Ynew + sin(angle)*Width/2 + cos(angle)*Height/2
    xDiff = width/2 - cos(-angle)*width/2 + sin(-angle)*height/2;
    yDiff = height/2 - sin(-angle)*width/2 - cos(-angle)*height/2;
}

void Utils::setupUnderLineStyle(const QString& msooxmlName, KoCharacterStyle* textStyleProperties)
{
    K_GLOBAL_STATIC(UnderlineStylesHash, s_underLineStyles)
    s_underLineStyles->setup(msooxmlName, textStyleProperties);
}

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
    if (m_origWriter || m_newWriter)
        return 0;
    m_origWriter = writer; // remember
    m_newWriter = new KoXmlWriter(&m_buffer, m_origWriter->indentLevel() + 1);
    return m_newWriter;
}

KoXmlWriter* Utils::XmlWriteBuffer::releaseWriter()
{
    Q_ASSERT(m_newWriter && m_origWriter);
    return releaseWriterInternal();
}

KoXmlWriter* Utils::XmlWriteBuffer::releaseWriterInternal()
{
    if (!m_newWriter || !m_origWriter)
        return 0;
    m_origWriter->addCompleteElement(&m_buffer);
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
        kWarning() << "No unit found in" << _string;
        return false;
    }
    bool ok;
    *number = string.toFloat(&ok);
    if (!ok)
        kWarning() << "Invalid number in" << _string;
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

MSOOXML_EXPORT QString Utils::ST_TwipsMeasure_to_pt(const QString& value)
{
    return ST_TwipsMeasure_to_ODF_with_unit(value, twipToPt, "pt");
}

qreal twipToCm(qreal v)
{
    return TWIP_TO_CM(v);
}

MSOOXML_EXPORT QString Utils::ST_TwipsMeasure_to_cm(const QString& value)
{
    return ST_TwipsMeasure_to_ODF_with_unit(value, twipToCm, "cm");
}

MSOOXML_EXPORT QString Utils::ST_PositiveUniversalMeasure_to_ODF(const QString& value)
{
    // a positive decimal number immediately following by a unit identifier.
    qreal number;
    QString unit;
    if (!splitNumberAndUnit(value, &number, &unit))
        return QString();
    // special case: pc is another name for pica
    if (unit == QString::fromLatin1("pc")) {
        return QString::number(number) + QLatin1String("pi");
    }
    if (!isUnitAcceptable(unit)) {
        kWarning() << "Unit" << unit << "not supported. Expected cm/mm/in/pt/pc/pi.";
        return QString();
    }
    return value; // the original is OK
}

MSOOXML_EXPORT QString Utils::rgbColor(const QString& color)
{
    QString newColor;
    if (color == "red") {
        newColor = "#ff0000";
    }
    else if (color == "green") {
        newColor = "#00ff00";
    }
    else if (color == "blue") {
        newColor = "#0000ff";
    }
    else if (color == "yellow") {
        newColor = "#ffff00 ";
    }
    else {
        newColor = color;
    }

    return newColor;
}

MSOOXML_EXPORT QString Utils::ST_PositiveUniversalMeasure_to_cm(const QString& value)
{
    QString v(ST_PositiveUniversalMeasure_to_ODF(value));
    if (v.isEmpty())
        return QString();
    return cmString(POINT_TO_CM(KoUnit::parseValue(v)));
}

// </units> -------------------

MSOOXML_EXPORT Utils::ParagraphBulletProperties::ParagraphBulletProperties() :
    m_startValue(0), m_type(ParagraphBulletProperties::BulletType), m_bulletChar(""), 
    m_align(""), m_indent(0)
{
}

MSOOXML_EXPORT bool Utils::ParagraphBulletProperties::isEmpty() const
{
    if (m_bulletChar == "" && m_type == ParagraphBulletProperties::BulletType) {
        return true;
    }
    return false;
}

MSOOXML_EXPORT void Utils::ParagraphBulletProperties::clear()
{
    m_startValue = 0;
    setBulletChar("");
    m_align = "";
    m_indent = 0;
}

MSOOXML_EXPORT void Utils::ParagraphBulletProperties::setAlign(const QString& align)
{
    m_align = align;
}

MSOOXML_EXPORT void Utils::ParagraphBulletProperties::setBulletChar(const QString& bulletChar)
{
    m_bulletChar = bulletChar;
    m_type = ParagraphBulletProperties::BulletType;
}

MSOOXML_EXPORT void Utils::ParagraphBulletProperties::setIndent(const qreal indent)
{
    m_indent = indent;
}

MSOOXML_EXPORT void Utils::ParagraphBulletProperties::setSuffix(const QString& suffixChar)
{
    m_suffix = suffixChar;
}

MSOOXML_EXPORT void Utils::ParagraphBulletProperties::setNumFormat(const QString& numFormat)
{
    m_numFormat = numFormat;
    m_type = ParagraphBulletProperties::NumberType;
}

MSOOXML_EXPORT QString Utils::ParagraphBulletProperties::convertToListProperties() const
{
    QString returnValue;
    if (m_type == ParagraphBulletProperties::NumberType) {
        returnValue = QString("<text:list-level-style-number text:level=\"%1\" ").arg(m_level);
        returnValue += QString("style:num-suffix=\"%1\" style:num-format=\"%2\" ").arg(m_suffix).arg(m_numFormat);
        if (m_startValue != 0) {
            returnValue += QString("style:start-value=\"%1\" ").arg(m_startValue);
        }
    }
    else {
        returnValue = QString("<text:list-level-style-bullet text:level=\"%1\" ").arg(m_level);
        returnValue += QString("text:bullet-char=\"%1\" ").arg(m_bulletChar);
    }
    if (!m_align.isEmpty()) {
        returnValue += QString("fo:text-align=\"%1\" ").arg(m_align);
    }
    if (m_indent > 0) {
        returnValue += QString("text:space-before=\"%1pt\" ").arg(m_indent);
    }
    returnValue += "/>";

    return returnValue;
}

MSOOXML_EXPORT void Utils::copyPropertiesFromStyle(const KoGenStyle& sourceStyle, KoGenStyle& targetStyle, KoGenStyle::PropertyType type)
{
    if (sourceStyle.isEmpty()) {
        return;
    }

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter tempWriter(&buffer);
    sourceStyle.writeStyleProperties(&tempWriter, type);

    QString content = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());

    // We have to add the properties in a loop
    // This works as long as text-properties don't have children
    // Currenty KoGenStyle doesn't support adding this in other ways.

    int separatorLocation = content.indexOf(' ');
    content = content.right(content.size() - (separatorLocation + 1));
    separatorLocation = content.indexOf(' ');
    if (separatorLocation < 0) {
        separatorLocation = content.indexOf('/');
    }
    while (separatorLocation > 0) {
        int equalSignLocation = content.indexOf('=');
        if (equalSignLocation < 0) {
            break;
        }
        QString propertyName = content.left(equalSignLocation);
        // Removing equal and one quota
        content = content.right(content.size() - (equalSignLocation) - 2);
        separatorLocation = content.indexOf('\"');
        if (separatorLocation < 0) {
            separatorLocation = content.indexOf('/');
        }
        QString propertyValue = content.left(separatorLocation);
        content = content.right(content.size() - (separatorLocation + 2));
        targetStyle.addProperty(propertyName, propertyValue, type);
    }
}

MSOOXML_EXPORT void Utils::modifyColor(QColor& color, qreal tint, qreal shade, qreal satMod)
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

    color = QColor(red, green, blue);
}

