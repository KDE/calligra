/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "MsooXmlReader.h"
#include "MsooXmlSchemas.h"

#include <KoXmlWriter.h>

QDebug operator<<(QDebug dbg, const QXmlStreamReader& reader)
{
    dbg.nospace() << "QXmlStreamReader(";
    if (reader.isStartElement()) {
        dbg.nospace() << "<";
        dbg.nospace() << reader.qualifiedName().toString().toLocal8Bit().constData();
        QString attrsString;
        const QXmlStreamAttributes& attrs = reader.attributes();
        for (int i = 0; i < attrs.count(); i++) {
            dbg.nospace() << " " << attrs[i].qualifiedName().toString().toLocal8Bit().constData();
            dbg.nospace() << "=";
            dbg.nospace() << attrs[i].value().toString();
        }
        if (reader.isEndElement()) {
            dbg.nospace() << "/>)";
        }
        else {
            dbg.nospace() << ">)";
        }
    }
    else if (reader.isEndElement()) {
        dbg.nospace() << "</" << reader.qualifiedName().toString().toLocal8Bit().constData() << ">)";
    }
    else if (reader.isCharacters()) {
        dbg.nospace() << "characters:" << reader.text() << ")";
    }
    else if (reader.isComment()) {
        dbg.nospace() << "<!-- " << reader.text().toString().toLocal8Bit().constData() << " -->)";
    }
    else if (reader.isCDATA()) {
        dbg.nospace() << "CDATA:" << reader.text() << ")";
    }
    else if (reader.isWhitespace()) {
        dbg.nospace() << "whitespace:" << reader.text() << ")";
    }
    else {
        dbg.nospace() << reader.tokenString() << reader.text();
    }
    return dbg.space();
}

using namespace MSOOXML;

MsooXmlReaderContext::MsooXmlReaderContext(MSOOXML::MsooXmlRelationships* _relationships)
 : relationships(_relationships)
{
}

MsooXmlReaderContext::~MsooXmlReaderContext()
{
}

enum State {
    Start,
    InsideDocument
};

MsooXmlReader::MsooXmlReader(KoOdfWriters *writers)
    : QXmlStreamReader()
    , KoOdfWriters(*writers)
{
    init();
}

MsooXmlReader::MsooXmlReader(QIODevice* io, KoOdfWriters *writers)
    : QXmlStreamReader(io)
    , KoOdfWriters(*writers)
{
    init();
}

MsooXmlReader::~MsooXmlReader()
{
}

const char* MsooXmlReader::constOn = "on";
const char* MsooXmlReader::constOff = "off";
const char* MsooXmlReader::constTrue = "true";
const char* MsooXmlReader::constFalse = "false";
const char* MsooXmlReader::const1 = "1";
const char* MsooXmlReader::const0 = "0";
const char* MsooXmlReader::constAuto = "auto";
const char* MsooXmlReader::constFloat = "float";
const char* MsooXmlReader::constPercentage = "percentage";
const char* MsooXmlReader::constCurrency = "currency";
const char* MsooXmlReader::constDate = "date";
const char* MsooXmlReader::constTime = "time";
const char* MsooXmlReader::constBoolean = "boolean";
const char* MsooXmlReader::constString = "string";


void MsooXmlReader::init()
{
    m_readUndoed = false;
}

static const char * tokenNames[] = {
    "NoToken", "Invalid", "StartDocument", "EndDocument",
    "StartElement", "EndElement", "Characters", "Comment",
    "DTD", "EntityReference", "ProcessingInstruction", "??"};

static const char* tokenName(QXmlStreamReader::TokenType t)
{
    int i = (int)t;
    if (i < 0 || i > QXmlStreamReader::ProcessingInstruction)
        i = QXmlStreamReader::ProcessingInstruction + 1;
    return tokenNames[i];
}

bool MsooXmlReader::readBooleanAttr(const char* attrName)
{
    const QString val(attributes().value(attrName).toString());
    return val != MsooXmlReader::constOff && val != MsooXmlReader::constFalse && val != MsooXmlReader::const0;
}

void MsooXmlReader::raiseError( const QString & message )
{
    QXmlStreamReader::raiseError(
        m_fileName.isEmpty() ?
            i18n("%1 (line %2, column %3)", message,
                 QString::number(lineNumber()), QString::number(columnNumber()))
          : i18n("%1 (%2, line %3, column %4)", message, m_fileName,
                 QString::number(lineNumber()), QString::number(columnNumber()))
    );
    kDebug() << errorString();
}

QXmlStreamReader::TokenType MsooXmlReader::readNext()
{
    if (m_readUndoed) {
        m_readUndoed = false;
    }
    else {
        m_recentType = QXmlStreamReader::readNext();
    }
    kDebug() << tokenName(m_recentType) << *this;
    return m_recentType;
}

void MsooXmlReader::undoReadNext()
{
    m_readUndoed = true;
}

bool MsooXmlReader::expectElName(const char* elementName)
{
    kDebug() << elementName << "found:" << name();
    if (!isStartElement() || name() != QLatin1String(elementName)) {
        raiseError( i18n("Element \"%1\" not found", elementName) );
        return false;
    }
    return true;
}

bool MsooXmlReader::expectElNameEnd(const char* elementName)
{
    kDebug() << elementName << "found:" << name();
    if (!isEndElement() || name() != QLatin1String(elementName)) {
        raiseError( i18n("Expected closing of element \"%1\"", elementName) );
        return false;
    }
    return true;
}

bool MsooXmlReader::expectEl(const char* qualifiedElementName)
{
    kDebug() << qualifiedElementName << "found:" << qualifiedName();
    if (!isStartElement() || qualifiedName() != QLatin1String(qualifiedElementName)) {
        raiseError( i18n("Element \"%1\" not found", qualifiedElementName) );
        return false;
    }
    return true;
}

bool MsooXmlReader::expectElEnd(const QString& qualifiedElementName)
{
    kDebug() << qualifiedElementName << "found:" << qualifiedName();
//    kDebug() << kBacktrace();
    if (!isEndElement() || qualifiedName() != qualifiedElementName) {
        raiseError( i18n("Expected closing of element \"%1\"", qualifiedElementName) );
        return false;
    }
    return true;
}

bool MsooXmlReader::expectElEnd(const char* qualifiedElementName)
{
    return expectElEnd(QLatin1String(qualifiedElementName));
}

bool MsooXmlReader::expectNS(const char* nsName)
{
    kDebug() << namespaceUri() << (namespaceUri().compare(nsName) == 0);
    if (0 != namespaceUri().compare(nsName)) {
        raiseError(i18n("Namespace \"%1\" not found", nsName));
        return false;
    }
    return true;
}

void MsooXmlReader::raiseUnexpectedAttributeValueError(const QString& value, const char* attrName)
{
    raiseError( i18n("Unexpected value \"%1\" of attribute \"%2\"", value, attrName) );
}
