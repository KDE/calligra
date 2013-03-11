/* This file is part of the KDE project

   Copyright (C) 2012-2013 Inge Wallin            <inge@lysator.liu.se>
   Copyright (C) 2012      Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

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
   Boston, MA 02110-1301, USA.
*/


// Own
#include "OdtTraverser.h"

// Qt
#include <QStringList>
#include <QBuffer>

// KDE
#include <kdebug.h>
#include <klocalizedstring.h>

// Calligra
#include <KoStore.h>
#include <KoXmlReader.h>
#include <KoXmlNS.h>
#include <KoOdfReadStore.h>

// Traverser
#include "OdtTraverserBackend.h"


OdtTraverser::OdtTraverser()
{
}

OdtTraverser::~OdtTraverser()

{
}


bool OdtTraverser::traverseContent(OdtTraverserContext *context,
                                   OdtTraverserBackend *backend)
                                   
{
    m_backend = backend;
    m_context = context;

    if (context->analyzeOdfFile() != KoFilter::OK) {
        return false;
    }

#if 0 // Debug
    kDebug(30503) << "======== >> Styles";
    foreach(const QString &name, context->styles().keys()) {
        kDebug(30503) << "==" << name << ":\t"
                      << m_styles.value(name)->parent
                      << m_styles.value(name)->family
                      << m_styles.value(name)->isDefaultStyle
                      << m_styles.value(name)->shouldBreakChapter
                      << m_styles.value(name)->attributes
            ;
    }
    kDebug(30503) << "======== << Styles";
#endif

    // ----------------------------------------------------------------
    // Parse body from content.xml

    KoStore *odfStore = m_context->odfStore();

    if (!odfStore->open("content.xml")) {
        kDebug(30503) << "Can not open content.xml .";
        return false;
    }

    KoXmlDocument doc;
    QString errorMsg;
    int errorLine;
    int errorColumn;
    if (!doc.setContent(odfStore->device(), true, &errorMsg, &errorLine, &errorColumn)) {
        kDebug(30503) << "Error occurred while parsing content.xml "
                      << errorMsg << " in Line: " << errorLine
                      << " Column: " << errorColumn;
        odfStore->close();
        return false;
    }

    KoXmlNode currentNode = doc.documentElement();
    currentNode = KoXml::namedItemNS(currentNode, KoXmlNS::office, "body");
    currentNode = KoXml::namedItemNS(currentNode, KoXmlNS::office, "text");

    // Start the actual conversion.
    m_backend->beginTraversal(m_context);

    // KoXmlElement element = currentNode.toElement();
    KoXmlElement nodeElement;    // currentNode as Element
    forEachElement (nodeElement, currentNode) {

        if (nodeElement.localName() == "p" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagP(nodeElement);
        }
        else if (nodeElement.localName() == "h" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagH(nodeElement);
        }
        else if (nodeElement.localName() == "span" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagSpan(nodeElement);
        }
        else if (nodeElement.localName() == "table" && nodeElement.namespaceURI() == KoXmlNS::table) {
            // Handle table
            handleTagTable(nodeElement);
        }
        else if (nodeElement.localName() == "frame" && nodeElement.namespaceURI() == KoXmlNS::draw)  {
            handleTagFrame(nodeElement);
        }
        else if (nodeElement.localName() == "soft-page-break" &&
                 nodeElement.namespaceURI() == KoXmlNS::text) {

            handleTagPageBreak(nodeElement);
        }
        else if (nodeElement.localName() == "list" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagList(nodeElement);
        }
        else if (nodeElement.localName() == "a" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagA(nodeElement);
        }
        else if (nodeElement.localName() == "table-of-content" &&
                 nodeElement.namespaceURI() == KoXmlNS::text) {

            handleTagTableOfContent(nodeElement);
        }
        else if (nodeElement.localName() == "line-break" && nodeElement.namespaceURI() == KoXmlNS::text) {
            handleTagLineBreak(nodeElement);
        }
        else {
            handleUnknownTags(nodeElement);
        }
    }

    m_backend->endTraversal(m_context);
    odfStore->close();

    return true;
}


// ----------------------------------------------------------------
//                 Traversal of the XML contents


