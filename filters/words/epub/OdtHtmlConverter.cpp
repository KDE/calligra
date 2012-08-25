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
#include "OdtHtmlConverter.h"

// Qt
#include <QStringList>
#include <QBuffer>

// KDE
#include <kdebug.h>

// Calligra
#include <KoStore.h>
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <KoXmlNS.h>

// EPUB filter
#include "libepub/EpubFile.h"




// ================================================================
//                         Style parsing


StyleInfo::StyleInfo()
    : isDefaultStyle(false)
    , hasBreakBefore(false)
    , inUse(false)
{
}


OdtHtmlConverter::OdtHtmlConverter()
    : m_currentChapter(1)
{
}

OdtHtmlConverter::~OdtHtmlConverter()

{
}

KoFilter::ConversionStatus OdtHtmlConverter::convertStyles(KoStore *odfStore,
                                                           QHash<QString, StyleInfo*> &styles)
{
    //kDebug(30517) << "parse content.xml styles";
    if (!odfStore->open("content.xml")) {
        kError(30517) << "Unable to open input file! content.xml" << endl;
        return KoFilter::FileNotFound;
    }

    // Try to set content.xml as a KoXmlDocument. Return if it failed.
    KoXmlDocument doc;
    QString errorMsg;
    int errorLine;
    int errorColumn;
    if (!doc.setContent(odfStore->device(), true, &errorMsg, &errorLine, &errorColumn)) {
        kDebug() << "Error occurred while parsing styles.xml "
                 << errorMsg << " in Line: " << errorLine
                 << " Column: " << errorColumn;
        odfStore->close();
        return KoFilter::ParsingError;
    }

    // Get the node that contains the styles.
    KoXmlNode stylesNode = doc.documentElement();
    stylesNode = KoXml::namedItemNS(stylesNode, KoXmlNS::office, "automatic-styles");

    // Collect info about the styles.
    collectStyles(stylesNode, styles);

    odfStore->close(); // end of parsing styles in content.xml

    // ----------------------------------------------------------------

    // Try to open and set styles.xml as a KoXmlDocument. Return if it failed.
    //kDebug(30517) << "************ parse styles.xml styles **********************";
    if (!odfStore->open("styles.xml")) {
        kError(30517) << "Unable to open input file! style.xml" << endl;
        return KoFilter::FileNotFound;
    }
    if (!doc.setContent(odfStore->device(), true, &errorMsg, &errorLine, &errorColumn)) {
        kDebug() << "Error occurred while parsing styles.xml "
                 << errorMsg << " in Line: " << errorLine
                 << " Column: " << errorColumn;
        odfStore->close();
        return KoFilter::ParsingError;
    }

    // Parse properties of the named styles referred by the automatic
    // styles. Only those styles that are actually used in the
    // document are converted.
    stylesNode = doc.documentElement();
    stylesNode = KoXml::namedItemNS(stylesNode, KoXmlNS::office, "styles");

    // Collect info about the styles.
    collectStyles(stylesNode, styles);

    odfStore->close();
    return KoFilter::OK;
}

void OdtHtmlConverter::collectStyles(KoXmlNode &stylesNode, QHash<QString, StyleInfo*> &styles)
{
    KoXmlElement styleElement;
    forEachElement (styleElement, stylesNode) {

        // FIXME: Handle text:outline-style also.
        QString tagName = styleElement.tagName();
        if (tagName != "style" && tagName != "default-style")
            continue;

        StyleInfo *styleInfo = new StyleInfo;

        // Get the style name. Default styles don't have a name so
        // give them a constructed name by combining "default" and the
        // style family in a way that should not collide with any real
        // style name.
        QString styleName = styleElement.attribute("name");
        if (tagName == "default-style") {
            // This name should not collide with any real name.
            styleName = QString("default%") + styleElement.attribute("family");
            styleInfo->isDefaultStyle = true;
        }

        styleInfo->family = styleElement.attribute("family");

        // Every style should have a parent. If the style has no
        // parent, then use the appropriate default style.
        QString parentName = styleElement.attribute("parent-style-name");
        if (!styleInfo->isDefaultStyle && parentName.isEmpty()) {
            parentName = QString("default%") + styleInfo->family;
        }
        styleInfo->parent = parentName;

        // Limit picture size to 99% of the page size whatever that may be.
        if (styleElement.attribute("family") == "graphic") {
            styleInfo->attributes.insert("max-height", "99%");
            styleInfo->attributes.insert("max-width", "99%");
            styleInfo->attributes.insert("height", "auto");
            styleInfo->attributes.insert("width", "auto");
        }

        styleInfo->hasBreakBefore = false;
        KoXmlElement propertiesElement;
        forEachElement (propertiesElement, styleElement) {
            //Check for fo:break-before
            if (propertiesElement.attribute("break-before") == "page") {
                //kDebug(30517) << "Found break-before=page in style" << styleName;
                styleInfo->hasBreakBefore = true;
            }
            collectStyleAttributes(propertiesElement, styleInfo);
        }

#if 0 // debug
        kDebug(30517) << "==" << styleName << ":\t"
                      << styleInfo->parent
                      << styleInfo->family
                      << styleInfo->isDefaultStyle
                      << styleInfo->hasBreakBefore
                      << styleInfo->attributes;
#endif
        styles.insert(styleName, styleInfo);
    }
}

