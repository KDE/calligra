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
#include "OdtMobiHtmlConverter.h"

// Qt
#include <QStringList>
#include <QBuffer>

// KF5
#include <klocalizedstring.h>

// Calligra
#include <KoStore.h>
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <KoXmlNS.h>

// EPUB filter
#include "FileCollector.h"
#include "MobiExportDebug.h"


// ================================================================
//                         Style parsing


StyleInfo::StyleInfo()
    : isDefaultStyle(false)
    , defaultOutlineLevel(-1)
    , shouldBreakChapter(false)
    , inUse(false)
{
}


OdtMobiHtmlConverter::OdtMobiHtmlConverter()
    : m_currentChapter(1),
      m_optionsTag(false),
      m_boldTag(false),
      m_underlineTag(false),
      m_italicTag(false),
      m_spanTag(false),
      m_fontColorTag(false)
{
    qDeleteAll(m_styles);
}

OdtMobiHtmlConverter::~OdtMobiHtmlConverter()

{
}

// ================================================================
//                         HTML conversion


const OdtMobiHtmlConverter::ConversionOptions defaultOptions = {
    true,                       // Put styles into styles.css
    true,                        // Do break the output into chapters
    false                       // It doesn't use Mobi convention
};


KoFilter::ConversionStatus
OdtMobiHtmlConverter::convertContent(KoStore *odfStore,
                                 QHash<QString, QString> &metaData,
                                 OdtMobiHtmlConverter::ConversionOptions *options,
                                 FileCollector *collector,
                                 // Out parameters:
                                 QHash<QString, QSizeF> &images)
{
    if (options)
        m_options = options;
    else
        m_options = &defaultOptions;
    m_collector = collector;

    m_doIndent = !m_options->useMobiConventions;
    m_imgIndex = 1;

    // 1. Parse styles

    KoFilter::ConversionStatus  status = collectStyles(odfStore, m_styles);
    if (status != KoFilter::OK) {
        return status;
    }

#if 0 // Debug
    debugMobi << "======== >> Styles";
    foreach(const QString &name, m_styles.keys()) {
        debugMobi << "==" << name << ":\t"
                      << m_styles.value(name)->parent
                      << m_styles.value(name)->family
                      << m_styles.value(name)->isDefaultStyle
                      << m_styles.value(name)->shouldBreakChapter
                      << m_styles.value(name)->attributes
            ;
    }
    debugMobi << "======== << Styles";
#endif

    // Propagate style inheritance.
    fixStyleTree(m_styles);

#if 0
     //2. Create CSS contents and store it in the file collector.
        status = createCSS(m_styles, m_cssContent);
        //debugMobi << "Styles:" << m_styles;
        //debugMobi << "CSS:" << m_cssContent;
        if (status != KoFilter::OK) {
            delete odfStore;
            return status;
        }
        if (m_options->stylesInCssFile) {
            m_collector->addContentFile("stylesheet",
                                        m_collector->pathPrefix() + "styles.css",
                                        "text/css", m_cssContent);
        }
#endif

    // ----------------------------------------------------------------
    // Parse body from content.xml

    if (!odfStore->open("content.xml")) {
        debugMobi << "Can not open content.xml .";
        return KoFilter::FileNotFound;
    }

    KoXmlDocument doc;
    QString errorMsg;
    int errorLine;
    int errorColumn;
    if (!doc.setContent(odfStore->device(), true, &errorMsg, &errorLine, &errorColumn)) {
        debugMobi << "Error occurred while parsing content.xml "
                      << errorMsg << " in Line: " << errorLine
                      << " Column: " << errorColumn;
        odfStore->close();
        return KoFilter::ParsingError;
    }

    KoXmlNode currentNode = doc.documentElement();
    KoXmlElement nodeElement;    // currentNode as Element

    currentNode = KoXml::namedItemNS(currentNode, KoXmlNS::office, "body");
    currentNode = KoXml::namedItemNS(currentNode, KoXmlNS::office, "text");

    // 3. Collect information about internal links.
    KoXmlElement element = currentNode.toElement(); // node for passing it to collectInter...()
    int chapter = 1; // Only necessary for the recursion.
    collectInternalLinksInfo(element, chapter);

    // 4. Start the actual conversion.

    // Write the beginning of the output.
    beginHtmlFile(metaData);

    QString currentChapterTitle;

    m_currentChapter = 1;       // Number of current output chapter.
    forEachElement (nodeElement, currentNode) {

        // text:h and text:p are treated special since they can have
        // styling that makes us start on a new html file,
        // a.k.a. chapter.
        if (nodeElement.namespaceURI() == KoXmlNS::text && (nodeElement.localName() == "p"
                                                            || nodeElement.localName() == "h")) {

#if 0
            // Check if this paragraph should break the text into a new chapter.
            //
            // This should happen either if the paragraph has
            // outline-level = 1 or if the style indicates that the
            // break should happen. The styles come into this function
            // preprocessed.
            //
            // Only create a new chapter if it is a top-level
            // paragraph and not at the very first node.
            //
            StyleInfo *style = m_styles.value(nodeElement.attribute("style-name"));
            bool  hasOutlineLevel1 = (nodeElement.attribute("outline-level") == "1"
                                      || (nodeElement.attribute("outline-level").isEmpty()
                                          && style && style->defaultOutlineLevel == 1));
            if (m_options->doBreakIntoChapters
                    && (hasOutlineLevel1 || (style && style->shouldBreakChapter)))
            {
                //debugMobi << "Found paragraph which breaks into new chapter";

                // Write out any footnotes
                if (!m_footNotes.isEmpty()) {
                    writeFootNotes(m_htmlWriter);
                }

                // And finally close all tags.
                endHtmlFile();

                // Write the result to the file collector object.
                QString fileId = m_collector->filePrefix() + QString::number(m_currentChapter);
                QString fileName = m_collector->pathPrefix() + fileId + ".xhtml";
                m_collector->addContentFile(fileId, fileName,
                                            "application/xhtml+xml", m_htmlContent, currentChapterTitle);


                if (nodeElement.localName() == "h") {
                    currentChapterTitle = nodeElement.text();
                } else {
                    currentChapterTitle = "";
                }

                // And begin a new chapter.
                beginHtmlFile(metaData);
                m_currentChapter++;
            }
#endif
            // Actually handle the contents.
            if (nodeElement.localName() == "p") {
                handleTagP(nodeElement, m_htmlWriter);
                m_htmlWriter->startElement("br");
                m_htmlWriter->endElement();
            }
            else
                handleTagH(nodeElement, m_htmlWriter);
        }
        else if (nodeElement.localName() == "span" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagSpan(nodeElement, m_htmlWriter);
        }
        else if (nodeElement.localName() == "table" && nodeElement.namespaceURI() == KoXmlNS::table) {
            // Handle table
            handleTagTable(nodeElement, m_htmlWriter);
        }
        else if (nodeElement.localName() == "frame" && nodeElement.namespaceURI() == KoXmlNS::draw)  {
            // Handle frame
            m_htmlWriter->startElement("div", m_doIndent);
            handleTagFrame(nodeElement, m_htmlWriter);
            m_htmlWriter->endElement(); // end div
        }
        else if (nodeElement.localName() == "soft-page-break" &&
                 nodeElement.namespaceURI() == KoXmlNS::text) {

            handleTagPageBreak(nodeElement, m_htmlWriter);
        }
        else if (nodeElement.localName() == "list" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagList(nodeElement, m_htmlWriter);
        }
        else if (nodeElement.localName() == "a" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagA(nodeElement, m_htmlWriter);
        }
        else if (nodeElement.localName() == "table-of-content" &&
                 nodeElement.namespaceURI() == KoXmlNS::text) {

            handleTagTableOfContent(nodeElement, m_htmlWriter);
        }
        else if (nodeElement.localName() == "line-break" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagLineBreak(m_htmlWriter);
        }
        else {
            m_htmlWriter->startElement("div", m_doIndent);
            handleUnknownTags(nodeElement, m_htmlWriter);
            m_htmlWriter->endElement();
        }
    }

    // Write out any footnotes
    if (!m_footNotes.isEmpty()) {
        m_htmlWriter->startElement("mbp:pagebreak");
        m_htmlWriter->endElement();
        writeFootNotes(m_htmlWriter);
    }
    if (!m_endNotes.isEmpty()) {
        m_htmlWriter->startElement("mbp:pagebreak");
        m_htmlWriter->endElement();
        writeEndNotes(m_htmlWriter);
    }
    m_htmlWriter->startElement("mbp:pagebreak");
    m_htmlWriter->endElement();
    m_htmlWriter->startElement("mbp:pagebreak");
    m_htmlWriter->endElement();

    endHtmlFile();

    generateMobiInternalLinks();

    // Write output of the last file to the file collector object.
    QString fileId = m_collector->filePrefix();
    if (m_options->doBreakIntoChapters)
        fileId += QString::number(m_currentChapter);
    QString fileName = m_collector->pathPrefix() + fileId + ".xhtml";
    m_collector->addContentFile(fileId, fileName, "application/xhtml+xml", m_htmlContent, currentChapterTitle);

#if 0
    // 5. Write any data that we have collected on the way.

        // If we had end notes, make a new chapter for end notes
        if (!m_endNotes.isEmpty()) {

            // Write the beginning of the output for the next file.
            beginHtmlFile(metaData);
            writeEndNotes(m_htmlWriter);
            endHtmlFile();

            QString fileId = "chapter-endnotes";
            QString fileName = m_collector->pathPrefix() + fileId + ".xhtml";
            m_collector->addContentFile(fileId, fileName, "application/xhtml+xml", m_htmlContent, i18n("End notes"));
        }
#endif

    odfStore->close();

    // Return the list of images.
    images = m_images;

    return KoFilter::OK;
}

