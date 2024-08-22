/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#include "VmlDrawingReader.h"
#include <KoGenStyles.h>
#include <KoOdfGraphicStyles.h>
#include <KoXmlWriter.h>
#include <MsooXmlRelationships.h>
#include <MsooXmlSchemas.h>
#include <MsooXmlUnits.h>
#include <MsooXmlUtils.h>
#include <limits.h>

#define MSOOXML_CURRENT_NS empty // Without this, the vml methods won't have ns identifier in them
#define MSOOXML_CURRENT_CLASS VmlDrawingReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>

VmlDrawingReaderContext::VmlDrawingReaderContext(MSOOXML::MsooXmlImport &_import,
                                                 const QString &_path,
                                                 const QString &_file,
                                                 MSOOXML::MsooXmlRelationships &_relationships)
    : MSOOXML::MsooXmlReaderContext(&_relationships)
    , import(&_import)
    , path(_path)
    , file(_file)
{
}

class Q_DECL_HIDDEN VmlDrawingReader::Private
{
public:
    Private() = default;
    ~Private() = default;
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
    m_currentVMLProperties.insideGroup = false;
    m_outputFrames = false;
}

QMap<QString, QString> VmlDrawingReader::content()
{
    return m_content;
}

QMap<QString, QString> VmlDrawingReader::frames()
{
    return m_frames;
}

KoFilter::ConversionStatus VmlDrawingReader::read(MSOOXML::MsooXmlReaderContext *context)
{
    m_context = static_cast<VmlDrawingReaderContext *>(context);

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }
    readNext();

    debugMsooXml << *this << namespaceUri();
    if (!expectEl(QList<QByteArray>() << "xml")) {
        return KoFilter::WrongFormat;
    }

    const QString qn(qualifiedName().toString());

    RETURN_IF_ERROR(read_xml())

    if (!expectElEnd(qn)) {
        return KoFilter::WrongFormat;
    }
    debugMsooXml << "===========finished============";

    return KoFilter::OK;
}

KoFilter::ConversionStatus VmlDrawingReader::read_xml()
{
    unsigned index = 0;
    KoXmlWriter *oldBody = nullptr;

    while (!atEnd()) {
        readNext();
        if (isEndElement() && qualifiedName() == QLatin1StringView("xml")) {
            break;
        }
        if (isStartElement()) {
            if (name() == QLatin1StringView("shapetype")) {
                TRY_READ(shapetype)
            } else if (name() == QLatin1StringView("shape")) {
                oldBody = body; // Body protection starts
                QBuffer frameBuf;
                KoXmlWriter frameWriter(&frameBuf);
                body = &frameWriter;
                TRY_READ(shape) // from vml
                m_content[m_currentVMLProperties.currentShapeId] = m_currentVMLProperties.imagedataPath;
                pushCurrentDrawStyle(new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic"));
                createFrameStart();
                popCurrentDrawStyle();
                m_frames[m_currentVMLProperties.currentShapeId] = QString::fromUtf8(frameBuf.buffer(), frameBuf.buffer().size()).append(">");
                body = oldBody; // Body protection ends
                ++index;
            }
        }
    }
    return KoFilter::OK;
}

#include <MsooXmlVmlReaderImpl.h>