void OdtHtmlConverter::collectStyleAttributes(KoXmlElement &propertiesElement, StyleInfo *styleInfo)
{
    // font properties
    QString attribute = propertiesElement.attribute("font-family");
    if (!attribute.isEmpty()) {
        attribute = '"' + attribute + '"';
        styleInfo->attributes.insert("font-family", attribute);
    }

    QStringList attributes;
    attributes
        // font
        << "font-style" << "font-variant" << "font-weight" << "font-size"
        // text
        << "text-indent" << "text-align" << "text-decoration" << "white-space"
        // color
        << "color" << "background-color"
        // visual formatting
        << "width" << "min-width" << "max-width"
        << "height" << "min-height" << "max-height" << "line-height" << "vertical-align"
        // border
        << "border-top-width" << "border-bottom-width"
        << "border-left-width" << "border-right-width" << "border-width"
        // border
        << "border-top-color" << "border-bottom-color"
        << "border-left-color" << "border-right-color" << "border-color"
        // border
        << "border-top-style" << "border-bottom-style"
        << "border-left-style" << "border-right-style" << "border-style"
        << "border-top" << "border-bottom" << "border-left" << "border-right" << "border"
        // padding
        << "padding-top" << "padding-bottom" << "padding-left" << "padding-right" << "padding"
        << "margin-top" << "margin-bottom" << "margin-left" << "margin-right" //<< "margin"
        << "auto";

    // Handle all general text formatting attributes
    foreach(const QString &attrName, attributes) {
        QString attrVal = propertiesElement.attribute(attrName);

        if (!attrVal.isEmpty()) {
            styleInfo->attributes.insert(attrName, attrVal);
        }
    }

    // Text Decorations
    attribute = propertiesElement.attribute("text-underline-style");
    if (!attribute.isEmpty() && attribute != "none") {
        styleInfo->attributes.insert("text-decoration", "underline");
    }
    attribute = propertiesElement.attribute("text-overline-style");
    if (!attribute.isEmpty() && attribute != "none") {
        styleInfo->attributes.insert("text-decoration", "overline");
    }
    attribute = propertiesElement.attribute("text-line-through-style");
    if (!attribute.isEmpty() && attribute != "none") {
        styleInfo->attributes.insert("text-decoration", "line-through");
    }

    // Visual Display Model
    attribute = propertiesElement.attribute("writing-mode");
    if (!attribute.isEmpty()) {
        if (attribute == "rl")
            attribute = "rtl";
        else if (attribute == "lr")
            attribute = "ltr";
        else
            attribute = "inherited";
        styleInfo->attributes.insert("direction", attribute);
    }

    // Image align
    attribute = propertiesElement.attribute("horizontal-pos");
    if (!attribute.isEmpty()) {
        kDebug(30517) << "horisontal pos attribute" << attribute;
        if (attribute == "right" || attribute == "from-left") {
            styleInfo->attributes.insert("float", "right");
            styleInfo->attributes.insert("margin", "5px 0 5px 15px");
        }
        // Center doesnt show very well.
//        else if (attribute == "center") {
//            styleInfo->attributes.insert("display", "block");
//            styleInfo->attributes.insert("margin", "10px auto");
//        }
        else if (attribute == "left") {
            styleInfo->attributes.insert("display", "inline");
            styleInfo->attributes.insert("float", "left");
            styleInfo->attributes.insert("margin","5px 15px 5px 0");
        }
    }

    // Lists and numbering
    if (propertiesElement.hasAttribute("num-format")) {
        attribute = propertiesElement.attribute("num-format");
        if (!attribute.isEmpty()) {
            if (attribute == "1")
                attribute = "decimal";
            else if (attribute == "i")
                attribute = "lower-roman";
            else if (attribute == "I")
                attribute = "upper-roman";
            else if (attribute == "a")
                attribute = "lower-alpha";
            else if (attribute == "A")
                attribute = "upper-alpha";
            else
                attribute = "decimal";
        }
        styleInfo->attributes.insert("list-style-type:", attribute);
        styleInfo->attributes.insert("list-style-position:", "outside");
    }
    else if (propertiesElement.hasAttribute("bullet-char")) {
        attribute = propertiesElement.attribute("bullet-char");
        if (!attribute.isEmpty()) {
            switch (attribute[0].unicode()) {
            case 0x2022:
                attribute = "disc";
                break;
            case 0x25CF:
                attribute = "disc";
                break;
            case 0x25CB:
                attribute = "circle";
                break;
            case 0x25A0:
                attribute = "square";
                break;
            default:
                attribute = "disc";
                break;
            }
        }
        styleInfo->attributes.insert("list-style-type:", attribute);
        styleInfo->attributes.insert("list-style-position:", "outside");
    }
}


