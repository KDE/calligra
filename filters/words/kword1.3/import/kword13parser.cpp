/* This file is part of the KDE project
   Copyright 2001, 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>

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

#include <kdebug.h>

#include "kword13formatone.h"
#include "kword13formatother.h"
#include "kword13layout.h"
#include "kword13frameset.h"
#include "kword13picture.h"
#include "kword13document.h"
#include "kword13parser.h"

Words13StackItem::Words13StackItem() : elementType(Words13TypeUnknown), m_currentFrameset(0)
{
}

Words13StackItem::~Words13StackItem()
{
}

Words13Parser::Words13Parser(Words13Document* kwordDocument)
        : m_kwordDocument(kwordDocument), m_currentParagraph(0),
        m_currentLayout(0), m_currentFormat(0)
{
    parserStack.setAutoDelete(true);
    Words13StackItem* bottom = new Words13StackItem;
    bottom->elementType = Words13TypeBottom;
    parserStack.push(bottom);   //Security item (not to empty the stack)
}

Words13Parser::~Words13Parser(void)
{
    parserStack.clear();
    delete m_currentParagraph;
    delete m_currentLayout;
    delete m_currentFormat;
}

bool Words13Parser::startElementFormatOneProperty(const QString& name, const QXmlAttributes& attributes, Words13StackItem *stackItem)
{
    // ### TODO: check status
    if (stackItem->elementType == Words13TypeLayoutFormatOne) {
        if (! m_currentLayout) {
            kError(30520) << "No current LAYOUT for storing FORMAT property: " << name;
            return false;
        }
        for (int i = 0; i < attributes.count(); ++i) {
            QString attrName(name);
            attrName += ':';
            attrName += attributes.qName(i);
            m_currentLayout->m_format.m_properties[ attrName ] = attributes.value(i);
            kDebug(30520) << "Format Property (for LAYOUT):" << attrName << " =" << attributes.value(i);
        }
        stackItem->elementType = Words13TypeEmpty;
        return true;
    } else if (stackItem->elementType == Words13TypeFormat) {
        if (! m_currentFormat) {
            kError(30520) << "No current FORMAT for storing FORMAT property: " << name;
            return false;
        }
        Words13FormatOneData* data = m_currentFormat->getFormatOneData();

        if (! data) {
            kError(30520) << "Current FORMAT cannot store FORMAT text property: " << name;
            return false;
        }

        for (int i = 0; i < attributes.count(); ++i) {
            QString attrName(name);
            attrName += ':';
            attrName += attributes.qName(i);
            data->m_properties[ attrName ] = attributes.value(i);
            kDebug(30520) << "Format Property (for FORMATS):" << attrName << " =" << attributes.value(i);
        }
        stackItem->elementType = Words13TypeEmpty;
        return true;
    } else if (stackItem->elementType == Words13TypeIgnore) {
        return true;
    } else {
        kError(30520) << "Wrong parents for FORMAT property: " << name;
        return false;
    }
}

bool Words13Parser::startElementLayoutProperty(const QString& name, const QXmlAttributes& attributes, Words13StackItem *stackItem)
{
    // ### TODO: check status
    if (stackItem->elementType == Words13TypeIgnore) {
        return true;
    } else if (m_currentLayout) {
        for (int i = 0; i < attributes.count(); ++i) {
            QString attrName(name);
            attrName += ':';
            attrName += attributes.qName(i);
            m_currentLayout->m_layoutProperties[ attrName ] = attributes.value(i);
            kDebug(30520) << "Layout Property:" << attrName << " =" << attributes.value(i);
        }
        stackItem->elementType = Words13TypeEmpty;
        return true;
    } else {
        kError(30520) << "No current layout for storing property: " << name;
        return false;
    }
}

bool Words13Parser::startElementName(const QString&, const QXmlAttributes& attributes, Words13StackItem *stackItem)
{
    if (stackItem->elementType != Words13TypeLayout) {
        // We have something else than a LAYOU/STYLE, so ignore for now.
        stackItem->elementType = Words13TypeIgnore;
        return true;
    }

    stackItem->elementType = Words13TypeEmpty;

    if (m_currentLayout) {
        m_currentLayout->m_name = attributes.value("value");
    }
    return  true;
}

bool Words13Parser::startElementFormat(const QString&, const QXmlAttributes& attributes, Words13StackItem *stackItem)
{
    // ### TODO: check parent?
    if (stackItem->elementType == Words13TypeIgnore) {
        return true;
    } else if (stackItem->elementType == Words13TypeLayout) {
        stackItem->elementType = Words13TypeLayoutFormatOne;
        return true; // Everything is done directly on the layout
    } else if (stackItem->elementType != Words13TypeFormatsPlural) {
        kError(30520) << "<FORMAT> is child neither of <FORMATS> nor of <LAYOUT> nor of <STYLE>! Aborting!";
        return false; // Assume parsing error!
    }

    stackItem->elementType = Words13TypeFormat;

    if (m_currentFormat) {
        kWarning(30520) << "Current format already defined!";
        delete m_currentFormat;
        m_currentFormat = 0;
    }

    bool ok = false;
    const int id = attributes.value("id").toInt(&ok);

    if (id == 1 && ok) { // Normal text
        Words13FormatOne* one = new Words13FormatOne;
        const int len = attributes.value("len").toInt(&ok);
        if (ok)
            one->m_length = len;
        m_currentFormat = one;
    } else if (id == 4 && ok) { // Variable
        stackItem->elementType = Words13TypeVariable;
        m_currentFormat = new Words13FormatFour;
    } else if (id == 6 && ok) { // Anchor
        stackItem->elementType = Words13TypeAnchor;
        m_currentFormat = new Words13FormatSix;
    } else {
        // ### TODO: Temporary
        stackItem->elementType = Words13TypeIgnore;
        m_currentFormat = new Words13Format;
        if (ok)
            m_currentFormat->m_id = id;
    }
    const int pos = attributes.value("pos").toInt(&ok);
    if (ok) {
        m_currentFormat->m_pos = pos;
    } else {
        kWarning(30520) << "Cannot set position of <FORMAT>: " << attributes.value("pos");
        return false; // Assume parse error!
    }

    kDebug(30520) << "<FORMAT id=\"" << id << "\" pos=\"" << pos << "\" len=\"" << attributes.value("len") << "\">";

    return true;
}

bool Words13Parser::startElementLayout(const QString&, const QXmlAttributes& attributes, Words13StackItem *stackItem)
{
    // ### TODO: check parent?
    if (stackItem->elementType == Words13TypeIgnore) {
        return true;
    }

    stackItem->elementType = Words13TypeLayout;

    if (m_currentFormat) {
        kWarning(30520) << "Current format defined! (Layout)";
        delete m_currentFormat;
        m_currentFormat = 0;
    }
    if (m_currentLayout) {
        // Delete an eventually already existing paragraph (should not happen)
        kWarning(30520) << "Current layout already defined!";
        delete m_currentLayout;
    }

    m_currentLayout = new Words13Layout;
    m_currentLayout->m_outline = (attributes.value("outline") == "true");

    return true;
}

bool Words13Parser::startElementParagraph(const QString&, const QXmlAttributes&, Words13StackItem *stackItem)
{
    if (stackItem->elementType == Words13TypeUnknownFrameset) {
        stackItem->elementType = Words13TypeIgnore;
        return true;
    }

    stackItem->elementType = Words13TypeParagraph;

    if (m_currentParagraph) {
        // Delete an eventually already existing paragraph (should not happen)
        kWarning(30520) << "Current paragraph already defined!";
        delete m_currentParagraph;
    }

    m_currentParagraph = new Words13Paragraph;

    return true;
}

bool Words13Parser::startElementFrame(const QString& name, const QXmlAttributes& attributes, Words13StackItem *stackItem)
{
    if (stackItem->elementType == Words13TypeFrameset || stackItem->elementType == Words13TypePictureFrameset) {
        stackItem->elementType = Words13TypeEmpty;
        if (stackItem->m_currentFrameset) {
            const int num = ++stackItem->m_currentFrameset->m_numFrames;
            for (int i = 0; i < attributes.count(); ++i) {
                QString attrName(name);
                attrName += ':';
                attrName += QString::number(num);
                attrName += ':';
                attrName += attributes.qName(i);
                stackItem->m_currentFrameset->m_frameData[ attrName ] = attributes.value(i);
                kDebug(30520) << "FrameData:" << attrName << " =" << attributes.value(i);
            }

        } else {
            kError(30520) << "Data of <FRAMESET> not found";
            return false;
        }
    } else if (stackItem->elementType != Words13TypeUnknownFrameset) {
        kError(30520) << "<FRAME> not child of <FRAMESET>";
        return false;
    }
    return true;
}

bool Words13Parser::startElementFrameset(const QString& name, const QXmlAttributes& attributes, Words13StackItem *stackItem)
{
    Q_UNUSED(name);
    const QString frameTypeStr(attributes.value("frameType"));
    const QString frameInfoStr(attributes.value("frameInfo"));

    if (frameTypeStr.isEmpty() || frameInfoStr.isEmpty()) {
        kError(30520) << "<FRAMESET> without frameType or frameInfo attribute!";
        return false;
    }

    const int frameType = frameTypeStr.toInt();
    const int frameInfo = frameInfoStr.toInt();

    if (frameType == 1) {
        stackItem->elementType = Words13TypeFrameset;
        WordsTextFrameset* frameset = new WordsTextFrameset(frameType, frameInfo, attributes.value("name"));

        // Normal text frame (in or outside a table)
        if ((!frameInfo) && attributes.value("grpMgr").isEmpty()) {
            m_kwordDocument->m_normalTextFramesetList.append(frameset);
            stackItem->m_currentFrameset = m_kwordDocument->m_normalTextFramesetList.current();
        } else if (!frameInfo) {
            // We just store the frameset in the frameset table list
            // Grouping the framesets by table will be done after the parsing, not now.
            m_kwordDocument->m_tableFramesetList.append(frameset);
            stackItem->m_currentFrameset = m_kwordDocument->m_tableFramesetList.current();
        } else if (frameInfo >= 1 && frameInfo <= 6) {
            m_kwordDocument->m_headerFooterFramesetList.append(frameset);
            stackItem->m_currentFrameset = m_kwordDocument->m_headerFooterFramesetList.current();
        } else if (frameInfo == 7) {
            m_kwordDocument->m_footEndNoteFramesetList.append(frameset);
            stackItem->m_currentFrameset = m_kwordDocument->m_footEndNoteFramesetList.current();
        } else {
            kError(30520) << "Unknown text frameset!";
            m_kwordDocument->m_otherFramesetList.append(frameset);
            stackItem->m_currentFrameset = m_kwordDocument->m_otherFramesetList.current();
        }
    } else if ((frameType == 2)   // picture or image
               || (frameType == 5)) {  // ciipart
        if (!frameInfo) {
            kWarning(30520) << "Unknown FrameInfo for pictures: " << frameInfo;
        }
        stackItem->elementType = Words13TypePictureFrameset;
        Words13PictureFrameset* frameset = new Words13PictureFrameset(frameType, frameInfo, attributes.value("name"));
        m_kwordDocument->m_otherFramesetList.append(frameset);
        stackItem->m_currentFrameset = m_kwordDocument->m_otherFramesetList.current();
    }
    // ### frameType == 6 : horizontal line (however Words did not save it correctly)
    // ### frameType == 4 : formula
    // ### frametype == 3 : embedded (but only in <SETTINGS>)
    else {
        // Frame of unknown/unsupported type
        kWarning(30520) << "Unknown/unsupported <FRAMESET> type! Type: " << frameTypeStr << " Info: " << frameInfoStr;
        stackItem->elementType = Words13TypeUnknownFrameset;
        Words13Frameset* frameset = new Words13Frameset(frameType, frameInfo, attributes.value("name"));
        m_kwordDocument->m_otherFramesetList.append(frameset);
        stackItem->m_currentFrameset = m_kwordDocument->m_otherFramesetList.current();
    }
    return true;
}


bool Words13Parser::startElementDocumentAttributes(const QString& name, const QXmlAttributes& attributes, Words13StackItem *stackItem,
        const Words13StackItemType& allowedParentType, const Words13StackItemType& newType)
{
    if (parserStack.current()->elementType == allowedParentType) {
        stackItem->elementType = newType;
        for (int i = 0; i < attributes.count(); ++i) {
            QString attrName(name);
            attrName += ':';
            attrName += attributes.qName(i);
            m_kwordDocument->m_documentProperties[ attrName ] = attributes.value(i);
            kDebug(30520) << "DocAttr:" <<  attrName << " =" << attributes.value(i);
        }
        return true;
    } else {
        kError(30520) << "Wrong parent!";
        return false;
    }
}

bool Words13Parser::startElementKey(const QString& name, const QXmlAttributes& attributes, Words13StackItem *stackItem)
{
    Q_UNUSED(name);
    const QString key(calculatePictureKey(attributes.value("filename"),
                                          attributes.value("year"), attributes.value("month"),  attributes.value("day"),
                                          attributes.value("hour"), attributes.value("minute"), attributes.value("second"),
                                          attributes.value("msec")));
    kDebug(30520) << "Picture key:" << key;

    if (stackItem->elementType == Words13TypePicturesPlural) {
        Words13Picture* pic = new Words13Picture;
        pic->m_storeName = attributes.value("name");
        if (pic->m_storeName.isEmpty()) {
            kError(30520) << "Picture defined without store name! Aborting!";
            delete pic;
            return false; // Assume parse error
        }
        // ### TODO: catch duplicate keys (should not happen but who knows?)
        m_kwordDocument->m_pictureDict.insert(key, pic);
    } else if (stackItem->elementType == Words13TypePicture) {
        // ### TODO: error messages?
        if (stackItem->m_currentFrameset) {
            stackItem->m_currentFrameset->setKey(key);
        }
    } else {
        // Neither child of <PICTURES>, <PIXMAPS>, <CLIPARTS>
        // nor of <PICTURE>, <IMAGE>, <CLIPART>
        // ### TODO: parse error?
    }
    return true;
}

bool Words13Parser::startElementAnchor(const QString& name, const QXmlAttributes& attributes, Words13StackItem *stackItem)
{
    if (stackItem->elementType == Words13TypeAnchor) {
        const QString anchorType(attributes.value("type"));
        if (anchorType == "grpMgr")
            kWarning(30520) << "Anchor of type grpMgr! Not tested!"; // ### TODO
        else if (anchorType != "frameset") {
            kError(30520) << "Unsupported anchor type: " << anchorType;
            return false;
        }
        const QString frameset(attributes.value("instance"));
        if (frameset.isEmpty()) {
            kError(30520) << "Anchor to an empty frameset name! Aborting!";
            return false;
        }
        if (m_currentFormat) {
            Words13FormatSix* six = (Words13FormatSix*) m_currentFormat;
            six->m_anchorName = frameset;
        }
        // add frameset name to the list of anchored framesets
        if (m_kwordDocument->m_anchoredFramesetNames.find(frameset) == m_kwordDocument->m_anchoredFramesetNames.end()) {
            m_kwordDocument->m_anchoredFramesetNames.append(frameset);
        }
    } else {
        kError(30520) << "Anchor not child of <FORMAT id=\"6\"> Aborting!";
        return false;
    }
    return true;
}


bool Words13Parser::startElement(const QString&, const QString&, const QString& name, const QXmlAttributes& attributes)
{
    kDebug(30520) << indent << "<" << name << ">"; // DEBUG
    indent += '*'; //DEBUG
    if (parserStack.isEmpty()) {
        kError(30520) << "Stack is empty!! Aborting! (in WordsParser::startElement)";
        return false;
    }

    // Create a new stack element copying the top of the stack.
    Words13StackItem *stackItem = new Words13StackItem(*parserStack.current());

    if (!stackItem) {
        kError(30520) << "Could not create Stack Item! Aborting! (in StructureParser::startElement)";
        return false;
    }

    stackItem->itemName = name;

    bool success = false;

    // Order of element names: probability in a document
    if (name == "COLOR" || name == "FONT" || name == "SIZE"
            || name == "WEIGHT" || name == "ITALIC" || name == "UNDERLINE"
            || name == "STRIKEOUT" || name == "VERTALIGN" || name == "SHADOW"
            || name == "FONTATTRIBUTE" || name == "LANGUAGE"
            || name == "TEXTBACKGROUNDCOLOR" || name == "OFFSETFROMBASELINE") {
        success = startElementFormatOneProperty(name, attributes, stackItem);
    } else if (name == "FLOW" || name == "INDENTS" || name == "OFFSETS"
               || name == "LINESPACING" || name == "PAGEBREAKING"
               || name == "LEFTBORDER" || name == "RIGHTBORDER" || name == "FOLLOWING"
               || name == "TOPBORDER" || name == "BOTTOMBORDER" || name == "COUNTER") {
        success = startElementLayoutProperty(name, attributes, stackItem);
    } else if (name == "TEXT") {
        if (stackItem->elementType == Words13TypeParagraph && m_currentParagraph) {
            stackItem->elementType = Words13TypeText;
            m_currentParagraph->setText(QString());
        } else {
            stackItem->elementType = Words13TypeIgnore;
        }
        success = true;
    } else if (name == "NAME") {
        success = startElementName(name, attributes, stackItem);
    } else if (name == "FORMATS") {
        if (stackItem->elementType == Words13TypeParagraph && m_currentParagraph) {
            stackItem->elementType = Words13TypeFormatsPlural;
        } else {
            stackItem->elementType = Words13TypeIgnore;
        }
        success = true;
    } else if (name == "PARAGRAPH") {
        success = startElementParagraph(name, attributes, stackItem);
    } else if (name == "FORMAT") {
        success = startElementFormat(name, attributes, stackItem);
    } else if (name == "LAYOUT") {
        success = startElementLayout(name, attributes, stackItem);
    } else if (name == "TYPE") {
        // ### TEMPORARY
        if (m_currentFormat && (stackItem->elementType == Words13TypeVariable)) {
            ((Words13FormatFour*) m_currentFormat) -> m_text =  attributes.value("text");
        }
        success = true;
    } else if (name == "KEY") {
        success = startElementKey(name, attributes, stackItem);
    } else if (name == "ANCHOR") {
        success = startElementAnchor(name, attributes, stackItem);
    } else if (name == "PICTURE" || name == "IMAGE" || name == "CLIPART") {
        // ### TODO: keepAspectRatio (but how to transform it to OASIS)
        if (stackItem->elementType == Words13TypePictureFrameset) {
            stackItem->elementType = Words13TypePicture;
        }
        success = true;
    } else if (name == "FRAME") {
        success = startElementFrame(name, attributes, stackItem);
    } else if (name == "FRAMESET") {
        success = startElementFrameset(name, attributes, stackItem);
    } else if (name == "STYLE") {
        success = startElementLayout(name, attributes, stackItem);
    } else if (name == "DOC") {
        success = startElementDocumentAttributes(name, attributes, stackItem, Words13TypeBottom, Words13TypeDocument);
    } else if (name == "PAPER") {
        success = startElementDocumentAttributes(name, attributes, stackItem, Words13TypeDocument, Words13TypePaper);
    } else if (name == "PAPERBORDERS") {
        success = startElementDocumentAttributes(name, attributes, stackItem, Words13TypePaper, Words13TypeEmpty);
    } else if ((name == "ATTRIBUTES") || (name == "VARIABLESETTINGS")
               || (name == "FOOTNOTESETTINGS") || (name == "ENDNOTESETTINGS")) {
        success = startElementDocumentAttributes(name, attributes, stackItem, Words13TypeDocument, Words13TypeEmpty);
    } else if (name == "FRAMESTYLE") {
        // ### TODO, but some of the <STYLE> children are also children of <FRAMESTYLE>, so we have to set it to "ignore"
        stackItem->elementType = Words13TypeIgnore;
        success = true;
    } else if (name == "PICTURES" || name == "PIXMAPS" || name == "CLIPARTS") {
        // We just need a separate "type" for the <KEY> children
        stackItem->elementType = Words13TypePicturesPlural;
        success = true;
    } else {
        stackItem->elementType = Words13TypeUnknown;
        success = true;
    }

    if (success) {
        parserStack.push(stackItem);
    } else {  // We have a problem so destroy our resources.
        delete stackItem;
    }

    return success;
}

bool Words13Parser :: endElement(const QString&, const QString& , const QString& name)
{
    indent.remove(0, 1);   // DEBUG
    //kDebug(30520) << indent <<"</" << name <<">"; // DEBUG
    if (parserStack.isEmpty()) {
        kError(30520) << "Stack is empty!! Aborting! (in StructureParser::endElement)";
        return false;
    }

    bool success = false;

    Words13StackItem *stackItem = parserStack.pop();

    if (name == "PARAGRAPH") {
        if (stackItem->m_currentFrameset && m_currentParagraph) {
            if (stackItem->m_currentFrameset->addParagraph(*m_currentParagraph)) {
                success = true;
            }
            // ### HACK: do not delete the data of <FORMATS>
            m_currentParagraph->m_formats.setAutoDelete(false);
        } else if (stackItem->elementType == Words13TypeIgnore) {
            success = true;
        }
        delete m_currentParagraph;
        m_currentParagraph = 0;
    } else if (name == "FORMAT") {
        if (stackItem->elementType == Words13TypeFormat) {
            if (m_currentParagraph) {
                m_currentParagraph->m_formats.append(m_currentFormat);
                kDebug(30520) << "Adding to <FORMATS>:" << ((void*) m_currentFormat);
                m_currentFormat = 0;
            } else {
                kError(30520) << "No paragraph to store <FORMAT>! Aborting!";
                delete m_currentFormat;
                m_currentFormat = 0;
                return false; // Assume parsing error!
            }

        } else if (stackItem->elementType == Words13TypeLayoutFormatOne) {
            // Nothing to do!
        }
        success = true;
    } else if (name == "LAYOUT") {
        if (m_currentLayout && m_currentParagraph) {
            m_currentParagraph->m_layout = *m_currentLayout;
        }
        delete m_currentLayout;
        m_currentLayout = 0;
        success = true;
    } else if (name == "STYLE") {
        if (m_kwordDocument && m_currentLayout) {
            if (m_currentLayout->m_name.isEmpty()) {
                // ### TODO: what should be really done with anonymous styles (should not happen but it would have consequences)
                kError(30520) << "Anonymous style found! Aborting";
                return false; // Assume a parsing error!
            }
            m_kwordDocument->m_styles.append(*m_currentLayout);
            success = true;
        }
        delete m_currentLayout;
        m_currentLayout = 0;
    } else if (name == "DOC") {
        success = true;
    } else {
        success = true; // No problem, so authorization to continue parsing
    }

    if (!success) {
        // If we have no success, then it was surely a tag mismatch. Help debugging!
        kError(30506) << "Found closing tag name: " << name << " expected: " << stackItem->itemName;
    }

    delete stackItem;

    return success;
}

bool Words13Parser :: characters(const QString & ch)
{
#if 0
    // DEBUG start
    if (ch == "\n") {
        kDebug(30520) << indent << " (LINEFEED)";
    } else if (ch.length() > 40) {  // 40 characters are enough (especially for image data)
        kDebug(30520) << indent << " :" << ch.left(40) << "...";
    } else {
        kDebug(30520) << indent << " :" << ch << ":";
    }
    // DEBUG end
#endif

    if (parserStack.isEmpty()) {
        kError(30520) << "Stack is empty!! Aborting! (in StructureParser::characters)";
        return false;
    }

    bool success = false;

    QString tmp(ch);
    Words13StackItem *stackItem = parserStack.current();

    if (stackItem->elementType == Words13TypeText) {
        // <TEXT>
        if (m_currentParagraph) {
            bool found = false; // Some unexpected control character?
            // ### TODO: this is perhaps a good point to check for non-XML characters
            const uint length = ch.length();
            for (uint i = 0; i < length; ++i) {
                const ushort uni = ch.at(i).unicode();
                if (uni >= 32)
                    continue; // Normal character
                else if (uni == 9 || uni == 10 || uni == 13)
                    continue; // Tabulator, Line Feed, Carriage Return
                else if (uni == 1) {
                    // Old Words documents have a QChar(1) as anchor character
                    // So replace it with the anchor character of recent Words versions
                    tmp[i] = '#';
                } else {
                    tmp[i] = '?';
                    found = true;
                }
            }
            if (found)
                kWarning(30520) << "Unexcepted control characters found in text!";
            m_currentParagraph->appendText(tmp);
            success = true;
        } else {
            kError(30520) << "No current paragraph defined! Tag mismatch?";
            success = false;
        }
    } else if (stackItem->elementType == Words13TypeEmpty) {
        success = ch.trimmed().isEmpty();
        if (!success) {
            // We have a parsing error, so abort!
            kError(30520) << "Empty element " << stackItem->itemName << " is not empty! Aborting! (in WordsParser::characters)";
        }
    } else {
        success = true;
    }

    return success;
}

bool Words13Parser::warning(const QXmlParseException& exception)
{
    kWarning(30520) << "XML parsing warning: line " << exception.lineNumber()
    << " col " << exception.columnNumber() << " message: " << exception.message();
    return true;
}

bool Words13Parser::error(const QXmlParseException& exception)
{
    // A XML error is recoverable, so it is only a KDE warning
    kWarning(30520) << "XML parsing error: line " << exception.lineNumber()
    << " col " << exception.columnNumber() << " message: " << exception.message();
    return true;
}

bool Words13Parser::fatalError(const QXmlParseException& exception)
{
    kError(30520) << "XML parsing fatal error: line " << exception.lineNumber()
    << " col " << exception.columnNumber() << " message: " << exception.message();
    // ### TODO: user message box
    return false; // Stop parsing now, we do not need further errors.
}

QString Words13Parser::calculatePictureKey(const QString& filename,
        const QString& year,  const QString& month,  const QString& day,
        const QString& hour,  const QString& minute,  const QString& second,
        const QString& microsecond) const
{
    bool ok;
    bool globalOk = true;

    ok = false;
    const int iYear = year.toInt(& ok);
    globalOk = globalOk && ok;

    ok = false;
    const int iMonth = month.toInt(& ok);
    globalOk = globalOk && ok;

    ok = false;
    const int iDay = day.toInt(& ok);
    globalOk = globalOk && ok;

    ok = false;
    const int iHour = hour.toInt(& ok);
    globalOk = globalOk && ok;

    ok = false;
    const int iMinute = minute.toInt(& ok);
    globalOk = globalOk && ok;

    ok = false;
    const int iSecond = second.toInt(& ok);
    globalOk = globalOk && ok;

    ok = false;
    const int iMicrosecond = microsecond.toInt(& ok);
    globalOk = globalOk && ok;

    if (globalOk) {
        // No error until then, so check if the date/time is valid at all
        globalOk = globalOk && QDate::isValid(iYear, iMonth, iDay);
        globalOk = globalOk && QTime::isValid(iHour, iMinute, iSecond, iMicrosecond);
    }

    QDateTime dt;
    if (globalOk) {
        // The date/time seems correct
        dt = QDateTime(QDate(iYear, iMonth, iDay), QTime(iHour, iMinute, iSecond, iMicrosecond));
    } else {
        // *NIX epoch (We do not really care if it is UTC or local time)
        dt = QDateTime(QDate(1970, 1, 1), QTime(0, 0, 0, 0));
    }

    // We put the date/time first, as if the date is useful, it will have faster a difference than a path
    // where the common pth might be very long.

    // Output the date/time as compact as possible
    QString result(dt.toString("yyyyMMddhhmmsszzz"));
    result += '@'; // A separator
    result += filename;
    return result;
}
