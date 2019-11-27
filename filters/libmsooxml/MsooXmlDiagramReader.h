/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * Copyright (C) 2010 Sebastian Sauer <sebsauer@kdab.com>
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

#ifndef MSOOXMLDIAGRAMREADER_H
#define MSOOXMLDIAGRAMREADER_H

#include <MsooXmlReader.h>
#include <MsooXmlCommonReader.h>

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
    KoGenStyles* m_styles;
    Diagram::Context* m_context;

    int shapeListSize() const;

    explicit MsooXmlDiagramReaderContext(KoGenStyles* styles);
    ~MsooXmlDiagramReaderContext() override;
    void saveIndex(KoXmlWriter* xmlWriter, const QRect &rect);
};

class KOMSOOXML_EXPORT MsooXmlDiagramReader : public MSOOXML::MsooXmlCommonReader
{
public:
    explicit MsooXmlDiagramReader(KoOdfWriters *writers);
    ~MsooXmlDiagramReader() override;
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0) override;

protected:
    //KoFilter::ConversionStatus read_layoutNode();
    //KoFilter::ConversionStatus read_choose();
    //KoFilter::ConversionStatus read_if();
    //KoFilter::ConversionStatus read_else();
    //KoFilter::ConversionStatus read_forEach();
private:
    MsooXmlDiagramReaderContext *m_context;

    enum Type {
        InvalidType,
        DataModelType,
        LayoutDefType,
        StyleDefType,
        ColorsDefType
    } m_type;
};

}

#endif