// ================================================================
//                         HTML conversion


KoFilter::ConversionStatus OdtHtmlConverter::convertContent(KoStore *odfStore,
                                                            QHash<QString, QString> &metaData,
                                                            EpubFile *epub,
                                                            QHash<QString, StyleInfo*> &styles,
                                                            QHash<QString, QSizeF> &imagesSrcList)
{
    if (!odfStore->open("content.xml")) {
        kDebug(30517) << "Can not open content.xml .";
        return KoFilter::FileNotFound;
    }

    QByteArray htmlContent;
    QBuffer *outBuf = new QBuffer(&htmlContent);
    KoXmlWriter *htmlWriter = new KoXmlWriter(outBuf);

    // ----------------------------------------------------------------
    // Parse body from content.xml

    KoXmlDocument doc;
    QString errorMsg;
    int errorLine;
    int errorColumn;
    if (!doc.setContent(odfStore->device(), true, &errorMsg, &errorLine, &errorColumn)) {
        kDebug(30517) << "Error occurred while parsing content.xml "
                 << errorMsg << " in Line: " << errorLine
                 << " Column: " << errorColumn;
        odfStore->close();
        return KoFilter::ParsingError;
    }

    KoXmlNode currentNode = doc.documentElement();
    KoXmlElement nodeElement;    // currentNode as Element

    currentNode = KoXml::namedItemNS(currentNode, KoXmlNS::office, "body");
    currentNode = KoXml::namedItemNS(currentNode, KoXmlNS::office, "text");

    // Write the beginning of the output.
    htmlWriter->startElement("html");
    htmlWriter->addAttribute("xmlns", "http://www.w3.org/1999/xhtml");
    createHtmlHead(htmlWriter, metaData);
    htmlWriter->startElement("body");

    // Collect information about internal links.
    KoXmlElement element = currentNode.toElement(); // node for passing it to collectInter...()
    int chapter = 1;
    collectInternalLinksInfo(element, styles, chapter);

    QString prefix = "chapter"; // Prefix of chapter names.
    m_currentChapter = 1;       // Number of current output chapter.
    forEachElement (nodeElement, currentNode) {

        //kDebug(30517) << nodeElement.tagName() <<pFlag;
        if ((nodeElement.localName() == "p" && nodeElement.namespaceURI() == KoXmlNS::text)
                || (nodeElement.localName() == "h" && nodeElement.namespaceURI() == KoXmlNS::text)) {

            // A fo:break-before="page" in the style means create a new chapter here,
            // but only if it is a top-level paragraph and not at the very first node.
            StyleInfo *style = styles.value(nodeElement.attribute("style-name"));
            if (style && style->hasBreakBefore) {
                //kDebug(30517) << "Found paragraph with style with break-before -- breaking new chapter";

                // Write out any footnotes
                if (!m_footNotes.isEmpty()) {
                    writeFootNotes(htmlWriter, styles, imagesSrcList);
                }
                // This paragraph is at top level so we should close
                // the html file and start on the next file.
                htmlWriter->endElement();
                htmlWriter->endElement();

                // Write output file to the epub object.
                QString fileId = prefix + QString::number(m_currentChapter);
                QString fileName = "OEBPS/" + fileId + ".xhtml";
                epub->addContentFile(fileId, fileName, "application/xhtml+xml", htmlContent);

                // Prepare for the next file.
                htmlContent.clear();
                delete htmlWriter;
                delete outBuf;
                outBuf = new QBuffer(&htmlContent);
                htmlWriter = new KoXmlWriter(outBuf);
                m_currentChapter++;

                // Write the beginning of the output for the next file.
                htmlWriter->startElement("html");
                htmlWriter->addAttribute("xmlns", "http://www.w3.org/1999/xhtml");
                createHtmlHead(htmlWriter, metaData);
                htmlWriter->startElement("body");
            }

            // Actually handle the contents.
            if (nodeElement.localName() == "p")
                handleTagP(nodeElement, htmlWriter, styles, imagesSrcList);
            else
                handleTagH(nodeElement, htmlWriter, styles, imagesSrcList);
        }
        else if (nodeElement.localName() == "span" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagSpan(nodeElement, htmlWriter, styles, imagesSrcList);
        }
        else if (nodeElement.localName() == "table" && nodeElement.namespaceURI() == KoXmlNS::table) {
            // Handle table
            handleTagTable(nodeElement, htmlWriter, styles, imagesSrcList);
        }
        else if (nodeElement.localName() == "frame" && nodeElement.namespaceURI() == KoXmlNS::draw)  {
            // Handle frame
            htmlWriter->startElement("div");
            handleTagFrame(nodeElement, htmlWriter, styles, imagesSrcList);
            htmlWriter->endElement(); // end div
        }
        else if (nodeElement.localName() == "soft-page-break" &&
                 nodeElement.namespaceURI() == KoXmlNS::text) {

            handleTagPageBreak(nodeElement, htmlWriter, styles);
        }
        else if (nodeElement.localName() == "list" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagList(nodeElement, htmlWriter, styles, imagesSrcList);
        }
        else if (nodeElement.localName() == "a" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagA(nodeElement, htmlWriter, styles, imagesSrcList);
        }
        else if (nodeElement.localName() == "table-of-content" &&
                 nodeElement.namespaceURI() == KoXmlNS::text) {

            handleTagTableOfContent(nodeElement, htmlWriter, styles, imagesSrcList);
        }
        else if (nodeElement.localName() == "line-break" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagLineBreak(htmlWriter);
        }
        else {
            htmlWriter->startElement("div");
            handleUnknownTags(nodeElement, htmlWriter, styles, imagesSrcList);
            htmlWriter->endElement();
        }
    }

    htmlWriter->endElement(); // body
    htmlWriter->endElement(); // html

    // Write output of the last file to the epub object.
    QString fileId = prefix + QString::number(m_currentChapter);
    QString fileName = "OEBPS/" + fileId + ".xhtml";
    epub->addContentFile(fileId, fileName, "application/xhtml+xml", htmlContent);

    // if we had end notes, make a new chapter for end notes
    if (!m_endNotes.isEmpty()) {

        htmlContent.clear();
        delete htmlWriter;
        delete outBuf;
        outBuf = new QBuffer(&htmlContent);
        htmlWriter = new KoXmlWriter(outBuf);

        // Write the beginning of the output for the next file.
        htmlWriter->startElement("html");
        htmlWriter->addAttribute("xmlns", "http://www.w3.org/1999/xhtml");
        createHtmlHead(htmlWriter, metaData);
        htmlWriter->startElement("body");

        writeEndNotes(htmlWriter, styles, imagesSrcList);

        htmlWriter->endElement(); // body
        htmlWriter->endElement(); // html

        QString fileId = "chapter-endnotes";
        QString fileName = "OEBPS/" + fileId + ".xhtml";
        epub->addContentFile(fileId, fileName, "application/xhtml+xml", htmlContent);

    }

    delete htmlWriter;
    delete outBuf;

    odfStore->close();
    return KoFilter::OK;
}