void OdtMobiHtmlConverter::beginHtmlFile(QHash<QString, QString> &metaData)
{
    m_htmlContent.clear();
    m_outBuf = new QBuffer(&m_htmlContent);
    m_htmlWriter = new KoXmlWriter(m_outBuf);

    m_htmlWriter->startElement("html", m_doIndent);
    //    if (!m_options->useMobiConventions)
    //        m_htmlWriter->addAttribute("xmlns", "http://www.w3.org/1999/xhtml");
    createHtmlHead(m_htmlWriter, metaData);
    m_htmlWriter->startElement("body", m_doIndent);

    // NOTE: At this point we have two open tags: <html> and <body>.
}

void OdtMobiHtmlConverter::endHtmlFile()
{
    // NOTE: At this point we have two open tags: <html> and <body>.

    // Close the two tags opened by beginHtmlFile().
    m_htmlWriter->endElement();
    m_htmlWriter->endElement();

    // Prepare for the next file.
    delete m_htmlWriter;
    delete m_outBuf;
}



void OdtMobiHtmlConverter::createHtmlHead(KoXmlWriter *writer, QHash<QString, QString> &/*metaData*/)
{
    writer->startElement("head", m_doIndent);
    writer->startElement("guide");
    writer->endElement();
    writer->endElement();
#if 0
        // We don't have title and meta tags in Mobi.
        if (!m_options->useMobiConventions) {
            writer->startElement("title", m_doIndent);
            writer->addTextNode(metaData.value("title"));
            writer->endElement(); // title

            writer->startElement("meta", m_doIndent);
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

                writer->startElement("meta", m_doIndent);
                writer->addAttribute("name", name);
                writer->addAttribute("content", metaData.value(name));
                writer->endElement(); // meta
            }
        }

        // Refer to the stylesheet or put the styles in the html file.
        if (m_options->stylesInCssFile) {
            writer->startElement("link", m_doIndent);
            writer->addAttribute("href", "styles.css");
            writer->addAttribute("type", "text/css");
            writer->addAttribute("rel", "stylesheet");
            writer->endElement(); // link
        }
        else {
            writer->startElement("style", m_doIndent);
            writer->addTextNode(m_cssContent);
            writer->endElement(); // style
        }

        writer->endElement(); // head
