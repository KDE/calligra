/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2011 Lukáš Tvrdý <lukas.tvrdy@ixonos.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef XMLWRITER_H
#define XMLWRITER_H

#include "kostore_export.h"
#include <QIODevice>
#include <QMap>
#include <QXmlStreamWriter>
#include <qanystringview.h>
#include <qstringview.h>
#include <qxmlstream.h>

using namespace Qt::StringLiterals;

/**
 * A class for writing out XML (to any QIODevice), with a special attention on performance.
 * The XML is being written out along the way, which avoids requiring the entire
 * document in memory (like QDom does), and avoids using QTextStream at all
 * (which in Qt3 has major performance issues when converting to utf8).
 */
class KOSTORE_EXPORT KoXmlWriter : public QXmlStreamWriter
{
public:
    /**
     * Create a KoXmlWriter instance to write out an XML document into
     * the given QIODevice.
     */
    explicit KoXmlWriter(QIODevice *dev);

    /**
     * Start the XML document.
     * This writes out the \<?xml?\> tag with utf8 encoding, and the DOCTYPE.
     * @param rootElemName the name of the root element, used in the DOCTYPE tag.
     * @param publicId the public identifier, e.g. "-//OpenOffice.org//DTD OfficeDocument 1.0//EN"
     * @param systemId the system identifier, e.g. "office.dtd" or a full URL to it.
     */
    [[deprecated]]
    inline void startDocument(QAnyStringView rootElemName, QAnyStringView publicId = {}, QAnyStringView systemId = {})
    {
        writeStartDocument("1.0"_L1);
        if (!publicId.isEmpty()) {
            writeDTD(QStringLiteral("%1 PUBLIC \"%2\" \"%3\"").arg(rootElemName.toString(), publicId.toString(), systemId.toString()));
        }
    }

    /// Call this to terminate an XML document.
    [[deprecated]] inline void endDocument()
    {
        writeEndDocument();
    }

    /**
     * Start a new element, as a child of the current element.
     * @param tagName the name of the tag. Warning: this string must
     * remain alive until endElement, no copy is internally made.
     * Usually tagName is a string constant so this is no problem anyway.
     * @param indentInside if set to false, there will be no indentation inside
     * this tag. This is useful for elements where whitespace matters.
     */
    [[deprecated]]
    inline void startElement(QAnyStringView tagName, bool indentInside = true)
    {
        Q_UNUSED(indentInside); // we never ident
        writeStartElement(tagName);
    }

    /**
     * Add an attribute whose value is an integer
     */
    [[deprecated]]
    inline void addAttribute(QAnyStringView attrName, int value)
    {
        QXmlStreamWriter::writeAttribute(attrName, QString::number(value));
    }

    /**
     * Add an attribute whose value is an integer
     */
    inline void writeAttribute(QAnyStringView attributeName, int value)
    {
        QXmlStreamWriter::writeAttribute(attributeName, QByteArray::number(value));
    }

    /**
     * Add an attribute whose value is an unsigned integer
     */
    inline void writeAttribute(QAnyStringView attrName, uint value)
    {
        QXmlStreamWriter::writeAttribute(attrName, QByteArray::number(value));
    }

    [[deprecated]]
    inline void addAttribute(QAnyStringView attrName, uint value)
    {
        writeAttribute(attrName, value);
    }

    /**
     * Add an attribute whose value is an bool
     * It is written as "true" or "false" based on value
     */
    inline void writeAttribute(QAnyStringView attrName, bool value)
    {
        QXmlStreamWriter::writeAttribute(attrName, value ? "true"_L1 : "false"_L1);
    }

    [[deprecated]]
    inline void addAttribute(const QByteArray &attrName, const QByteArray &value)
    {
        QXmlStreamWriter::writeAttribute(attrName, value);
    }

    [[deprecated]]
    inline void addAttribute(QAnyStringView attrName, bool value)
    {
        writeAttribute(attrName, value);
    }
    /**
     * Add an attribute whose value is a floating point number
     * The number is written out with the highest possible precision
     * (unlike QString::number and setNum, which default to 6 digits)
     */
    void writeAttribute(QAnyStringView attrName, double value);

    [[deprecated]]
    void addAttribute(QAnyStringView attrName, double value)
    {
        writeAttribute(attrName, value);
    }

    /**
     * Add an attribute whose value is a floating point number
     * The number is written out with the highest possible precision
     * (unlike QString::number and setNum, which default to 6 digits)
     */
    void writeAttribute(QAnyStringView attrName, float value);

