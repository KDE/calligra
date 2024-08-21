/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * SPDX-FileCopyrightText: 2010 Sebastian Sauer <sebsauer@kdab.com>
 * SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#ifndef MSOOXMLDIAGRAMREADER_H
#define MSOOXMLDIAGRAMREADER_H

#include <MsooXmlCommonReader.h>
#include <MsooXmlReader.h>

#include "komsooxml_export.h"

namespace MSOOXML
{

namespace Diagram
{
class Context;
class DataModel;
}

class KOMSOOXML_EXPORT MsooXmlDiagramReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    KoGenStyles *m_styles;
    Diagram::Context *m_context;

    int shapeListSize() const;

    explicit MsooXmlDiagramReaderContext(KoGenStyles *styles);
    ~MsooXmlDiagramReaderContext() override;
    void saveIndex(KoXmlWriter *xmlWriter, const QRect &rect);
};

class KOMSOOXML_EXPORT MsooXmlDiagramReader : public MSOOXML::MsooXmlCommonReader
{
public:
    explicit MsooXmlDiagramReader(KoOdfWriters *writers);
    ~MsooXmlDiagramReader() override;
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext *context = nullptr) override;

protected:
    // KoFilter::ConversionStatus read_layoutNode();
    // KoFilter::ConversionStatus read_choose();
    // KoFilter::ConversionStatus read_if();
    // KoFilter::ConversionStatus read_else();
    // KoFilter::ConversionStatus read_forEach();
private:
    MsooXmlDiagramReaderContext *m_context;

    enum Type { InvalidType, DataModelType, LayoutDefType, StyleDefType, ColorsDefType } m_type;
};

}

#endif
