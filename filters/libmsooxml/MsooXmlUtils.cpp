/*
 * This file is part of Office 2007 Filters for KOffice
 * Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
 * Copyright (c) 2003 Lukas Tinkl <lukas@kde.org>
 * Copyright (C) 2003 David Faure <faure@kde.org>
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
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
#include "MsooXmlContentTypes.h"
#include "MsooXmlSchemas.h"
#include "MsooXmlReader.h"

#include <KoOdfReadStore.h>
#include <styles/KoCharacterStyle.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoUnit.h>

#include <qdom.h>
#include <QColor>
#include <QBrush>
#include <QImage>
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

// spreadsheetml-specific content types
const char* const MSOOXML::ContentTypes::spreadsheetDocument =        "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml";
const char* const MSOOXML::ContentTypes::spreadsheetPrinterSettings = "application/vnd.openxmlformats-officedocument.spreadsheetml.printerSettings";
const char* const MSOOXML::ContentTypes::spreadsheetStyles =          "application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml";
const char* const MSOOXML::ContentTypes::spreadsheetWorksheet =       "application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml";
const char* const MSOOXML::ContentTypes::spreadsheetCalcChain =       "application/vnd.openxmlformats-officedocument.spreadsheetml.calcChain+xml";
const char* const MSOOXML::ContentTypes::spreadsheetSharedStrings =   "application/vnd.openxmlformats-officedocument.spreadsheetml.sharedStrings+xml";

// common namespaces
const char* const MSOOXML::Schemas::contentTypes =                               "http://schemas.openxmlformats.org/package/2006/content-types";

const char* const MSOOXML::Schemas::relationships =                              "http://schemas.openxmlformats.org/package/2006/relationships";

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
    // Error variables for QDomDocument::setContent
    errorMessage.clear();
    QString errorMsg;
    int errorLine, errorColumn;
    // We need to be able to see the space in <text:span> </text:span>, this is why
    // we activate the "report-whitespace-only-CharData" feature.
    // Unfortunately this leads to lots of whitespace text nodes in between real
    // elements in the rest of the document, watch out for that.
    QXmlInputSource source(io);
    // Copied from QDomDocumentPrivate::setContent, to change the whitespace thing
    QXmlSimpleReader reader;
    KoOdfReadStore::setupXmlReader( reader, true /*namespaceProcessing*/ );

    bool ok = doc.setContent(&source, &reader, &errorMsg, &errorLine, &errorColumn);
    if (!ok) {
        kError() << "Parsing error in " << fileName << ", aborting!" << endl
        << " In line: " << errorLine << ", column: " << errorColumn << endl
        << " Error message: " << errorMsg;
        errorMessage = i18n("Parsing error in the main document at line %1, column %2\n"
                            "Error message: %3", errorLine , errorColumn , i18n("QXml", errorMsg));
        return KoFilter::ParsingError;
    }
    kDebug() << "File" << fileName << "loaded and parsed.";
    return KoFilter::OK;
}

KoFilter::ConversionStatus Utils::loadAndParse(KoXmlDocument& doc, const KZip* zip,
                                               QString& errorMessage, const QString& fileName)
{
    KoFilter::ConversionStatus status;
    std::auto_ptr<QIODevice> device( openDeviceForFile(zip, errorMessage, fileName, status) );
    if (!device.get())
        return status;
    return loadAndParse( device.get(), doc, errorMessage, fileName );
}

KoFilter::ConversionStatus Utils::loadAndParseDocument(MsooXmlReader* reader,
                                                       const KZip* zip,
                                                       KoOdfWriters *writers,
                                                       QString& errorMessage,
                                                       const QString& fileName,
                                                       MsooXmlReaderContext* context)
{
    Q_UNUSED(writers)
    KoFilter::ConversionStatus status;
    std::auto_ptr<QIODevice> device( openDeviceForFile(zip, errorMessage, fileName, status) );
    if (!device.get())
        return status;
    reader->setDevice(device.get());
    reader->setFileName(fileName); // for error reporting
    status = reader->read(context);
    if (status != KoFilter::OK) {
        errorMessage = reader->errorString();
        return status;
    }
    errorMessage.clear();
    kDebug() << "File" << fileName << "loaded and parsed.";
    return KoFilter::OK;
}

