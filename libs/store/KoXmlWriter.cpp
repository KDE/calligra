/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoXmlWriter.h"

#include <QByteArray>
#include <QStack>
#include <StoreDebug.h>
#include <cfloat>

KoXmlWriter::KoXmlWriter(QIODevice *dev)
    : QXmlStreamWriter(dev)
{
}

void KoXmlWriter::addCompleteElement(QAnyStringView cstr)
{
    device()->write(">");
    device()->write(cstr.toString().toUtf8());
    writeEndElement();
}

void KoXmlWriter::addCompleteElement(QIODevice *indev)
{
    const bool wasOpen = indev->isOpen();
    // Always (re)open the device in readonly mode, it might be
    // already open but for writing, and we need to rewind.
    const bool openOk = indev->open(QIODevice::ReadOnly);
    Q_ASSERT(openOk);
    if (!openOk) {
        warnStore << "Failed to re-open the device! wasOpen=" << wasOpen;
        return;
    }

    device()->write(">");

    static const int MAX_CHUNK_SIZE = 8 * 1024; // 8 KB
    QByteArray buffer;
    buffer.resize(MAX_CHUNK_SIZE);
    while (!indev->atEnd()) {
        qint64 len = indev->read(buffer.data(), buffer.size());
        if (len <= 0) // e.g. on error
            break;
        device()->write(buffer.data(), len);
    }
    if (!wasOpen) {
        // Restore initial state
        indev->close();
    }

    writeEndElement();
}

void KoXmlWriter::writeAttribute(QAnyStringView attrName, double value)
{
    QByteArray str;
    str.setNum(value, 'f', 11);
    QXmlStreamWriter::writeAttribute(attrName, str.data());
}

void KoXmlWriter::writeAttribute(QAnyStringView attrName, float value)
{
    QByteArray str;
    str.setNum(value, 'f', FLT_DIG);
    QXmlStreamWriter::writeAttribute(attrName, str);
}

void KoXmlWriter::writeAttributePt(QAnyStringView attrName, double value)
{
    QByteArray str;
    str.setNum(value, 'f', 11);
    str += "pt";
    QXmlStreamWriter::writeAttribute(attrName, str);
}

void KoXmlWriter::writeAttributePt(QAnyStringView attrName, float value)
{
    QByteArray str;
    str.setNum(value, 'f', FLT_DIG);
    str += "pt";
    QXmlStreamWriter::writeAttribute(attrName, str);
}

void KoXmlWriter::addManifestEntry(QAnyStringView fullPath, QAnyStringView mediaType)
{
    writeStartElement("manifest:file-entry"_L1);
    QXmlStreamWriter::writeAttribute("manifest:media-type"_L1, mediaType);
    QXmlStreamWriter::writeAttribute("manifest:full-path"_L1, fullPath);
    writeEndElement();
}

void KoXmlWriter::addConfigItem(QAnyStringView configName, QAnyStringView value)
{
    writeStartElement("config:config-item"_L1);
    QXmlStreamWriter::writeAttribute("config:name"_L1, configName);
    QXmlStreamWriter::writeAttribute("config:type"_L1, "string");
    writeCharacters(value);
    writeEndElement();
}

void KoXmlWriter::addConfigItem(QAnyStringView configName, bool value)
{
    writeStartElement("config:config-item"_L1);
    QXmlStreamWriter::writeAttribute("config:name"_L1, configName);
    QXmlStreamWriter::writeAttribute("config:type"_L1, "boolean"_L1);
    writeCharacters(value ? "true"_L1 : "false"_L1);
    writeEndElement();
}

void KoXmlWriter::addConfigItem(QAnyStringView configName, int value)
{
    writeStartElement("config:config-item"_L1);
    QXmlStreamWriter::writeAttribute("config:name"_L1, configName);
    QXmlStreamWriter::writeAttribute("config:type"_L1, "int"_L1);
    writeCharacters(QString::number(value));
    writeEndElement();
}

void KoXmlWriter::addConfigItem(QAnyStringView configName, double value)
{
    writeStartElement("config:config-item"_L1);
    QXmlStreamWriter::writeAttribute("config:name"_L1, configName);
    QXmlStreamWriter::writeAttribute("config:type"_L1, "double"_L1);
    writeCharacters(QString::number(value));
    writeEndElement();
}