#endif
}


// ----------------------------------------------------------------
//                 Traversal of the XML contents


void OdtMobiHtmlConverter::handleTagTable(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter)
{
    if ( m_optionsTag) {
        closeFontOptionsElement(htmlWriter);
    }
    //QString styleName = nodeElement.attribute("style-name");
    //StyleInfo *styleInfo = m_styles.value(styleName);
    htmlWriter->startElement("table", m_doIndent);
    //    if (styleInfo) {
    //        styleInfo->inUse = true;
    //        htmlWriter->addAttribute("class", styleName);
    //    }
    htmlWriter->addAttribute("border", "1");

    //===== table-row ======
    KoXmlElement tableElement;
    forEachElement (tableElement, nodeElement) {
        if (tableElement.localName() != "table-column" && tableElement.namespaceURI() == KoXmlNS::table) {
            htmlWriter->startElement("tr", m_doIndent);

            // ===== table-cell ======
            KoXmlElement cellElement;
            forEachElement (cellElement, tableElement) {
                QString styleName = cellElement.attribute("style-name");
                StyleInfo *styleInfo = m_styles.value(styleName);
                Q_UNUSED(styleInfo);
                htmlWriter->startElement("td", m_doIndent);
//                if (styleInfo) {
//                    styleInfo->inUse = true;
//                    htmlWriter->addAttribute("class", styleName);
//                }

                // ==== cell text ====
                // FIXME: This is wrong. A cell element can contain
                //        the same tags as the full contents, not just
                //        what is inside a paragraph. (Beside, this
                //        function has a strange name.)
                handleInsideElementsTag(cellElement, htmlWriter);
                if ( m_optionsTag) {
                    closeFontOptionsElement(htmlWriter);
                }
                // ===================
                htmlWriter->endElement(); // td
            } // end for write tag cell

            htmlWriter->endElement(); // tr
        } // end while write tag row
    }

    htmlWriter->endElement();
}

void OdtMobiHtmlConverter::handleTagFrame(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter)
{

    if ( m_optionsTag) {
        closeFontOptionsElement(htmlWriter);
    }

    //QString styleName = nodeElement.attribute("style-name");
    //StyleInfo *styleInfo = m_styles.value(styleName);
    htmlWriter->startElement("img", m_doIndent);
    //    if (styleInfo) {
    //        styleInfo->inUse = true;
    //        htmlWriter->addAttribute("class", styleName);
    //    }

    // Find height and width
    QString height = nodeElement.attribute("height");
    QString width  = nodeElement.attribute("width");

    // Remove characters "in" or "pt" from their end.
    //
    // FIXME: This is WRONG!
    //        First, there is no way to tell if the unit is 2 chars
    //        Second, it is not sure that there *is* a unit.
    //        Instead, use some function in KoUnit that converts the size.  /IW
    height = height.left(height.length()-2);
    width  = width.left(width.length()-2);

    // Convert them to real.
    qreal qHeight = height.toFloat();
    qreal qWidth = width.toFloat();
    QSizeF size(qWidth, qHeight);

    // Check image tag to find image source
    KoXmlElement imgElement;
    forEachElement (imgElement, nodeElement) {
        if (imgElement.localName() == "image" && imgElement.namespaceURI() == KoXmlNS::draw) {
            QString imgSrc = imgElement.attribute("href").section('/', -1);

            if (m_options->useMobiConventions) {
                // Mobi
                // First check for repeated images.
                if (m_imagesIndex.contains(imgSrc)) {
                    htmlWriter->addAttribute("recindex", QString::number(m_imagesIndex.value(imgSrc)));
                }
                else {
                    htmlWriter->addAttribute("recindex", QString::number(m_imgIndex));
                    m_imagesIndex.insert(imgSrc, m_imgIndex);
                    m_imgIndex++;
                }
                htmlWriter->addAttribute("align", "baseline");
                htmlWriter->addAttribute("height", height);
                htmlWriter->addAttribute("width", width);
            }
            else {
                htmlWriter->addAttribute("src", m_collector->filePrefix() + imgSrc);
            }

            m_images.insert(imgElement.attribute("href"), size);
        }
    }
    htmlWriter->endElement(); // end img
}