QIODevice* Utils::openDeviceForFile(const KZip* zip, QString& errorMessage, const QString& fileName,
                                    KoFilter::ConversionStatus& status)
{
    kDebug() << "Trying to open" << fileName;
    const KArchiveEntry* entry = zip->directory()->entry( fileName );
    errorMessage.clear();
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
KoFilter::ConversionStatus Utils::copyFile(const KZip* zip, QString& errorMessage,
                                           const QString& sourceName,
                                           KoStore *outputStore,
                                           const QString& destinationName)
{
    KoFilter::ConversionStatus status;
    std::auto_ptr<QIODevice> inputDevice( Utils::openDeviceForFile(zip, errorMessage, sourceName, status) );
    if (!inputDevice.get()) {
        return status;
    }
    kDebug() << "mode:" << outputStore->mode();
    if (!outputStore->open(destinationName)) {
        errorMessage = i18n("Could not open entry \"%1\" for writing.", destinationName);
        return KoFilter::CreationError;
    }
    status = KoFilter::OK;
//    QIODevice* outputDevice = outputStore->device();
    char block[BLOCK_SIZE];
    while (true) {
        const qint64 in = inputDevice->read(block, BLOCK_SIZE);
//        kDebug() << "in:" << in;
        if (in == 0 || in == -1)
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

KoFilter::ConversionStatus Utils::loadThumbnail( QImage& thumbnail, KZip* zip )
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

KoFilter::ConversionStatus Utils::loadContentTypes(
    const KoXmlDocument& contentTypesXML, QMultiHash<QByteArray, QByteArray>& contentTypes)
{
    KoXmlElement typesEl( contentTypesXML.documentElement() );
    if (!checkTag(typesEl, "Types", "documentElement")) {
        return KoFilter::WrongFormat;
    }
    if (!checkNsUri(typesEl, Schemas::contentTypes)) {
        return KoFilter::WrongFormat;
    }
    KoXmlElement e;
    forEachElement( e, typesEl ) {
        const QString tagName( e.tagName() );
        if (!checkNsUri(e, Schemas::contentTypes)) {
            return KoFilter::WrongFormat;
        }

        if (tagName == "Override") {
            //ContentType -> PartName mapping
            const QByteArray atrPartName( e.attribute("PartName").toLatin1() );
            const QByteArray atrContentType( e.attribute("ContentType").toLatin1() );
            if (atrPartName.isEmpty() || atrContentType.isEmpty()) {
                kWarning() << "Invalid data for" << tagName
                    << "element: PartName=" << atrPartName << "ContentType=" << atrContentType;
                return KoFilter::WrongFormat;
            }
//kDebug() << atrContentType << "->" << atrPartName;
            contentTypes.insert(atrContentType, atrPartName);
        }
        else if (tagName == "Default") {
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
        INSERT_HC("white", 0xffffff);
#undef INSERT_HC
    }
};

QBrush Utils::ST_HighlightColor_to_QColor(const QString& colorName)
{
    K_GLOBAL_STATIC(ST_HighlightColorMapping, s_ST_HighlightColor_to_QColor)
    const QColor c( s_ST_HighlightColor_to_QColor->value(colorName) );
    if (c.isValid())
        return QBrush(c);
    return QBrush(); // for "none" or anything unsupported
}

double Utils::ST_Percentage_to_double(const QString& val, bool& ok)
{
    if (!val.endsWith('%')) {
        ok = false;
        return 0.0;
    }
    QString result(val);
    result.truncate(1);
    return result.toDouble(&ok);
}

double Utils::ST_Percentage_withMsooxmlFix_to_double(const QString& val, bool& ok)
{
    const double result = ST_Percentage_to_double(val, ok);
    if (ok)
        return result;
    // MSOOXML fix: the format is int({ST_Percentage}*1000)
    const int resultInt = val.toInt(&ok);
    if (!ok)
        return 0.0;
    return double(resultInt) / 1000.0;
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
        }
        else {
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

QByteArray Utils::ST_PlaceholderType_to_ODF(const QByteArray& ecmaType)
{
    K_GLOBAL_STATIC(ST_PlaceholderType_to_ODFMapping, s_ST_PlaceholderType_to_ODF)
    QHash<QByteArray, QByteArray>::ConstIterator it( s_ST_PlaceholderType_to_ODF->constFind(ecmaType) );
    if (it == s_ST_PlaceholderType_to_ODF->constEnd())
        return "outline";
    return it.value();
}

QString Utils::EMU_to_ODF_CM(const QString& emuValue)
{
    if (emuValue.isEmpty())
        return QLatin1String("0cm");
    bool ok;
    const int emu = emuValue.toInt(&ok);
    if (!ok)
        return QString();
    if (emu == 0)
        return QLatin1String("0cm");
    return EMU_TO_CM_STRING(emu);
//    return QString::number(, 'g', 3) + QLatin1String("cm");
}

//! Mapping for handling u element, used in setupUnderLineStyle()
struct UnderlineStyle {
    UnderlineStyle(
        KoCharacterStyle::LineStyle style_,
        KoCharacterStyle::LineType type_,
        KoCharacterStyle::LineWeight weight_,
        KoCharacterStyle::LineMode mode_ = KoCharacterStyle::ContinuousLineMode)
    : style(style_), type(type_), weight(weight_), mode(mode_)
    {
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
    UnderlineStylesHash()
    {
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

    ~UnderlineStylesHash()
    {
        qDeleteAll(*this);
    }

    void setup(const QString& msooxmlName,
               KoCharacterStyle* textStyleProperties)
    {
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
