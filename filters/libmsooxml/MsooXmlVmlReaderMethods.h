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
    KoFilter::ConversionStatus genericReader();
    KoFilter::ConversionStatus read_oval();
    KoFilter::ConversionStatus read_roundrect();
    KoFilter::ConversionStatus read_rect();
    KoFilter::ConversionStatus read_fill();
    KoFilter::ConversionStatus read_VML_background();
    KoFilter::ConversionStatus read_shapetype();
    KoFilter::ConversionStatus read_formulas();
    KoFilter::ConversionStatus read_f();
    KoFilter::ConversionStatus read_shape();
    KoFilter::ConversionStatus read_imagedata();
    KoFilter::ConversionStatus read_textbox();
    KoFilter::ConversionStatus read_group();
    KoFilter::ConversionStatus read_stroke();

    void handleStrokeAndFill(const QXmlStreamAttributes& attrs);

    // w:10 namespace:
    KoFilter::ConversionStatus read_wrap();

    enum FrameStartElement {FrameStart, RectStart, StraightConnectorStart, CustomStart, GroupStart};

    void createFrameStart(FrameStartElement startType = FrameStart);
    KoFilter::ConversionStatus createFrameEnd();

    // utils:
    KoFilter::ConversionStatus parseCSS(const QString& style);

    //writer where style:background-image is stored for style:page-layout-properties
    KoXmlWriter* m_pDocBkgImageWriter;

    struct VMLShapeProperties {
        QString currentEl;

        QMap<QByteArray, QString> vmlStyle;

        QString strokeColor; // stroke color
        QString strokeWidth; // stroke width
        QString lineCapStyle;
        QString joinStyle;
        QString strokeStyleName;
        QString shapeColor; //!< set in read_shape()

        bool wrapRead;
        QString currentShapeId; //!< set in read_shape()
        QString imagedataPath; //!< set in read_shape()
        QString imagedataFile; //!< set in read_shape()
        QString shapeAltText; //!< set in read_shape()
        QString shapeTitle; //!< set in read_shape()

        // Relative group widths
        int groupWidth, groupHeight;

        // Relative group original
        int groupX, groupY;

        // Offset caused by the group parent
        qreal groupXOffset, groupYOffset;

        QString groupWidthUnit; // pt, cm etc.
        QString groupHeightUnit;
        qreal real_groupWidth;
        qreal real_groupHeight;

        QString anchorType;

        //!< Width of the object. Set in read_OLEObject() or read_shape(). Used in writeRect().
        //! If both w:object/v:shape and w:object/o:OLEObject exist, information from v:shape is used.
        QString currentObjectWidthCm;
        QString currentObjectHeightCm; //!< See m_currentObjectWidthCm for description

        // For group shape situation
        bool insideGroup;
        int formulaIndex;
        QString shapeTypeString;
        QString extraShapeFormulas;
        int extraFormulaIndex;
    };

    VMLShapeProperties m_currentVMLProperties;

    // Using stack to make sure correct properties are handled in a case when
    // there are group shapes
    QStack<VMLShapeProperties> m_VMLShapeStack;

    bool m_outputFrames; // Whether read_shape should output something to shape

    QMap<QString, QString> m_shapeTypeStrings;
    QMap<QString, QString> m_strokeTypeStrings;
    QMap<QString, QString> m_fillTypeStrings;
