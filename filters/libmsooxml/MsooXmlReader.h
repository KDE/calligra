/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MSOOXMLREADER_H
#define MSOOXMLREADER_H

#include "msooxml_export.h"

#include <QXmlStreamReader>
#include <QStack>
#include <QUrl>

#include <KLocale>
#include <KDebug>
#include <KoXmlReader.h>
#include <KoFilter.h>
#include <KoOdfExporter.h>

class KoCharacterStyle;

namespace MSOOXML
{

class MsooXmlRelationships;

//! Context for MsooXmlReader::read()
class MSOOXML_EXPORT MsooXmlReaderContext
{
protected:
    MsooXmlReaderContext(MsooXmlRelationships* _relationships = 0);
public:
    virtual ~MsooXmlReaderContext();
    MSOOXML::MsooXmlRelationships* relationships;
};

//! A base class reading MSOOXML parts like document.xml or styles.xml.
class MSOOXML_EXPORT MsooXmlReader : public QXmlStreamReader, public KoOdfWriters
{
public:
    explicit MsooXmlReader(KoOdfWriters *writers);

    MsooXmlReader(QIODevice* io, KoOdfWriters *writers);

    virtual ~MsooXmlReader();

    //! Reads/parses the file
    virtual KoFilter::ConversionStatus read(MsooXmlReaderContext* context = 0) = 0;

    //! Sets filename for the document being read.
    //! Only for error reporting purposes, used in raiseError().
    void setFileName(const QString &fileName) {
        m_fileName = fileName;
    }

    //! @return filename for the document being read.
    //! Only for error reporting purposes, used in raiseError().
    QString fileName() const  {
        return m_fileName;
    }

    //! Reimplemented after QXmlStreamReader: adds line, column and filename information
    void raiseError(const QString & message = QString());

    //! Reimplemented after QXmlStreamReader for supporting undo read and for debugging purposes
    TokenType readNext();

    //! Undoes recent readNext(); only one recent readNext() can be undoed
    void undoReadNext();

    // const strings (for optimization)
    static const char* constOn;
    static const char* constOff;
    static const char* constTrue;
    static const char* constFalse;
    static const char* constNone;
    static const char* const1;
    static const char* const0;
    static const char* constAuto;
    static const char* constFloat;
    static const char* constPercentage;
    static const char* constCurrency;
    static const char* constDate;
    static const char* constTime;
    static const char* constBoolean;
    static const char* constString;

protected:
    // -- general
    bool expectElName(const char* qualifiedElementName);
    bool expectElNameEnd(const char* elementName);
    bool expectEl(const char* elementName);
    bool expectEl(const QList<QByteArray>& qualifiedElementNames);
    bool expectElEnd(const QString& qualifiedElementName);
    bool expectElEnd(const char* qualifiedElementName);
    bool expectNS(const char* nsName);
    void raiseElNotFoundError(const char* elementName);
    void raiseAttributeNotFoundError(const char* attrName);
    void raiseNSNotFoundError(const char* nsName);
    void raiseUnexpectedAttributeValueError(const QString& value, const char* attrName);
    void raiseUnexpectedSecondOccurenceOfElError(const char* elementName);

    //! Decodes boolean attribute. Used by read_b(), read_i(), etc.
    bool readBooleanAttr(const char* attrName, bool defaultValue = false) const;

    QString m_defaultNamespace; //!< stores namespace (for optimization)

    QStack<QByteArray> m_callsNames;
#ifndef NDEBUG
    QStack<QByteArray> m_callsNamesDebug;
#endif

private:
    QString m_fileName;
    bool m_readUndoed;
    QXmlStreamReader::TokenType m_recentType;

    void init();
};

} // namespace MSOOXML

//! kDebug() stream operator. Writes this reader to the debug output in a nicely formatted way.
//! @todo add the same for QXmlStreamWriter
MSOOXML_EXPORT QDebug operator<<(QDebug dbg, const QXmlStreamReader& reader);

#endif //MSOOXMLREADER_H