void OdtMobiHtmlConverter::handleTagP(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter)
{
    if ( m_optionsTag) {
        closeFontOptionsElement(htmlWriter);
    }
    QString styleName = nodeElement.attribute("style-name");
    StyleInfo *styleInfo = m_styles.value(styleName);
    htmlWriter->startElement("p", m_doIndent);

    htmlWriter->addAttribute("height", "6pt");
    htmlWriter->addAttribute("width", "2em");
    if (styleInfo) {
        //        styleInfo->inUse = true;
        //                htmlWriter->addAttribute("class", styleName);
        if (styleInfo->attributes.value("text-align").isEmpty())
            htmlWriter->addAttribute("align", "left");
        else
            htmlWriter->addAttribute("align", styleInfo->attributes.value("text-align"));
        openFontOptionsElement(htmlWriter, styleInfo);
    }
    //closeFontOptionsElement(htmlWriter);
    handleInsideElementsTag(nodeElement, htmlWriter);
    if ( m_optionsTag) {
        closeFontOptionsElement(htmlWriter);
    }
    htmlWriter->endElement();
}

void OdtMobiHtmlConverter::handleCharacterData(KoXmlNode &node, KoXmlWriter *htmlWriter)
{
    KoXmlText charData = node.toText();
    htmlWriter->addTextNode(charData.data());
}

void OdtMobiHtmlConverter::handleTagSpan(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter)
{
    if ( m_optionsTag) {
        closeFontOptionsElement(htmlWriter);
    }
    QString styleName = nodeElement.attribute("style-name");
    StyleInfo *styleInfo = m_styles.value(styleName);
    if (styleInfo) {
        //        styleInfo->inUse = true;
        //        htmlWriter->addAttribute("class", styleName);
        openFontOptionsElement(htmlWriter, styleInfo);
    }
    handleInsideElementsTag(nodeElement, htmlWriter);
    if ( m_optionsTag) {
        closeFontOptionsElement(htmlWriter);
    }
    //htmlWriter->endElement(); // span
}

void OdtMobiHtmlConverter::handleTagPageBreak(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter)
{
    htmlWriter->startElement("mbp:pagebreak");
    htmlWriter->endElement();
    htmlWriter->addTextNode(nodeElement.text().toUtf8());
}

void OdtMobiHtmlConverter::handleTagH(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter)
{
    if ( m_optionsTag) {
        closeFontOptionsElement(htmlWriter);
    }

    QString styleName = nodeElement.attribute("style-name");
    StyleInfo *styleInfo = m_styles.value(styleName);
    htmlWriter->startElement("h1", m_doIndent);

    htmlWriter->addAttribute("height", "6pt");
    htmlWriter->addAttribute("width", "2em");
    if (styleInfo) {
     //   styleInfo->inUse = true;
     //        htmlWriter->addAttribute("class", styleName);
        if (styleInfo->attributes.value("text-align").isEmpty())
            htmlWriter->addAttribute("align", "left");
        else
            htmlWriter->addAttribute("align", styleInfo->attributes.value("text-align"));

        openFontOptionsElement(htmlWriter, styleInfo);
    }
    handleInsideElementsTag(nodeElement, htmlWriter);

    if ( m_optionsTag) {
        closeFontOptionsElement(htmlWriter);
    }
    htmlWriter->endElement();
}

void OdtMobiHtmlConverter::handleTagList(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter)
{
    if ( m_optionsTag) {
        closeFontOptionsElement(htmlWriter);
    }
//        QString styleName = nodeElement.attribute("style-name");
//        StyleInfo *styleInfo = m_styles.value(styleName);
        htmlWriter->startElement("ul", m_doIndent);
    //    if (styleInfo) {
    //        styleInfo->inUse = true;
    //        htmlWriter->addAttribute("class", styleName);
    //    }
    KoXmlElement listItem;
    forEachElement (listItem, nodeElement) {
        htmlWriter->startElement("li", m_doIndent);
        handleInsideElementsTag(listItem, htmlWriter);
        if ( m_optionsTag) {
            closeFontOptionsElement(htmlWriter);
        }
        htmlWriter->endElement();
    }
    if ( m_optionsTag) {
        closeFontOptionsElement(htmlWriter);
    }
    htmlWriter->endElement();
}

void OdtMobiHtmlConverter::handleTagA(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter)
{
    if ( m_optionsTag) {
        closeFontOptionsElement(htmlWriter);
    }
    htmlWriter->startElement("a", m_doIndent);
    QString reference = nodeElement.attribute("href");
    //    QString chapter = m_linksInfo.value(reference);
    QString mark = m_linksInfo.value(reference); // reference without # at its beginning.
    if (!mark.isEmpty() /*&& !m_options->stylesInCssFile*/) {
//        // This is internal link.
//        reference.remove('|');
//        reference.remove(' ');// remove spaces
//        reference = chapter+reference;
//        htmlWriter->addAttribute("href", reference);
        m_refrencesList.insert(htmlWriter->device()->pos(), mark);
    }
    else {
        // This is external link.
        htmlWriter->addAttribute("href", reference);
    }
    handleInsideElementsTag(nodeElement, htmlWriter);
    if ( m_optionsTag) {
        closeFontOptionsElement(htmlWriter);
    }
    htmlWriter->endElement();
}

