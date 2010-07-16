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

#ifndef MSOOXMLCT_H
#define MSOOXMLCT_H

#include "msooxml_export.h"

#include <QString>
#include <KoFilterChain.h>
#include <KoXmlReader.h>

class QColor;
class QDomElement;
class KoStyleStack;
class QDomDocument;
class KZip;

namespace MSOOXML
{

//! Content type names for handling MSOOXML formats
class MSOOXML_EXPORT ContentTypes
{
public:
    // common officedocument namespaces
    static const char* const coreProps;
    static const char* const extProps;
    static const char* const theme;

    // wordprocessingml-specific namespaces
    static const char* const wordDocument;
    static const char* const wordSettings;
    static const char* const wordStyles;
    static const char* const wordHeader;
    static const char* const wordFooter;
    static const char* const wordFootnotes;
    static const char* const wordEndnotes;
    static const char* const wordFontTable;
    static const char* const wordWebSettings;
    static const char* const wordTemplate;
    static const char* const wordComments;

    // presentationml-specific namespaces
    static const char* const presentationDocument;
    static const char* const presentationSlide;
    static const char* const presentationSlideLayout;
    static const char* const presentationSlideShow;
    static const char* const presentationTemplate;
    static const char* const presentationNotes;
    static const char* const presentationTableStyles;
    static const char* const presentationProps;
    static const char* const presentationViewProps;
    static const char* const presentationComments;

    // spreadsheetml-specific content types
    static const char* const spreadsheetDocument;
    static const char* const spreadsheetPrinterSettings;
    static const char* const spreadsheetStyles;
    static const char* const spreadsheetWorksheet;
    static const char* const spreadsheetCalcChain;
    static const char* const spreadsheetSharedStrings;
    static const char* const spreadsheetTemplate;
    static const char* const spreadsheetComments;
};

} // MSOOXML namespace

#endif /* MSOOXMLCT_H */
