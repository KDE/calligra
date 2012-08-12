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
#include "htmlconvert.h"

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


static void handleStyles(KoXmlNode &stylesNode,
                         QHash<QString, StyleInfo*> &styles);
static void handleStyleAttributes(KoXmlElement &propertiesElement, QList<QString> &attList,
                                  StyleInfo *styleInfo);

static void createHtmlHead(KoXmlWriter *writer, QHash<QString, QString> &metaData);


// All handleTag*() are named after the tag in the ODF that they handle.
static void handleCharacterData(KoXmlNode &node, KoXmlWriter *bodyWriter,
                                QHash<QString, StyleInfo*> &styles);

static void handleTagTable(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                           QHash<QString, StyleInfo*> &styles, QHash<QString, QSizeF> &imagesSrcList,
                           QHash<QString, QString> linksInfo, QHash<QString, KoXmlElement> &footNotes,
                           QHash<QString, KoXmlElement> &endNotes);

static void handleTagFrame(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                           QHash<QString, StyleInfo*> &styles, QHash<QString, QSizeF> &imagesSrcList);

static void handleTagP(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                       QHash<QString, StyleInfo*> &styles, QHash<QString, QSizeF> &imagesSrcList,
                       QHash<QString, QString> linksInfo, QHash<QString, KoXmlElement> &footNotes,
                       QHash<QString, KoXmlElement> &endNotes);

static void handleTagSpan(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                          QHash<QString, StyleInfo*> &styles, QHash<QString, QSizeF> &imagesSrcList,
                          QHash<QString, QString> linksInfo, QHash<QString, KoXmlElement> &footNotes,
                          QHash<QString, KoXmlElement> &endNotes);

static void handleTagPageBreak(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                               QHash<QString, StyleInfo*> &styles);

static void handleTagH(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                       QHash<QString, StyleInfo*> &styles, QHash<QString, QSizeF> &imagesSrcList,
                       QHash<QString, QString> linksInfo, QHash<QString, KoXmlElement> &footNotes,
                       QHash<QString, KoXmlElement> &endNotes);

static void handleTagList(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                       QHash<QString, StyleInfo*> &styles, QHash<QString, QSizeF> &imagesSrcList,
                       QHash<QString, QString> linksInfo, QHash<QString, KoXmlElement> &footNotes,
                       QHash<QString, KoXmlElement> &endNotes);

static void handleTagA(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                       QHash<QString, StyleInfo*> &styles, QHash<QString, QSizeF> &imagesSrcList,
                       QHash<QString, QString> linksInfo, QHash<QString, KoXmlElement> &footNotes,
                       QHash<QString, KoXmlElement> &endNotes);

static void handleTagTab(KoXmlWriter *bodyWriter);
static void handleTagTableOfContent(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                       QHash<QString, StyleInfo*> &styles, QHash<QString, QSizeF> &imagesSrcList,
                       QHash<QString, QString> linksInfo, QHash<QString, KoXmlElement> &footNotes,
                       QHash<QString, KoXmlElement> &endNotes);

static void handleTagTableOfContentBody(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                       QHash<QString, StyleInfo*> &styles, QHash<QString, QSizeF> &imagesSrcList,
                       QHash<QString, QString> linksInfo,QHash<QString, KoXmlElement> &footNotes,
                       QHash<QString, KoXmlElement> &endNotes);

static void handleTagLineBreak(KoXmlWriter *bodyWriter);
static void handleTagBookMark(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter);
static void handleTagBookMarkStart(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter);
static void handleTagBookMarkEnd(KoXmlWriter *bodyWriter);


static void handleUnknownTags(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                              QHash<QString, StyleInfo*> &styles, QHash<QString, QSizeF> &imagesSrcList,
                              QHash<QString, QString> linksInfo, QHash<QString, KoXmlElement> &footNotes,
                              QHash<QString, KoXmlElement> &endNotes);

static void handleInsideElementsTag(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                                    QHash<QString, StyleInfo*> &styles,
                                    QHash<QString, QSizeF> &imagesSrcList,
                                    QHash<QString, QString> linksInfo,
                                    QHash<QString, KoXmlElement> &footNotes,
                                    QHash<QString, KoXmlElement> &endNotes);

static void handleTagNote(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                                    QHash<QString, KoXmlElement> &footNotes,
                                    QHash<QString, KoXmlElement> &endNotes);

