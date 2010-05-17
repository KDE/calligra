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

#ifndef MSOOXMLCMLREADERMETHODS_H
#define MSOOXMLCMLREADERMETHODS_H

/*! @file MsooXmlVmlReaderMethods.h
    @brief Collects MS VML parser code.

    Included by MsooXmlCommonReader.
    MS VML is not a part of the approved OOXML,
    it is described in ECMA-376 Part 4: "Transitional Migration Features".
    Required for interoperability in any version of MSOOXML.
    VML elements are referenced in ECMA-376 Part 1.

    All page numbers of the ECMA-376 documents refer to the 2nd edition.
*/

protected:
    // w namespace:
    KoFilter::ConversionStatus read_pict();

    // v namespace:
    KoFilter::ConversionStatus read_rect();
    KoFilter::ConversionStatus read_fill();
    KoFilter::ConversionStatus read_VML_background();
    KoFilter::ConversionStatus read_shapetype();
    KoFilter::ConversionStatus read_shape();
    KoFilter::ConversionStatus read_imagedata();
    KoFilter::ConversionStatus read_textbox();
    KoFilter::ConversionStatus read_txbxContent();

    // utils:
    KoFilter::ConversionStatus parseCSS(const QString& style);

    QMap<QByteArray, QString> m_vmlStyle;

    //writer where style:background-image is stored for style:page-layout-properties
    KoXmlWriter* m_pDocBkgImageWriter;

    QString m_imagedataPath; //!< set in read_shape()
    QString m_imagedataFile; //!< set in read_shape()
    QString m_shapeAltText; //!< set in read_shape()
    QString m_shapeTitle; //!< set in read_shape()
#endif
