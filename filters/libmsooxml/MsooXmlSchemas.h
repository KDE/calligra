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

#ifndef MSOOXMLNS_H
#define MSOOXMLNS_H

#include "msooxml_export.h"

namespace MSOOXML {

//! MSOOXML Schemas
class MSOOXML_EXPORT Schemas {
public:
    //! Content types
    static const char* const contentTypes;

    //! Relationships
    static const char* const relationships;

    //! A.1 WordprocessingML
    static const char* const wordprocessingml;

    //! A.2 SpreadsheetML
    static const char* const spreadsheetml;

    //! A.3 PresentationML
    static const char* const presentationml;

    //! DrawingML
    class MSOOXML_EXPORT drawingml {
    public:
        //! A.4 DrawingML - Framework
        static const char* const main;
        static const char* const wordprocessingDrawing;
        static const char* const spreadsheetDrawing;
        static const char* const compatibility;
        static const char* const lockedCanvas;
        static const char* const picture;

        //! A.5 DrawingML - Components
        static const char* const chart;
        static const char* const chartDrawing;
        static const char* const diagram;
    };


    //! A.6 Shared MLs
    class MSOOXML_EXPORT officeDocument {
    public:
        static const char* const math;
        static const char* const bibliography;
        static const char* const characteristics;
        static const char* const customXml;
        static const char* const custom_properties;
        static const char* const docPropsVTypes;
        static const char* const extended_properties;
        static const char* const relationships;
        static const char* const sharedTypes;
    };

    //! A.7 Custom XML Schema References
    static const char* const schemaLibrary;
}; // Schemas

} // MSOOXML namespace

#endif /* MSOOXMLNS_H */