void OdtHtmlConverter::createHtmlHead(KoXmlWriter *writer, QHash<QString, QString> &metaData)
{
    writer->startElement("head");

    writer->startElement("title");
    writer->addTextNode(metaData.value("title"));
    writer->endElement(); // title

    writer->startElement("meta");
    writer->addAttribute("http-equiv", "Content-Type");
    writer->addAttribute("content", "text/html; charset=utf-8");
    writer->endElement(); // meta

    // write meta tag
    // m-meta <Tagname, Text>
    // <meta name = "Tagname" content = "Text" />
    foreach (const QString &name, metaData.keys()) {
        // Title is handled above.
        if (name == "title")
            continue;

        writer->startElement("meta");
        writer->addAttribute("name", name);
        writer->addAttribute("content", metaData.value(name));
        writer->endElement(); // meta
    }

    writer->startElement("link");
    writer->addAttribute("href", "styles.css");
    writer->addAttribute("type", "text/css");
    writer->addAttribute("rel", "stylesheet");
    writer->endElement(); // link

    writer->endElement(); // head
}


void OdtHtmlConverter::handleTagTable(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                                      QHash<QString, StyleInfo *> &styles,
                                      QHash<QString, QSizeF> &imagesSrcList)
{
    QString styleName = nodeElement.attribute("style-name");
    StyleInfo *styleInfo = styles.value(styleName);
    htmlWriter->startElement("table");
    if (styleInfo) {
        styleInfo->inUse = true;
        htmlWriter->addAttribute("class", styleName);
    }
    htmlWriter->addAttribute("border", "1");

    //===== table-row ======
    KoXmlElement tableElement;
    forEachElement (tableElement, nodeElement) {
        if (tableElement.localName() != "table-column" && tableElement.namespaceURI() == KoXmlNS::table) {
            htmlWriter->startElement("tr");

            // ===== table-cell ======
            KoXmlElement cellElement;
            forEachElement (cellElement, tableElement) {
                QString styleName = cellElement.attribute("style-name");
                StyleInfo *styleInfo = styles.value(styleName);
                htmlWriter->startElement("td");
                if (styleInfo) {
                    styleInfo->inUse = true;
                    htmlWriter->addAttribute("class", styleName);
                }

                // ==== cell text ====
                // FIXME: This is wrong. A cell element can contain
                //        the same tags as the full contents, not just
                //        what is inside a paragraph. (Beside, this
                //        function has a strange name.)
                handleInsideElementsTag(cellElement, htmlWriter, styles, imagesSrcList);
                // ===================
                htmlWriter->endElement(); // td
            } // end for write tag cell

            htmlWriter->endElement(); // tr
        } // end while write tag row
    }

    htmlWriter->endElement();
}