void OdtTraverser::handleInsideElementsTag(KoXmlElement &nodeElement)
{
    KoXmlNode node = nodeElement.firstChild();
    KoXmlElement element = node.toElement();

    m_backend->beginInsideElementsTag(element, m_context);

    // We have characterData or image or span or s or soft-page break in a tag p
    // FIXME: we should add if there are more tags.
    while (!node.isNull()) {

        if (node.isText()) {
            handleCharacterData(node);
        }
        else if (element.localName() == "p" && element.namespaceURI() == KoXmlNS::text) {
            handleTagP(element);
        }
        else if (element.localName() == "h" && element.namespaceURI() == KoXmlNS::text) {
            handleTagH(element);
        }
        else if (element.localName() == "table" && element.namespaceURI() == KoXmlNS::table) {
            handleTagTable(element);
        }
        else if (element.localName() == "span" && element.namespaceURI() == KoXmlNS::text) {
            handleTagSpan(element);
        }
        else if (element.localName() == "frame" && element.namespaceURI() == KoXmlNS::draw) {
            handleTagFrame(element);
        }
        else if (element.localName() == "list" && element.namespaceURI() == KoXmlNS::text) {
            handleTagList(element);
        }
        else if (element.localName() == "soft-page-break" && element.namespaceURI() == KoXmlNS::text) {
            handleTagPageBreak(element);
        }
        else if (element.localName() == "a" && element.namespaceURI() == KoXmlNS::text) {
            handleTagA(element);
        }
        else if (element.localName() == "s" && element.namespaceURI() == KoXmlNS::text) {
            handleTagS(element);
        }
        else if (element.localName() == "line-break" && element.namespaceURI() == KoXmlNS::text) {
            handleTagLineBreak(element);
        }
        else if (element.localName() == "tab" && element.namespaceURI() == KoXmlNS::text) {
            handleTagTab(element);
        }
        else if (element.localName() == "bookmark" && element.namespaceURI() == KoXmlNS::text) {
            handleTagBookMark(element);
        }
        else if (element.localName() == "bookmark-start" && element.namespaceURI() == KoXmlNS::text) {
            handleTagBookMarkStart(element);
        }
        else if (element.localName() == "bookmark-end" && element.namespaceURI() == KoXmlNS::text) {
            // End tag <a> started in bookmark or bookmark-start.
//            handleTagBookMarkEnd(htmlWriter);
        }
        else if (element.localName() == "note" && element.namespaceURI() == KoXmlNS::text) {
            handleTagNote(element);
        }
        else {
            handleUnknownTags(element);
        }

        node = node.nextSibling();
        element = node.toElement();
    }

    m_backend->endInsideElementsTag(element, m_context);
}

void OdtTraverser::handleTagP(KoXmlElement &element)
{
    m_backend->beginTagP(element, m_context);
    handleInsideElementsTag(element);
    m_backend->endTagP(element, m_context);
}

void OdtTraverser::handleTagH(KoXmlElement &element)
{
    m_backend->beginTagH(element, m_context);
    handleInsideElementsTag(element);
    m_backend->endTagH(element, m_context);
}

void OdtTraverser::handleTagSpan(KoXmlElement &element)
{
    m_backend->beginTagSpan(element, m_context);
    handleInsideElementsTag(element);
    m_backend->endTagSpan(element, m_context);
}

void OdtTraverser::handleTagS(KoXmlElement &element)
{
    m_backend->beginTagS(element, m_context);
    handleInsideElementsTag(element);  // BUG?
    m_backend->endTagS(element, m_context);
}

void OdtTraverser::handleTagTab (KoXmlElement &element)
{
    m_backend->beginTagTab(element, m_context);
    m_backend->endTagTab(element, m_context);
}

void OdtTraverser::handleTagLineBreak(KoXmlElement &element)
{
    m_backend->beginTagLineBreak(element, m_context);
    m_backend->endTagLineBreak(element, m_context);
}

void OdtTraverser::handleTagPageBreak(KoXmlElement &element)
{
    m_backend->beginTagPageBreak(element, m_context);
    m_backend->endTagPageBreak(element, m_context);
}

void OdtTraverser::handleCharacterData(KoXmlNode &node)
{
    // FIXME: Do we really need two calls here?  Doubtful...
    m_backend->beginCharacterData(node, m_context);
    m_backend->endCharacterData(node, m_context);
}


void OdtTraverser::handleTagList(KoXmlElement &element)
{
    m_backend->beginTagList(element, m_context);

#if 0  // FIXME: Find out how to handle list items in ODT
    KoXmlElement listItem;
    forEachElement (listItem, element) {
        htmlWriter->startElement("li", m_doIndent);
        handleInsideElementsTag(listItem);
        htmlWriter->endElement();
    }
#endif

    m_backend->endTagList(element, m_context);
}


// ----------------------------------------------------------------


