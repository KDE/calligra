/* This file is part of the Calligra project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>
   Copyright (C) 2008 Benjamin Cail <cricketc@gmail.com>
   Copyright (C) 2010, 2011 Matus Uzak <matus.uzak@ixonos.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "mswordodfimport.h"

#include <QFile>
#include <QString>
#include <QBuffer>
#include <QByteArray>

#include <kpluginfactory.h>

#include <KoFilterChain.h>
#include <KoOdfWriteStore.h>

#include "document.h"
#include "exceptions.h"
#include "msdoc.h"
#include "MsDocDebug.h"

#include "generated/simpleParser.h"
#include "pole.h"

//function prototypes of local functions
bool readStream(POLE::Storage& storage, const char* streampath, QBuffer& buffer);

K_PLUGIN_FACTORY_WITH_JSON(MSWordOdfImportFactory, "calligra_filter_doc2odt.json",
                           registerPlugin<MSWordOdfImport>();)


MSWordOdfImport::MSWordOdfImport(QObject *parent, const QVariantList&)
        : KoFilter(parent)
{
}

MSWordOdfImport::~MSWordOdfImport()
{
}

KoFilter::ConversionStatus MSWordOdfImport::convert(const QByteArray &from, const QByteArray &to)
{
    // check for proper conversion
    if ((to != "application/vnd.oasis.opendocument.text") ||
        (from != "application/msword")) {
        return KoFilter::NotImplemented;
    }

    debugMsDoc << "######################## MSWordOdfImport::convert ########################";

    QString inputFile = m_chain->inputFile();
    QString outputFile = m_chain->outputFile();

    /*
     * ************************************************
     *  POLE storage, POLE and LEInput streams
     * ************************************************
     */
    //TODO: POLE:Stream or LEInputStream ?

    POLE::Storage storage(inputFile.toLocal8Bit());
    if (!storage.open()) {
        debugMsDoc << "Cannot open " << inputFile;
        return KoFilter::InvalidFormat;
    }
    //WordDocument Stream
    QBuffer buff1;
    if (!readStream(storage, "/WordDocument", buff1)) {
        return KoFilter::InvalidFormat;
    }
    LEInputStream wdstm(&buff1);

    MSO::FibBase fibBase;
    LEInputStream::Mark m = wdstm.setMark();
    try {
        parseFibBase(wdstm, fibBase);
    } catch (const IOException &e) {
        errorMsDoc << e.msg;
        return KoFilter::InvalidFormat;
    } catch (...) {
        warnMsDoc << "Warning: Caught an unknown exception!";
        return KoFilter::InvalidFormat;
    }
    wdstm.rewind(m);

    //document is encrypted or obfuscated
    if (fibBase.fEncrypted) {
        return KoFilter::PasswordProtected;
    }

    //1Table Stream or 0Table Stream
    const char* tblstm_name = fibBase.fWhichTblStm ? "1Table" : "0Table";
    POLE::Stream tblstm_pole(&storage, tblstm_name);
    if (tblstm_pole.fail()) {
        if (fibBase.nFib >= Word8nFib) {
            debugMsDoc << "Either the 1Table stream or the 0Table stream MUST be present in the file!";
            return KoFilter::InvalidFormat;
        }
    }

    //Data Stream
    LEInputStream* datastm = 0;
    QBuffer buff3;
    if (!readStream(storage, "/Data", buff3)) {
        debugMsDoc << "Failed to open /Data stream, no big deal (OPTIONAL).";
    } else {
        datastm = new LEInputStream(&buff3);
    }

    //Summary Information Stream
    LEInputStream* sistm = 0;
    QBuffer buff4;
    if (!readStream(storage, "/SummaryInformation", buff4)) {
        debugMsDoc << "Failed to open /SummaryInformation stream, no big deal (OPTIONAL).";
    } else {
        sistm = new LEInputStream(&buff4);
    }

    /*
     * ************************************************
     *  Processing file
     * ************************************************
     */
    struct Finalizer {
    public:
        Finalizer(LEInputStream *ds, LEInputStream *sis) : m_store(0), m_genStyles(0), m_document(0),
                                      m_contentWriter(0), m_bodyWriter(0),
                                      m_datastm(ds), m_sistm(sis) { }
        ~Finalizer() {
            delete m_store; delete m_genStyles; delete m_document; delete m_contentWriter; delete m_bodyWriter;
            if (m_datastm) {
                delete m_datastm;
            }
            if (m_sistm) {
              delete m_sistm;
            }
        }

        KoStore *m_store;
        KoGenStyles *m_genStyles;
        Document *m_document;
        KoXmlWriter *m_contentWriter;
        KoXmlWriter *m_bodyWriter;
        LEInputStream* m_datastm;
        LEInputStream* m_sistm;
    };
    Finalizer finalizer(datastm, sistm);

    // Create output files
    KoStore *storeout;
    storeout = KoStore::createStore(outputFile, KoStore::Write,
                                    "application/vnd.oasis.opendocument.text", KoStore::Zip);
    if (!storeout) {
        warnMsDoc << "Unable to open output file!";
        return KoFilter::FileNotFound;
    }
    finalizer.m_store = storeout;
    debugMsDoc << "created storeout.";
    KoOdfWriteStore oasisStore(storeout);

    debugMsDoc << "created oasisStore.";

    //create KoGenStyles for writing styles while we're parsing
    KoGenStyles* mainStyles = new KoGenStyles();
    finalizer.m_genStyles = mainStyles; // will delete this as it goes out of scope.

    //create a writer for meta.xml
    QBuffer buf;
    buf.open(QIODevice::WriteOnly);
    KoXmlWriter metaWriter(&buf);

    //create a writer for manifest.xml
    QBuffer manifestBuf;
    manifestBuf.open(QIODevice::WriteOnly);
    KoXmlWriter manifestWriter(&manifestBuf);

    //open contentWriter & bodyWriter *temp* writers
    //so we can write picture files while we parse
    QBuffer contentBuf;
    QBuffer bodyBuf;
    KoXmlWriter *contentWriter = new KoXmlWriter(&contentBuf);
    finalizer.m_contentWriter = contentWriter;
    KoXmlWriter *bodyWriter = new KoXmlWriter(&bodyBuf);
    finalizer.m_bodyWriter = bodyWriter;
    if (!bodyWriter || !contentWriter) {
        //not sure if this is the right error to return
        return KoFilter::CreationError;
    }

    debugMsDoc << "created temp contentWriter and bodyWriter.";

    //open tags in bodyWriter
    bodyWriter->startElement("office:body");
    bodyWriter->startElement("office:text");

    //create our document object, writing to the temporary buffers
    Document *document = 0;

    try {
        document = new Document(QFile::encodeName(inputFile).data(), this,
                                bodyWriter, &metaWriter, &manifestWriter,
                                storeout, mainStyles,
                                wdstm, tblstm_pole, datastm, sistm);
    } catch (const InvalidFormatException &_e) {
        debugMsDoc << _e.msg;
        return KoFilter::InvalidFormat;
    } catch (...) {
        warnMsDoc << "Warning: Caught an unknown exception!";
        return KoFilter::StupidError;
    }

    finalizer.m_document = document;

    //check that we can parse the document?
    if (!document->hasParser()) {
        return KoFilter::WrongFormat;
    }
    //actual parsing & action
    try {
        quint8 ret = document->parse();
        switch (ret) {
        case 1:
            return KoFilter::CreationError;
        case 2:
            return KoFilter::StupidError;
        }
    } catch (const InvalidFormatException &_e) {
        debugMsDoc << _e.msg;
        return KoFilter::InvalidFormat;
    } catch (...) {
        warnMsDoc << "Warning: Caught an unknown exception!";
        return KoFilter::StupidError;
    }

    document->processSubDocQueue(); //process the queues we've created?
    document->finishDocument(); //process footnotes, pictures, ...

    if (!document->bodyFound()) {
        return KoFilter::WrongFormat;
    }
    debugMsDoc << "finished parsing.";

    //save the office:automatic-styles & and fonts in content.xml
    mainStyles->saveOdfStyles(KoGenStyles::FontFaceDecls, contentWriter);
    mainStyles->saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, contentWriter);

    //close tags in bodyWriter
    bodyWriter->endElement();//office:text
    bodyWriter->endElement();//office:body

    //now create real content/body writers & dump the information there
    KoXmlWriter* realContentWriter = oasisStore.contentWriter();
    if(!realContentWriter) {
        warnMsDoc << "Error writing content.";
        return KoFilter::CreationError;
    }
    realContentWriter->addCompleteElement(&contentBuf);

    KoXmlWriter *realBodyWriter = oasisStore.bodyWriter();
    realBodyWriter->addCompleteElement(&bodyBuf);

    //now close content & body writers
    if (!oasisStore.closeContentWriter()) {
        warnMsDoc << "Error closing content.";
        return KoFilter::CreationError;
    }

    debugMsDoc << "closed content & body writers.";

    //create the settings file
    storeout->open("settings.xml");
    KoStoreDevice settingsDev(storeout);
    KoXmlWriter *settingsWriter = oasisStore.createOasisXmlWriter(&settingsDev, "office:document-settings");
    settingsWriter->startElement("office:settings");
    settingsWriter->startElement("config:config-item-set");
    settingsWriter->addAttribute("config:name", "ooo:configuration-settings");

    settingsWriter->startElement("config:config-item");
    settingsWriter->addAttribute("config:name", "UseFormerLineSpacing");
    settingsWriter->addAttribute("config:type", "boolean");
    settingsWriter->addTextSpan("false");
    settingsWriter->endElement();

    settingsWriter->startElement("config:config-item");
    settingsWriter->addAttribute("config:name", "TabsRelativeToIndent");
    settingsWriter->addAttribute("config:type", "boolean");
    settingsWriter->addTextSpan("false");
    settingsWriter->endElement();

    // This config-item is used in KoTextLayoutArea::handleBordersAndSpacing
    // during layouting.  The defined 'Above paragraph' and 'Below paragraph'
    // paragraph spacing (which is written in the ODF as fo:margin-top for the
    // KoParagraphStyle) are not applied to the first and the last paragraph if
    // this value is true.
    settingsWriter->startElement("config:config-item");
    settingsWriter->addAttribute("config:name", "AddParaTableSpacingAtStart");
    settingsWriter->addAttribute("config:type", "boolean");
    settingsWriter->addTextSpan("true");
    settingsWriter->endElement();

    // OOo requires this config item to display files produced by this filter
    // correctly.  If true, then the fo:text-indent attribute will be ignored.
    settingsWriter->startElement("config:config-item");
    settingsWriter->addAttribute("config:name", "IgnoreFirstLineIndentInNumbering");
    settingsWriter->addAttribute("config:type", "boolean");
    settingsWriter->addTextSpan("false");
    settingsWriter->endElement();

    settingsWriter->endElement(); // config-item-set

    settingsWriter->endElement(); // settings
    settingsWriter->endElement(); // document-settings
    settingsWriter->endDocument();
    delete settingsWriter;
    storeout->close();

    debugMsDoc << "created settings.xml";

    //create the manifest file
    KoXmlWriter *realManifestWriter = oasisStore.manifestWriter("application/vnd.oasis.opendocument.text");
    //create the styles.xml file
    mainStyles->saveOdfStylesDotXml(storeout, realManifestWriter);
    realManifestWriter->addManifestEntry("content.xml", "text/xml");
    realManifestWriter->addManifestEntry("settings.xml", "text/xml");
    realManifestWriter->addCompleteElement(&manifestBuf);

    debugMsDoc << "created manifest and styles.xml";

    //create meta.xml
    if (!storeout->open("meta.xml")) {
        return KoFilter::CreationError;
    }

    KoStoreDevice metaDev(storeout);
    KoXmlWriter *meta = KoOdfWriteStore::createOasisXmlWriter(&metaDev, "office:document-meta");
    meta->startElement("office:meta");
    meta->addCompleteElement(&buf);
    meta->endElement(); //office:meta
    meta->endElement(); //office:document-meta
    meta->endDocument();
    delete meta;
    if (!storeout->close()) {
        return KoFilter::CreationError;
    }

    realManifestWriter->addManifestEntry("meta.xml", "text/xml");
    oasisStore.closeManifestWriter();

    debugMsDoc << "######################## MSWordOdfImport::convert done ####################";
    return KoFilter::OK;
}

void
MSWordOdfImport::setProgress(const int percent)
{
    emit sigProgress(percent);
}

/*
 * Read the stream content into buffer.
 * @param storage; POLE storage
 * @param streampath; stream path into the POLE storage
 * @param buffer; buffer provided by the user
 */
bool
readStream(POLE::Storage& storage, const char* streampath, QBuffer& buffer)
{
    std::string path(streampath);
    POLE::Stream stream(&storage, path);
    if (stream.fail()) {
        errorMsDoc << "Unable to construct " << streampath << "stream";
        return false;
    }

    QByteArray array;
    array.resize(stream.size());
    unsigned long r = stream.read((unsigned char*)array.data(), stream.size());
    if (r != stream.size()) {
        errorMsDoc << "Error while reading from " << streampath << "stream";
        return false;
    }
    buffer.setData(array);
    buffer.open(QIODevice::ReadOnly);
    return true;
}

#include <mswordodfimport.moc>