void KoXmlWriter::addConfigItem(QAnyStringView configName, float value)
{
    writeStartElement("config:config-item"_L1);
    QXmlStreamWriter::writeAttribute("config:name"_L1, configName);
    QXmlStreamWriter::writeAttribute("config:type"_L1, "double"_L1);
    writeCharacters(QString::number(value));
    writeEndElement();
}

void KoXmlWriter::addConfigItem(QAnyStringView configName, long value)
{
    writeStartElement("config:config-item"_L1);
    QXmlStreamWriter::writeAttribute("config:name"_L1, configName);
    QXmlStreamWriter::writeAttribute("config:type"_L1, "long"_L1);
    writeCharacters(QByteArray::number(value));
    writeEndElement();
}

void KoXmlWriter::addConfigItem(QAnyStringView configName, short value)
{
    writeStartElement("config:config-item"_L1);
    QXmlStreamWriter::writeAttribute("config:name"_L1, configName);
    QXmlStreamWriter::writeAttribute("config:type"_L1, "short"_L1);
    writeCharacters(QByteArray::number(value));
    writeEndElement();
}

void KoXmlWriter::addTextSpan(const QString &text)
{
    QMap<int, int> tabCache;
    addTextSpan(text, tabCache);
}

void KoXmlWriter::addTextSpan(const QString &text, const QMap<int, int> &tabCache)
{
    qsizetype len = text.length();
    int nrSpaces = 0; // number of consecutive spaces
    bool leadingSpace = false;
    QString str;
    str.reserve(len);

    // Accumulate chars either in str or in nrSpaces (for spaces).
    // Flush str when writing a subelement (for spaces or for another reason)
    // Flush nrSpaces when encountering two or more consecutive spaces
    for (int i = 0; i < len; ++i) {
        QChar ch = text[i];
        ushort unicode = ch.unicode();
        if (unicode == ' ') {
            if (i == 0)
                leadingSpace = true;
            ++nrSpaces;
        } else {
            if (nrSpaces > 0) {
                // For the first space we use ' '.
                // "it is good practice to use (text:s) for the second and all following SPACE
                // characters in a sequence." (per the ODF spec)
                // however, per the HTML spec, "authors should not rely on user agents to render
                // white space immediately after a start tag or immediately before an end tag"
                // (and both we and OO.o ignore leading spaces in <text:p> or <text:h> elements...)
                if (!leadingSpace) {
                    str += ' ';
                    --nrSpaces;
                }
                if (nrSpaces > 0) { // there are more spaces
                    if (!str.isEmpty()) {
                        writeCharacters(str);
                    }
                    str.clear();
                    writeStartElement("text:s"_L1);
                    if (nrSpaces > 1) {
                        // it's 1 by default
                        writeAttribute("text:c"_L1, nrSpaces);
                    }
                    writeEndElement();
                }
            }
            nrSpaces = 0;
            leadingSpace = false;

            switch (unicode) {
            case '\t':
                if (!str.isEmpty()) {
                    writeCharacters(str);
                }
                str.clear();
                writeStartElement("text:tab"_L1);
                if (tabCache.contains(i)) {
                    writeAttribute("text:tab-ref"_L1, tabCache[i] + 1);
                }
                writeEndElement();
                break;
            // gracefully handle \f form feed in text input.
            // otherwise the xml will not be valid.
            // \f can be added e.g. in ascii import filter.
            case '\f':
            case '\n':
            case QChar::LineSeparator:
                if (!str.isEmpty()) {
                    writeCharacters(str);
                }
                str.clear();
                writeStartElement("text:line-break"_L1);
                writeEndElement();
                break;
            default:
                // don't add stuff that is not allowed in xml. The stuff we need we have already handled above
                if (ch.unicode() >= 0x20) {
                    str += text[i];
                }
                break;
            }
        }
    }
    // either we still have text in str or we have spaces in nrSpaces
    if (!str.isEmpty()) {
        writeCharacters(str);
    }
    if (nrSpaces > 0) { // there are more spaces
        writeStartElement("text:s"_L1);
        if (nrSpaces > 1) // it's 1 by default
            writeAttribute("text:c"_L1, nrSpaces);
        writeEndElement();
    }
}