void OdtHtmlConverter::handleTagFrame(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                                      QHash<QString, StyleInfo*> &styles,
                                      QHash<QString, QSizeF> &imagesSrcList)
{
    QString styleName = nodeElement.attribute("style-name");
    StyleInfo *styleInfo = styles.value(styleName);
    htmlWriter->startElement("img");
    if (styleInfo) {
        styleInfo->inUse = true;
        htmlWriter->addAttribute("class", styleName);
    }
    htmlWriter->addAttribute("alt", "(No Description)");

    // Find height and width
    QString height = nodeElement.attribute("height");
    QString width  = nodeElement.attribute("width");
    // remove characters "in" or "pt" from their end
    height = height.left(height.length()-2);
    width  = width.left(width.length()-2);
    // Convert them to real
    qreal qHeight = height.toFloat();
    qreal qWidth = width.toFloat();
    QSizeF size(qWidth, qHeight);

    // Check image tag to find image source
    KoXmlElement imgElement;
    forEachElement (imgElement, nodeElement) {
        if (imgElement.localName() == "image" && imgElement.namespaceURI() == KoXmlNS::draw) {
            QString imgSrc = imgElement.attribute("href").section('/', -1);
            htmlWriter->addAttribute("src", imgSrc);

            imagesSrcList.insert(imgElement.attribute("href"), size);
        }
    }
    htmlWriter->endElement(); // end img
}

void OdtHtmlConverter::handleTagP(KoXmlElement &nodeElement,
                                  KoXmlWriter *htmlWriter, QHash<QString, StyleInfo *> &styles,
                                  QHash<QString, QSizeF> &imagesSrcList)
{
    QString styleName = nodeElement.attribute("style-name");
    StyleInfo *styleInfo = styles.value(styleName);
    htmlWriter->startElement("p");
    if (styleInfo) {
        styleInfo->inUse = true;
        htmlWriter->addAttribute("class", styleName);
    }
    handleInsideElementsTag(nodeElement, htmlWriter, styles, imagesSrcList);
    htmlWriter->endElement();
}

void OdtHtmlConverter::handleCharacterData(KoXmlNode &node, KoXmlWriter *htmlWriter,
                                           QHash<QString, StyleInfo*> &styles)
{
    Q_UNUSED(styles);

    KoXmlText charData = node.toText();
    htmlWriter->addTextNode(charData.data());
}

void OdtHtmlConverter::handleTagSpan(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                                     QHash<QString, StyleInfo *> &styles,
                                     QHash<QString, QSizeF> &imagesSrcList)
{
    QString styleName = nodeElement.attribute("style-name");
    StyleInfo *styleInfo = styles.value(styleName);
    htmlWriter->startElement("span");
    if (styleInfo) {
        styleInfo->inUse = true;
        htmlWriter->addAttribute("class", styleName);
    }
    handleInsideElementsTag(nodeElement, htmlWriter, styles, imagesSrcList);
    htmlWriter->endElement(); // span
}