void OdtTraverser::handleTagTable(KoXmlElement &element)
{
    m_backend->beginTagTable(element, m_context);

    KoXmlElement tableElement;
    forEachElement (tableElement, element) {

        //Table headers
        if (tableElement.localName() == "table-header-rows" && tableElement.namespaceURI() == KoXmlNS::table) {
            KoXmlElement headerRow;
            forEachElement (headerRow, tableElement) {
                handleTagTableRow(headerRow, TableHeaderType);
            }
        }

        //Table body
        else if (tableElement.localName() == "table-rows" && tableElement.namespaceURI() == KoXmlNS::table) {
            KoXmlElement rowElement;
            forEachElement (rowElement, tableElement) {
                handleTagTableRow(rowElement);
            }
        }

        // Tables without headers have no table-rows element and
        // instead embed rows directly in the table, so handle that
        // properly.
        else if (tableElement.localName() == "table-row" && tableElement.namespaceURI() == KoXmlNS::table) {
            handleTagTableRow(tableElement);
        }
    }

    m_backend->beginTagTable(element, m_context);
}

void OdtTraverser::handleTagTableRow(KoXmlElement& element, OdtTraverser::TableCellType type)
{
    m_backend->beginTagTableRow(element, m_context, type);

#if 0    // FIXME: We need more advanced handling of tables and more handle...Tag functions

    htmlWriter->startElement("tr", m_doIndent);

    KoXmlElement cellElement;
    forEachElement (cellElement, element) {

        if (cellElement.localName() == "covered-table-cell") {
            continue;
        }

        htmlWriter->startElement(type == TableHeaderType ? "th" : "td", m_doIndent);

        if (cellElement.hasAttributeNS(KoXmlNS::table, "style-name")) {
            QString styleName = cssClassName(cellElement.attribute("style-name"));
            StyleInfo *styleInfo = m_styles.value(styleName);
            if(styleInfo) {
                styleInfo->inUse = true;
                htmlWriter->addAttribute("class", styleName);
            }
        }

        if (cellElement.hasAttributeNS(KoXmlNS::table, "number-rows-spanned")) {
            htmlWriter->addAttribute("rowspan", cellElement.attribute("number-rows-spanned"));
        }

        if (cellElement.hasAttributeNS(KoXmlNS::table, "number-columns-spanned")) {
            htmlWriter->addAttribute("colspan", cellElement.attribute("number-columns-spanned"));
        }

        // ==== cell text ====
        // FIXME: This is wrong. A cell element can contain
        //        the same tags as the full contents, not just
        //        what is inside a paragraph. (Beside, this
        //        function has a strange name.)
        handleInsideElementsTag(cellElement);

        htmlWriter->endElement(); //td
    }

    htmlWriter->endElement(); //tr
#endif
    m_backend->endTagTableRow(element, m_context, type);
}

// ----------------------------------------------------------------
//                         Embedded stuff


//qwe

void OdtTraverser::handleTagFrame(KoXmlElement &element)
{
    // FIXME: NYI
    return;
#if 0
    QString styleName = cssClassName(element.attribute("style-name"));
    StyleInfo *styleInfo = m_styles.value(styleName);

    // Find height and width
    QString height = element.attribute("height");
    QString width  = element.attribute("width");

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

    // Go through the frame's content and see what we can handle.
    KoXmlElement framePartElement;
    forEachElement (framePartElement, element) {

        // Handle at least a few types of objects (hopefully more in the future).
        if (framePartElement.localName() == "object"
            && framePartElement.namespaceURI() == KoXmlNS::draw)
        {
            QString href = framePartElement.attribute("href");
            if (href.isEmpty()) {
                // Check for inline stuff.
                // So far only math:math is supported.
                if (!framePartElement.hasChildNodes())
                    continue;

                // Handle inline math:math
                KoXmlElement childElement = framePartElement.firstChildElement();
                if (childElement.localName() == "math"
                    && childElement.namespaceURI() == KoXmlNS::math)
                {
                    QHash<QString, QString> unknownNamespaces;
                    copyXmlElement(childElement, *htmlWriter, unknownNamespaces);

                    // We are done with the whole frame.
                    break;
                }

                // We couldn't handle this inline object. Check for
                // object replacements (pictures).
                continue;
            }

            // If we get here, this frame part was not an inline object.
            // We already have an object reference.

            // Normalize the object reference
            if (href.startsWith("./"))
                href.remove(0, 2);
            QString type = m_manifest->value(href);

            // So far we can only an handle embedded object (formula).
            // In the future we will probably be able to handle more types.
            if (type == "application/vnd.oasis.opendocument.formula") {

                handleEmbeddedFormula(href);
                break; // Only one object per frame.
            }
            // ...more types here in the future, e.g. video.

            // Ok, so we couldn't handle this one.
            continue;
        }
        else if (framePartElement.localName() == "image"
                 && framePartElement.namespaceURI() == KoXmlNS::draw)
        {
            // Handle image
            htmlWriter->startElement("img", m_doIndent);
            if (styleInfo) {
                styleInfo->inUse = true;
                htmlWriter->addAttribute("class", styleName);
            }
            htmlWriter->addAttribute("alt", "(No Description)");

            QString imgSrc = framePartElement.attribute("href").section('/', -1);

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
            }
            else {
                htmlWriter->addAttribute("src", imgSrc);
            }

            m_images.insert(framePartElement.attribute("href"), size);

            htmlWriter->endElement(); // end img
            break; // Only one image per frame.
        }
        // Handle video
        else if (framePartElement.localName() == "plugin"
                 && framePartElement.namespaceURI() == KoXmlNS::draw) {
            QString videoSource = framePartElement.attribute("href");
            QString videoId = "media_id_" + QString::number(m_mediaId);
            m_mediaId++;

            htmlWriter->addAttribute("id", videoId);
            QString id = "chapter" + QString::number(m_currentChapter) +
                    m_collector->fileSuffix() + "#" + videoId;
            m_mediaFilesList.insert(id, videoSource);
        }
    } // foreach