void OdtMobiHtmlConverter::handleTagTab (KoXmlWriter */*htmlWriter*/)
{
//    for (int i = 0; i <10; ++i)
//        htmlWriter->addTextNode("\u00a0");
}

void OdtMobiHtmlConverter::handleTagTableOfContent(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter)
{
    KoXmlNode indexBody = KoXml::namedItemNS(nodeElement, KoXmlNS::text, "index-body");
    KoXmlElement  element;
    forEachElement (element, indexBody) {
        if (element.localName() == "index-title" && element.namespaceURI() == KoXmlNS::text) {
            handleInsideElementsTag(element, htmlWriter);
        }
        else
            handleTagTableOfContentBody(element, htmlWriter);
    }
}

void OdtMobiHtmlConverter::handleTagTableOfContentBody(KoXmlElement &nodeElement,
                                                   KoXmlWriter *htmlWriter)
{
    if (nodeElement.localName() == "p" && nodeElement.namespaceURI() == KoXmlNS::text) {
        handleTagP(nodeElement, htmlWriter);
    }
}

void OdtMobiHtmlConverter::handleTagLineBreak(KoXmlWriter *htmlWriter)
{
    htmlWriter->startElement("br", m_doIndent);
    htmlWriter->endElement();
}

void OdtMobiHtmlConverter::handleTagBookMark(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter)
{
    if ( m_optionsTag) {
        closeFontOptionsElement(htmlWriter);
    }
    QString anchor = nodeElement.attribute("name");
    // This is haed codevalidator gets error for characters "|" and spaces
    // FIXME : we should handle ids better after move file to class
//    anchor.remove('|');
//    anchor.remove(' ');//remove spaces
//    htmlWriter->startElement("a", m_doIndent);
//    htmlWriter->addAttribute("id", anchor);
    m_bookMarksList.insert(anchor, htmlWriter->device()->pos());
}

void OdtMobiHtmlConverter::handleTagBookMarkStart(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter)
{
    if ( m_optionsTag) {
        closeFontOptionsElement(htmlWriter);
    }

    QString anchor = nodeElement.attribute("name");
//    htmlWriter->startElement("a", m_doIndent);
//    htmlWriter->addAttribute("id", anchor);
    m_bookMarksList.insert(anchor, htmlWriter->device()->pos());
}

void OdtMobiHtmlConverter::handleTagBookMarkEnd(KoXmlWriter */*htmlWriter*/)
{
//    htmlWriter->endElement();
}

void OdtMobiHtmlConverter::handleUnknownTags(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter)
{
    // Just go deeper to find known tags.
    handleInsideElementsTag(nodeElement, htmlWriter);
}


void OdtMobiHtmlConverter::handleTagNote(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter)
{
    QString noteClass = nodeElement.attribute("note-class");
    if (noteClass != "footnote" && noteClass != "endnote") {
        return;
    }

    QString id = nodeElement.attribute("id");
    KoXmlElement noteElements;
    forEachElement(noteElements, nodeElement) {
        if (noteElements.localName() == "note-citation" && noteElements.namespaceURI() == KoXmlNS::text) {
            htmlWriter->startElement("sup", m_doIndent);

            htmlWriter->startElement("a", m_doIndent);
            m_refrencesList.insert(htmlWriter->device()->pos(), id);
//            if (noteClass == "footnote")
//                htmlWriter->addAttribute("href", "#" + id + "n"); // n reference to note foot-note or end-note
//            else { // endnote
//                QString endRef = "chapter-endnotes.xhtml#" + id + "n";
//                htmlWriter->addAttribute("href", endRef);
//            }
//            htmlWriter->addAttribute("id", id + "t"); // t is for text
            htmlWriter->addTextNode(noteElements.text());
            htmlWriter->endElement();

            htmlWriter->endElement();
        }
        else if (noteElements.localName() == "note-body" && noteElements.namespaceURI() == KoXmlNS::text) {
            if (noteClass == "footnote")
                m_footNotes.insert(id, noteElements);
            else {
                QString noteChapter = m_collector->filePrefix();
                if (m_options->doBreakIntoChapters)
                    noteChapter += QString::number(m_currentChapter);
//                m_endNotes.insert(noteChapter + "/" + id, noteElements);
                m_endNotes.insert(id, nodeElement);
                // we insert this: m_currentChapter/id
                // to can add reference for text in end note
            }
        }
    }
}