void OdtHtmlConverter::handleTagPageBreak(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                                          QHash<QString, StyleInfo*> &styles)
{
    Q_UNUSED(styles);

    htmlWriter->addTextNode(nodeElement.text().toUtf8());
}

void OdtHtmlConverter::handleTagH(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                                  QHash<QString, StyleInfo *> &styles,
                                  QHash<QString, QSizeF> &imagesSrcList)
{
    QString styleName = nodeElement.attribute("style-name");
    StyleInfo *styleInfo = styles.value(styleName);
    htmlWriter->startElement("h1");
    if (styleInfo) {
        styleInfo->inUse = true;
        htmlWriter->addAttribute("class", styleName);
    }
    handleInsideElementsTag(nodeElement, htmlWriter, styles, imagesSrcList);
    htmlWriter->endElement();
}

void OdtHtmlConverter::handleTagList(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                                     QHash<QString, StyleInfo *> &styles,
                                     QHash<QString, QSizeF> &imagesSrcList)
{
    QString styleName = nodeElement.attribute("style-name");
    StyleInfo *styleInfo = styles.value(styleName);
    htmlWriter->startElement("ul");
    if (styleInfo) {
        styleInfo->inUse = true;
        htmlWriter->addAttribute("class", styleName);
    }

    KoXmlElement listItem;
    forEachElement (listItem, nodeElement) {
        htmlWriter->startElement("li");
        handleInsideElementsTag(listItem, htmlWriter, styles, imagesSrcList);
        htmlWriter->endElement();
    }
    htmlWriter->endElement();
}

void OdtHtmlConverter::handleTagA(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                                  QHash<QString, StyleInfo *> &styles,
                                  QHash<QString, QSizeF> &imagesSrcList)
{
    htmlWriter->startElement("a");
    QString refrence = nodeElement.attribute("href");
    QString chapter = m_linksInfo.value(refrence);
    if (!chapter.isEmpty()) {
        // This is internal link.
        refrence = refrence.remove("|");
        refrence = refrence.remove(" ");// remove spaces
        refrence = chapter+refrence;
        htmlWriter->addAttribute("href", refrence);
    }
    else {
        // This is external link.
        htmlWriter->addAttribute("href", refrence);
    }

    handleInsideElementsTag(nodeElement, htmlWriter, styles, imagesSrcList);
    htmlWriter->endElement();
}

void OdtHtmlConverter::handleTagTab (KoXmlWriter *htmlWriter)
{
    for (int i = 0; i <10; ++i)
        htmlWriter->addTextNode("\u00a0");
}

void OdtHtmlConverter::handleTagTableOfContent(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                                               QHash<QString, StyleInfo *> &styles,
                                               QHash<QString, QSizeF> &imagesSrcList)
{
    KoXmlNode indexBody = KoXml::namedItemNS(nodeElement, KoXmlNS::text, "index-body");
    KoXmlElement  element;
    forEachElement (element, indexBody) {
        if (element.localName() == "index-title" && element.namespaceURI() == KoXmlNS::text) {
            handleInsideElementsTag(element, htmlWriter, styles, imagesSrcList);
        }
        else
            handleTagTableOfContentBody(element, htmlWriter, styles, imagesSrcList);
    }
}

void OdtHtmlConverter::handleTagTableOfContentBody(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                                                   QHash<QString, StyleInfo *> &styles,
                                                   QHash<QString, QSizeF> &imagesSrcList)
{
    if (nodeElement.localName() == "p" && nodeElement.namespaceURI() == KoXmlNS::text) {
        handleTagP(nodeElement, htmlWriter, styles, imagesSrcList);
    }
}

void OdtHtmlConverter::handleTagLineBreak(KoXmlWriter *htmlWriter)
{
    htmlWriter->startElement("br");
    htmlWriter->endElement();
}

void OdtHtmlConverter::handleTagBookMark(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter)
{
    QString anchor = nodeElement.attribute("name");
    // This is haed codevalidator gets error for characters "|" and spaces
    // FIXME : we should handle ids better after move file to class
    anchor = anchor.remove("|");
    anchor = anchor.remove(" ");//remove spaces
    htmlWriter->startElement("a");
    htmlWriter->addAttribute("id", anchor);
}

void OdtHtmlConverter::handleTagBookMarkStart(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter)
{
    QString anchor = nodeElement.attribute("name");
    htmlWriter->startElement("a");
    htmlWriter->addAttribute("id", anchor);
}