#endif
}

void OdtTraverser::handleEmbeddedFormula(const QString &href)
{
    // FIXME: NYI
    return;
#if 0
    // FIXME: Track down why we need to close() the store here and
    //        whip that code with a wet noodle.
    m_odfStore->close();

    // Open the formula content file if possible.
    if (!m_odfStore->open(href + "/content.xml")) {
        kDebug(30503) << "Can not open" << href << "/content.xml .";
        return;
    }

    // Copy the math:math xml tree.
    KoXmlDocument doc;
    QString errorMsg;
    int errorLine;
    int errorColumn;
    if (!doc.setContent(m_odfStore->device(), true, &errorMsg, &errorLine, &errorColumn)) {
        kDebug(30503) << "Error occurred while parsing content.xml "
                      << errorMsg << " in Line: " << errorLine
                      << " Column: " << errorColumn;
        m_odfStore->close();
        return;
    }

    KoXmlNode n = doc.documentElement();
    for (; !n.isNull(); n = n.nextSibling()) {
        if (n.isElement()) {
            KoXmlElement el = n.toElement();
            if (el.tagName() == "math") {
                QHash<QString, QString> unknownNamespaces;
                copyXmlElement(el, *htmlWriter, unknownNamespaces);

                // No need to continue once we have the math:math node.
                break;
            }
        }
    }

    m_odfStore->close();
#endif
}

// Note: This code was copied from libs/flake/KoUnavailShape.  It
// should probably be placed near /libs/odf/KoXml* instead.

void OdtTraverser::copyXmlElement(const KoXmlElement &el, KoXmlWriter &writer,
                                      QHash<QString, QString> &unknownNamespaces)
{
    // FIXME: NYI
    return;
#if 0
    // Start the element;
    // keep the name in a QByteArray so that it stays valid until end element is called.
    const QByteArray name(el.nodeName().toAscii());
    kDebug(30503) << "Copying element;" << name;
    writer.startElement(name.constData());

    // Copy all the attributes, including namespaces.
    const QList< QPair<QString, QString> >  &attributeNames = el.attributeFullNames();
    for (int i = 0; i < attributeNames.size(); ++i) {
        const QPair<QString, QString>  &attrPair(attributeNames.value(i));
        if (attrPair.first.isEmpty()) {
            kDebug(30503) << "Copying attribute;" << attrPair.second;
            writer.addAttribute(attrPair.second.toAscii(), el.attribute(attrPair.second));
        }
        else {
            // This somewhat convoluted code is because we need the
            // namespace, not the namespace URI.
            QString nsShort = KoXmlNS::nsURI2NS(attrPair.first.toAscii());
            // in case we don't find the namespace in our list create a own one and use that
            // so the document created on saving is valid.
            if (nsShort.isEmpty()) {
                nsShort = unknownNamespaces.value(attrPair.first);
                if (nsShort.isEmpty()) {
                    nsShort = QString("ns%1").arg(unknownNamespaces.size() + 1);
                    unknownNamespaces.insert(attrPair.first, nsShort);
                }
                writer.addAttribute("xmlns:" + nsShort.toAscii(), attrPair.first);
            }
            QString attr(nsShort + ':' + attrPair.second);
            writer.addAttribute(attr.toAscii(), el.attributeNS(attrPair.first,
                                                               attrPair.second));
        }
    }

    // Child elements
    // Loop through all the child elements of the draw:frame.
    KoXmlNode n = el.firstChild();
    for (; !n.isNull(); n = n.nextSibling()) {
        if (n.isElement()) {
            copyXmlElement(n.toElement(), writer, unknownNamespaces);
        }
        else if (n.isText()) {
            writer.addTextNode(n.toText().data()/*.toUtf8()*/);
        }
    }

    // End the element
    writer.endElement();
#endif
}


