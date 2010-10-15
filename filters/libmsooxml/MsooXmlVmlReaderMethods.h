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

    // v namespace:
    KoFilter::ConversionStatus read_roundrect();
    KoFilter::ConversionStatus read_rect();
    KoFilter::ConversionStatus read_fill();
    KoFilter::ConversionStatus read_VML_background();
    KoFilter::ConversionStatus read_shapetype();
    KoFilter::ConversionStatus read_shape();
    KoFilter::ConversionStatus read_imagedata();
    KoFilter::ConversionStatus read_textbox();
    KoFilter::ConversionStatus read_group();
    KoFilter::ConversionStatus read_stroke();

    void createFrameStart();
    KoFilter::ConversionStatus createFrameEnd();

    // utils:
    KoFilter::ConversionStatus parseCSS(const QString& style);

    QMap<QByteArray, QString> m_vmlStyle;

    //writer where style:background-image is stored for style:page-layout-properties
    KoXmlWriter* m_pDocBkgImageWriter;

    /*! true if w:object/v:shape or w:object/o:OLEObject has been handled, .
     When w:object/o:OLEObject is visited and m_objectRectInitialized is true, handling
     w:object/o:OLEObject is (except for copying the OLE binary) skipped because
     w:object/v:shape is of higher priority.
     This flag is reset to false each time read_object() is called. */
    bool m_objectRectInitialized;

    //!< Width of the object. Set in read_OLEObject() or read_shape(). Used in writeRect().
    //! If both w:object/v:shape and w:object/o:OLEObject exist, information from v:shape is used.
    QString m_currentObjectWidthCm;

    QString m_currentObjectHeightCm; //!< See m_currentObjectWidthCm for description
    QString m_currentObjectXCm; //!< See m_currentObjectWidthCm for description
    QString m_currentObjectYCm; //!< See m_currentObjectWidthCm for description

    QString m_imagedataPath; //!< set in read_shape()
    QString m_imagedataFile; //!< set in read_shape()
    QString m_shapeAltText; //!< set in read_shape()
    QString m_shapeTitle; //!< set in read_shape()
    QString m_shapeColor; //!< set in read_shape()
    QString m_currentShapeId; //!< set in read_shape()

    QString m_strokeColor; // stroke color
    qreal m_strokeWidth; // stroke width

    bool m_outputFrames; // Whether read_shape should output something to shape

    // For group shape situation
    bool m_insideGroup;

    // Relative group widths
    int m_groupWidth;
    int m_groupHeight;

    // Relative group original
    int m_groupX;
    int m_groupY;

    QString m_groupUnit; // pt, cm etc.
    qreal m_real_groupWidth;
    qreal m_real_groupHeight;
