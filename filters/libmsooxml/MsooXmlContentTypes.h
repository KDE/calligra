/*
 * This file is part of Office 2007 Filters for Calligra
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

#ifndef MSOOXMLCONTENTTYPES_H
#define MSOOXMLCONTENTTYPES_H

#include "komsooxml_export.h"

#include <QString>
#include <KoFilterChain.h>
#include <KoXmlReader.h>


namespace MSOOXML
{

//! Content type names for handling MSOOXML formats
class KOMSOOXML_EXPORT ContentTypes
{
public:
    // common officedocument namespaces
    static const char coreProps[];
    static const char extProps[];
    static const char theme[];

    // wordprocessingml-specific namespaces
    static const char wordDocument[];
    static const char wordSettings[];
    static const char wordStyles[];
    static const char wordHeader[];
    static const char wordFooter[];
    static const char wordFootnotes[];
    static const char wordEndnotes[];
    static const char wordFontTable[];
    static const char wordWebSettings[];
    static const char wordTemplate[];
    static const char wordComments[];

    // presentationml-specific namespaces
    static const char presentationDocument[];
    static const char presentationSlide[];
    static const char presentationSlideLayout[];
    static const char presentationSlideShow[];
    static const char presentationTemplate[];
    static const char presentationNotes[];
    static const char presentationTableStyles[];
    static const char presentationProps[];
    static const char presentationViewProps[];
    static const char presentationComments[];

    // spreadsheetml-specific content types
    static const char spreadsheetDocument[];
    static const char spreadsheetMacroDocument[];
    static const char spreadsheetPrinterSettings[];
    static const char spreadsheetStyles[];
    static const char spreadsheetWorksheet[];
    static const char spreadsheetCalcChain[];
    static const char spreadsheetSharedStrings[];
    static const char spreadsheetTemplate[];
    static const char spreadsheetComments[];
};

} // MSOOXML namespace

#endif /* MSOOXMLCONTENTTYPES_H */