void OdtHtmlConverter::handleTagBookMarkEnd(KoXmlWriter *htmlWriter)
{
    htmlWriter->endElement();
}

void OdtHtmlConverter::handleUnknownTags(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                                         QHash<QString, StyleInfo *> &styles,
                                         QHash<QString, QSizeF> &imagesSrcList)
{
    //just go dipper to find known tags
    handleInsideElementsTag(nodeElement, htmlWriter, styles, imagesSrcList);
}


void OdtHtmlConverter::handleTagNote(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter)
{
    QString noteClass = nodeElement.attribute("note-class");
    if (noteClass != "footnote" && noteClass != "endnote") {
        return;
    }

    QString id = nodeElement.attribute("id");
    KoXmlElement noteElements;
    forEachElement(noteElements, nodeElement) {
        if (noteElements.localName() == "note-citation" && noteElements.namespaceURI() == KoXmlNS::text) {
            htmlWriter->startElement("sup");

            htmlWriter->startElement("a");
            if (noteClass == "footnote")
                htmlWriter->addAttribute("href", "#" + id + "n"); // n rerence to note foot-note or end-note
            else { // endnote
                QString endRef = "chapter-endnotes.xhtml#" + id + "n";
                htmlWriter->addAttribute("href", endRef);
            }
            htmlWriter->addAttribute("id", id + "t"); // t is for text
            htmlWriter->addTextNode(noteElements.text());
            htmlWriter->endElement();

            htmlWriter->endElement();
        }
        else if (noteElements.localName() == "note-body" && noteElements.namespaceURI() == KoXmlNS::text) {
            if (noteClass == "footnote")
                m_footNotes.insert(id, noteElements);
            else {
                QString noteChpater = "chapter" + QString::number(m_currentChapter) + ".xhtml";
                m_endNotes.insert(noteChpater + "/" + id, noteElements);
                // we insert this: m_currentChapter/id
                // to can add refrence for text in end note
            }
        }
    }
}

void OdtHtmlConverter::handleInsideElementsTag(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                                               QHash<QString, StyleInfo *> &styles,
                                               QHash<QString, QSizeF> &imagesSrcList)
{
    KoXmlNode node = nodeElement.firstChild();
    KoXmlElement element = node.toElement();

    // We should end tag "a" after text for bookmark-start we can
    // handle it with tag bookmark-end but for bookmark there is no
    // bookmark-end so i use this flag to close the tag "a" that i
    // have opened it in handleTagBookMark()
    bool bookMarkFlag = false;

    // We have characterData or image or span or s  or soft-page break in a tag p
    // FIXME: we should add if there are more tags.
    while (!node.isNull()) {

        if (node.isText()) {
            handleCharacterData(node, htmlWriter, styles);
            if (bookMarkFlag) {
                htmlWriter->endElement(); // end tag "a"
                bookMarkFlag = false;
            }
        }
        else if (element.localName() == "p" && element.namespaceURI() == KoXmlNS::text) {
            handleTagP(element, htmlWriter, styles, imagesSrcList);
        }
        else if (element.localName() == "h" && element.namespaceURI() == KoXmlNS::text) {
            handleTagH(element, htmlWriter, styles, imagesSrcList);
        }
        else if (element.localName() == "table" && element.namespaceURI() == KoXmlNS::table) {
            handleTagH(element, htmlWriter, styles, imagesSrcList);
        }
        else if (element.localName() == "span" && element.namespaceURI() == KoXmlNS::text) {
            handleTagSpan(element, htmlWriter, styles, imagesSrcList);
        }
        else if (element.localName() == "frame" && element.namespaceURI() == KoXmlNS::draw) {
            handleTagFrame(element, htmlWriter, styles, imagesSrcList);
        }
        else if (nodeElement.localName() == "list" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagList(nodeElement, htmlWriter, styles, imagesSrcList);
        }
        else if (element.localName() == "soft-page-break" && element.namespaceURI() == KoXmlNS::text) {
            handleTagPageBreak(element, htmlWriter, styles);
        }
        else if (element.localName() == "a" && element.namespaceURI() == KoXmlNS::text) {
            handleTagA(element, htmlWriter, styles, imagesSrcList);
        }
        else if (element.localName() == "s" && element.namespaceURI() == KoXmlNS::text) {
            htmlWriter->addTextNode("\u00a0");
        }
        else if (element.localName() == "line-break" && element.namespaceURI() == KoXmlNS::text) {
            handleTagLineBreak(htmlWriter);
        }
        else if (element.localName() == "tab" && element.namespaceURI() == KoXmlNS::text) {
            handleTagTab(htmlWriter);
        }
        else if (element.localName() == "bookmark" && element.namespaceURI() == KoXmlNS::text) {
            handleTagBookMark(element, htmlWriter);
            bookMarkFlag = true;
        }
        else if (element.localName() == "bookmark-start" && element.namespaceURI() == KoXmlNS::text) {
            handleTagBookMarkStart(element, htmlWriter);
        }
        else if (element.localName() == "bookmark-end" && element.namespaceURI() == KoXmlNS::text) {
            handleTagBookMarkEnd(htmlWriter);
        }
        else if (element.localName() == "note" && element.namespaceURI() == KoXmlNS::text) {
            handleTagNote(element, htmlWriter);
        }
        else
            handleUnknownTags(element, htmlWriter, styles, imagesSrcList);

        node = node.nextSibling();
        element = node.toElement();
    }
}