void OdtMobiHtmlConverter::handleInsideElementsTag(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter)
{
    KoXmlNode node = nodeElement.firstChild();
    KoXmlElement element = node.toElement();

    // We have characterData or image or span or s  or soft-page break in a tag p
    // FIXME: we should add if there are more tags.
    while (!node.isNull()) {

        if (node.isText()) {
            handleCharacterData(node, htmlWriter);
        }
        else if (element.localName() == "p" && element.namespaceURI() == KoXmlNS::text) {
            handleTagP(element, htmlWriter);
        }
        else if (element.localName() == "h" && element.namespaceURI() == KoXmlNS::text) {
            handleTagH(element, htmlWriter);
        }
        else if (element.localName() == "table" && element.namespaceURI() == KoXmlNS::table) {
            handleTagTable(element, htmlWriter);
        }
        else if (element.localName() == "span" && element.namespaceURI() == KoXmlNS::text) {
            handleTagSpan(element, htmlWriter);
        }
        else if (element.localName() == "frame" && element.namespaceURI() == KoXmlNS::draw) {
            handleTagFrame(element, htmlWriter);
        }
        else if (nodeElement.localName() == "list" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagList(nodeElement, htmlWriter);
        }
        else if (element.localName() == "soft-page-break" && element.namespaceURI() == KoXmlNS::text) {
            handleTagPageBreak(element, htmlWriter);
        }
        else if (element.localName() == "a" && element.namespaceURI() == KoXmlNS::text) {
            handleTagA(element, htmlWriter);
        }
        else if (element.localName() == "s" && element.namespaceURI() == KoXmlNS::text) {
//            htmlWriter->addTextNode("\u00a0");
        }
        else if (element.localName() == "line-break" && element.namespaceURI() == KoXmlNS::text) {
            handleTagLineBreak(htmlWriter);
        }
        else if (element.localName() == "tab" && element.namespaceURI() == KoXmlNS::text) {
            handleTagTab(htmlWriter);
        }
        else if (element.localName() == "bookmark" && element.namespaceURI() == KoXmlNS::text) {
            handleTagBookMark(element, htmlWriter);
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
        else {
            // FIXME: The same code in convertContent() inserts <div>
            //        around this call.
            handleUnknownTags(element, htmlWriter);
        }

        node = node.nextSibling();
        element = node.toElement();
    }
}


// ----------------------------------------------------------------


void OdtMobiHtmlConverter::collectInternalLinksInfo(KoXmlElement &currentElement, int &chapter)
{
    KoXmlElement nodeElement;
    forEachElement (nodeElement, currentElement) {
        if ( (nodeElement.localName() == "p" || nodeElement.localName() == "h")
             && nodeElement.namespaceURI() == KoXmlNS::text)
        {
            // A break-before in the style means create a new chapter here,
            // but only if it is a top-level paragraph and not at the very first node.
            StyleInfo *style = m_styles.value(nodeElement.attribute("style-name"));
            if (m_options->doBreakIntoChapters && style && style->shouldBreakChapter) {
                chapter++;
            }
        }
        else if ((nodeElement.localName() == "bookmark-start" || nodeElement.localName() == "bookmark")
                  && nodeElement.namespaceURI() == KoXmlNS::text) {
            QString key = "#" + nodeElement.attribute("name");
//            QString value = m_collector->filePrefix();
            QString value = nodeElement.attribute("name");
            if (m_options->doBreakIntoChapters)
                value += QString::number(chapter);
            //value += ".xhtml";
            m_linksInfo.insert(key, value);
            continue;
        }

        // Check for links recursively also inside this element.
        collectInternalLinksInfo(nodeElement, chapter);
    }
}

void OdtMobiHtmlConverter::writeFootNotes(KoXmlWriter *htmlWriter)
{
//    htmlWriter->startElement("p", m_doIndent);
//    handleTagLineBreak(htmlWriter);
//    htmlWriter->addTextNode("___________________________________________");
//    htmlWriter->endElement();

    htmlWriter->startElement("ul", m_doIndent);
    int noteCounts = 1;
    foreach(const QString &id, m_footNotes.keys()) {
        htmlWriter->startElement("li", m_doIndent);
//        htmlWriter->addAttribute("id", id + "n");
//        htmlWriter->startElement("a", m_doIndent);
//        htmlWriter->addAttribute("href", "#" + id + "t"); // reference to text
        m_bookMarksList.insert(id, htmlWriter->device()->pos());
        htmlWriter->addTextNode("[" + QString::number(noteCounts) + "]");
//        htmlWriter->endElement();

        KoXmlElement bodyElement = m_footNotes.value(id);
        handleInsideElementsTag(bodyElement, htmlWriter);

        htmlWriter->endElement();
        noteCounts++;
    }
    htmlWriter->endElement();
    m_footNotes.clear(); // clear for next chapter
}

void OdtMobiHtmlConverter::writeEndNotes(KoXmlWriter *htmlWriter)
{
    htmlWriter->startElement("h1", m_doIndent);
    htmlWriter->addTextNode("End Notes");
    handleTagLineBreak(htmlWriter);
    htmlWriter->endElement();

    htmlWriter->startElement("ul", m_doIndent);
    int noteCounts = 1;
    foreach(const QString &id, m_endNotes.keys()) {
        htmlWriter->startElement("li", m_doIndent);
//        htmlWriter->addAttribute("id", id.section("/", 1) + "n");

//        htmlWriter->startElement("a", m_doIndent);
//        // id = chapter-endnotes.xhtml/endnoteId
//        htmlWriter->addAttribute("href",id.section("/", 0, 0) + "#" + id.section("/", 1) + "t");
        m_bookMarksList.insert(id, htmlWriter->device()->pos());
        htmlWriter->addTextNode("["+QString::number(noteCounts)+"]");
//        htmlWriter->endElement();
        KoXmlElement bodyElement = m_endNotes.value(id);
        handleInsideElementsTag(bodyElement, htmlWriter);

        htmlWriter->endElement();
        noteCounts++;
    }
    htmlWriter->endElement();
}


// ================================================================
//                         Style handling


KoFilter::ConversionStatus OdtMobiHtmlConverter::collectStyles(KoStore *odfStore,
                                                           QHash<QString, StyleInfo*> &styles)
{
    KoXmlDocument doc;
    QString errorMsg;
    int errorLine;
    int errorColumn;

    // ----------------------------------------------------------------
    // Get style info from content.xml.

    // Try to open content.xml. Return if it failed.
    //debugMobi << "parse content.xml styles";
    if (!odfStore->open("content.xml")) {
        errorMobi << "Unable to open input file! content.xml" << endl;
        return KoFilter::FileNotFound;
    }

    if (!doc.setContent(odfStore->device(), true, &errorMsg, &errorLine, &errorColumn)) {
        debugMobi << "Error occurred while parsing styles.xml "
                 << errorMsg << " in Line: " << errorLine
                 << " Column: " << errorColumn;
        odfStore->close();
        return KoFilter::ParsingError;
    }

    // Get the xml node that contains the styles.
    KoXmlNode stylesNode = doc.documentElement();
    stylesNode = KoXml::namedItemNS(stylesNode, KoXmlNS::office, "automatic-styles");

    // Collect info about the styles.
    collectStyleSet(stylesNode, styles);

    odfStore->close(); // end of parsing styles in content.xml

    // ----------------------------------------------------------------
    // Get style info from styles.xml.

    // Try to open and set styles.xml as a KoXmlDocument. Return if it failed.
    if (!odfStore->open("styles.xml")) {
        errorMobi << "Unable to open input file! style.xml" << endl;
        return KoFilter::FileNotFound;
    }
    if (!doc.setContent(odfStore->device(), true, &errorMsg, &errorLine, &errorColumn)) {
        debugMobi << "Error occurred while parsing styles.xml "
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
    collectStyleSet(stylesNode, styles);

    odfStore->close();
    return KoFilter::OK;
}

void OdtMobiHtmlConverter::collectStyleSet(KoXmlNode &stylesNode, QHash<QString, StyleInfo*> &styles)
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
        // NOTE: This only makes sense when we convert to HTML.
        if (styleElement.attribute("family") == "graphic") {
            styleInfo->attributes.insert("max-height", "99%");
            styleInfo->attributes.insert("max-width", "99%");
            styleInfo->attributes.insert("height", "auto");
            styleInfo->attributes.insert("width", "auto");
        }

        // Collect default outline level separately because it's used
        // to determine chapter breaks.
        QString dummy = styleElement.attribute("default-outline-level");
        bool  ok;
        styleInfo->defaultOutlineLevel = dummy.toInt(&ok);
        if (!ok)
            styleInfo->defaultOutlineLevel = -1;

        // Go through all property lists (like text-properties,
        // paragraph-properties, etc) and collect the relevant
        // attributes from them.
        styleInfo->shouldBreakChapter = false;
        KoXmlElement propertiesElement;
        forEachElement (propertiesElement, styleElement) {
#if 0 // Disable - use outline-level = 1 instead.
            // Check for fo:break-before
            if (propertiesElement.attribute("break-before") == "page") {
                //debugMobi << "Found break-before=page in style" << styleName;
                styleInfo->shouldBreakChapter = true;
            }
#endif

            // Collect general formatting attributes that we can
            // translate to CSS.
            collectStyleAttributes(propertiesElement, styleInfo);
        }

#if 0 // debug
        debugMobi << "==" << styleName << ":\t"
                      << styleInfo->parent
                      << styleInfo->family
                      << styleInfo->isDefaultStyle
                      << styleInfo->shouldBreakChapter
                      << styleInfo->attributes;
#endif
        styles.insert(styleName, styleInfo);
    }
}