static void writeFootNotes(KoXmlWriter *bodyWriter,
                           QHash<QString, StyleInfo*> &styles, QHash<QString, QSizeF> &imagesSrcList,
                           QHash<QString, QString> linksInfo, QHash<QString, KoXmlElement> &footNotes,
                           QHash<QString, KoXmlElement> &endNotes);

static void writeEndNotes(KoXmlWriter *bodyWriter, QHash<QString, StyleInfo*> &styles,
                          QHash<QString, QSizeF> &imagesSrcList, QHash<QString, QString> linksInfo,
                          QHash<QString, KoXmlElement> &footNotes, QHash<QString, KoXmlElement> &endNotes);

/** Before start parsing go inside content.xml and collect links id from
  * book-mark-start tag and save its id in hash as key for its value save
  * the current chapter (as we are looking forbook-mark-start tag, we identify
  * page break before too, and we have an id that goes up when i see page break
  * so we now we are in which file or chapter and this id is the value of hash
  * and at the end when we want to write html file, when we see an id, find it in hash
  * and set it instead < a  href = hash.value(key) + #key /> */
static void collectInternalLinksInfo(KoXmlElement &currentElement, QHash<QString, StyleInfo*> &styles,
                                     QHash<QString, QString> &linksInfo, int &chapter);

StyleInfo::StyleInfo()
    : hasBreakBefore(false)
    , inUse(false)
{
}

//FIXME : we should make an object out of it
static int currentChapter;
// ================================================================
//                         Style parsing



KoFilter::ConversionStatus parseStyles(KoStore *odfStore,
                                       QHash<QString, StyleInfo*> &styles)
{
    //kDebug(30517) << "parse content.xml styles";
    if (!odfStore->open("content.xml")) {
        kError(30517) << "Unable to open input file! content.xml" << endl;
        return KoFilter::FileNotFound;
    }

    KoXmlDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;
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

    // Collect attributes in the styles.
    handleStyles(stylesNode, styles);

    odfStore->close(); // end of parsing styles in content.xml

    // ----------------------------------------------------------------

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

    // Collect attributes in the styles.
    handleStyles(stylesNode, styles);

    odfStore->close();
    return KoFilter::OK;
}

void handleStyles(KoXmlNode &stylesNode,
                  QHash<QString, StyleInfo*> &styles)
{
    QString attribute;
    QList <QString> attList;

    KoXmlElement styleElement;
    forEachElement (styleElement, stylesNode) {
        StyleInfo *styleInfo = new StyleInfo;

        QString styleName  = styleElement.attribute("name");
        QString parentName = styleElement.attribute("parent-style-name");
        styleInfo->parent = parentName;

        // Limit picture size to 99% of the page size whatever that may be.
        if (styleElement.attribute("family") == "graphic") {
            attList << "max-heigh: 99%" << "max-width: 99%";
            attList << "width: auto" << "height: auto";
        }

        styleInfo->hasBreakBefore = false;
        KoXmlElement propertiesElement;
        forEachElement (propertiesElement, styleElement) {
            //Check for fo:break-before
            if (propertiesElement.hasAttribute("break-before")) {
                //kDebug(30517) << "Found break-before in style" << styleName;
                styleInfo->hasBreakBefore = true;
            }
            handleStyleAttributes(propertiesElement, attList, styleInfo);
        }
        styles.insert(styleName, styleInfo);
        attList.clear();
    }
}

