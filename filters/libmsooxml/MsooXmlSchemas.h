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

#ifndef MSOOXMLSCHEMAS_H
#define MSOOXMLSCHEMAS_H

#include "komsooxml_export.h"

namespace MSOOXML
{

//! MSOOXML Schemas
class KOMSOOXML_EXPORT Schemas
{
public:
    //! Content types
    static const char contentTypes[];

    //! Relationships
    static const char relationships[];

    //! Core properties (document properties)
    static const char core_properties[];

    //! Dublin Core vocabulary
    //@todo generic, move somewhere else?
    static const char dublin_core[];

    //! A.1 WordprocessingML
    static const char wordprocessingml[];

    //! A.2 SpreadsheetML
    static const char spreadsheetml[];

    //! A.3 PresentationML
    static const char presentationml[];

    //! DrawingML
    class KOMSOOXML_EXPORT drawingml
    {
    public:
        //! A.4 DrawingML - Framework
        static const char main[];
        static const char wordprocessingDrawing[];
        static const char spreadsheetDrawing[];
        static const char compatibility[];
        static const char lockedCanvas[];
        static const char picture[];

        //! A.5 DrawingML - Components
        static const char chart[];
        static const char chartDrawing[];
        static const char diagram[];
    };

    //! A.6 Shared MLs
    class KOMSOOXML_EXPORT officeDocument
    {
    public:
        static const char math[];
        static const char bibliography[];
        static const char characteristics[];
        static const char customXml[];
        static const char custom_properties[];
        static const char docPropsVTypes[];
        static const char extended_properties[];
        static const char relationships[];
        static const char sharedTypes[];
    };

    //! A.7 Custom XML Schema References
    static const char schemaLibrary[];
}; // Schemas

} // MSOOXML namespace

#endif /* MSOOXMLSCHEMAS_H */
