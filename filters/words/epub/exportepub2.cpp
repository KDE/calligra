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


#include "exportepub2.h"

#include <kdebug.h>
#include <KoFilterChain.h>
#include <kpluginfactory.h>
#include <KoOdfWriteStore.h>
#include <KoGenStyles.h>
#include <KoXmlWriter.h>
#include <KoStoreDevice.h>
#include <KoXmlReader.h>
#include <KoXmlNS.h>

#include "htmlconvert.h"
#include "libepub/EpubFile.h"

#include <QSvgGenerator>
#include <QBuffer>
#include <QPainter>

#include "WmfPainterBackend.h"

#include "EmfParser.h"
#include "EmfOutputPainterStrategy.h"
#include "EmfOutputDebugStrategy.h"
#include "SvmParser.h"
#include "SvmPainterBackend.h"

K_PLUGIN_FACTORY(ExportEpub2Factory, registerPlugin<ExportEpub2>();)
K_EXPORT_PLUGIN(ExportEpub2Factory("calligrafilters"))

ExportEpub2::ExportEpub2(QObject *parent, const QVariantList&) :
    KoFilter(parent)
{
}

ExportEpub2::~ExportEpub2()
{
}


KoFilter::ConversionStatus ExportEpub2::convert(const QByteArray &from, const QByteArray &to)
{
    // Check mimetypes
    if (to != "application/epub+zip" || from != "application/vnd.oasis.opendocument.text") {
        return KoFilter::NotImplemented;
    }

    // Open the infile and return an error if it fails.
    KoStore *odfStore = KoStore::createStore(m_chain->inputFile(), KoStore::Read,
                                             "", KoStore::Auto);
    odfStore->disallowNameExpansion();
    if (!odfStore->open("mimetype")) {
        kError(30517) << "Unable to open input file!" << endl;
        delete odfStore;
        return KoFilter::FileNotFound;
    }
    odfStore->close();

    // Start the conversion
    EpubFile  epub;
    KoFilter::ConversionStatus  status;

    // ----------------------------------------------------------------
    // Parse input files

    // Parse meta.xml
    status = parseMetadata(odfStore);
    if (status != KoFilter::OK) {
        delete odfStore;
        return status;
    }

    // Parse styles
    QHash<QString, StyleInfo*> styles;
    status = parseStyles(odfStore, styles);
    if (status != KoFilter::OK) {
        delete odfStore;
        return status;
    }
    // propagate some inherited stuff.
    fixStyleTree(styles);

#if 0
    kDebug(30517) << "======== >> Styles";
    foreach(const QString &name, styles.keys()) {
        kDebug(30517) << name << styles.value(name)->parent
                      << styles.value(name)->hasBreakBefore
                      << styles.value(name)->attributes;
    }
    kDebug(30517) << "======== << Styles";
#endif

    // ----------------------------------------------------------------

    // Create content files.

    // Create html contents.
    // Note that this also sets the inUse flag for the styles thare are used.
    status = convertContent(odfStore, m_meta, &epub, styles, m_imagesSrcList);
    if (status != KoFilter::OK) {
        delete odfStore;
        return status;
    }

    // Extract images

    // Check for the pictures directory in the odf store.
        status = extractImages(odfStore, &epub);
        if (status != KoFilter::OK) {
            delete odfStore;
            return status;
        }

    // Create CSS contents
    QByteArray  cssContent;
    status = createCSS(styles, cssContent);
    if (status != KoFilter::OK) {
        delete odfStore;
        return status;
    }
    epub.addContentFile("stylesheet", "OEBPS/styles.css", "text/css", cssContent);

    // ----------------------------------------------------------------
    // Write the finished epub file to disk

    epub.writeEpub(m_chain->outputFile(), to, m_meta);

    delete odfStore;
    qDeleteAll(styles);

    return KoFilter::OK;
}


void ExportEpub2::fixStyleTree(QHash<QString, StyleInfo*> &styles)
{
    // For all styles:
    //    Propagate the hasBreakBefore bool upwards in the inheritance tree.
    foreach (const QString &styleName, styles.keys()) {
        QVector<StyleInfo *> styleStack(styles.size());

        // Create a stack of styles that we have to check.
        //
        // After this, styleStack will contain a list of styles to
        // check with the deepest one last in the list.
        StyleInfo *style = styles[styleName];
        int index = 0;
        while (style) {
            styleStack[index++] = style;

            // Quit when we are at the bottom or found a break-before.
            if (style->hasBreakBefore || style->parent.isEmpty()) {
                break;
            }

            style = styles[style->parent];
        }

        // If the bottom most has a break, then all the ones in the list should inherit it.
        if (styleStack[index - 1]->hasBreakBefore) {
            for (int i = 0; i < index - 1; ++i) {
                styleStack[i]->hasBreakBefore = true;
            }
        }
    }

}


