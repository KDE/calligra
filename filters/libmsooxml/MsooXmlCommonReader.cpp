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

#include "MsooXmlCommonReader.h"
#include <KoXmlWriter.h>
#include <KoGenStyles.h>

using namespace MSOOXML;

MsooXmlCommonReader::MsooXmlCommonReader(KoOdfWriters *writers)
        : MsooXmlReader(writers)
{
    init();
}

MsooXmlCommonReader::MsooXmlCommonReader(QIODevice* io, KoOdfWriters *writers)
        : MsooXmlReader(io, writers)
{
    init();
}

MsooXmlCommonReader::~MsooXmlCommonReader()
{
    delete m_currentTextStyleProperties;
}

void MsooXmlCommonReader::init()
{
    m_currentTextStyleProperties = 0;
    m_read_p_args = 0;
    m_lstStyleFound = false;
    m_pPr_lvl = 0;
}

//! CASE #420
bool MsooXmlCommonReader::isDefaultTocStyle(const QString& name) const
{
    if (name == QLatin1String("TOCHeading"))
        return true;
    if (name.startsWith(QLatin1String("TOC"))) {
        const QString num(name.mid(3));
        if (num.length() == 1 && num[0].isDigit() && num[0] != '0')
            return true;
    }
    return false;
}

void MsooXmlCommonReader::setupParagraphStyle()
{
    QString currentParagraphStyleName(mainStyles->lookup(m_currentParagraphStyle));
    if (currentParagraphStyleName.isEmpty()) {
        currentParagraphStyleName = QLatin1String("Standard");
    }
    body->addAttribute("text:style-name", currentParagraphStyleName);
    m_paragraphStyleNameWritten = true;
//kDebug() << "currentParagraphStyleName:" << currentParagraphStyleName;
}
