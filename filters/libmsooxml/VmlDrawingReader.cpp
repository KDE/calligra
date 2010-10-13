/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "VmlDrawingReader.h"
#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <MsooXmlRelationships.h>
#include <MsooXmlUnits.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoOdfGraphicStyles.h>
#include <limits.h>

#define MSOOXML_CURRENT_NS empty // Without this, the vml methods won't have ns identifier in them
#define MSOOXML_CURRENT_CLASS VmlDrawingReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>

VmlDrawingReaderContext::VmlDrawingReaderContext(MSOOXML::MsooXmlImport& _import, const QString& _path,
    const QString& _file, MSOOXML::MsooXmlRelationships& _relationships) :
    MSOOXML::MsooXmlReaderContext(&_relationships), import(&_import), path(_path), file(_file)
{
}

class VmlDrawingReader::Private
{
public:
    Private() {
    }
    ~Private() {
    }
};

VmlDrawingReader::VmlDrawingReader(KoOdfWriters *writers)
    : MSOOXML::MsooXmlCommonReader(writers)
    , d(new Private)
{
    init();
}

VmlDrawingReader::~VmlDrawingReader()
{
    delete d;
}

void VmlDrawingReader::init()
{
    m_insideGroup = false;
    m_outputFrames = false;
}

QMap<QString, QString> VmlDrawingReader::content()
{
    return m_content;
}

KoFilter::ConversionStatus VmlDrawingReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = static_cast<VmlDrawingReaderContext*>(context);

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }
    readNext();

    kDebug() << *this << namespaceUri();
    if (!expectEl(QList<QByteArray>() << "xml")) {
        return KoFilter::WrongFormat;
    }

    const QString qn(qualifiedName().toString());

    RETURN_IF_ERROR(read_xml())

    if (!expectElEnd(qn)) {
        return KoFilter::WrongFormat;
    }
    kDebug() << "===========finished============";

    return KoFilter::OK;
}

KoFilter::ConversionStatus VmlDrawingReader::read_xml()
{
    unsigned index = 0;

    while (!atEnd()) {
        readNext();
        if (isEndElement() && qualifiedName() == "xml") {
            break;
        }
        if (isStartElement()) {
            if (qualifiedName() == "v:shape") {
                TRY_READ(shape) //from vml
                m_content[m_currentShapeId] = m_imagedataPath;
                ++index;
            }
        }
    }
    return KoFilter::OK;
}

#include <MsooXmlVmlReaderImpl.h>