void OdtHtmlConverter::collectInternalLinksInfo(KoXmlElement &currentElement,
                                                QHash<QString, StyleInfo *> &styles, int &chapter)
{
    KoXmlElement nodeElement;
    forEachElement (nodeElement, currentElement) {
        if ( (nodeElement.localName() == "p" || nodeElement.localName() == "h") 
             && nodeElement.namespaceURI() == KoXmlNS::text)
        {
            // A break-before in the style means create a new chapter here,
            // but only if it is a top-level paragraph and not at the very first node.
            StyleInfo *style = styles.value(nodeElement.attribute("style-name"));
            if (style && style->hasBreakBefore) {
                chapter++;
            }
        }
        else if ((nodeElement.localName() == "bookmark-start" || nodeElement.localName() == "bookmark")
                  && nodeElement.namespaceURI() == KoXmlNS::text) {
            QString key = "#" + nodeElement.attribute("name");
            QString value = "chapter" + QString::number(chapter) + ".xhtml";
            m_linksInfo.insert(key, value);
            continue;
        }

        // Check for links recursively also inside this element.
        collectInternalLinksInfo(nodeElement, styles, chapter);
    }
}

void OdtHtmlConverter::writeFootNotes(KoXmlWriter *htmlWriter, QHash<QString, StyleInfo *> &styles,
                                      QHash<QString, QSizeF> &imagesSrcList)
{
    htmlWriter->startElement("p");
    handleTagLineBreak(htmlWriter);
    htmlWriter->addTextNode("___________________________________________");
    htmlWriter->endElement();

    htmlWriter->startElement("ul");
    int noteCounts = 1;
    foreach(QString id, m_footNotes.keys()) {
        htmlWriter->startElement("li");
        htmlWriter->addAttribute("id", id + "n");

        htmlWriter->startElement("a");
        htmlWriter->addAttribute("href", "#" + id + "t"); // refrence to text
        htmlWriter->addTextNode("[" + QString::number(noteCounts) + "]");
        htmlWriter->endElement();

        KoXmlElement bodyElement = m_footNotes.value(id);
        handleInsideElementsTag(bodyElement, htmlWriter, styles, imagesSrcList);

        htmlWriter->endElement();
        noteCounts++;
    }
    htmlWriter->endElement();
    m_footNotes.clear(); // clear for next chapter
}

void OdtHtmlConverter::writeEndNotes(KoXmlWriter *htmlWriter, QHash<QString, StyleInfo *> &styles,
                                     QHash<QString, QSizeF> &imagesSrcList)
{
    htmlWriter->startElement("h1");
    htmlWriter->addTextNode("End Notes");
    handleTagLineBreak(htmlWriter);
    htmlWriter->endElement();

    htmlWriter->startElement("ul");
    int noteCounts = 1;
    foreach(QString id, m_endNotes.keys()) {
        htmlWriter->startElement("li");
        htmlWriter->addAttribute("id", id.section("/", 1) + "n");

        htmlWriter->startElement("a");
        // id = chapter-endnotes.xhtml/endnoteId
        htmlWriter->addAttribute("href",id.section("/", 0, 0) + "#" + id.section("/", 1) + "t");
        htmlWriter->addTextNode("["+QString::number(noteCounts)+"]");
        htmlWriter->endElement();

        KoXmlElement bodyElement = m_endNotes.value(id);
        handleInsideElementsTag(bodyElement, htmlWriter, styles, imagesSrcList);

        htmlWriter->endElement();
        noteCounts++;
    }
    htmlWriter->endElement();
}