KoFilter::ConversionStatus ExportEpub2::parseMetadata(KoStore *odfStore)
{
    if (!odfStore->open("meta.xml")) {
        kDebug(30517) << "Cannot open meta.xml";
        return KoFilter::FileNotFound;
    }

    KoXmlDocument doc;
    QString errorMsg;
    int errorLine;
    int errorColumn;
    if (!doc.setContent(odfStore->device(), true, &errorMsg, &errorLine, &errorColumn)) {
        kDebug() << "Error occurred while parsing meta.xml "
                 << errorMsg << " in Line: " << errorLine
                 << " Column: " << errorColumn;
        odfStore->close();
        return KoFilter::ParsingError;
    }

    KoXmlNode childNode = doc.documentElement();
    childNode = KoXml::namedItemNS(childNode, KoXmlNS::office, "meta");
    KoXmlElement element;
    forEachElement (element, childNode) {
        m_meta.insert(element.tagName(), element.text());
    }

    odfStore->close();
    return KoFilter::OK;
}

KoFilter::ConversionStatus ExportEpub2::createCSS(QHash<QString, StyleInfo*> &styles,
                                                  QByteArray &cssContent)
{
    // There is no equivalent to the ODF style inheritance using
    // parent-style-name in CSS. This means that to simulate the same
    // behaviour we have to "flatten" the style tree, i.e. we have to
    // transfer all the attributes from a style's parent into itself.
    flattenStyles(styles);

    QByteArray begin("{\n");
    QByteArray end("}\n");
    foreach (QString styleName, styles.keys()) {
        QByteArray head;
        QByteArray attributeList;

        StyleInfo *styleInfo = styles.value(styleName);
        if (!styleInfo || !styleInfo->inUse)
            continue;

        head = QString("." + styleName).toUtf8();
        cssContent.append(head);
        cssContent.append(begin);

        foreach (const QString &propName, styleInfo->attributes.keys()) {
            attributeList += (propName + ':' + styleInfo->attributes.value(propName)).toUtf8() + ";\n";
        }

        cssContent.append(attributeList);
        cssContent.append(end);
    }

    return KoFilter::OK;
}

void ExportEpub2::flattenStyles(QHash<QString, StyleInfo*> &styles)
{
    QSet<QString> doneStyles;
    foreach (const QString &styleName, styles.keys()) {
        if (!doneStyles.contains(styleName)) {
            flattenStyle(styleName, styles, doneStyles);
        }
    }
}

void ExportEpub2::flattenStyle(const QString &styleName, QHash<QString, StyleInfo*> &styles,
                               QSet<QString> &doneStyles)
{
    StyleInfo *styleInfo = styles.value(styleName);
    if (!styleInfo) {
        return;
    }

    QString parentName = styleInfo->parent;
    if (parentName.isEmpty())
        return;

    flattenStyle(styleInfo->parent, styles, doneStyles);

    // Copy all attributes from the parent that is not alreayd in
    // this style into this style.
    StyleInfo *parentInfo = styles.value(parentName);
    if (!parentInfo)
        return;

    foreach(const QString &paramName, parentInfo->attributes.keys()) {
        if (styleInfo->attributes.value(paramName).isEmpty()) {
            styleInfo->attributes.insert(paramName, parentInfo->attributes.value(paramName));
        }
    }

    doneStyles.insert(styleName);
}


