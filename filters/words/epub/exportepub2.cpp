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
    kDebug(30517) << " ************Start reading styles";
    QHash<QString, StyleInfo*> styles;
    status = parseStyles(odfStore, styles);
    if (status != KoFilter::OK) {
        delete odfStore;
        return status;
    }

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
    // Extract images

    // Check for the pictures directory in the odf store.
    //
    // FIXME: Don't  hardcode the "Pictures"  directory, it is  only a
    //        convention, not a standard.
    if (odfStore->enterDirectory("Pictures")) {
        // We have pictures so leave directory and extract images.
        odfStore->leaveDirectory();
        status = extractImages(odfStore, &epub);
        if (status != KoFilter::OK) {
            delete odfStore;
            return status;
        }
    }
    kDebug(30517) << "parse images finished";

    // ----------------------------------------------------------------
    // Create content files.

    // Create html contents.
    // Note that this also sets the inUse flag for the styles thare are used.
    status = convertContent(odfStore, m_meta, &epub, styles);
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
    kDebug(30517) << "create CSS finished";
    epub.addContentFile("stylesheet", "OEBPS/styleSheet.css", "text/css", cssContent);

    // ----------------------------------------------------------------
    // Write the finished epub file to disk

    epub.writeEpub(m_chain->outputFile(), to, m_meta);

    delete odfStore;
    qDeleteAll(styles);

    return KoFilter::OK;
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
        kDebug() << "Error occured while parsing meta.xml "
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
    QByteArray imageData;
    foreach (const QString &src,  metaImagesData.keys()) {
        odfStore->extractFile(src, imageData);
        QString imageSrc = "OEBPS/" + src.section('/', 1);
        epubFile->addContentFile(src.section('/', 1), imageSrc,
                                 metaImagesData.value(src).toUtf8(), imageData);
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
        kDebug() << "Error occured while parsing meta.xml "
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
        childNode = childNode.nextSibling();
        nodeElement = childNode.toElement();
    }

    odfStore->close();
    return KoFilter::OK;
}