void handleStyleAttributes(KoXmlElement &propertiesElement, QList<QString> &attList,
                           StyleInfo *styleInfo)
{
    // font properties
    QString attribute = propertiesElement.attribute("font-family");
    if (!attribute.isEmpty()) {
        attribute = '"' + attribute + '"';
        attList << ("font-family:" + attribute);
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

    foreach(const QString &attrName, attributes) {
        QString attrVal = propertiesElement.attribute(attrName);

        if (!attrVal.isEmpty()) {
            attList << attrName + ':' + attrVal;
            styleInfo->attributes.insert(attrName, attrVal);
        }
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
        attList << ("direction:" + attribute);
        styleInfo->attributes.insert("direction", attribute);
    }

    // image align
    attribute = propertiesElement.attribute("horizontal-pos");
    if (!attribute.isEmpty()) {
        kDebug(30517) << "horisontal pos attribute"<<attribute;
        if (attribute == "right" || attribute == "from-left") {
            styleInfo->attributes.insert("float", "right");
            styleInfo->attributes.insert("margin","5px 0 5px 15px");
        }
        // Center doesnt show very well.
//        if (attribute == "center") {
//            styleInfo->attributes.insert("display", "block");
//            styleInfo->attributes.insert("margin", "10px auto");
//        }
        if (attribute == "left") {
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
    else if (propertiesElement.hasAttribute("bullet-char")){
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


KoFilter::ConversionStatus convertContent(KoStore *odfStore, QHash<QString, QString> &metaData,
                                          EpubFile *epub,QHash<QString, StyleInfo*> &styles,
                                          QHash<QString, QSizeF> &imagesSrcList)
{
    if (!odfStore->open("content.xml")) {
        kDebug(30517) << "Can not open content.xml .";
        return KoFilter::FileNotFound;
    }

    QByteArray htmlContent;
    QBuffer *outBuf = new QBuffer(&htmlContent);
    KoXmlWriter *bodyWriter = new KoXmlWriter(outBuf);

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
    bodyWriter->startElement("html");
    bodyWriter->addAttribute("xmlns", "http://www.w3.org/1999/xhtml");
    createHtmlHead(bodyWriter, metaData);
    bodyWriter->startElement("body");

    QString prefix = "chapter";
    currentChapter = 1;            // Number of current output chapter.

    // make ready to collect informatio for internal links.
    QHash<QString, QString> linksInfo;
    KoXmlElement element = currentNode.toElement(); // node for passing it to collectInter...()
    int chapetr = 1;
    // Collect internal links information.
    collectInternalLinksInfo(element, styles,linksInfo, chapetr);

    // hash<key, value> key: footnote id, value: copy of note-body element
    //This will use for add foot-notes at the end of each chapter.
    QHash<QString, KoXmlElement> footNotes;
    QHash<QString, KoXmlElement> endNotes;

    forEachElement (nodeElement, currentNode) {

        //kDebug(30517) << nodeElement.tagName() <<pFlag;
        if ((nodeElement.localName() == "p" && nodeElement.namespaceURI() == KoXmlNS::text)
                || (nodeElement.localName() == "h" && nodeElement.namespaceURI() == KoXmlNS::text)) {

            // A break-before in the style means create a new chapter here,
            // but only if it is a top-level paragraph and not at the very first node.
            StyleInfo *style = styles.value(nodeElement.attribute("style-name"));
            if (style && style->hasBreakBefore) {
                //kDebug(30517) << "Found paragraph with style with break-before -- breaking new chapter";

                // Before to sart new chapter we check our foot notes list if then we add the foot notes
                // to the end of current chapter
                if (!footNotes.isEmpty()) {
                    writeFootNotes(bodyWriter, styles, imagesSrcList, linksInfo, footNotes, endNotes);
                }
                // This paragraph is at top level so we should close
                // the html file and start on the next file.
                bodyWriter->endElement();
                bodyWriter->endElement();

                // Write output file to the epub object.
                QString fileId = prefix + QString::number(currentChapter);
                QString fileName = "OEBPS/" + fileId + ".xhtml";
                epub->addContentFile(fileId, fileName, "application/xhtml+xml", htmlContent);

                // Prepare for the next file.
                htmlContent.clear();
                delete bodyWriter;
                delete outBuf;
                outBuf = new QBuffer(&htmlContent);
                bodyWriter = new KoXmlWriter(outBuf);
                currentChapter++;

                // Write the beginning of the output for the next file.
                bodyWriter->startElement("html");
                bodyWriter->addAttribute("xmlns", "http://www.w3.org/1999/xhtml");
                createHtmlHead(bodyWriter, metaData);
                bodyWriter->startElement("body");
            }
            if (nodeElement.localName() == "p")
                handleTagP(nodeElement, bodyWriter, styles, imagesSrcList,
                           linksInfo, footNotes, endNotes);
            else
                handleTagH(nodeElement, bodyWriter, styles, imagesSrcList, linksInfo, footNotes, endNotes);
        }
        else if (nodeElement.localName() == "span" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagSpan(nodeElement, bodyWriter, styles, imagesSrcList, linksInfo, footNotes, endNotes);
        }
        else if (nodeElement.localName() == "table" && nodeElement.namespaceURI() == KoXmlNS::table) {
            // Handle table
            handleTagTable(nodeElement, bodyWriter, styles, imagesSrcList, linksInfo, footNotes, endNotes);
        }
        else if (nodeElement.localName() == "frame" && nodeElement.namespaceURI() == KoXmlNS::draw)  {
            // Handle frame
            bodyWriter->startElement("div");
            handleTagFrame(nodeElement, bodyWriter, styles, imagesSrcList);
            bodyWriter->endElement(); // end div
        }
        else if (nodeElement.localName() == "soft-page-break" &&
                 nodeElement.namespaceURI() == KoXmlNS::text) {

            handleTagPageBreak(nodeElement, bodyWriter, styles);
        }
        else if (nodeElement.localName() == "list" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagList(nodeElement, bodyWriter, styles, imagesSrcList, linksInfo, footNotes, endNotes);
        }
        else if (nodeElement.localName() == "a" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagA(nodeElement, bodyWriter, styles, imagesSrcList, linksInfo, footNotes, endNotes);
        }
        else if (nodeElement.localName() == "table-of-content" &&
                 nodeElement.namespaceURI() == KoXmlNS::text) {

            handleTagTableOfContent(nodeElement, bodyWriter, styles, imagesSrcList,
                                    linksInfo, footNotes, endNotes);
        }
        else if (nodeElement.localName() == "line-break" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagLineBreak(bodyWriter);
        }
        else {
            bodyWriter->startElement("div");
            handleUnknownTags(nodeElement, bodyWriter, styles, imagesSrcList,
                              linksInfo, footNotes, endNotes);
            bodyWriter->endElement();
        }
    }

    bodyWriter->endElement(); // body
    bodyWriter->endElement(); // html

    // Write output of the last file to the epub object.
    QString fileId = prefix + QString::number(currentChapter);
    QString fileName = "OEBPS/" + fileId + ".xhtml";
    epub->addContentFile(fileId, fileName, "application/xhtml+xml", htmlContent);

    // if we had end notes, make a new chapter for end notes
    if (!endNotes.isEmpty()) {

        htmlContent.clear();
        delete bodyWriter;
        delete outBuf;
        outBuf = new QBuffer(&htmlContent);
        bodyWriter = new KoXmlWriter(outBuf);

        // Write the beginning of the output for the next file.
        bodyWriter->startElement("html");
        bodyWriter->addAttribute("xmlns", "http://www.w3.org/1999/xhtml");
        createHtmlHead(bodyWriter, metaData);
        bodyWriter->startElement("body");

        writeEndNotes(bodyWriter, styles, imagesSrcList, linksInfo, footNotes, endNotes);

        bodyWriter->endElement(); // body
        bodyWriter->endElement(); // html

        QString fileId = "chapter-endnotes";
        QString fileName = "OEBPS/" + fileId + ".xhtml";
        epub->addContentFile(fileId, fileName, "application/xhtml+xml", htmlContent);

    }

    delete bodyWriter;
    delete outBuf;

    odfStore->close();
    return KoFilter::OK;
}


void createHtmlHead(KoXmlWriter *writer, QHash<QString, QString> &metaData)
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


void handleTagTable(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter, QHash<QString, StyleInfo *> &styles,
                    QHash<QString, QSizeF> &imagesSrcList, QHash<QString, QString> linksInfo,
                    QHash<QString, KoXmlElement> &footNotes, QHash<QString, KoXmlElement> &endNotes)
{
    QString styleName = nodeElement.attribute("style-name");
    StyleInfo *styleInfo = styles.value(styleName);
    bodyWriter->startElement("table");
    if (styleInfo) {
        styleInfo->inUse = true;
        bodyWriter->addAttribute("class", styleName);
    }
    bodyWriter->addAttribute("border", "1");

    //===== table-row ======
    KoXmlElement tableElement;
    forEachElement (tableElement, nodeElement) {
        if (tableElement.localName() != "table-column" && tableElement.namespaceURI() == KoXmlNS::table) {
            bodyWriter->startElement("tr");

            // ===== table-cell ======
            KoXmlElement cellElement;
            forEachElement (cellElement, tableElement) {
                QString styleName = cellElement.attribute("style-name");
                StyleInfo *styleInfo = styles.value(styleName);
                bodyWriter->startElement("td");
                if (styleInfo) {
                    styleInfo->inUse = true;
                    bodyWriter->addAttribute("class", styleName);
                }

                // ==== cell text ====
                handleInsideElementsTag(cellElement, bodyWriter, styles, imagesSrcList,
                                        linksInfo, footNotes, endNotes);
                // ===================
                bodyWriter->endElement(); // td
            } // end for write tag cell
            bodyWriter->endElement(); //tr
        } // end while write tag row
    }

    bodyWriter->endElement();
}

void handleTagFrame(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                    QHash<QString, StyleInfo*> &styles, QHash<QString, QSizeF> &imagesSrcList)
{
    QString styleName = nodeElement.attribute("style-name");
    StyleInfo *styleInfo = styles.value(styleName);
    bodyWriter->startElement("img");
    if (styleInfo) {
        styleInfo->inUse = true;
        bodyWriter->addAttribute("class", styleName);
    }
    bodyWriter->addAttribute("alt", "(No Description)");

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
            bodyWriter->addAttribute("src", imgSrc);

            imagesSrcList.insert(imgElement.attribute("href"), size);
        }
    }
    bodyWriter->endElement(); // end img
}

void handleTagP(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter, QHash<QString, StyleInfo *> &styles,
                QHash<QString, QSizeF> &imagesSrcList, QHash<QString, QString> linksInfo,
                QHash<QString, KoXmlElement> &footNotes, QHash<QString, KoXmlElement> &endNotes)
{
    QString styleName = nodeElement.attribute("style-name");
    StyleInfo *styleInfo = styles.value(styleName);
    bodyWriter->startElement("p");
    if (styleInfo) {
        styleInfo->inUse = true;
        bodyWriter->addAttribute("class", styleName);
    }
    handleInsideElementsTag(nodeElement, bodyWriter, styles, imagesSrcList,
                            linksInfo, footNotes, endNotes);
    bodyWriter->endElement();
}

void handleCharacterData(KoXmlNode &node, KoXmlWriter *bodyWriter,
                         QHash<QString, StyleInfo*> &styles)
{
    Q_UNUSED(styles);

    KoXmlText charData = node.toText();
    bodyWriter->addTextNode(charData.data());
}

void handleTagSpan(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter, QHash<QString, StyleInfo *> &styles,
                   QHash<QString, QSizeF> &imagesSrcList, QHash<QString, QString> linksInfo,
                   QHash<QString, KoXmlElement> &footNotes, QHash<QString, KoXmlElement> &endNotes)
{
    QString styleName = nodeElement.attribute("style-name");
    StyleInfo *styleInfo = styles.value(styleName);
    bodyWriter->startElement("span");
    if (styleInfo) {
        styleInfo->inUse = true;
        bodyWriter->addAttribute("class", styleName);
    }
    handleInsideElementsTag(nodeElement, bodyWriter, styles, imagesSrcList, linksInfo, footNotes, endNotes);
    bodyWriter->endElement(); // span
}

void handleTagPageBreak(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                        QHash<QString, StyleInfo*> &styles)
{
    Q_UNUSED(styles);

    bodyWriter->addTextNode(nodeElement.text().toUtf8());
}

void handleTagH(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter, QHash<QString, StyleInfo *> &styles,
                QHash<QString, QSizeF> &imagesSrcList, QHash<QString, QString> linksInfo,
                QHash<QString, KoXmlElement> &footNotes, QHash<QString, KoXmlElement> &endNotes)
{
    QString styleName = nodeElement.attribute("style-name");
    StyleInfo *styleInfo = styles.value(styleName);
    bodyWriter->startElement("h1");
    if (styleInfo) {
        styleInfo->inUse = true;
        bodyWriter->addAttribute("class", styleName);
    }
    handleInsideElementsTag(nodeElement, bodyWriter, styles, imagesSrcList, linksInfo, footNotes, endNotes);
    bodyWriter->endElement();
}

void handleTagList(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter, QHash<QString, StyleInfo *> &styles,
                   QHash<QString, QSizeF> &imagesSrcList, QHash<QString, QString> linksInfo,
                   QHash<QString, KoXmlElement> &footNotes, QHash<QString, KoXmlElement> &endNotes)
{
    QString styleName = nodeElement.attribute("style-name");
    StyleInfo *styleInfo = styles.value(styleName);
    bodyWriter->startElement("ul");
    if (styleInfo) {
        styleInfo->inUse = true;
        bodyWriter->addAttribute("class", styleName);
    }

    KoXmlElement listItem;
    forEachElement (listItem, nodeElement) {
        bodyWriter->startElement("li");
        handleInsideElementsTag(listItem, bodyWriter, styles, imagesSrcList, linksInfo, footNotes, endNotes);
        bodyWriter->endElement();
    }
    bodyWriter->endElement();
}

void handleTagA(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter, QHash<QString, StyleInfo *> &styles,
                QHash<QString, QSizeF> &imagesSrcList, QHash<QString, QString> linksInfo,
                QHash<QString, KoXmlElement> &footNotes, QHash<QString, KoXmlElement> &endNotes)
{
    bodyWriter->startElement("a");
    QString refrence = nodeElement.attribute("href");
    QString chapter = linksInfo.value(refrence);
    if (!chapter.isEmpty()) {
        // This is internal link.
        refrence = refrence.remove("|");
        refrence = refrence.remove(" ");// remove spaces
        refrence = chapter+refrence;
        bodyWriter->addAttribute("href", refrence);
    }
    else {
        // This is external link.
        bodyWriter->addAttribute("href", refrence);
    }

    handleInsideElementsTag(nodeElement, bodyWriter, styles, imagesSrcList, linksInfo, footNotes, endNotes);
    bodyWriter->endElement();
}

void handleTagTab (KoXmlWriter *bodyWriter)
{
    for (int i = 0; i <10; i++)
        bodyWriter->addTextNode("\u00a0");
}

void handleTagTableOfContent(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                             QHash<QString, StyleInfo *> &styles,QHash<QString, QSizeF> &imagesSrcList,
                             QHash<QString, QString> linksInfo,QHash<QString, KoXmlElement> &footNotes,
                             QHash<QString, KoXmlElement> &endNotes)
{
    KoXmlNode indexBody = KoXml::namedItemNS(nodeElement, KoXmlNS::text, "index-body");
    KoXmlElement  element;
    forEachElement (element, indexBody) {
        if (element.localName() == "index-title" && element.namespaceURI() == KoXmlNS::text) {
            handleInsideElementsTag(element, bodyWriter, styles, imagesSrcList,
                                    linksInfo, footNotes, endNotes);
        }// end of index-title
        else
            handleTagTableOfContentBody(element, bodyWriter, styles, imagesSrcList,
                                        linksInfo, footNotes, endNotes);
    }
}

void handleTagTableOfContentBody(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                                 QHash<QString, StyleInfo *> &styles,QHash<QString, QSizeF> &imagesSrcList,
                                 QHash<QString, QString> linksInfo, QHash<QString, KoXmlElement> &footNotes,
                                 QHash<QString, KoXmlElement> &endNotes)
{
    if (nodeElement.localName() == "p" && nodeElement.namespaceURI() == KoXmlNS::text) {
        handleTagP(nodeElement, bodyWriter, styles, imagesSrcList,
                                linksInfo, footNotes, endNotes);
    }
}

void handleTagLineBreak(KoXmlWriter *bodyWriter)
{
    bodyWriter->startElement("br");
    bodyWriter->endElement();
}

void handleTagBookMark(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter)
{
    QString anchor = nodeElement.attribute("name");
    // This is haed codevalidator gets error for characters "|" and spaces
    // FIXME : we should handle ids better after move file to class
    anchor = anchor.remove("|");
    anchor = anchor.remove(" ");//remove spaces
    bodyWriter->startElement("a");
    bodyWriter->addAttribute("id", anchor);
}

void handleTagBookMarkStart(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter)
{
    QString anchor = nodeElement.attribute("name");
    bodyWriter->startElement("a");
    bodyWriter->addAttribute("id", anchor);
}

void handleTagBookMarkEnd(KoXmlWriter *bodyWriter)
{
    bodyWriter->endElement();
}

void handleUnknownTags(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                       QHash<QString, StyleInfo *> &styles,
                       QHash<QString, QSizeF> &imagesSrcList, QHash<QString, QString> linksInfo,
                       QHash<QString, KoXmlElement> &footNotes, QHash<QString, KoXmlElement> &endNotes)
{
    //just go dipper to find known tags
    handleInsideElementsTag(nodeElement, bodyWriter, styles, imagesSrcList,
                            linksInfo, footNotes, endNotes);
}


void handleTagNote(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                   QHash<QString, KoXmlElement> &footNotes, QHash<QString, KoXmlElement> &endNotes)
{
    QString noteClass = nodeElement.attribute("note-class");
    if (noteClass != "footnote" && noteClass != "endnote") {
        return;
    }

    QString id = nodeElement.attribute("id");
    KoXmlElement noteElements;
    forEachElement(noteElements, nodeElement) {
        if (noteElements.localName() == "note-citation" && noteElements.namespaceURI() == KoXmlNS::text) {
            bodyWriter->startElement("sup");

            bodyWriter->startElement("a");
            if (noteClass == "footnote")
                bodyWriter->addAttribute("href", "#" + id + "n"); // n rerence to note foot-note or end-note
            else { // endnote
                QString endRef = "chapter-endnotes.xhtml#" + id + "n";
                bodyWriter->addAttribute("href", endRef);
            }
            bodyWriter->addAttribute("id", id + "t"); // t is for text
            bodyWriter->addTextNode(noteElements.text());
            bodyWriter->endElement();

            bodyWriter->endElement();
        }
        else if (noteElements.localName() == "note-body" && noteElements.namespaceURI() == KoXmlNS::text) {
            if (noteClass == "footnote")
                footNotes.insert(id, noteElements);
            else {
                QString noteChpater = "chapter" + QString::number(currentChapter) + ".xhtml";
                endNotes.insert(noteChpater + "/" + id, noteElements);
                // we insert this: currentChapter/id
                // to can add refrence for text in end note
            }
        }
    }
}

void handleInsideElementsTag(KoXmlElement &nodeElement, KoXmlWriter *bodyWriter,
                             QHash<QString, StyleInfo *> &styles, QHash<QString, QSizeF> &imagesSrcList,
                             QHash<QString, QString> linksInfo, QHash<QString, KoXmlElement> &footNotes,
                             QHash<QString, KoXmlElement> &endNotes)
{
    KoXmlNode node = nodeElement.firstChild();
    KoXmlElement element = node.toElement();

    // i should end tag "a" after text for bookmark-start we can handle it
    // with tag bookmark-end but for bookmark there is no bookmark-end
    // so i use this flag to close the tag "a" that i have opened it in
    // handleTagBookMark()
    bool bookMarkFlag = false;
    // We have characterData or image or span or s  or soft-page break in a tag p
    // FIXME: we should add if there are more tags.
    while (!node.isNull()) {

        if (node.isText()) {
            handleCharacterData(node, bodyWriter, styles);
            if (bookMarkFlag) {
                bodyWriter->endElement(); // end tag "a"
                bookMarkFlag = false;
            }
        }
        else if (element.localName() == "p" && element.namespaceURI() == KoXmlNS::text) {
            handleTagP(element, bodyWriter, styles, imagesSrcList, linksInfo, footNotes, endNotes);
        }
        else if (element.localName() == "h" && element.namespaceURI() == KoXmlNS::text) {
            handleTagH(element, bodyWriter, styles, imagesSrcList, linksInfo, footNotes, endNotes);
        }
        else if (element.localName() == "table" && element.namespaceURI() == KoXmlNS::table) {
            handleTagH(element, bodyWriter, styles, imagesSrcList, linksInfo, footNotes, endNotes);
        }
        else if (element.localName() == "span" && element.namespaceURI() == KoXmlNS::text) {
            handleTagSpan(element, bodyWriter, styles, imagesSrcList, linksInfo, footNotes, endNotes);
        }
        else if (element.localName() == "frame" && element.namespaceURI() == KoXmlNS::draw) {
            handleTagFrame(element, bodyWriter, styles, imagesSrcList);
        }
        else if (nodeElement.localName() == "list" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagList(nodeElement, bodyWriter, styles, imagesSrcList, linksInfo, footNotes, endNotes);
        }
        else if (element.localName() == "soft-page-break" && element.namespaceURI() == KoXmlNS::text) {
            handleTagPageBreak(element, bodyWriter, styles);
        }
        else if (element.localName() == "a" && element.namespaceURI() == KoXmlNS::text) {
            handleTagA(element, bodyWriter, styles, imagesSrcList, linksInfo, footNotes, endNotes);
        }
        else if (element.localName() == "s" && element.namespaceURI() == KoXmlNS::text) {
            bodyWriter->addTextNode("\u00a0");
        }
        else if (element.localName() == "line-break" && element.namespaceURI() == KoXmlNS::text) {
            handleTagLineBreak(bodyWriter);
        }
        else if (element.localName() == "tab" && element.namespaceURI() == KoXmlNS::text) {
            handleTagTab(bodyWriter);
        }
        else if (element.localName() == "bookmark" && element.namespaceURI() == KoXmlNS::text) {
            handleTagBookMark(element, bodyWriter);
            bookMarkFlag = true;
        }
        else if (element.localName() == "bookmark-start" && element.namespaceURI() == KoXmlNS::text) {
            handleTagBookMarkStart(element, bodyWriter);
        }
        else if (element.localName() == "bookmark-end" && element.namespaceURI() == KoXmlNS::text) {
            handleTagBookMarkEnd(bodyWriter);
        }
        else if (element.localName() == "note" && element.namespaceURI() == KoXmlNS::text) {
            handleTagNote(element, bodyWriter,footNotes, endNotes);
        }
        else
            handleUnknownTags(element, bodyWriter, styles, imagesSrcList,
                              linksInfo, footNotes, endNotes);

        node = node.nextSibling();
        element = node.toElement();
    }
}

void collectInternalLinksInfo(KoXmlElement &currentElement, QHash<QString, StyleInfo *> &styles,
                              QHash<QString, QString> &linksInfo, int &chapter)
{
    KoXmlElement nodeElement;
    forEachElement (nodeElement, currentElement) {
        if ((nodeElement.localName() == "p" && nodeElement.namespaceURI() == KoXmlNS::text)
                || (nodeElement.localName() == "h" && nodeElement.namespaceURI() == KoXmlNS::text)) {

            // A break-before in the style means create a new chapter here,
            // but only if it is a top-level paragraph and not at the very first node.
            StyleInfo *style = styles.value(nodeElement.attribute("style-name"));
            if (style && style->hasBreakBefore) {
                chapter++;
            }
        }
        else if ((nodeElement.localName() == "bookmark-start" || nodeElement.localName() == "bookmark")
                  && nodeElement.namespaceURI() == KoXmlNS::text) {
            QString value = "chapter" + QString::number(chapter) + ".xhtml";
            QString key = "#" + nodeElement.attribute("name");
            linksInfo.insert(key, value);
            continue;
        }
        collectInternalLinksInfo(nodeElement, styles, linksInfo, chapter);
    }
}

void writeFootNotes(KoXmlWriter *bodyWriter, QHash<QString, StyleInfo *> &styles,
                    QHash<QString, QSizeF> &imagesSrcList, QHash<QString, QString> linksInfo,
                    QHash<QString, KoXmlElement> &footNotes,
                    QHash<QString, KoXmlElement> &endNotes)
{
    bodyWriter->startElement("p");
    handleTagLineBreak(bodyWriter);
    bodyWriter->addTextNode("___________________________________________");
    bodyWriter->endElement();

    bodyWriter->startElement("ul");
    int noteCounts = 1;
    foreach(QString id, footNotes.keys()) {
        bodyWriter->startElement("li");
        bodyWriter->addAttribute("id", id + "n");

        bodyWriter->startElement("a");
        bodyWriter->addAttribute("href","#" + id + "t"); // refrence to text
        bodyWriter->addTextNode("["+QString::number(noteCounts)+"]");
        bodyWriter->endElement();

        KoXmlElement bodyElement = footNotes.value(id);
        handleInsideElementsTag(bodyElement, bodyWriter, styles, imagesSrcList,
                                linksInfo, footNotes, endNotes);

        bodyWriter->endElement();
        noteCounts++;
    }
    bodyWriter->endElement();
    footNotes.clear(); // clear for next chapter
}

void writeEndNotes(KoXmlWriter *bodyWriter, QHash<QString, StyleInfo *> &styles,
                   QHash<QString, QSizeF> &imagesSrcList, QHash<QString, QString> linksInfo,
                   QHash<QString, KoXmlElement> &footNotes, QHash<QString, KoXmlElement> &endNotes)
{
    bodyWriter->startElement("h1");
    bodyWriter->addTextNode("End Notes");
    handleTagLineBreak(bodyWriter);
    bodyWriter->endElement();

    bodyWriter->startElement("ul");
    int noteCounts = 1;
    foreach(QString id, endNotes.keys()) {
        bodyWriter->startElement("li");
        bodyWriter->addAttribute("id", id.section("/", 1) + "n");

        bodyWriter->startElement("a");
        // id = chapter-endnotes.xhtml/endnoteId
        bodyWriter->addAttribute("href",id.section("/", 0, 0) + "#" + id.section("/", 1) + "t");
        bodyWriter->addTextNode("["+QString::number(noteCounts)+"]");
        bodyWriter->endElement();

        KoXmlElement bodyElement = endNotes.value(id);
        handleInsideElementsTag(bodyElement, bodyWriter, styles, imagesSrcList,
                                linksInfo, footNotes, endNotes);

        bodyWriter->endElement();
        noteCounts++;
    }
    bodyWriter->endElement();
    footNotes.clear(); // clear for next chapter
}
