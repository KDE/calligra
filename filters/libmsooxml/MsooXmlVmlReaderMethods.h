/*
 * This file is part of Office 2007 Filters for Calligra
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

    enum FrameStartElement {FrameStart, EllipseStart, RectStart, LineStart, CustomStart, GroupStart};

    // v namespace:
    KoFilter::ConversionStatus genericReader(FrameStartElement startType);
    KoFilter::ConversionStatus read_oval();
    KoFilter::ConversionStatus read_line();
    KoFilter::ConversionStatus read_roundrect();
    KoFilter::ConversionStatus read_rect();
    KoFilter::ConversionStatus read_fill();
    KoFilter::ConversionStatus read_VML_background();
    KoFilter::ConversionStatus read_shapetype();
    KoFilter::ConversionStatus read_formulas();
    KoFilter::ConversionStatus read_path();
    KoFilter::ConversionStatus read_f();
    KoFilter::ConversionStatus read_shape();
    KoFilter::ConversionStatus read_imagedata();
    KoFilter::ConversionStatus read_textbox();
    KoFilter::ConversionStatus read_group();
    KoFilter::ConversionStatus read_stroke();
    KoFilter::ConversionStatus read_shadow();

    void handlePathValues(const QXmlStreamAttributes& attrs);
    void handleStrokeAndFill(const QXmlStreamAttributes& attrs);
    void takeDefaultValues();

    QString rgbColor(QString color);

    // w:10 namespace:
    KoFilter::ConversionStatus read_wrap();

    void createFrameStart(FrameStartElement startType = FrameStart);

    // utils:
    KoFilter::ConversionStatus parseCSS(const QString& style);

    //writer where style:background-image is stored for style:page-layout-properties
    KoXmlWriter* m_pDocBkgImageWriter;

public:

    struct VMLShapeProperties {
        QString currentEl;

        QMap<QByteArray, QString> vmlStyle;

        QString strokeColor; // stroke color
        QString strokeWidth; // stroke width
        QString lineCapStyle;
        QString joinStyle;
        QString strokeStyleName;
        QString fillType;
        QString gradientStyle;
        QString shapeColor; //!< set in read_shape()
        QString shapeSecondaryColor; // used eg. for some gradients

        qreal opacity;

        bool wrapRead;
        QString currentShapeId; //!< set in read_shape()
        QString imagedataPath; //!< set in read_shape()
        QString shapeAltText; //!< set in read_shape()
        QString shapeTitle; //!< set in read_shape()

        bool stroked, filled, shadowed;

        QString shadowColor;
        QString shadowXOffset, shadowYOffset;
        qreal shadowOpacity;

        QString anchorType;

        int formulaIndex;
        QString shapeTypeString;
        QString extraShapeFormulas;
        QString normalFormulas;
        QString modifiers;
        QString viewBox;
        QString shapePath;
        int extraFormulaIndex;

        QString internalMarginLeft;
        QString internalMarginRight;
        QString internalMarginTop;
        QString internalMarginBottom;

        QString marginLeft;
        QString marginTop;
        QString marginRight;
        QString marginBottom;

        bool fitTextToShape, fitShapeToText;

        // Parameters for group shape situation
        bool insideGroup;
        int groupWidth, groupHeight; // Relative group extends
        int groupX, groupY; // Relative group origin
        qreal groupXOffset, groupYOffset; // Offset caused by the group parent
        qreal real_groupWidth, real_groupHeight;
    };

    // Elements defined by v:shapeType
    QMap<QString, VMLShapeProperties> m_definedShapeTypes;

protected:

    VMLShapeProperties m_currentVMLProperties;

    // Using stack to make sure correct properties are handled in a case when
    // there are group shapes
    QStack<VMLShapeProperties> m_VMLShapeStack;

    bool m_outputFrames; // Whether read_shape should output something to shape
