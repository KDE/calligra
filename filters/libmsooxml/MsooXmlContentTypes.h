/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * SPDX-FileCopyrightText: 2009 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#ifndef MSOOXMLCONTENTTYPES_H
#define MSOOXMLCONTENTTYPES_H

#include "komsooxml_export.h"

#include <KoFilterChain.h>
#include <KoXmlReader.h>
#include <QString>

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