KoFilter::ConversionStatus ExportEpub2::extractImages(KoStore *odfStore, EpubFile *epubFile)
{
    QHash <QString, QString> metaImagesData; // hash <src, mimetype>

    KoFilter::ConversionStatus status;
    status = parseMetaInfImagesData(odfStore, metaImagesData);
    if (status != KoFilter::OK) {
        return status;
    }


    // Extract images and add them to epubFile one bye one
    QByteArray imgContent;
    int imgId = 1;
    foreach (const QString imgSrc, m_imagesSrcList.keys()) {
        kDebug(30517) << imgSrc;
        if (!odfStore->extractFile(imgSrc, imgContent)) {
            kDebug(30517) << "Can not to extract file";
            return KoFilter::FileNotFound;
        }

        VectorType type = vectorType(imgContent);
        QSizeF qSize = m_imagesSrcList.value(imgSrc);
        switch (type) {

        case ExportEpub2::VectorTypeSvm: {

            kDebug(30517) << "Svm file";
            QSize size(qSize.width(), qSize.height());
            QByteArray outPut;
            if (!convertSvm(imgContent,outPut, size)) {
                kDebug(30517) << "Svm Parse error";
                return KoFilter::ParsingError;
            }

            epubFile->addContentFile(("image"+ QString::number(imgId)), ("OEBPS/" + imgSrc.section('/', -1)),
                                     "image/svg+xml", outPut);
            break;
        }
        case ExportEpub2::VectorTypeEmf: {
            kDebug(30517) << "EMF file";
            QSize size(qSize.width(), qSize.height());
            QByteArray outPut;
            if (!convertEmf(imgContent,outPut, size)) {
                kDebug(30517) << "Svm Parse error";
                return KoFilter::ParsingError;
            }

            epubFile->addContentFile(("image"+ QString::number(imgId)), ("OEBPS/" + imgSrc.section('/', -1)),
                                     "image/x-emf", outPut);
            break;
        }
        case ExportEpub2::VectorTypeWmf: {

            kDebug(30517) << "WMF file";
            QByteArray outPut;
            if (!convertWmf(imgContent,outPut, qSize)) {
                kDebug(30517) << "Svm Parse error";
                return KoFilter::ParsingError;
            }

            epubFile->addContentFile(("image"+ QString::number(imgId)), ("OEBPS/" + imgSrc.section('/', -1)),
                                     "image/x-wmf", outPut);
            break;
        }
            // epub support and doesnt need to convert to other format so we add
            // we add it directly and find its mimtype form metaInf
        case ExportEpub2::VectorTypeOther: {
            kDebug(30517) << "Other file";
            epubFile->addContentFile(("image"+ QString::number(imgId)), ("OEBPS/" + imgSrc.section('/', -1)),
                                     metaImagesData.value(imgSrc).toUtf8(), imgContent);
            break;
        }

        default:
            kDebug(30517) << "";
        }
    }
    return KoFilter::OK;
}

KoFilter::ConversionStatus ExportEpub2::parseMetaInfImagesData(KoStore *odfStore,
                                                               QHash<QString, QString> &imagesData)
{
    if (!odfStore->open("META-INF/manifest.xml")) {
        kDebug(30517) << "Cannot to open manifest.xml.";
        return KoFilter::FileNotFound;
    }

    KoXmlDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;
    if (!doc.setContent(odfStore->device(), true, &errorMsg, &errorLine, &errorColumn)) {
        kDebug() << "Error occurred while parsing meta.xml "
                 << errorMsg << " in Line: " << errorLine
                 << " Column: " << errorColumn;
        return KoFilter::ParsingError;
    }

    KoXmlNode childNode = doc.documentElement();
    KoXmlElement nodeElement;
    forEachElement (nodeElement, childNode) {
        QString type = nodeElement.attribute("media-type");
        QString path = nodeElement.attribute("full-path");

        // We need just images
        if (type.contains("image")) {
            imagesData.insert(path, type);
        }
    }

    odfStore->close();
    return KoFilter::OK;
}

bool ExportEpub2::convertSvm(QByteArray &input, QByteArray &output, QSize size)
{

    QBuffer *outBuf = new QBuffer(&output);
    QSvgGenerator generator;
    generator.setOutputDevice(outBuf);
    generator.setSize(QSize(200, 200));
    generator.setTitle("Svg image");
    generator.setDescription("This is a svg image that is converted from svm by Calligra");

    Libsvm::SvmParser  svmParser;

    QPainter painter;

    if (!painter.begin(&generator)) {
        kDebug(30517) << "Can not open the painter";
        return false;
    }

    painter.scale(50,50);
    Libsvm::SvmPainterBackend svmpainterBackend(&painter, size);
    svmParser.setBackend(&svmpainterBackend);
    if (!svmParser.parse(input)) {
        kDebug(30517) << "Can not Parse the Svm file";
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
    generator.setDescription("This is a svg image that is converted from svm by Calligra");

    Libemf::Parser  emfParser;

    QPainter painter;

    if (!painter.begin(&generator)) {
        kDebug(30517) << "Can not open the painter";
        return false;
    }

    painter.scale(50,50);
    Libemf::OutputPainterStrategy  emfPaintOutput(painter, size, true );
    emfParser.setOutput( &emfPaintOutput );
    if (!emfParser.load(input)) {
        kDebug(30517) << "Can not Parse the Svm file";
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
    generator.setDescription("This is a svg image that is converted from svm by Calligra");

    QPainter painter;

    if (!painter.begin(&generator)) {
        kDebug(30517) << "Can not open the painter";
        return false;
    }

    painter.scale(50,50);
    Libwmf::WmfPainterBackend  wmfPainter(&painter, size);
    if (!wmfPainter.load(input)) {
        kDebug(30517) << "Can not Parse the Svm file";
        return false;
    }
    painter.save();
    // Actually paint the WMF.
    wmfPainter.play();
    painter.restore();
    painter.end();

    return true;
}

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