// ----------------------------------------------------------------

void OdtTraverser::handleTagA(KoXmlElement &element)
{
    // FIXME: NYI
    return;
#if 0
    htmlWriter->startElement("a", m_doIndent);
    QString reference = element.attribute("href");
    QString chapter = m_linksInfo.value(reference);
    if (!chapter.isEmpty() && !m_options->stylesInCssFile) {
        // This is internal link.
        reference = reference.remove("|");
        reference = reference.remove(" ");// remove spaces
        reference = chapter+reference;
        htmlWriter->addAttribute("href", reference);
    }
    else {
        // This is external link.
        htmlWriter->addAttribute("href", reference);
    }

    handleInsideElementsTag(element);
    htmlWriter->endElement();
#endif
}

void OdtTraverser::handleTagTableOfContent(KoXmlElement &element)
{
    // FIXME: NYI
    return;
#if 0
    KoXmlNode indexBody = KoXml::namedItemNS(element, KoXmlNS::text, "index-body");
    KoXmlElement  element;
    forEachElement (element, indexBody) {
        if (element.localName() == "index-title" && element.namespaceURI() == KoXmlNS::text) {
            handleInsideElementsTag(element);
        }
        else
            handleTagTableOfContentBody(element);
    }
#endif
}

void OdtTraverser::handleTagTableOfContentBody(KoXmlElement &element)
{
    // FIXME: NYI
    return;
#if 0
    if (element.localName() == "p" && element.namespaceURI() == KoXmlNS::text) {
        handleTagP(element);
    }
#endif
}

void OdtTraverser::handleTagBookMark(KoXmlElement &element)
{
    // FIXME: NYI
    return;
#if 0
    QString anchor = element.attribute("name");
    // This is haed codevalidator gets error for characters "|" and spaces
    // FIXME : we should handle ids better after move file to class
    anchor = anchor.remove("|");
    anchor = anchor.remove(" ");//remove spaces
    htmlWriter->startElement("a", m_doIndent);
    htmlWriter->addAttribute("id", anchor);
#endif
}

void OdtTraverser::handleTagBookMarkStart(KoXmlElement &element)
{
    // FIXME: NYI
    return;
#if 0
    QString anchor = element.attribute("name");
    htmlWriter->startElement("a", m_doIndent);
    htmlWriter->addAttribute("id", anchor);
#endif
}

void OdtTraverser::handleTagBookMarkEnd(KoXmlElement &element)
{
    // FIXME: NYI
    return;
#if 0
    htmlWriter->endElement();
#endif
}

void OdtTraverser::handleUnknownTags(KoXmlElement &element)
{
    // FIXME: NYI
    return;
#if 0
    // Just go deeper to find known tags.
    handleInsideElementsTag(element);
#endif
}


void OdtTraverser::handleTagNote(KoXmlElement &element)
{
    // FIXME: NYI
    return;
#if 0
    QString noteClass = element.attribute("note-class");
    if (noteClass != "footnote" && noteClass != "endnote") {
        return;
    }

    QString id = element.attribute("id");
    KoXmlElement noteElements;
    forEachElement(noteElements, element) {
        if (noteElements.localName() == "note-citation" && noteElements.namespaceURI() == KoXmlNS::text) {
            htmlWriter->startElement("sup", m_doIndent);

            htmlWriter->startElement("a", m_doIndent);
            if (noteClass == "footnote")
                htmlWriter->addAttribute("href", "#" + id + "n"); // n rerence to note foot-note or end-note
            else { // endnote
                QString endRef = "chapter-endnotes" + m_collector->fileSuffix() + '#' + id + 'n';
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
                QString noteChapter = m_collector->filePrefix();
                if (m_options->doBreakIntoChapters)
                    noteChapter += QString::number(m_currentChapter);
                m_endNotes.insert(noteChapter + "/" + id, noteElements);
                // we insert this: m_currentChapter/id
                // to can add reference for text in end note
            }
        }
    }
#endif
}