    [[deprecated]]
    inline void addAttribute(QAnyStringView attrName, float value)
    {
        writeAttribute(attrName, value);
    }

    /**
     * Add an attribute which represents a distance, measured in pt
     * The number is written out with the highest possible precision
     * (unlike QString::number and setNum, which default to 6 digits),
     * and the unit name ("pt") is appended to it.
     */
    void writeAttributePt(QAnyStringView attrName, double value);

    [[deprecated]]
    inline void addAttributePt(QAnyStringView attrName, double value)
    {
        writeAttributePt(attrName, value);
    }

    /**
     * Add an attribute which represents a distance, measured in pt
     * The number is written out with the highest possible precision
     * (unlike QString::number and setNum, which default to 6 digits),
     * and the unit name ("pt") is appended to it.
     */
    void writeAttributePt(QAnyStringView attrName, float value);

    [[deprecated]]
    inline void addAttributePt(QAnyStringView attrName, float value)
    {
        writeAttributePt(attrName, value);
    }

    /**
     * Add an attribute to the current element.
     */
    [[deprecated]]
    void addAttribute(QAnyStringView attrName, QAnyStringView value)
    {
        QXmlStreamWriter::writeAttribute(attrName, value);
    }

    /**
     * Terminate the current element. After this you should start a new one (sibling),
     * add a sibling text node, or close another one (end of siblings).
     */
    [[deprecated]]
    inline void endElement()
    {
        writeEndDocument();
    }

    /**
     * @brief Adds a text node as a child of the current element.
     *
     * This is appends the literal content of @p str to the contents of the element.
     * E.g. addTextNode( "foo" ) inside a \<p\> element gives \<p\>foo\</p\>,
     * and startElement( "b" ); endElement( "b" ); addTextNode( "foo" ) gives \<p\>\<b/\>foo\</p\>
     */
    [[deprecated("use writeCharacters")]]
    inline void addTextNode(QAnyStringView text)
    {
        writeCharacters(text);
    }

    /**
     * This is quite a special-purpose method, not for everyday use.
     * It adds a complete element (with its attributes and child elements)
     * as a child of the current element. The string is supposed to be escaped
     * for XML already, so it will usually come from another KoXmlWriter.
     */
    void addCompleteElement(QAnyStringView cstr);

    /**
     * This is quite a special-purpose method, not for everyday use.
     * It adds a complete element (with its attributes and child elements)
     * as a child of the current element. The iodevice is supposed to be escaped
     * for XML already, so it will usually come from another KoXmlWriter.
     * This is usually used with KTempFile.
     */
    void addCompleteElement(QIODevice *dev);

    // #### Maybe we want to subclass KoXmlWriter for manifest files.
    /**
     * Special helper for writing "manifest" files
     * This is equivalent to startElement/2*addAttribute/endElement
     * This API will probably have to change (or not be used anymore)
     * when we add support for encrypting/signing.
     * @note OASIS-specific
     */
    void addManifestEntry(QAnyStringView fullPath, QAnyStringView mediaType);

    /**
     * Special helper for writing config item into settings.xml
     * @note OASIS-specific
     */
    void addConfigItem(QAnyStringView configName, QAnyStringView value);
    /// @note OASIS-specific
    void addConfigItem(QAnyStringView configName, bool value);
    /// @note OASIS-specific
    void addConfigItem(QAnyStringView configName, int value);
    /// @note OASIS-specific
    void addConfigItem(QAnyStringView configName, double value);
    /// @note OASIS-specific
    void addConfigItem(QAnyStringView configName, float value);
    /// @note OASIS-specific
    void addConfigItem(QAnyStringView configName, long value);
    /// @note OASIS-specific
    void addConfigItem(QAnyStringView configName, short value);

    // TODO addConfigItem for datetime and base64Binary

    /**
     * @brief Adds a text span as nodes of the current element.
     *
     * Unlike KoXmlWriter::addTextNode it handles tabulations, linebreaks,
     * and multiple spaces by using the appropriate OASIS tags.
     *
     * @param text the text to write
     *
     * @note OASIS-specific
     */
    void addTextSpan(const QString &text);
    /**
     * Overloaded version of addTextSpan which takes an additional tabCache map.
     * @param text the text to write
     * @param tabCache optional map allowing to find a tab for a given character index
     * @note OASIS-specific
     */
    void addTextSpan(const QString &text, const QMap<int, int> &tabCache);
};

#endif /* XMLWRITER_H */