void OdtMobiHtmlConverter::collectStyleAttributes(KoXmlElement &propertiesElement, StyleInfo *styleInfo)
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
        //debugMobi << "horizontal pos attribute" << attribute;
        if (attribute == "right" || attribute == "from-left") {
            styleInfo->attributes.insert("float", "right");
            styleInfo->attributes.insert("margin", "5px 0 5px 15px");
        }
        // Center doesn't show very well.
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

void OdtMobiHtmlConverter::fixStyleTree(QHash<QString, StyleInfo*> &styles)
{
    // For all styles:
    //    Propagate the shouldBreakChapter bool upwards in the inheritance tree.
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
            if (style->shouldBreakChapter || style->parent.isEmpty()) {
                break;
            }

            style = styles[style->parent];
        }

        // If the bottom most has a break, then all the ones in the list should inherit it.
        if (styleStack[index - 1]->shouldBreakChapter) {
            for (int i = 0; i < index - 1; ++i) {
                styleStack[i]->shouldBreakChapter = true;
            }
        }
    }
}



KoFilter::ConversionStatus OdtMobiHtmlConverter::createCSS(QHash<QString, StyleInfo*> &styles,
                                                       QByteArray &cssContent)
{
    // There is no equivalent to the ODF style inheritance using
    // parent-style-name in CSS. This means that to simulate the same
    // behaviour we have to "flatten" the style tree, i.e. we have to
    // transfer all the inherited attributes from a style's parent
    // into itself.
    flattenStyles(styles);

    QByteArray begin("{\n");
    QByteArray end("}\n");
    foreach (const QString &styleName, styles.keys()) {
        QByteArray head;
        QByteArray attributeList;

        StyleInfo *styleInfo = styles.value(styleName);
        // Disable the test for inUse since we moved the call to before the traversal of the content.
        if (!styleInfo/* || !styleInfo->inUse*/)
            continue;

        // The style name
        head = QString('.' + styleName).toUtf8();
        cssContent.append(head);
        cssContent.append(begin);

        foreach (const QString &propName, styleInfo->attributes.keys()) {
            attributeList += QString(propName + ':' + styleInfo->attributes.value(propName)).toUtf8() + ";\n";
        }

        cssContent.append(attributeList);
        cssContent.append(end);
    }

    return KoFilter::OK;
}

