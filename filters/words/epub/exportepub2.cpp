/* This file is part of the KDE project

   Copyright (C) 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
   Copyright (C) 2012 Inge Wallin            <inge@lysator.liu.se>

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


// Own
#include "exportepub2.h"

// Qt
#include <QSvgGenerator>
#include <QBuffer>
#include <QPainter>
#include <QMimeDatabase>

// KF5
#include <kpluginfactory.h>

// Calligra
#include <KoFilterChain.h>
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <KoXmlNS.h>

// This plugin
#include "OdfParser.h"
#include "OdtHtmlConverter.h"
#include "EpubFile.h"
#include "EpubExportDebug.h"

#include "WmfPainterBackend.h"

#include "EmfParser.h"
#include "EmfOutputPainterStrategy.h"
#include "SvmParser.h"
#include "SvmPainterBackend.h"


K_PLUGIN_FACTORY_WITH_JSON(ExportEpub2Factory, "calligra_filter_odt2epub2.json",
			   registerPlugin<ExportEpub2>();)

// Needed to instantiate the plugin factory.
#include "exportepub2.moc"

ExportEpub2::ExportEpub2(QObject *parent, const QVariantList&)
    : KoFilter(parent)
{
}

ExportEpub2::~ExportEpub2()
{
}


KoFilter::ConversionStatus ExportEpub2::convert(const QByteArray &from, const QByteArray &to)
{
    // Check mimetypes
    if (from != "application/vnd.oasis.opendocument.text" || to != "application/epub+zip") {
        return KoFilter::NotImplemented;
    }

    // Open the infile and return an error if it fails.
    KoStore *odfStore = KoStore::createStore(m_chain->inputFile(), KoStore::Read,
                                             "", KoStore::Auto);
    if (!odfStore->open("mimetype")) {
        errorEpub << "Unable to open input file!" << endl;
        delete odfStore;
        return KoFilter::FileNotFound;
    }
    odfStore->close();

    // Start the conversion
    OdtHtmlConverter converter;
    OdfParser        odfParser;
    EpubFile         epub;
    KoFilter::ConversionStatus  status;

    // ----------------------------------------------------------------
    // Parse input files

    // Parse meta.xml into m_metadata
    status = odfParser.parseMetadata(odfStore, m_metadata);
    if (status != KoFilter::OK) {
        delete odfStore;
        return status;
    }

    // Parse manifest
    status = odfParser.parseManifest(odfStore, m_manifest);
    if (status != KoFilter::OK) {
        delete odfStore;
        return status;
    }

    // ----------------------------------------------------------------
    // Create content files.

    // Create html contents.
    // m_imagesSrcList is an output parameter from the conversion.
    OdtHtmlConverter::ConversionOptions options = {
        true,                   // do put styles in css file
        true,                    // do break into chapters
        false                    // It is not mobi
    };
    status = converter.convertContent(odfStore, m_metadata, &m_manifest, &options, &epub,
                                      m_imagesSrcList, m_mediaFilesList);
    if (status != KoFilter::OK) {
        delete odfStore;
        return status;
    }

    // Extract images
    status = extractImages(odfStore, &epub);
    if (status != KoFilter::OK) {
        delete odfStore;
        return status;
    }
    // Extract media files
    status = extractMediaFiles(&epub);
    if (status != KoFilter::OK) {
        delete odfStore;
        return status;
    }

    // Check for cover image
    status = extractCoverImage(odfStore, &epub);
    if (status != KoFilter::OK) {
        delete odfStore;
        return status;
    }

    // ----------------------------------------------------------------
    // Write the finished epub file to disk

    epub.writeEpub(m_chain->outputFile(), to, m_metadata);

    delete odfStore;

    return KoFilter::OK;
}


KoFilter::ConversionStatus ExportEpub2::extractImages(KoStore *odfStore, EpubFile *epubFile)
{
    // Extract images and add them to epubFile one by one
    QByteArray imgContent;
    int imgId = 1;
    foreach (const QString &imgSrc, m_imagesSrcList.keys()) {
        debugEpub << imgSrc;
        if (!odfStore->hasFile(imgSrc)) {
            warnEpub << "Can not to extract this image, image "<< imgSrc<< "is an external image";
            // Ignore the external image.
            continue;
        }
        if (!odfStore->extractFile(imgSrc, imgContent)) {
            debugEpub << "Can not to extract file";
            return KoFilter::FileNotFound;
        }

        VectorType type = vectorType(imgContent);
        QSizeF qSize = m_imagesSrcList.value(imgSrc);
        switch (type) {

        case ExportEpub2::VectorTypeSvm:
            {
                debugEpub << "Svm file";
                QSize size(qSize.width(), qSize.height());
                QByteArray output;
                if (!convertSvm(imgContent, output, size)) {
                    debugEpub << "Svm Parse error";
                    return KoFilter::ParsingError;
                }

                epubFile->addContentFile(("image" + QString::number(imgId++)),
                                         (epubFile->pathPrefix() + imgSrc.section('/', -1)),
                                         "image/svg+xml", output);
                break;
            }
        case ExportEpub2::VectorTypeEmf:
            {
                debugEpub << "EMF file";
                QSize size(qSize.width(), qSize.height());
                QByteArray output;
                if (!convertEmf(imgContent, output, size)) {
                    debugEpub << "EMF Parse error";
                    return KoFilter::ParsingError;
                }

                epubFile->addContentFile(("image" + QString::number(imgId++)),
                                         (epubFile->pathPrefix() + imgSrc.section('/', -1)),
                                         "image/svg+xml", output);
                break;
            }
        case ExportEpub2::VectorTypeWmf:
            {
                debugEpub << "WMF file";
                 QByteArray output;
                if (!convertWmf(imgContent, output, qSize)) {
                    debugEpub << "WMF Parse error";
                    return KoFilter::ParsingError;
                }

                epubFile->addContentFile(("image" + QString::number(imgId++)),
                                         (epubFile->pathPrefix() + imgSrc.section('/', -1)),
                                         "image/svg+xml", output);
                break;
            }

            // If it's not one of the types we can convert, let's just
            // assume that the image can be used as it is. The user
            // will find out soon anyway when s/he tries to look at
            // the image.
        case ExportEpub2::VectorTypeOther:
            {
                debugEpub << "Other file";
                epubFile->addContentFile(("image" + QString::number(imgId++)),
                                         (epubFile->pathPrefix() + imgSrc.section('/', -1)),
                                         m_manifest.value(imgSrc).toUtf8(), imgContent);
                break;
            }

        default:
            debugEpub << "";
        }
    }
    return KoFilter::OK;
}


bool ExportEpub2::convertSvm(QByteArray &input, QByteArray &output, QSize size)
{

    QBuffer *outBuf = new QBuffer(&output);
    QSvgGenerator generator;
    generator.setOutputDevice(outBuf);
    generator.setSize(QSize(200, 200));
    generator.setTitle("Svg image");
    generator.setDescription("This is an svg image that is converted from svm by Calligra");

    Libsvm::SvmParser  svmParser;

    QPainter painter;

    if (!painter.begin(&generator)) {
        debugEpub << "Can not open the painter";
        return false;
    }

    painter.scale(50,50);
    Libsvm::SvmPainterBackend svmPainterBackend(&painter, size);
    svmParser.setBackend(&svmPainterBackend);
    if (!svmParser.parse(input)) {
        debugEpub << "Can not Parse the Svm file";
        return false;
    }
    painter.end();

    return true;
}

bool ExportEpub2::convertEmf(QByteArray &input, QByteArray &output, QSize size)
{
    QBuffer *outBuf = new QBuffer(&output);
    QSvgGenerator generator;
    generator.setOutputDevice(outBuf);
    generator.setSize(QSize(200, 200));
    generator.setTitle("Svg image");
    generator.setDescription("This is an svg image that is converted from EMF by Calligra");

    Libemf::Parser  emfParser;

    QPainter painter;

    if (!painter.begin(&generator)) {
        debugEpub << "Can not open the painter";
        return false;
    }

    painter.scale(50,50);
    Libemf::OutputPainterStrategy  emfPaintOutput(painter, size, true );
    emfParser.setOutput( &emfPaintOutput );
    if (!emfParser.load(input)) {
        debugEpub << "Can not Parse the EMF file";
        return false;
    }
    painter.end();

    return true;
}

bool ExportEpub2::convertWmf(QByteArray &input, QByteArray &output, QSizeF size)
{
    QBuffer *outBuf = new QBuffer(&output);
    QSvgGenerator generator;
    generator.setOutputDevice(outBuf);
    generator.setSize(QSize(200, 200));
    generator.setTitle("Svg image");
    generator.setDescription("This is an svg image that is converted from WMF by Calligra");

    QPainter painter;

    if (!painter.begin(&generator)) {
        debugEpub << "Can not open the painter";
        return false;
    }

    painter.scale(50,50);
    Libwmf::WmfPainterBackend  wmfPainter(&painter, size);
    if (!wmfPainter.load(input)) {
        debugEpub << "Can not Parse the WMF file";
        return false;
    }

    // Actually paint the WMF.
    painter.save();
    wmfPainter.play();
    painter.restore();
    painter.end();

    return true;
}

// ----------------------------------------------------------------
// These functions were taken from the vector shape.

ExportEpub2::VectorType  ExportEpub2::vectorType(QByteArray &content)
{
    if (isSvm(content))
        return ExportEpub2::VectorTypeSvm;
    if (isEmf(content))
        return ExportEpub2::VectorTypeEmf;
    if (isWmf(content))
        return ExportEpub2::VectorTypeWmf;

    return ExportEpub2::VectorTypeOther;
}

bool ExportEpub2::isSvm(QByteArray &content)
{
    if (content.startsWith("VCLMTF"))
        return true;
    return false;
}

bool ExportEpub2::isEmf(QByteArray &content)
{
    const char *data = content.constData();
    const int   size = content.count();

    // This is how the 'file' command identifies an EMF.
    // 1. Check type
    int offset = 0;
    int result = (int) data[offset];
    result |= (int) data[offset+1] << 8;
    result |= (int) data[offset+2] << 16;
    result |= (int) data[offset+3] << 24;

    qint32 mark = result;
    if (mark != 0x00000001) {
        return false;
    }

    // 2. An EMF has the string " EMF" at the start + offset 40.
    if (size > 44 && data[40] == ' ' && data[41] == 'E' && data[42] == 'M' && data[43] == 'F'){
        return true;
    }

    return false;
}

bool ExportEpub2::isWmf(QByteArray &content)
{
    const char *data = content.constData();
    const int   size = content.count();

    if (size < 10)
        return false;

    // This is how the 'file' command identifies a WMF.
    if (data[0] == '\327' && data[1] == '\315' && data[2] == '\306' && data[3] == '\232'){
        return true;
    }

    if (data[0] == '\002' && data[1] == '\000' && data[2] == '\011' && data[3] == '\000'){
        return true;
    }

    if (data[0] == '\001' && data[1] == '\000' && data[2] == '\011' && data[3] == '\000'){
        return true;
    }

    return false;
}

KoFilter::ConversionStatus ExportEpub2::extractMediaFiles(EpubFile *epubFile)
{
    QByteArray mediaContent;
    foreach (const QString &mediaId, m_mediaFilesList.keys()) {
        QString mediaSrc = m_mediaFilesList.value(mediaId);
        // Remove scheme (file://) from the path
        QUrl mediaPath(mediaSrc);
        mediaSrc = mediaPath.path();

        QFile file (mediaSrc);
        if (!file.open(QIODevice::ReadOnly)) {
            debugEpub << "Unable to open" << mediaSrc;
            return KoFilter::FileNotFound;
        }
        mediaContent = file.readAll();

        const QString mimetype = QMimeDatabase().mimeTypeForFileNameAndData(mediaSrc.section("/", -1), mediaContent).name();
        epubFile->addContentFile(mediaId.section("#", -1), epubFile->pathPrefix() + mediaSrc.section("/", -1),
                                 mimetype.toUtf8(), mediaContent);
    }
    return KoFilter::OK;
}

KoFilter::ConversionStatus ExportEpub2::extractCoverImage(KoStore *odfStore, EpubFile *epubFile)
{
    // Find Cover from manifest
    QString coverPath;
    foreach (const QString &path, m_manifest.keys()) {
        if (path.contains("coverImage.")) {
            coverPath = path;
            break;
        }
    }

    // There is no cover image.
    if (coverPath.isEmpty()) {
        return KoFilter::OK;
    }

    // Extact cover data.
    QByteArray coverData;
    if (!odfStore->extractFile(coverPath, coverData)) {
        debugEpub << "Can not to extract file" + coverPath;
        return KoFilter::FileNotFound;
    }

    // Add it to epub file content.
    QByteArray mime = m_manifest.value(coverPath).toUtf8();
    epubFile->addContentFile(QString("cover-image"),
                             QString((epubFile->pathPrefix() + coverPath.section('/', -1))),
                             mime, coverData);
    // Write the html for cover.
    writeCoverImage(epubFile, coverPath.section('/', -1));

    return KoFilter::OK;
}

void ExportEpub2::writeCoverImage(EpubFile *epubFile, const QString coverPath)
{
    QByteArray coverHtmlContent;
    QBuffer buff(&coverHtmlContent);
    KoXmlWriter writer(&buff);

    writer.startDocument(nullptr, nullptr, nullptr); //xml version, etc...
    writer.startElement("html");
    writer.addAttribute("xmlns", "http://www.w3.org/1999/xhtml");
    writer.addAttribute("xml:lang", "en");

    writer.startElement("head");

    writer.startElement("meta");
    writer.addAttribute("http-equiv", "Content-Type");
    writer.addAttribute("content", "text/html; charset=UTF-8");
    writer.endElement();

    writer.startElement("title");
    writer.addTextNode("Cover");
    writer.endElement();

    writer.startElement("style");
    writer.addAttribute("type", "text/css");
    writer.addAttribute("title", "override_css");

    writer.addTextNode("\n");
    writer.addTextNode("   @page { padding:Opt; margin:Opt } \n");
    writer.addTextNode("   body { text-align:center; padding:Opt; margin:Opt } \n");
    writer.addTextNode("   img { padding:Opt; margin:Opt; max-height: 100% ; max-width: 100% } \n");

    writer.endElement(); //style

    writer.endElement(); // head

    writer.startElement("body");

    writer.startElement("div");
    writer.addAttribute("id", "cover-image");

    writer.startElement("img");
    writer.addAttribute("src", coverPath);
    writer.addAttribute("alt", "Cover Image");
    writer.endElement();

    writer.endElement(); // div

    writer.endElement(); // body

    writer.endElement(); // html

    // Add cover html to content
    epubFile->addContentFile(QString("cover"), QString(epubFile->pathPrefix() + "cover.xhtml")
                            , "application/xhtml+xml", coverHtmlContent, QString("Cover"));
}