void OdtMobiHtmlConverter::flattenStyles(QHash<QString, StyleInfo*> &styles)
{
    QSet<QString> doneStyles;
    foreach (const QString &styleName, styles.keys()) {
        if (!doneStyles.contains(styleName)) {
            flattenStyle(styleName, styles, doneStyles);
        }
    }
}

void OdtMobiHtmlConverter::flattenStyle(const QString &styleName, QHash<QString, StyleInfo*> &styles,
                                    QSet<QString> &doneStyles)
{
    StyleInfo *styleInfo = styles.value(styleName);
    if (!styleInfo) {
        return;
    }

    // FIXME: Should we also handle styleInfo->defaultOutlineLevel and
    //        styleInfo->shouldBreakChapter?

    QString parentName = styleInfo->parent;
    if (parentName.isEmpty())
        return;

    flattenStyle(styleInfo->parent, styles, doneStyles);

    // Copy all attributes from the parent that is not already in
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

void OdtMobiHtmlConverter::openFontOptionsElement(KoXmlWriter *htmlWriter, StyleInfo *styleInfo)
{


    if (styleInfo->attributes.value("text-decoration") == "underline") {
        htmlWriter->startElement("u");
        m_underlineTag = true;
    }
    if (styleInfo->attributes.value("font-style") == "italic") {
        htmlWriter->startElement("i");
        m_italicTag = true;
    }
    if (styleInfo->attributes.value("font-weight") == "bold") {
        htmlWriter->startElement("font");
        htmlWriter->addAttribute("size", "4"); // When text is bold.
        htmlWriter->startElement("b");
        m_boldTag = true;
    }

    if (!styleInfo->attributes.value("background-color").isEmpty()) {
        htmlWriter->startElement("span");
        htmlWriter->addAttribute("bgcolor", styleInfo->attributes.value("background-color"));
        m_spanTag = true;
    }

    if (!styleInfo->attributes.value("color").isEmpty()) {
        htmlWriter->startElement("font");
        htmlWriter->addAttribute("color", styleInfo->attributes.value("color"));
        m_fontColorTag = true;
    }
    else if (m_spanTag) {
        htmlWriter->startElement("font");
        htmlWriter->addAttribute("color", "#000");
        m_fontColorTag = true;
    }


    m_optionsTag = true;
}

void OdtMobiHtmlConverter::closeFontOptionsElement(KoXmlWriter *htmlWriter)
{
    if (m_fontColorTag) {
        htmlWriter->endElement();
        m_fontColorTag = false;
    }

    if (m_spanTag) {
        htmlWriter->endElement();
        m_spanTag = false;
    }
    //htmlWriter->endElement(); // end font element
    if (m_boldTag) {
        htmlWriter->endElement(); // end tag b
        htmlWriter->endElement(); // end font tag
        m_boldTag = false;
    }
    if (m_italicTag) {
        htmlWriter->endElement(); // end tag i
        m_italicTag = false;
    }
    if (m_underlineTag) {
        htmlWriter->endElement(); // end tag u
        m_underlineTag = false;
    }

    m_optionsTag = false;
}

void OdtMobiHtmlConverter::generateMobiInternalLinks()
{
    if (m_linksInfo.isEmpty()) {
        debugMobi << "### There is no internal links. ###";
        return;
    }

    // For each reference that we add to file, should update bookmark position.
    foreach (qint64 refPosition, m_refrencesList.keys()) {
        foreach (const QString &id, m_bookMarksList.keys()) {
            if (m_bookMarksList.value(id) > refPosition) {
                qint64 newPos = (qint64)11 + (qint64)QString::number(m_bookMarksList.value(id)).size()
                        + m_bookMarksList.value(id) + 1;
                m_bookMarksList.insert(id, newPos);
            }
        }
    }

    QByteArray quto = "\"";
    qint64 move = 0;
    foreach (qint64 refPosition, m_refrencesList.keys()) {
        QByteArray attribute = " filepos=";

        QString id = m_refrencesList.value(refPosition);

        refPosition = refPosition + move;

        qint64 markPosition = m_bookMarksList.value(id);
        QByteArray pos = QString::number(markPosition).toUtf8();

        attribute = attribute + quto + pos + quto;

        m_htmlContent.insert(refPosition, attribute);

        move = move + (qint64)11 + (qint64)QString::number(markPosition).size();
    }

}
