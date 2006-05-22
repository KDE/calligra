/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2003 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDChart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KDChart licenses may use this file in
 ** accordance with the KDChart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.klaralvdalens-datakonsult.se/?page=products for
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@klaralvdalens-datakonsult.se if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#include "KDChartParams.h"
#include "KDXMLTools.h"

#include <qintdict.h>

/**
  \fn QTextStream& operator<<( QTextStream& s, const KDChartParams& p );

  Writes the KDChartParams object p as an XML document to the text stream s.

  \param s the text stream to write to
  \param p the KDChartParams object to write
  \return the text stream after the write operation
  */
QTextStream& operator<<( QTextStream& s, const KDChartParams& p )
{
    QDomDocument document = p.saveXML();
    s << document.toString();

    return s;
}

/**
  Helper method saving the axis parameters to an XML file,
  this is called by saveXML() only.
*/
void KDChartParams::saveAxesToXML(QDomDocument& doc, QDomElement& docRoot) const
{
    // the AxisSettings elements
    for( int axis = 0; axis < 13; axis++ ) {
        QDomElement axisSettingsElement =
            doc.createElement( "AxisSettings" );
        docRoot.appendChild( axisSettingsElement );
        axisSettingsElement.setAttribute( "Dataset",
                _axisSettings[axis].dataset );
        axisSettingsElement.setAttribute( "Dataset2",
                _axisSettings[axis].dataset2 );
        axisSettingsElement.setAttribute( "Chart",
                _axisSettings[axis].chart );
        {
            // the Type element
            KDXML::createStringNode( doc, axisSettingsElement, "Type",
                    KDChartAxisParams::axisTypeToString( _axisSettings[axis].params._axisType ) );

            // the Visible element
            KDXML::createBoolNode( doc, axisSettingsElement, "Visible",
                    _axisSettings[axis].params._axisVisible );

            // the LabelsTouchEdges element
            KDXML::createBoolNode( doc, axisSettingsElement, "LabelsTouchEdges",
                    _axisSettings[axis].params._axisLabelsTouchEdges );

            // the AreaMode element
            KDXML::createStringNode( doc, axisSettingsElement, "AreaMode",
                    KDChartAxisParams::axisAreaModeToString( _axisSettings[axis].params._axisAreaMode ) );

            // the UseAvailableSpaceFrom element
            KDXML::createIntNode( doc, axisSettingsElement, "UseAvailableSpaceFrom",
                    _axisSettings[axis].params._axisUseAvailableSpaceFrom );

            // the UseAvailableSpaceTo element
            KDXML::createIntNode( doc, axisSettingsElement, "UseAvailableSpaceTo",
                    _axisSettings[axis].params._axisUseAvailableSpaceTo );

            // the IsometricReferenceAxis element
            KDXML::createIntNode( doc, axisSettingsElement, "IsometricReferenceAxis",
                    _axisSettings[axis].params._axisIsoRefAxis );

            // the AreaMin element
            KDXML::createIntNode( doc, axisSettingsElement, "AreaMin",
                    _axisSettings[axis].params._axisAreaMin );

            // the AreaMax element
            KDXML::createIntNode( doc, axisSettingsElement, "AreaMax",
                    _axisSettings[axis].params._axisAreaMax );

            // the CalcMode element
            KDXML::createStringNode( doc, axisSettingsElement, "CalcMode",
                    KDChartAxisParams::axisCalcModeToString( _axisSettings[axis].params._axisCalcMode ) );

            // the TrueAreaSize element
            KDXML::createIntNode( doc, axisSettingsElement, "TrueAreaSize",
                    _axisSettings[axis].params._axisTrueAreaSize );

            // the TrueAreaRect element
            KDXML::createRectNode( doc, axisSettingsElement, "TrueAreaRect",
                    _axisSettings[axis].params._axisTrueAreaRect );

            // the ShowSubDelimiters element
            KDXML::createBoolNode( doc, axisSettingsElement, "ShowSubDelimiters",
                    _axisSettings[axis].params._axisShowSubDelimiters );

            // the LineVisible element
            KDXML::createBoolNode( doc, axisSettingsElement, "LineVisible",
                    _axisSettings[axis].params._axisLineVisible );

            // the LineWidth element
            KDXML::createIntNode( doc, axisSettingsElement, "LineWidth",
                    _axisSettings[axis].params._axisLineWidth );

            // the TrueLineWidth element
            KDXML::createIntNode( doc, axisSettingsElement, "TrueLineWidth",
                    _axisSettings[axis].params._axisTrueLineWidth );

            // the LineColor element
            KDXML::createColorNode( doc, axisSettingsElement, "LineColor",
                    _axisSettings[axis].params._axisLineColor );

            // the ShowGrid element
            KDXML::createBoolNode( doc, axisSettingsElement, "ShowGrid",
                    _axisSettings[axis].params._axisShowGrid );

            // the GridColor element
            KDXML::createColorNode( doc, axisSettingsElement, "GridColor",
                    _axisSettings[axis].params._axisGridColor );

            // the GridLineWidth element
            KDXML::createIntNode( doc, axisSettingsElement, "GridLineWidth",
                    _axisSettings[axis].params._axisGridLineWidth );

            // the GridStyle element
            KDXML::createStringNode( doc, axisSettingsElement, "GridStyle",
                    KDXML::penStyleToString( _axisSettings[axis].params._axisGridStyle ) );

            // the GridSubColor element
            KDXML::createColorNode( doc, axisSettingsElement, "GridSubColor",
                    _axisSettings[axis].params._axisGridSubColor );

            // the GridSubLineWidth element
            KDXML::createIntNode( doc, axisSettingsElement, "GridSubLineWidth",
                    _axisSettings[axis].params._axisGridSubLineWidth );

            // the GridSubStyle element
            KDXML::createStringNode( doc, axisSettingsElement, "GridSubStyle",
                    KDXML::penStyleToString( _axisSettings[axis].params._axisGridSubStyle ) );

            // the ZeroLineColor element
            KDXML::createColorNode( doc, axisSettingsElement, "ZeroLineColor",
                    _axisSettings[axis].params._axisZeroLineColor );

            // the LabelsVisible element
            KDXML::createBoolNode( doc, axisSettingsElement, "LabelsVisible",
                    _axisSettings[axis].params._axisLabelsVisible );

            // the LabelsFont element
            createChartFontNode( doc, axisSettingsElement, "LabelsFont",
                    _axisSettings[axis].params._axisLabelsFont,
                    _axisSettings[axis].params._axisLabelsFontUseRelSize,
                    _axisSettings[axis].params._axisLabelsFontRelSize,
                    _axisSettings[axis].params._axisLabelsFontMinSize );

            // the LabelsDontShrinkFont element
            KDXML::createBoolNode( doc, axisSettingsElement, "LabelsDontShrinkFont",
                    _axisSettings[axis].params._axisLabelsDontShrinkFont );

            // the LabelsDontAutoRotate element
            KDXML::createBoolNode( doc, axisSettingsElement, "LabelsDontAutoRotate",
                    _axisSettings[axis].params._axisLabelsDontAutoRotate );

            // the LabelsRotation element
            KDXML::createIntNode( doc, axisSettingsElement, "LabelsRotation",
                    _axisSettings[axis].params._axisLabelsRotation );

            // the LabelsLeaveOut element
            KDXML::createIntNode( doc, axisSettingsElement, "LabelsLeaveOut",
                    _axisSettings[axis].params._axisValueLeaveOut );

            // the LabelsColor element
            KDXML::createColorNode( doc, axisSettingsElement, "LabelsColor",
                    _axisSettings[axis].params._axisLabelsColor );

            // the SteadyValueCalc element
            KDXML::createBoolNode( doc, axisSettingsElement, "SteadyValueCalc",
                    _axisSettings[axis].params._axisSteadyValueCalc );

            // the ValueStart element
            if( ! ( KDCHART_AXIS_LABELS_AUTO_LIMIT == _axisSettings[axis].params._axisValueStart ))
                createChartValueNode( doc, axisSettingsElement, "ValueStart",
                        _axisSettings[axis].params._axisValueStart,
                        0.0,
                        0 );

            // the ValueStartIsExact element
            KDXML::createBoolNode( doc, axisSettingsElement, "ValueStartIsExact",
                    _axisSettings[axis].params._axisValueStartIsExact );

            // the ValueEnd element
            if( ! ( KDCHART_AXIS_LABELS_AUTO_LIMIT == _axisSettings[axis].params._axisValueEnd ))
                createChartValueNode( doc, axisSettingsElement, "ValueEnd",
                        _axisSettings[axis].params._axisValueEnd,
                        0.0,
                        0 );

            // the ValueDelta element
            if( ! ( KDCHART_AXIS_LABELS_AUTO_DELTA == _axisSettings[axis].params._axisValueDelta ))
                KDXML::createDoubleNode( doc, axisSettingsElement, "ValueDelta",
                        _axisSettings[axis].params._axisValueDelta );
            KDXML::createIntNode( doc, axisSettingsElement, "ValueDeltaScale",
                    _axisSettings[axis].params._axisValueDeltaScale );

            // the ValuesDecreasing element
            KDXML::createBoolNode( doc, axisSettingsElement, "ValuesDecreasing",
                    _axisSettings[axis].params._axisValuesDecreasing );

            // the TrueLow element
            KDXML::createDoubleNode( doc, axisSettingsElement, "TrueLow",
                    _axisSettings[axis].params._trueLow );

            // the TrueHigh element
            KDXML::createDoubleNode( doc, axisSettingsElement, "TrueHigh",
                    _axisSettings[axis].params._trueHigh );

            // the TrueDelta element
            KDXML::createDoubleNode( doc, axisSettingsElement, "TrueDelta",
                    _axisSettings[axis].params._trueDelta );

            // the ZeroLineStart element
            QDomElement zeroLineStartElement = doc.createElement( "ZeroLineStart" );
            axisSettingsElement.appendChild( zeroLineStartElement );
            zeroLineStartElement.setAttribute( "X", _axisSettings[axis].params._axisZeroLineStartX );
            zeroLineStartElement.setAttribute( "Y", _axisSettings[axis].params._axisZeroLineStartY );

            // the DigitsBehindComma element
            KDXML::createIntNode( doc, axisSettingsElement, "DigitsBehindComma",
                    _axisSettings[axis].params._axisDigitsBehindComma );

            // the LabelsDateTimeFormat element
            KDXML::createStringNode( doc, axisSettingsElement, "LabelsDateTimeFormat",
                    _axisSettings[axis].params._axisLabelsDateTimeFormat );

            // the MaxEmptyInnerSpan element
            KDXML::createIntNode( doc, axisSettingsElement, "MaxEmptyInnerSpan",
                    _axisSettings[axis].params._axisMaxEmptyInnerSpan );

            // the LabelsFromDataRow element
            KDXML::createStringNode( doc, axisSettingsElement, "LabelsFromDataRow",
                    KDChartAxisParams::labelsFromDataRowToString( _axisSettings[axis].params._takeLabelsFromDataRow ) );

            // the TextsDataRow element
            KDXML::createIntNode( doc, axisSettingsElement, "TextsDataRow",
                    _axisSettings[axis].params._labelTextsDataRow );

            // the LabelString elements
            KDXML::createStringListNodes( doc, axisSettingsElement, "LabelString",
                    &_axisSettings[axis].params._axisLabelStringList );

            // the ShortLabelString elements
            KDXML::createStringListNodes( doc, axisSettingsElement, "ShortLabelString",
                    &_axisSettings[axis].params._axisShortLabelsStringList );

            // the LabelText elements
            KDXML::createStringListNodes( doc, axisSettingsElement, "LabelText",
                    &_axisSettings[axis].params._axisLabelTexts );

            // the LabelTextsDirty element
            KDXML::createBoolNode( doc, axisSettingsElement, "LabelTextsDirty",
                    _axisSettings[axis].params._axisLabelTextsDirty );

            // labels formatting:

            // the extra FirstLabelText element
            KDXML::createStringNode( doc, axisSettingsElement, "FirstLabelReplacementText",
                    _axisSettings[axis].params._axisFirstLabelText );

            // the extra LastLabelText element
            KDXML::createStringNode( doc, axisSettingsElement, "LastLabelReplacementText",
                    _axisSettings[axis].params._axisLastLabelText );

            // the LabelsDivPow10 element
            KDXML::createIntNode( doc, axisSettingsElement, "LabelsDivPow10",
                    _axisSettings[axis].params._axisLabelsDivPow10 );

            // the LabelsDecimalPoint element
            KDXML::createStringNode( doc, axisSettingsElement, "LabelsDecimalPoint",
                    _axisSettings[axis].params._axisLabelsDecimalPoint );

            // the LabelsThousandsPoint element
            KDXML::createStringNode( doc, axisSettingsElement, "LabelsThousandsPoint",
                    _axisSettings[axis].params._axisLabelsThousandsPoint );

            // the LabelsPrefix element
            KDXML::createStringNode( doc, axisSettingsElement, "LabelsPrefix",
                    _axisSettings[axis].params._axisLabelsPrefix );

            // the LabelsPostfix element
            KDXML::createStringNode( doc, axisSettingsElement, "LabelsPostfix",
                    _axisSettings[axis].params._axisLabelsPostfix );

            // the LabelsTotalLen element
            KDXML::createIntNode( doc, axisSettingsElement, "LabelsTotalLen",
                    _axisSettings[axis].params._axisLabelsTotalLen );

            // the LabelsPadFill element
            KDXML::createStringNode( doc, axisSettingsElement, "LabelsPadFill",
                    _axisSettings[axis].params._axisLabelsPadFill );

            // the LabelsBlockAlign element
            KDXML::createBoolNode( doc, axisSettingsElement, "LabelsBlockAlign",
                    _axisSettings[axis].params._axisLabelsBlockAlign );
        }
    }
}


/**
  Saves the parameters to an XML document.

  \return the XML document that represents the parameters
  \sa loadXML
  */
QDomDocument KDChartParams::saveXML( bool withPI ) const
{
    // Create an inital DOM document
    QString docstart = "<ChartParams/>";

    QDomDocument doc( "ChartParams" );
    doc.setContent( docstart );
    if( withPI )
        doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement docRoot = doc.documentElement();
    docRoot.setAttribute( "xmlns", "http://www.klaralvdalens-datakonsult.se/kdchart" );
    docRoot.setAttribute( "xmlns:xsi", "http://www.w3.org/2000/10/XMLSchema-instance" );
    docRoot.setAttribute( "xsi:schemaLocation", "http://www.klaralvdalens-datakonsult.se/kdchart" );

    // the ChartType element
    QDomElement chartTypeElement = doc.createElement( "ChartType" );
    docRoot.appendChild( chartTypeElement );
    chartTypeElement.setAttribute( "primary",
            chartTypeToString( _chartType ) );
    chartTypeElement.setAttribute( "secondary",
            chartTypeToString( _additionalChartType ) );


    // the NumValues element
    KDXML::createIntNode( doc, docRoot, "NumValues", _numValues );

    // the ModeAndChartMap element
    QDomElement modeAndChartMapElement =
        doc.createElement( "ModeAndChartMap" );
    docRoot.appendChild( modeAndChartMapElement );
    for( QMap<uint,KDChartParams::ModeAndChart>::ConstIterator it = _dataSourceModeAndChart.begin();
            it != _dataSourceModeAndChart.end(); ++it ) {
        // Dataset element
        QDomElement datasetElement = doc.createElement( "Dataset" );
        QDomText datasetContent =
            doc.createTextNode( QString::number( it.key() ) );
        datasetElement.appendChild( datasetContent );
        modeAndChartMapElement.appendChild( datasetElement );
        // ModeAndChart element
        QDomElement modeAndChartElement = doc.createElement( "ModeAndChart" );
        modeAndChartElement.setAttribute( "Mode", chartSourceModeToString( it.data().mode() ) );
        modeAndChartElement.setAttribute( "Chart", it.data().chart() );
        modeAndChartMapElement.appendChild( modeAndChartElement );
    }

    // the PropertySetMap element
    QDomElement propertySetMapElement =
        doc.createElement( "PropertySetMap" );
    docRoot.appendChild( propertySetMapElement );
    QIntDictIterator<KDChartPropertySet> it2( _propertySetList );
    for( ; it2.current(); ++it2 )
        propertySetMapElement.appendChild( it2.current()->saveXML( doc ) );

    KDXML::createBoolNode( doc, docRoot, "ChartSourceModeWasUsed",
            _setChartSourceModeWasUsed );

    // the MaxDatasetSourceMode element
    KDXML::createIntNode( doc, docRoot, "MaxDatasetSourceMode",
            _maxDatasetSourceMode );

    // the ColorSettings element
    QDomElement colorSettingsElement =
        doc.createElement( "ColorSettings" );
    docRoot.appendChild( colorSettingsElement );

    {
        // the DataColors element
        createColorMapNode( doc, colorSettingsElement,
                "DataColors", _dataColors );

        // the MaxDatasetColor element
        KDXML::createIntNode( doc, colorSettingsElement, "MaxDatasetColor",
                _maxDatasetColor );

        // the ShadowBrightnessFactor element
        KDXML::createDoubleNode( doc, colorSettingsElement,
                "ShadowBrightnessFactor",
                _shadowBrightnessFactor );

        // the ShadowPattern element
        KDXML::createStringNode( doc, colorSettingsElement,
                "ShadowPattern",
                KDXML::brushStyleToString(_shadowPattern ) );

        // the ThreeDShadowColors element
        KDXML::createBoolNode( doc, colorSettingsElement,
                "ThreeDShadowColors",
                _threeDShadowColors );

        // the DataColorsShadow1 element
        createColorMapNode( doc, colorSettingsElement,
                "DataColorsShadow1",
                _dataColorsShadow1 );

        // the DataColorsShadow2 element
        createColorMapNode( doc, colorSettingsElement,
                "DataColorsShadow2",
                _dataColorsShadow2 );

        // the OutlineDataColor element
        KDXML::createColorNode( doc, colorSettingsElement,
                "OutlineDataColor",
                _outlineDataColor );

        // the OutlineDataLineWidth element
        KDXML::createIntNode( doc, colorSettingsElement,
                "OutlineDataLineWidth",
                _outlineDataLineWidth );

        // the OutlineDataLineStyle element
        QDomElement outlineDataLineStyleElement =
            doc.createElement( "OutlineDataLineStyle" );
        colorSettingsElement.appendChild( outlineDataLineStyleElement );
        outlineDataLineStyleElement.setAttribute( "Style",
                KDXML::penStyleToString( _outlineDataLineStyle ) );
    }


    // the BarSettings elemenet
    QDomElement barSettingsElement =
        doc.createElement( "BarSettings" );
    docRoot.appendChild( barSettingsElement );

    {
        // the SubType element
        KDXML::createStringNode( doc, barSettingsElement,
                "SubType", KDChartParams::barChartSubTypeToString( _barChartSubType ) );

        KDXML::createBoolNode( doc, barSettingsElement,
                "ThreeDBars", _threeDBars );

        KDXML::createDoubleNode( doc, barSettingsElement,
                "ThreeDBarDepth", _threeDBarDepth );

        KDXML::createIntNode( doc, barSettingsElement,
                "DatasetGap", _datasetGap );

        KDXML::createBoolNode( doc, barSettingsElement,
                "DatasetGapIsRelative", _datasetGapIsRelative );

        KDXML::createIntNode( doc, barSettingsElement,
                "ValueBlockGap", _valueBlockGap );

        KDXML::createBoolNode( doc, barSettingsElement,
                "ValueBlockGapIsRelative",
                _valueBlockGapIsRelative );

        KDXML::createIntNode( doc, barSettingsElement,
                "BarWidth", _barWidth );

        KDXML::createBoolNode( doc, barSettingsElement,
                "SolidExcessArrows", _solidExcessArrows );
    }


    // the LineSettings element
    QDomElement lineSettingsElement =
        doc.createElement( "LineSettings" );
    docRoot.appendChild( lineSettingsElement );

    {
        // the SubType element
        KDXML::createStringNode( doc, lineSettingsElement,
                "SubType", KDChartParams::lineChartSubTypeToString( _lineChartSubType ) );

        // the Marker element
        KDXML::createBoolNode( doc, lineSettingsElement,
                "Marker", _lineMarker );

        // the MarkerStyles elements
        {for( QMap<uint,KDChartParams::LineMarkerStyle>::ConstIterator it = _lineMarkerStyles.begin();
                it != _lineMarkerStyles.end(); ++it ) {
            QDomElement markerStyleElement = doc.createElement( "MarkerStyle" );
            lineSettingsElement.appendChild( markerStyleElement );
            markerStyleElement.setAttribute( "Dataset", it.key() );
            markerStyleElement.setAttribute( "Style", KDChartParams::lineMarkerStyleToString( it.data() ) );
        }}

        // the MarkerSize element
        KDXML::createSizeNode( doc, lineSettingsElement,
                "MarkerSize", _lineMarkerSize );

        // the LineWidth element
        KDXML::createIntNode( doc, lineSettingsElement,
                "LineWidth", _lineWidth );

        // the LineColor element
        KDXML::createColorNode( doc, lineSettingsElement,
                "LineColor",
                _lineColor );

        // the LineStyle element
        QDomElement lineStyleElement =
            doc.createElement( "LineStyle" );
        lineSettingsElement.appendChild( lineStyleElement );
        lineStyleElement.setAttribute( "Style",
                KDXML::penStyleToString( _lineStyle ) );

        // the DatasetLineStyles elements
        {for( QMap<uint,  PenStyle>::ConstIterator it = _datasetLineStyles.begin();
                it != _datasetLineStyles.end(); ++it ) {
            QDomElement lineStyleElement = doc.createElement( "DatasetLineStyle" );
            lineSettingsElement.appendChild( lineStyleElement );
            lineStyleElement.setAttribute( "Dataset", it.key() );
            lineStyleElement.setAttribute( "Style", KDXML::penStyleToString( it.data() ) );
        }}

        // the ThreeD element
        KDXML::createBoolNode( doc, lineSettingsElement,
                "ThreeD", _threeDLines );

        // the ThreeDDepth element
        KDXML::createIntNode( doc, lineSettingsElement,
                "ThreeDDepth", _threeDLineDepth );

        // the ThreeDXRotation element
        KDXML::createIntNode( doc, lineSettingsElement,
                "ThreeDXRotation", _threeDLineXRotation );

        // the ThreeDYRotation element
        KDXML::createIntNode( doc, lineSettingsElement,
                "ThreeDYRotation", _threeDLineYRotation );
    }


    // the AreaSettings element
    QDomElement areaSettingsElement =
        doc.createElement( "AreaSettings" );
    docRoot.appendChild( areaSettingsElement );

    {
        // the SubType element
        KDXML::createStringNode( doc, areaSettingsElement, "SubType",
                KDChartParams::areaChartSubTypeToString( _areaChartSubType ) );

        // the Location elemenet
        KDXML::createStringNode( doc, areaSettingsElement, "Location",
                KDChartParams::areaLocationToString( _areaLocation ) );
    }


    // the PieRingSettings element
    QDomElement pieRingSettingsElement =
        doc.createElement( "PieRingSettings" );
    docRoot.appendChild( pieRingSettingsElement );

    {
        // the Explode element
        KDXML::createBoolNode( doc, pieRingSettingsElement, "Explode",
                _explode );

        // the DefaultExplodeFactor element
        KDXML::createDoubleNode( doc, pieRingSettingsElement, "DefaultExplodeFactor",
                _explodeFactor );

        // the ExplodeFactors element
        createDoubleMapNode( doc, pieRingSettingsElement, "ExplodeFactors",
                _explodeFactors );

        // the ExplodeSegment element
        for( QValueList<int>::ConstIterator it = _explodeList.begin();
                it != _explodeList.end(); ++it )
            KDXML::createIntNode( doc, pieRingSettingsElement,
                    "ExplodeSegment", *it );

        // the ThreeDPies element
        KDXML::createBoolNode( doc, pieRingSettingsElement, "ThreeDPies",
                _threeDPies );

        // the ThreeDPieHeight element
        KDXML::createIntNode( doc, pieRingSettingsElement, "ThreeDPieHeight",
                _threeDPieHeight );

        // the PieStart element
        KDXML::createIntNode( doc, pieRingSettingsElement, "PieStart",
                _pieStart );

        // the RingStart element
        KDXML::createIntNode( doc, pieRingSettingsElement, "RingStart",
                _ringStart );

        // the RelativeRingThickness element
        KDXML::createBoolNode( doc, pieRingSettingsElement,
                "RelativeRingThickness", _relativeRingThickness );
    }

    // the HiLoSettings element
    QDomElement hiLoSettingsElement =
        doc.createElement( "HiLoSettings" );
    docRoot.appendChild( hiLoSettingsElement );
    {
        // the SubType element
        KDXML::createStringNode( doc, hiLoSettingsElement, "SubType",
                KDChartParams::hiLoChartSubTypeToString( _hiLoChartSubType ) );

        // the PrintLowValues element
        KDXML::createBoolNode( doc, hiLoSettingsElement, "PrintLowValues",
                _hiLoChartPrintLowValues );

        // the LowValuesFont element
        createChartFontNode( doc, hiLoSettingsElement, "LowValuesFont",
                _hiLoChartLowValuesFont,
                _hiLoChartLowValuesUseFontRelSize,
                _hiLoChartLowValuesFontRelSize );

        // the LowValuesColor element
        KDXML::createColorNode( doc, hiLoSettingsElement, "LowValuesColor",
                _hiLoChartLowValuesColor );

        // the PrintHighValues element
        KDXML::createBoolNode( doc, hiLoSettingsElement, "PrintHighValues",
                _hiLoChartPrintHighValues );

        // the HighValuesFont element
        createChartFontNode( doc, hiLoSettingsElement, "HighValuesFont",
                _hiLoChartHighValuesFont,
                _hiLoChartHighValuesUseFontRelSize,
                _hiLoChartHighValuesFontRelSize );

        // the HighValuesColor element
        KDXML::createColorNode( doc, hiLoSettingsElement, "HighValuesColor",
                _hiLoChartHighValuesColor );

        // the PrintOpenValues element
        KDXML::createBoolNode( doc, hiLoSettingsElement, "PrintOpenValues",
                _hiLoChartPrintOpenValues );

        // the OpenValuesFont element
        createChartFontNode( doc, hiLoSettingsElement, "OpenValuesFont",
                _hiLoChartOpenValuesFont,
                _hiLoChartOpenValuesUseFontRelSize,
                _hiLoChartOpenValuesFontRelSize );

        // the OpenValuesColor element
        KDXML::createColorNode( doc, hiLoSettingsElement, "OpenValuesColor",
                _hiLoChartOpenValuesColor );

        // the PrintCloseValues element
        KDXML::createBoolNode( doc, hiLoSettingsElement, "PrintCloseValues",
                _hiLoChartPrintCloseValues );

        // the CloseValuesFont element
        createChartFontNode( doc, hiLoSettingsElement, "CloseValuesFont",
                _hiLoChartCloseValuesFont,
                _hiLoChartCloseValuesUseFontRelSize,
                _hiLoChartCloseValuesFontRelSize );

        // the CloseValuesColor element
        KDXML::createColorNode( doc, hiLoSettingsElement, "CloseValuesColor",
                _hiLoChartCloseValuesColor );
    }



    // the BoxAndWhiskerSettings element
    QDomElement bWSettingsElement =
        doc.createElement( "BoxAndWhiskerSettings" );
    docRoot.appendChild( bWSettingsElement );
    {
        // the SubType element
        KDXML::createStringNode( doc, bWSettingsElement, "SubType",
                KDChartParams::bWChartSubTypeToString( _BWChartSubType ) );

        // the fences elements
        KDXML::createDoubleNode( doc, bWSettingsElement, "FenceUpperInner",
                _BWChartFenceUpperInner );
        KDXML::createDoubleNode( doc, bWSettingsElement, "FenceLowerInner",
                _BWChartFenceLowerInner );
        KDXML::createDoubleNode( doc, bWSettingsElement, "FenceUpperOuter",
                _BWChartFenceUpperInner );
        KDXML::createDoubleNode( doc, bWSettingsElement, "FenceLowerOuter",
                _BWChartFenceLowerOuter );

        // the brush to be used
        KDXML::createBrushNode( doc, bWSettingsElement, "Brush",
                _BWChartBrush );

        // the size of the outlier markers
        KDXML::createIntNode( doc, bWSettingsElement, "OutlierSize",
                _BWChartOutValMarkerSize );

        // the PrintStatistics elements
        for( int i = BWStatValSTART; i <= BWStatValEND; ++i ){
            QDomElement printStatElement =
                doc.createElement( "PrintStatistics"+bWChartStatValToString( (BWStatVal)i ) );
            KDXML::createBoolNode(  doc, printStatElement, "Active",
                    _BWChartStatistics[ i ].active );
            createChartFontNode(    doc, printStatElement, "Font",
                    _BWChartStatistics[ i ].font,
                    _BWChartStatistics[ i ].useRelSize,
                    _BWChartStatistics[ i ].relSize );
            KDXML::createColorNode( doc, printStatElement, "Color",
                    _BWChartStatistics[ i ].color );
            KDXML::createBrushNode( doc, printStatElement, "Brush",
                    _BWChartStatistics[ i ].brush );
        }
    }



    // the PolarSettings element
    QDomElement polarSettingsElement =
        doc.createElement( "PolarSettings" );
    docRoot.appendChild( polarSettingsElement );

    {
        // the SubType element
        KDXML::createStringNode( doc, polarSettingsElement,
                "SubType", KDChartParams::polarChartSubTypeToString( _polarChartSubType ) );

        // the Marker element
        KDXML::createBoolNode( doc, polarSettingsElement,
                "Marker", _polarMarker );

        // the MarkerStyles elements
        for( QMap<uint,KDChartParams::PolarMarkerStyle>::ConstIterator it = _polarMarkerStyles.begin();
                it != _polarMarkerStyles.end(); ++it ) {
            QDomElement markerStyleElement = doc.createElement( "MarkerStyle" );
            polarSettingsElement.appendChild( markerStyleElement );
            markerStyleElement.setAttribute( "Dataset", it.key() );
            markerStyleElement.setAttribute( "Style", KDChartParams::polarMarkerStyleToString( it.data() ) );
        }

        // the MarkerSize element
        KDXML::createSizeNode( doc, polarSettingsElement,
                "MarkerSize", _polarMarkerSize );

        // the PolarLineWidth element
        KDXML::createIntNode( doc, polarSettingsElement,
                "PolarLineWidth", _polarLineWidth );
    }



    // the LegendSettings element
    QDomElement legendSettingsElement =
        doc.createElement( "LegendSettings" );
    docRoot.appendChild( legendSettingsElement );
    {
        // the Position element
        KDXML::createStringNode( doc, legendSettingsElement, "Position",
                KDChartParams::legendPositionToString( _legendPosition ) );

        // the Orientation element
        KDXML::createOrientationNode( doc, legendSettingsElement,
                                      "Orientation", _legendOrientation );

        // the ShowLines element
        KDXML::createBoolNode( doc, legendSettingsElement,
                                    "ShowLines", _legendShowLines );

        // the Source element
        KDXML::createStringNode( doc, legendSettingsElement, "Source",
                KDChartParams::legendSourceToString( _legendSource ) );

        // the Text elements
        for( QMap<int,QString>::ConstIterator it = _legendText.begin();
                it != _legendText.end(); ++it ) {
            QDomElement legendTextElement = doc.createElement( "LegendText" );
            legendSettingsElement.appendChild( legendTextElement );
            legendTextElement.setAttribute( "Dataset", it.key() );
            legendTextElement.setAttribute( "Text", it.data() );
        }

        // the TextColor element
        KDXML::createColorNode( doc, legendSettingsElement, "TextColor",
                _legendTextColor );

        // the TextFont element
        createChartFontNode( doc, legendSettingsElement, "TextFont",
                _legendFont,
                _legendFontUseRelSize,
                _legendFontRelSize );

        // the TitleText element
        KDXML::createStringNode( doc, legendSettingsElement, "TitleText",
                _legendTitleText );

        // the TitleColor element
        KDXML::createColorNode( doc, legendSettingsElement, "TitleColor",
                _legendTitleTextColor );

        // the TitleFont element
        createChartFontNode( doc, legendSettingsElement, "TitleFont",
                _legendTitleFont,
                _legendTitleFontUseRelSize,
                _legendTitleFontRelSize );

        // the Spacing element
        KDXML::createIntNode( doc, legendSettingsElement, "Spacing",
                _legendSpacing );
    }

    saveAxesToXML(doc, docRoot);

    // the HeaderFooterSettings elements
    for( int hf = 0; hf < 18; hf++ ) {
        QDomElement hfSettingsElement =
            doc.createElement( "HeaderFooterSettings" );
        docRoot.appendChild( hfSettingsElement );
        {
            KDXML::createStringNode( doc, hfSettingsElement, "Text",
                    _hdFtParams[hf]._text );
            createChartFontNode( doc, hfSettingsElement, "Font",
                    _hdFtParams[hf]._font,
                    _hdFtParams[hf]._fontUseRelSize,
                    _hdFtParams[hf]._fontRelSize );
            KDXML::createColorNode( doc, hfSettingsElement, "Color",
                    _hdFtParams[hf]._color );
        }
    }


    // the GlobalLeading element
    QDomElement globalLeadingElement =
        doc.createElement( "GlobalLeading" );
    docRoot.appendChild( legendSettingsElement );
    {
        KDXML::createIntNode( doc, globalLeadingElement, "Left",
                _globalLeadingLeft );
        KDXML::createIntNode( doc, globalLeadingElement, "Top",
                _globalLeadingTop );
        KDXML::createIntNode( doc, globalLeadingElement, "Right",
                _globalLeadingRight );
        KDXML::createIntNode( doc, globalLeadingElement, "Bottom",
                _globalLeadingBottom );
    }

    // the DataValuesSettings1 element
    QDomElement dataValuesSettings1Element =
        doc.createElement( "DataValuesSettings1" );
    docRoot.appendChild( dataValuesSettings1Element );
    {
        KDXML::createBoolNode( doc, dataValuesSettings1Element, "PrintDataValues",
                _printDataValuesSettings._printDataValues );
        KDXML::createIntNode( doc, dataValuesSettings1Element, "DivPow10",
                _printDataValuesSettings._divPow10 );
        KDXML::createIntNode( doc, dataValuesSettings1Element, "DigitsBehindComma",
                _printDataValuesSettings._digitsBehindComma );
        createChartFontNode( doc, dataValuesSettings1Element, "Font",
                _printDataValuesSettings._dataValuesFont,
                _printDataValuesSettings._dataValuesUseFontRelSize,
                _printDataValuesSettings._dataValuesFontRelSize );
        KDXML::createColorNode( doc, dataValuesSettings1Element, "Color",
                _printDataValuesSettings._dataValuesColor );
        KDXML::createBrushNode( doc, dataValuesSettings1Element, "Background",
                _printDataValuesSettings._dataValuesBrush );
        KDXML::createBoolNode( doc, dataValuesSettings1Element, "AutoColor",
                _printDataValuesSettings._dataValuesAutoColor );
        KDXML::createStringNode( doc, dataValuesSettings1Element,
                "AnchorNegativePosition",
                KDChartEnums::positionFlagToString( _printDataValuesSettings._dataValuesAnchorNegativePosition ) );
        KDXML::createIntNode( doc, dataValuesSettings1Element,
                "AnchorNegativeAlign",
                _printDataValuesSettings._dataValuesAnchorNegativeAlign );
        KDXML::createIntNode( doc, dataValuesSettings1Element,
                "AnchorNegativeDeltaX",
                _printDataValuesSettings._dataValuesAnchorNegativeDeltaX );
        KDXML::createIntNode( doc, dataValuesSettings1Element,
                "AnchorNegativeDeltaY",
                _printDataValuesSettings._dataValuesAnchorNegativeDeltaY );
        KDXML::createIntNode( doc, dataValuesSettings1Element,
                "NegativeRotation",
                _printDataValuesSettings._dataValuesNegativeRotation );
        KDXML::createStringNode( doc, dataValuesSettings1Element,
                "AnchorPositivePosition",
                KDChartEnums::positionFlagToString( _printDataValuesSettings._dataValuesAnchorPositivePosition ) );
        KDXML::createIntNode( doc, dataValuesSettings1Element,
                "AnchorPositiveAlign",
                _printDataValuesSettings._dataValuesAnchorPositiveAlign );
        KDXML::createIntNode( doc, dataValuesSettings1Element,
                "AnchorPositiveDeltaX",
                _printDataValuesSettings._dataValuesAnchorPositiveDeltaX );
        KDXML::createIntNode( doc, dataValuesSettings1Element,
                "AnchorPositiveDeltaY",
                _printDataValuesSettings._dataValuesAnchorPositiveDeltaY );
        KDXML::createIntNode( doc, dataValuesSettings1Element,
                "PositiveRotation",
                _printDataValuesSettings._dataValuesPositiveRotation );

        KDXML::createStringNode( doc, dataValuesSettings1Element,
                "LayoutPolicy",
                KDChartEnums::layoutPolicyToString( _printDataValuesSettings._dataValuesLayoutPolicy ) );

        KDXML::createBoolNode( doc, dataValuesSettings1Element, "ShowInfinite",
                               _printDataValuesSettings._dataValuesShowInfinite );
    }

    // the DataValuesSettings2 element
    QDomElement dataValuesSettings2Element =
        doc.createElement( "DataValuesSettings2" );
    docRoot.appendChild( dataValuesSettings2Element );
    {
        KDXML::createBoolNode( doc, dataValuesSettings2Element, "PrintDataValues",
                _printDataValuesSettings2._printDataValues );
        KDXML::createIntNode( doc, dataValuesSettings2Element, "DivPow10",
                _printDataValuesSettings2._divPow10 );
        KDXML::createIntNode( doc, dataValuesSettings2Element, "DigitsBehindComma",
                _printDataValuesSettings2._digitsBehindComma );
        createChartFontNode( doc, dataValuesSettings2Element, "Font",
                _printDataValuesSettings2._dataValuesFont,
                _printDataValuesSettings2._dataValuesUseFontRelSize,
                _printDataValuesSettings2._dataValuesFontRelSize );
        KDXML::createColorNode( doc, dataValuesSettings2Element, "Color",
                _printDataValuesSettings2._dataValuesColor );
        KDXML::createBrushNode( doc, dataValuesSettings2Element, "Background",
                _printDataValuesSettings2._dataValuesBrush );
        KDXML::createBoolNode( doc, dataValuesSettings2Element, "AutoColor",
                _printDataValuesSettings2._dataValuesAutoColor );
        KDXML::createStringNode( doc, dataValuesSettings2Element,
                "AnchorNegativePosition",
                KDChartEnums::positionFlagToString( _printDataValuesSettings2._dataValuesAnchorNegativePosition ) );
        KDXML::createIntNode( doc, dataValuesSettings2Element,
                "AnchorNegativeAlign",
                _printDataValuesSettings2._dataValuesAnchorNegativeAlign );
        KDXML::createIntNode( doc, dataValuesSettings2Element,
                "AnchorNegativeDeltaX",
                _printDataValuesSettings2._dataValuesAnchorNegativeDeltaX );
        KDXML::createIntNode( doc, dataValuesSettings2Element,
                "AnchorNegativeDeltaY",
                _printDataValuesSettings2._dataValuesAnchorNegativeDeltaY );
        KDXML::createIntNode( doc, dataValuesSettings2Element,
                "NegativeRotation",
                _printDataValuesSettings2._dataValuesNegativeRotation );
        KDXML::createStringNode( doc, dataValuesSettings2Element,
                "AnchorPositivePosition",
                KDChartEnums::positionFlagToString( _printDataValuesSettings2._dataValuesAnchorPositivePosition ) );
        KDXML::createIntNode( doc, dataValuesSettings2Element,
                "AnchorPositiveAlign",
                _printDataValuesSettings2._dataValuesAnchorPositiveAlign );
        KDXML::createIntNode( doc, dataValuesSettings2Element,
                "AnchorPositiveDeltaX",
                _printDataValuesSettings2._dataValuesAnchorPositiveDeltaX );
        KDXML::createIntNode( doc, dataValuesSettings2Element,
                "AnchorPositiveDeltaY",
                _printDataValuesSettings2._dataValuesAnchorPositiveDeltaY );
        KDXML::createIntNode( doc, dataValuesSettings2Element,
                "PositiveRotation",
                _printDataValuesSettings2._dataValuesPositiveRotation );

        KDXML::createStringNode( doc, dataValuesSettings2Element,
                "LayoutPolicy",
                KDChartEnums::layoutPolicyToString( _printDataValuesSettings2._dataValuesLayoutPolicy ) );

        KDXML::createBoolNode( doc, dataValuesSettings2Element, "ShowInfinite",
                               _printDataValuesSettings2._dataValuesShowInfinite );
    }

    // global settings for data value settings of _all_ charts
    QDomElement dataValuesGlobalSettingsElement =
        doc.createElement( "DataValuesGlobalSettings" );
    docRoot.appendChild( dataValuesGlobalSettingsElement );
    {
        KDXML::createBoolNode( doc, dataValuesGlobalSettingsElement,
                "allowOverlappingTexts",
                _allowOverlappingDataValueTexts );
    }

    // the AreaMap element
    QDomElement areaMapElement =
        doc.createElement( "AreaMap" );
    docRoot.appendChild( areaMapElement );
    {
        QDictIterator<KDChartFrameSettings> it( _areaDict );
        for( ; it.current(); ++it ){
            KDChartFrameSettings::createFrameSettingsNode( doc, areaMapElement,
                    "FrameSettings",
                    it.current(),
                    it.currentKey().left(5).stripWhiteSpace().toUInt() );
        }
    }

    // the CustomBoxMap element
    QDomElement customBoxMapElement =
        doc.createElement( "CustomBoxMap" );
    docRoot.appendChild( customBoxMapElement );
    {
        QIntDictIterator<KDChartCustomBox> it( _customBoxDict );
        for( ; it.current(); ++it ){
            KDXML::createIntNode( doc, customBoxMapElement, "Number", it.currentKey() );
            KDChartCustomBox::createCustomBoxNode( doc, customBoxMapElement,
                    "CustomBox", it );
        }
    }


    return doc;
}


/**
  \fn QTextStream& operator>>( QTextStream& s, KDChartParams& p );

  Reads the an XML document from the text stream s into the
  KDChartParams object p

  \param s the text stream to read from
  \param p the KDChartParams object to read into
  \return the text stream after the read operation
  */
QTextStream& operator>>( QTextStream& s, KDChartParams& p )
{
    QDomDocument doc( "ChartParams" );
    // would be nicer if QDomDocument could read from a QTextStream...
    QString docString = s.read();
    doc.setContent( docString );

    p.loadXML( doc );

    return s;
}


/**
  Helper method called by loadXML() only.
  */
void KDChartParams::loadAxesFormXML(int& curAxisSettings, QDomElement& element)
{
    KDChartAxisParams* axisSettings =
        &( _axisSettings[ curAxisSettings ].params );
    QDomNode node = element.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "Type" ) {
                QString string;
                if( KDXML::readStringNode( element, string ) )
                    axisSettings->_axisType = KDChartAxisParams::stringToAxisType( string );
            } else if( tagName == "Visible" ) {
                bool visible;
                if( KDXML::readBoolNode( element, visible ) )
                    axisSettings->_axisVisible = visible;
            } else if( tagName == "LabelsTouchEdges" ) {
                bool labelsTouchEdges;
                if( KDXML::readBoolNode( element, labelsTouchEdges ) )
                    axisSettings->_axisLabelsTouchEdges = labelsTouchEdges;
            } else if( tagName == "AreaMode" ) {
                QString string;
                if( KDXML::readStringNode( element, string ) )
                    axisSettings->_axisAreaMode = KDChartAxisParams::stringToAxisAreaMode( string );
            } else if( tagName == "UseAvailableSpaceFrom" ) {
                int spaceFrom;
                if( KDXML::readIntNode( element, spaceFrom ) )
                    axisSettings->_axisUseAvailableSpaceFrom = spaceFrom;
            } else if( tagName == "UseAvailableSpaceTo" ) {
                int spaceTo;
                if( KDXML::readIntNode( element, spaceTo ) )
                    axisSettings->_axisUseAvailableSpaceTo = spaceTo;
            } else if( tagName == "IsometricReferenceAxis" ) {
                int isoRefAxis;
                if( KDXML::readIntNode( element,    isoRefAxis ) )
                    axisSettings->_axisIsoRefAxis = isoRefAxis;
            } else if( tagName == "AreaMin" ) {
                int areaMin;
                if( KDXML::readIntNode( element, areaMin ) )
                    axisSettings->_axisAreaMin = areaMin;
            } else if( tagName == "AreaMax" ) {
                int areaMax;
                if( KDXML::readIntNode( element, areaMax ) )
                    axisSettings->_axisAreaMax = areaMax;
            } else if( tagName == "CalcMode" ) {
                QString string;
                if( KDXML::readStringNode( element, string ) )
                    axisSettings->_axisCalcMode = KDChartAxisParams::stringToAxisCalcMode( string );
            } else if( tagName == "TrueAreaSize" ) {
                int trueAreaSize;
                if( KDXML::readIntNode( element, trueAreaSize ) )
                    axisSettings->_axisTrueAreaSize = trueAreaSize;
            } else if( tagName == "TrueAreaRect" ) {
                QRect trueAreaRect;
                if( KDXML::readRectNode( element, trueAreaRect ) )
                    axisSettings->_axisTrueAreaRect = trueAreaRect;
            } else if( tagName == "ShowSubDelimiters" ) {
                bool showSubDelimiters;
                if( KDXML::readBoolNode( element, showSubDelimiters ) )
                    axisSettings->_axisShowSubDelimiters = showSubDelimiters;
            } else if( tagName == "LineVisible" ) {
                bool lineVisible;
                if( KDXML::readBoolNode( element, lineVisible ) )
                    axisSettings->_axisLineVisible = lineVisible;
            } else if( tagName == "LineWidth" ) {
                int lineWidth;
                if( KDXML::readIntNode( element, lineWidth ) )
                    axisSettings->_axisLineWidth = lineWidth;
            } else if( tagName == "TrueLineWidth" ) {
                int trueLineWidth;
                if( KDXML::readIntNode( element, trueLineWidth ) )
                    axisSettings->_axisTrueLineWidth = trueLineWidth;
            } else if( tagName == "LineColor" ) {
                QColor color;
                if( KDXML::readColorNode( element, color ) )
                    axisSettings->_axisLineColor = color;
            } else if( tagName == "ShowGrid" ) {
                bool showGrid;
                if( KDXML::readBoolNode( element, showGrid ) )
                    axisSettings->_axisShowGrid = showGrid;
            } else if( tagName == "GridColor" ) {
                QColor color;
                if( KDXML::readColorNode( element, color ) )
                    axisSettings->_axisGridColor = color;
            } else if( tagName == "GridLineWidth" ) {
                int gridLineWidth;
                if( KDXML::readIntNode( element, gridLineWidth ) )
                    axisSettings->_axisGridLineWidth = gridLineWidth;
            } else if( tagName == "GridStyle" ) {
                if( element.hasAttribute( "Style" ) )
                    axisSettings->_axisGridStyle = KDXML::stringToPenStyle( element.attribute( "Style" ) );
            } else if( tagName == "GridSubColor" ) {
                QColor color;
                if( KDXML::readColorNode( element, color ) )
                    axisSettings->_axisGridSubColor = color;
            } else if( tagName == "GridSubLineWidth" ) {
                int gridSubLineWidth;
                if( KDXML::readIntNode( element, gridSubLineWidth ) )
                    axisSettings->_axisGridSubLineWidth = gridSubLineWidth;
            } else if( tagName == "GridSubStyle" ) {
                if( element.hasAttribute( "Style" ) )
                    axisSettings->_axisGridSubStyle = KDXML::stringToPenStyle( element.attribute( "Style" ) );
            } else if( tagName == "ZeroLineColor" ) {
                QColor color;
                if( KDXML::readColorNode( element, color ) )
                    axisSettings->_axisZeroLineColor = color;
            } else if( tagName == "LabelsVisible" ) {
                bool labelsVisible;
                if( KDXML::readBoolNode( element, labelsVisible ) )
                    axisSettings->_axisLabelsVisible = labelsVisible;
            } else if( tagName == "LabelsFont" ) {
                readChartFontNode( element,
                        axisSettings->_axisLabelsFont,
                        axisSettings->_axisLabelsFontUseRelSize,
                        axisSettings->_axisLabelsFontRelSize,
                        &axisSettings->_axisLabelsFontMinSize );
            } else if( tagName == "LabelsDontShrinkFont" ) {
                bool dontShrink;
                if( KDXML::readBoolNode( element, dontShrink ) )
                    axisSettings->_axisLabelsDontShrinkFont = dontShrink;
            } else if( tagName == "LabelsDontAutoRotate" ) {
                bool dontRotate;
                if( KDXML::readBoolNode( element, dontRotate ) )
                    axisSettings->_axisLabelsDontAutoRotate = dontRotate;
            } else if( tagName == "LabelsRotation" ) {
                int rotation;
                if( KDXML::readIntNode( element, rotation ) )
                    axisSettings->_axisLabelsRotation = rotation;
            } else if( tagName == "LabelsLeaveOut" ) {
                int leaveOut;
                if( KDXML::readIntNode( element, leaveOut ) )
                    axisSettings->_axisValueLeaveOut = leaveOut;
            } else if( tagName == "LabelsColor" ) {
                QColor color;
                if( KDXML::readColorNode( element, color ) )
                    axisSettings->_axisLabelsColor = color;
            } else if( tagName == "SteadyValueCalc" ) {
                bool steadyValueCalc;
                if( KDXML::readBoolNode( element, steadyValueCalc ) )
                    axisSettings->_axisSteadyValueCalc = steadyValueCalc;
            } else if( tagName == "ValueStart" ) {
                QVariant valX, valY;
                int propID;
                if( readChartValueNode( element, valY, valX, propID ) &&
                    QVariant::Double == valY.type() )
                    axisSettings->_axisValueStart = valY.toDouble();
            } else if( tagName == "ValueStartIsExact" ) {
                bool isExactValue;
                if( KDXML::readBoolNode( element, isExactValue ) )
                    axisSettings->_axisValueStartIsExact = isExactValue;
            } else if( tagName == "ValueEnd" ) {
                QVariant valX, valY;
                int propID;
                if( readChartValueNode( element, valY, valX, propID ) &&
                    QVariant::Double == valY.type()  )
                    axisSettings->_axisValueEnd = valY.toDouble();
            } else if( tagName == "ValueDelta" ) {
                double valueDelta;
                if( KDXML::readDoubleNode( element, valueDelta ) )
                    axisSettings->_axisValueDelta = valueDelta;
            } else if( tagName == "ValueDeltaScale" ) {
                int valueDeltaScale;
                if( KDXML::readIntNode( element, valueDeltaScale ) )
                    axisSettings->_axisValueDeltaScale = (KDChartAxisParams::ValueScale)valueDeltaScale;
            } else if( tagName == "ValuesDecreasing" ) {
                bool decreasing;
                if( KDXML::readBoolNode( element, decreasing ) )
                    axisSettings->_axisValuesDecreasing = decreasing;
            } else if( tagName == "TrueLow" ) {
                double trueLow;
                if( KDXML::readDoubleNode( element, trueLow ) )
                    axisSettings->_trueLow  = trueLow;
            } else if( tagName == "TrueHigh" ) {
                double trueHigh;
                if( KDXML::readDoubleNode( element, trueHigh ) )
                    axisSettings->_trueHigh  = trueHigh;
            } else if( tagName == "TrueDelta" ) {
                double trueDelta;
                if( KDXML::readDoubleNode( element, trueDelta ) )
                    axisSettings->_trueDelta  = trueDelta;
            } else if( tagName == "ZeroLineStart" ) {
                double x = 0.0;
                double y = 0.0;
                bool ok = true;
                if( element.hasAttribute( "X" ) &&
                        element.hasAttribute( "Y" ) ) {
                    x = element.attribute( "X" ).toDouble( &ok );
                    if( ok )
                        y = element.attribute( "Y" ).toDouble( &ok );
                } else
                    ok = false;

                if( ok ) {
                    axisSettings->_axisZeroLineStartX = x;
                    axisSettings->_axisZeroLineStartY = y;
                }
            } else if( tagName == "DigitsBehindComma" ) {
                int digitsBehindComma;
                if( KDXML::readIntNode( element, digitsBehindComma ) )
                    axisSettings->_axisDigitsBehindComma = digitsBehindComma;
            } else if( tagName == "LabelsDateTimeFormat" ) {
                QString string;
                if( KDXML::readStringNode( element, string ) )
                    axisSettings->_axisLabelsDateTimeFormat = string;
            } else if( tagName == "MaxEmptyInnerSpan" ) {
                int maxEmptyInnerSpan;
                if( KDXML::readIntNode( element, maxEmptyInnerSpan ) )
                    axisSettings->_axisMaxEmptyInnerSpan = maxEmptyInnerSpan;
            } else if( tagName == "LabelsFromDataRow" ) {
                QString string;
                if( KDXML::readStringNode( element, string ) )
                    axisSettings->_labelTextsDataRow = KDChartAxisParams::stringToLabelsFromDataRow( string );
            } else if( tagName == "TextsDataRow" ) {
                int textsDataRow;
                if( KDXML::readIntNode( element, textsDataRow ) )
                    axisSettings->_labelTextsDataRow = textsDataRow;
            } else if( tagName == "LabelString" ) {
                QString string;
                if( KDXML::readStringNode( element, string ) )
                    axisSettings->_axisLabelStringList << string;
            } else if( tagName == "ShortLabelString" ) {
                QString string;
                if( KDXML::readStringNode( element, string ) )
                    axisSettings->_axisShortLabelsStringList << string;
            } else if( tagName == "LabelText" ) {
                QString string;
                if( KDXML::readStringNode( element, string ) )
                    axisSettings->_axisLabelTexts = string;
            } else if( tagName == "LabelTextsDirty" ) {
                bool labelTextsDirty;
                if( KDXML::readBoolNode( element, labelTextsDirty ) )
                    axisSettings->_axisLabelTextsDirty = labelTextsDirty;
            } else if( tagName == "FirstLabelReplacementText" ) {
                QString value;
                if( KDXML::readStringNode( element, value ) )
                    axisSettings->_axisFirstLabelText = value;
            } else if( tagName == "LastLabelReplacementText" ) {
                QString value;
                if( KDXML::readStringNode( element, value ) )
                    axisSettings->_axisLastLabelText = value;
            } else if( tagName == "LabelsDivPow10" ) {
                int divPow10;
                if( KDXML::readIntNode( element, divPow10 ) )
                    axisSettings->_axisLabelsDivPow10 = divPow10;
            } else if( tagName == "LabelsDecimalPoint" ) {
                QString value;
                if( KDXML::readStringNode( element, value ) )
                    axisSettings->_axisLabelsDecimalPoint = value;
            } else if( tagName == "LabelsThousandsPoint" ) {
                QString value;
                if( KDXML::readStringNode( element, value ) )
                    axisSettings->_axisLabelsThousandsPoint = value;
            } else if( tagName == "LabelsPrefix" ) {
                QString value;
                if( KDXML::readStringNode( element, value ) )
                    axisSettings->_axisLabelsPrefix = value;
            } else if( tagName == "LabelsPostfix" ) {
                QString value;
                if( KDXML::readStringNode( element, value ) )
                    axisSettings->_axisLabelsPostfix = value;
            } else if( tagName == "LabelsTotalLen" ) {
                int totalLen;
                if( KDXML::readIntNode( element, totalLen ) )
                    axisSettings->_axisLabelsTotalLen = totalLen;
            } else if( tagName == "LabelsPadFill" ) {
                QString value;
                if( KDXML::readStringNode( element, value ) ){
                    if( value.isEmpty() )
                        axisSettings->_axisLabelsPadFill = ' ';
                    else
                        axisSettings->_axisLabelsPadFill = value.at(0);
                }
            } else if( tagName == "LabelsBlockAlign" ) {
                bool blockAlign;
                if( KDXML::readBoolNode( element, blockAlign ) )
                    axisSettings->_axisLabelsBlockAlign = blockAlign;
            } else {
                qDebug( "Unknown subelement of AxisSettings found: %s", tagName.latin1() );
            }
        }
        node = node.nextSibling();
    }
    curAxisSettings++; // one axis settings entry finished
}


/**
  Reads the parameters from an XML document.
  \param doc the XML document to read from
  \return true if the parameters could be read, false if a file
  format error occurred
  \sa saveXML
  */
bool KDChartParams::loadXML( const QDomDocument& doc )
{
    int curAxisSettings = 0;
    int curHFSettings = 0;

    QDomElement docRoot = doc.documentElement(); // ChartParams element
    QDomNode node = docRoot.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "ChartType" ) {
                if( element.hasAttribute( "primary" ) )
                    _chartType = KDChartParams::stringToChartType( element.attribute( "primary" ) );
                if( element.hasAttribute( "secondary" ) )
                    _additionalChartType = KDChartParams::stringToChartType( element.attribute( "secondary" ) );
            } else if( tagName == "NumValues" ) {
                int numValues;
                if( KDXML::readIntNode( element, numValues ) )
                    _numValues = numValues;
            } else if( tagName == "ModeAndChartMap" ) {
                int dataset = -1;
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "Dataset" ) {
                            KDXML::readIntNode( element, dataset );
                        } else if( tagName == "ModeAndChart" ) {
                            Q_ASSERT( dataset != -1 ); // there must have been a dataset tag before
                            if( element.hasAttribute( "Mode" ) &&
                                    element.hasAttribute( "Chart" ) ) {
                                KDChartParams::SourceMode sourceMode = KDChartParams::stringToChartSourceMode( element.attribute( "Mode" ) );
                                bool ok = false;
                                uint chart = element.attribute( "Chart" ).toUInt( &ok );
                                if( ok )
                                    _dataSourceModeAndChart[dataset] = KDChartParams::ModeAndChart( sourceMode, chart );
                            }
                        } else {
                            qDebug( "Unknown subelement of ModeAndChartMap found: %s", tagName.latin1() );
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "PropertySetMap" ) {
                // the PropertySetMap element
                QDomNode node = element.firstChild();
                KDChartPropertySet set;
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if(    !element.isNull() // was really an element
                            && KDChartPropertySet::loadXML( element, set ) ) // parsed OK
                        setProperties( set.id(), set );
                    node = node.nextSibling();
                }
            } else if( tagName == "ChartSourceModeWasUsed" ) {
                bool chartSourceModeWasUsed;
                if( KDXML::readBoolNode( element, chartSourceModeWasUsed ) )
                    _setChartSourceModeWasUsed = chartSourceModeWasUsed;
            } else if( tagName == "MaxDatasetSourceMode" ) {
                int maxDatasetSourceMode;
                if( KDXML::readIntNode( element, maxDatasetSourceMode ) )
                    _maxDatasetSourceMode = maxDatasetSourceMode;
            } else if( tagName == "ColorSettings" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "DataColors" ) {
                            QMap<uint,QColor>* map = &_dataColors;
                            readColorMapNode( element, map );
                        } else if( tagName == "MaxDatasetColor" ) {
                            int maxDatasetColor;
                            if( KDXML::readIntNode( element, maxDatasetColor ) )
                                _maxDatasetColor = maxDatasetColor;
                        } else if( tagName == "ShadowBrightnessFactor" ) {
                            double shadowBrightnessFactor;
                            if( KDXML::readDoubleNode( element, shadowBrightnessFactor ) )
                                _shadowBrightnessFactor = shadowBrightnessFactor;
                        } else if( tagName == "ShadowPattern" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _shadowPattern = KDXML::stringToBrushStyle( value );
                        } else if( tagName == "ThreeDShadowColors" ) {
                            bool threeDShadowColors;
                            if( KDXML::readBoolNode( element, threeDShadowColors ) )
                                _threeDShadowColors = threeDShadowColors;
                        } else if( tagName == "DataColorsShadow1" ) {
                            QMap<uint,QColor>* map = &_dataColorsShadow1;
                            readColorMapNode( element, map );
                        } else if( tagName == "DataColorsShadow2" ) {
                            QMap<uint,QColor>* map = &_dataColorsShadow2;
                            readColorMapNode( element, map );
                        } else if( tagName == "OutlineDataColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                _outlineDataColor = color;
                        } else if( tagName == "OutlineDataLineWidth" ) {
                            int outlineDataLineWidth;
                            if( KDXML::readIntNode( element, outlineDataLineWidth ) )
                                _outlineDataLineWidth = outlineDataLineWidth;
                        } else if( tagName == "OutlineDataLineStyle" ) {
                            if( element.hasAttribute( "Style" ) )
                                _outlineDataLineStyle = KDXML::stringToPenStyle( element.attribute( "Style" ) );
                        } else {
                            qDebug( "!!!Unknown subelement of ColorSettings found: %s", tagName.latin1() );
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "BarSettings" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "SubType" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _barChartSubType = KDChartParams::stringToBarChartSubType( value );
                        } else if( tagName == "ThreeDBars" ) {
                            bool threeDBars;
                            if( KDXML::readBoolNode( element, threeDBars ) )
                                _threeDBars = threeDBars;
                        } else if( tagName == "ThreeDBarDepth" ) {
                            double threeDBarDepth;
                            if( KDXML::readDoubleNode( element, threeDBarDepth ) )
                                _threeDBarDepth = threeDBarDepth;
                        } else if( tagName == "DatasetGap" ) {
                            int datasetGap;
                            if( KDXML::readIntNode( element, datasetGap ) )
                                _datasetGap = datasetGap;
                        } else if( tagName == "DatasetGapIsRelative" ) {
                            bool datasetGapIsRelative;
                            if( KDXML::readBoolNode( element, datasetGapIsRelative ) )
                                _datasetGapIsRelative = datasetGapIsRelative;
                        } else if( tagName == "ValueBlockGap" ) {
                            int valueBlockGap;
                            if( KDXML::readIntNode( element, valueBlockGap ) )
                                _valueBlockGap = valueBlockGap;
                        } else if( tagName == "ValueBlockGapIsRelative" ) {
                            bool valueBlockGapIsRelative;
                            if( KDXML::readBoolNode( element, valueBlockGapIsRelative ) )
                                _valueBlockGapIsRelative = valueBlockGapIsRelative;
                         } else if( tagName == "BarWidth" ) {
                            int barWidth;
                            if( KDXML::readIntNode( element, barWidth ) )
                                _barWidth = barWidth;
                        } else if( tagName == "SolidExcessArrows" ) {
                            bool solidExcessArrows;
                            if( KDXML::readBoolNode( element, solidExcessArrows ) )
                                _solidExcessArrows = solidExcessArrows;
                        } else {
                            qDebug( "Unknown subelement of BarSettings found: %s", tagName.latin1() );
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "LineSettings" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "SubType" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _lineChartSubType = KDChartParams::stringToLineChartSubType( value );
                        } else if( tagName == "Marker" ) {
                            bool marker;
                            if( KDXML::readBoolNode( element, marker ) )
                                _lineMarker = marker;
                        } else if( tagName == "MarkerStyle" ) {
                            bool ok = true;
                            uint dataset;
                            KDChartParams::LineMarkerStyle style = LineMarkerCircle;
                            if( element.hasAttribute( "Dataset" ) &&
                                    element.hasAttribute( "Style" ) ) {
                                dataset = element.attribute( "Dataset" ).toUInt( &ok );
                                if( ok )
                                    style = KDChartParams::stringToLineMarkerStyle( element.attribute( "Style" ) );
                            } else
                                ok = false;
                            if( ok )
                                _lineMarkerStyles[dataset] = style;
                        } else if( tagName == "MarkerSize" ) {
                            int width = 1;
                            int height= 1;
                            bool ok = true;
                            if( element.hasAttribute( "Width" ) &&
                                element.hasAttribute( "Height" ) ) {
                                width = element.attribute( "Width" ).toInt( &ok );
                                if( ok )
                                    height = element.attribute( "Height" ).toInt( &ok );
                            } else
                                ok = false;

                            if( ok ) {
                                _lineMarkerSize.setWidth( width );
                                _lineMarkerSize.setHeight( height );
                            }
                        } else if( tagName == "LineWidth" ) {
                            int lineWidth;
                            if( KDXML::readIntNode( element, lineWidth ) )
                                _lineWidth = lineWidth;
                        } else if( tagName == "LineColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                _lineColor = color;
                        } else if( tagName == "LineStyle" ) {
                            if( element.hasAttribute( "Style" ) )
                                _lineStyle = KDXML::stringToPenStyle( element.attribute( "Style" ) );
                        } else if( tagName == "DatasetLineStyle" ) {
                            bool ok = true;
                            uint dataset;
                            PenStyle style = Qt::SolidLine;
                            if( element.hasAttribute( "Dataset" ) &&
                                element.hasAttribute( "Style" ) ) {
                                dataset = element.attribute( "Dataset" ).toUInt( &ok );
                                if( ok )
                                    style = KDXML::stringToPenStyle( element.attribute( "Style" ) );
                            } else
                                ok = false;
                            if( ok )
                                _datasetLineStyles[dataset] = style;
                        } else if( tagName == "ThreeD" ) {
                            bool threeD;
                            if( KDXML::readBoolNode( element, threeD ) )
                                _threeDLines = threeD;
                        } else if( tagName == "ThreeDDepth" ) {
                            int depth;
                            if( KDXML::readIntNode( element, depth ) )
                                _threeDLineDepth = depth;
                        } else if( tagName == "ThreeDXRotation" ) {
                            int rotation;
                            if( KDXML::readIntNode( element, rotation ) )
                                _threeDLineXRotation = rotation;
                        } else if( tagName == "ThreeDYRotation" ) {
                            int rotation;
                            if( KDXML::readIntNode( element, rotation ) )
                                _threeDLineYRotation = rotation;
                        } else {
                            qDebug( "Unknown subelement of LineSettings found: %s", tagName.latin1() );
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "AreaSettings" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "SubType" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _areaChartSubType = KDChartParams::stringToAreaChartSubType( value );
                        } else if( tagName == "Location" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                _areaLocation = KDChartParams::stringToAreaLocation( string );
                        } else {
                            qDebug( "Unknown subelement of AreaSettings found: %s", tagName.latin1() );
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "PieRingSettings" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "Explode" ) {
                            bool explode;
                            if( KDXML::readBoolNode( element, explode ) )
                                _explode = explode;
                        } else if( tagName == "DefaultExplodeFactor" ) {
                            double defaultExplodeFactor;
                            if( KDXML::readDoubleNode( element, defaultExplodeFactor ) )
                                _explodeFactor = defaultExplodeFactor;
                        } else if( tagName == "ExplodeFactors" ) {
                            QMap<int,double>* map = &_explodeFactors;
                            readDoubleMapNode( element, map );
                        } else if( tagName == "ExplodeSegment" ) {
                            int explodeSegment;
                            if( KDXML::readIntNode( element, explodeSegment ) )
                                _explodeList << explodeSegment;
                        } else if( tagName == "ThreeDPies" ) {
                            bool threeDPies;
                            if( KDXML::readBoolNode( element, threeDPies ) )
                                _threeDPies = threeDPies;
                        } else if( tagName == "ThreeDPieHeight" ) {
                            int threeDPieHeight;
                            if( KDXML::readIntNode( element, threeDPieHeight ) )
                                _threeDPieHeight = threeDPieHeight;
                        } else if( tagName == "PieStart" ) {
                            int pieStart;
                            if( KDXML::readIntNode( element, pieStart ) )
                                _pieStart = pieStart;
                        } else if( tagName == "RingStart" ) {
                            int ringStart;
                            if( KDXML::readIntNode( element, ringStart ) )
                                _ringStart = ringStart;
                        } else if( tagName == "RelativeRingThickness" ) {
                            bool relativeRingThickness;
                            if( KDXML::readBoolNode( element, relativeRingThickness ) )
                                _relativeRingThickness = relativeRingThickness;
                        } else {
                            qDebug( "Unknown subelement of PieRingSettings found: %s", tagName.latin1() );
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "HiLoSettings" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "SubType" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _hiLoChartSubType = KDChartParams::stringToHiLoChartSubType( value );
                        } else if( tagName == "PrintLowValues" ) {
                            bool printLowValues;
                            if( KDXML::readBoolNode( element, printLowValues ) )
                                _hiLoChartPrintLowValues = printLowValues;
                        } else if( tagName == "LowValuesFont" ) {
                            readChartFontNode( element,
                                    _hiLoChartLowValuesFont,
                                    _hiLoChartLowValuesUseFontRelSize,
                                    _hiLoChartLowValuesFontRelSize );
                        } else if( tagName == "LowValuesColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                _hiLoChartLowValuesColor = color;
                        } else if( tagName == "PrintHighValues" ) {
                            bool printHighValues;
                            if( KDXML::readBoolNode( element, printHighValues ) )
                                _hiLoChartPrintHighValues = printHighValues;
                        } else if( tagName == "HighValuesFont" ) {
                            readChartFontNode( element,
                                    _hiLoChartHighValuesFont,
                                    _hiLoChartHighValuesUseFontRelSize,
                                    _hiLoChartHighValuesFontRelSize );
                        } else if( tagName == "HighValuesColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                _hiLoChartHighValuesColor = color;
                        } else if( tagName == "PrintOpenValues" ) {
                            bool printOpenValues;
                            if( KDXML::readBoolNode( element, printOpenValues ) )
                                _hiLoChartPrintOpenValues = printOpenValues;
                        } else if( tagName == "OpenValuesFont" ) {
                            readChartFontNode( element,
                                    _hiLoChartOpenValuesFont,
                                    _hiLoChartOpenValuesUseFontRelSize,
                                    _hiLoChartOpenValuesFontRelSize );
                        } else if( tagName == "OpenValuesColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                _hiLoChartOpenValuesColor = color;
                        } else if( tagName == "PrintCloseValues" ) {
                            bool printCloseValues;
                            if( KDXML::readBoolNode( element, printCloseValues ) )
                                _hiLoChartPrintCloseValues = printCloseValues;
                        } else if( tagName == "CloseValuesFont" ) {
                            readChartFontNode( element,
                                    _hiLoChartCloseValuesFont,
                                    _hiLoChartCloseValuesUseFontRelSize,
                                    _hiLoChartCloseValuesFontRelSize );
                        } else if( tagName == "CloseValuesColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                _hiLoChartCloseValuesColor = color;
                        } else {
                            qDebug( "Unknown subelement of HiLoSettings found: %s", tagName.latin1() );
                        }
                    }
                    node = node.nextSibling();
                }


            } else if( tagName == "BoxAndWhiskerSettings" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "SubType" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _BWChartSubType = KDChartParams::stringToBWChartSubType( value );
                        } else if( tagName == "FenceUpperInner" ) {
                            double fence;
                            if( KDXML::readDoubleNode( element, fence ) )
                                _BWChartFenceUpperInner = fence;
                        } else if( tagName == "FenceLowerInner" ) {
                            double fence;
                            if( KDXML::readDoubleNode( element, fence ) )
                                _BWChartFenceUpperOuter = fence;
                        } else if( tagName == "FenceUpperOuter" ) {
                            double fence;
                            if( KDXML::readDoubleNode( element, fence ) )
                                _BWChartFenceUpperOuter = fence;
                        } else if( tagName == "FenceLowerOuter" ) {
                            double fence;
                            if( KDXML::readDoubleNode( element, fence ) )
                                _BWChartFenceUpperOuter = fence;
                        } else if( tagName == "Brush" ) {
                            QBrush brush;
                            if( KDXML::readBrushNode( element, brush ) )
                                _BWChartBrush = brush;
                        } else if( tagName == "OutlierSize" ) {
                            int size;
                            if( KDXML::readIntNode( element, size ) )
                                _BWChartOutValMarkerSize = size;
                        } else if( tagName.startsWith("PrintStatistics") ) {
                            // 012345678901234
                            QString statName(tagName.mid( 15 ));
                            BWStatVal i( stringToBWChartStatVal( statName ) );
                            if( BWStatValSTART <= i && i <= BWStatValEND ) {
                                QDomNode node = element.firstChild();
                                while( !node.isNull() ) {
                                    QDomElement element = node.toElement();
                                    if( !element.isNull() ) { // was really an element
                                        QString tagName = element.tagName();
                                        if( tagName == "Active" ) {
                                            bool active;
                                            if( KDXML::readBoolNode( element, active ) )
                                                _BWChartStatistics[ i ].active = active;
                                        } else if( tagName == "Font" ) {
                                            readChartFontNode( element,
                                                    _BWChartStatistics[ i ].font,
                                                    _BWChartStatistics[ i ].useRelSize,
                                                    _BWChartStatistics[ i ].relSize );
                                        } else if( tagName == "Color" ) {
                                            QColor color;
                                            if( KDXML::readColorNode( element, color ) )
                                                _BWChartStatistics[ i ].color = color;
                                        } else if( tagName == "Brush" ) {
                                            QBrush brush;
                                            if( KDXML::readBrushNode( element, brush ) )
                                                _BWChartStatistics[ i ].brush = brush;
                                        } else {
                                            qDebug( "Unknown subelement of BoxAndWhiskerSettings found: %s", tagName.latin1() );
                                        }
                                    }
                                }
                            } else {
                                qDebug( "Unknown subelement of BoxAndWhiskerSettings found: %s", tagName.latin1() );
                            }
                        } else {
                            qDebug( "Unknown subelement of BoxAndWhiskerSettings found: %s", tagName.latin1() );
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "PolarSettings" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "SubType" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _polarChartSubType = KDChartParams::stringToPolarChartSubType( value );
                        } else if( tagName == "Marker" ) {
                            bool marker;
                            if( KDXML::readBoolNode( element, marker ) )
                                _polarMarker = marker;
                        } else if( tagName == "MarkerStyle" ) {
                            bool ok = true;
                            uint dataset;
                            KDChartParams::PolarMarkerStyle style = PolarMarkerCircle;
                            if( element.hasAttribute( "Dataset" ) &&
                                    element.hasAttribute( "Style" ) ) {
                                dataset = element.attribute( "Dataset" ).toUInt( &ok );
                                if( ok )
                                    style = KDChartParams::stringToPolarMarkerStyle( element.attribute( "Style" ) );
                            } else
                                ok = false;
                            if( ok )
                                _polarMarkerStyles[dataset] = style;
                        } else if( tagName == "MarkerSize" ) {
                            int width = 1;
                            int height= 1;
                            bool ok = true;
                            if( element.hasAttribute( "Width" ) &&
                                    element.hasAttribute( "Height" ) ) {
                                width = element.attribute( "Width" ).toInt( &ok );
                                if( ok )
                                    height = element.attribute( "Height" ).toInt( &ok );
                            } else
                                ok = false;

                            if( ok ) {
                                _polarMarkerSize.setWidth( width );
                                _polarMarkerSize.setHeight( height );
                            }
                        } else if( tagName == "PolarLineWidth" ) {
                            int polarLineWidth;
                            if( KDXML::readIntNode( element, polarLineWidth ) )
                                _polarLineWidth = polarLineWidth;
                        } else {
                            qDebug( "Unknown subelement of PolarSettings found: %s", tagName.latin1() );
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "LegendSettings" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "Position" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                _legendPosition = KDChartParams::stringToLegendPosition( string );
                        } else if( tagName == "Orientation" ) {
                            Qt::Orientation value=Qt::Vertical;
                            if( KDXML::readOrientationNode( element, value ) )
                                _legendOrientation = value;
                        } else if( tagName == "ShowLines" ) {
                            bool showLines;
                            if( KDXML::readBoolNode( element, showLines ) )
                                _legendShowLines = showLines;
                        } else if( tagName == "Source" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                _legendSource = KDChartParams::stringToLegendSource( string );
                        } else if( tagName == "LegendText" ) {
                            bool ok = true;
                            uint dataset = 0;
                            QString text;
                            if( element.hasAttribute( "Dataset" ) &&
                                    element.hasAttribute( "Text" ) ) {
                                dataset = element.attribute( "Dataset" ).toUInt( &ok );
                                if( ok )
                                    text = element.attribute( "Text" );
                            } else
                                ok = false;
                            if( ok )
                                _legendText[dataset] = text;
                        } else if( tagName == "TextColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                _legendTextColor = color;
                        } else if( tagName == "TextFont" ) {
                            readChartFontNode( element,
                                    _legendFont,
                                    _legendFontUseRelSize,
                                    _legendFontRelSize );
                        } else if( tagName == "TitleText" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                _legendTitleText = string;
                        } else if( tagName == "TitleColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                _legendTitleTextColor = color;
                        } else if( tagName == "TitleFont" ) {
                            readChartFontNode( element,
                                    _legendTitleFont,
                                    _legendTitleFontUseRelSize,
                                    _legendTitleFontRelSize );
                        } else if( tagName == "Spacing" ) {
                            int spacing;
                            if( KDXML::readIntNode( element, spacing ) )
                                _legendSpacing = spacing;
                        } else {
                            qDebug( "Unknown subelement of LegendSettings found: %s", tagName.latin1() );
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "AxisSettings" ) {

                loadAxesFormXML(curAxisSettings, element);

            } else if( tagName == "HeaderFooterSettings" ) {
                KDChartParams::HdFtParams* hfSettings =
                    &( _hdFtParams[ curHFSettings ] );
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "Text" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                hfSettings->_text = string;
                        } else if( tagName == "Font" ) {
                            readChartFontNode( element,
                                    hfSettings->_font,
                                    hfSettings->_fontUseRelSize,
                                    hfSettings->_fontRelSize );
                        } else if( tagName == "Color" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                hfSettings->_color = color;
                        } else {
                            qDebug( "Unknown subelement of HeaderFooterSettings found: %s", tagName.latin1() );
                        }
                    }
                    node = node.nextSibling();
                }
                curHFSettings++; // one header/footer setting finished
            } else if( tagName == "GlobalLeading" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an elemente
                        QString tagName = element.tagName();
                        if( tagName == "Left" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _globalLeadingLeft = value;
                        }
                        else if( tagName == "Top" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _globalLeadingTop = value;
                        }
                        else if( tagName == "Right" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _globalLeadingRight = value;
                        }
                        else if( tagName == "Bottom" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _globalLeadingBottom = value;
                        } else {
                            qDebug( "Unknown subelement of GlobalLeading found: %s", tagName.latin1() );
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "DataValuesSettings1" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "PrintDataValues" ) {
                            bool value;
                            if( KDXML::readBoolNode( element, value ) )
                                _printDataValuesSettings._printDataValues = value;
                        } else if( tagName == "DivPow10" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _printDataValuesSettings._divPow10 = value;
                        } else if( tagName == "DigitsBehindComma" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _printDataValuesSettings._digitsBehindComma = value;
                        } else if( tagName == "Font" ) {
                            readChartFontNode( element,
                                    _printDataValuesSettings._dataValuesFont,
                                    _printDataValuesSettings._dataValuesUseFontRelSize,
                                    _printDataValuesSettings._dataValuesFontRelSize );
                        } else if( tagName == "Color" ) {
                            KDXML::readColorNode( element, _printDataValuesSettings._dataValuesColor );
                        } else if( tagName == "Background" ) {
                            KDXML::readBrushNode( element, _printDataValuesSettings._dataValuesBrush );
                        } else if( tagName == "AutoColor" ) {
                            KDXML::readBoolNode( element,
                                    _printDataValuesSettings._dataValuesAutoColor );
                        } else if( tagName == "AnchorNegativePosition" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _printDataValuesSettings._dataValuesAnchorNegativePosition = KDChartEnums::stringToPositionFlag( value );
                        } else if( tagName == "AnchorNegativeAlign" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _printDataValuesSettings._dataValuesAnchorNegativeAlign = value;
                        } else if( tagName == "AnchorNegativeDeltaX" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _printDataValuesSettings._dataValuesAnchorNegativeDeltaX = value;
                        } else if( tagName == "AnchorNegativeDeltaY" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _printDataValuesSettings._dataValuesAnchorNegativeDeltaY = value;
                        } else if( tagName == "NegativeRotation" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _printDataValuesSettings._dataValuesNegativeRotation = value;
                        } else if( tagName == "AnchorPositivePosition" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _printDataValuesSettings._dataValuesAnchorPositivePosition = KDChartEnums::stringToPositionFlag( value );
                        } else if( tagName == "AnchorPositiveAlign" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _printDataValuesSettings._dataValuesAnchorPositiveAlign = value;
                        } else if( tagName == "AnchorPositiveDeltaX" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _printDataValuesSettings._dataValuesAnchorPositiveDeltaX = value;
                        } else if( tagName == "AnchorPositiveDeltaY" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _printDataValuesSettings._dataValuesAnchorPositiveDeltaY = value;
                        } else if( tagName == "PositiveRotation" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _printDataValuesSettings._dataValuesPositiveRotation = value;
                        } else if( tagName == "LayoutPolicy" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _printDataValuesSettings._dataValuesLayoutPolicy = KDChartEnums::stringToLayoutPolicy( value );
                        } else if( tagName == "ShowInfinite" ) {
                            KDXML::readBoolNode( element, _printDataValuesSettings._dataValuesShowInfinite );
                        } else {
                            qDebug( "Unknown subelement of DataValuesSettings1 found: %s", tagName.latin1() );
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "DataValuesSettings2" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "PrintDataValues" ) {
                            bool value;
                            if( KDXML::readBoolNode( element, value ) )
                                _printDataValuesSettings2._printDataValues = value;
                        } else if( tagName == "DivPow10" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _printDataValuesSettings2._divPow10 = value;
                        } else if( tagName == "DigitsBehindComma" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _printDataValuesSettings2._digitsBehindComma = value;
                        } else if( tagName == "Font" ) {
                            readChartFontNode( element,
                                    _printDataValuesSettings2._dataValuesFont,
                                    _printDataValuesSettings2._dataValuesUseFontRelSize,
                                    _printDataValuesSettings2._dataValuesFontRelSize );
                        } else if( tagName == "Color" ) {
                            KDXML::readColorNode( element, _printDataValuesSettings2._dataValuesColor );
                        } else if( tagName == "Background" ) {
                            KDXML::readBrushNode( element, _printDataValuesSettings2._dataValuesBrush );
                        } else if( tagName == "AutoColor" ) {
                            KDXML::readBoolNode( element,
                                    _printDataValuesSettings2._dataValuesAutoColor );
                        } else if( tagName == "AnchorNegativePosition" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _printDataValuesSettings2._dataValuesAnchorNegativePosition = KDChartEnums::stringToPositionFlag( value );
                        } else if( tagName == "AnchorNegativeAlign" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _printDataValuesSettings2._dataValuesAnchorNegativeAlign = value;
                        } else if( tagName == "AnchorNegativeDeltaX" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _printDataValuesSettings2._dataValuesAnchorNegativeDeltaX = value;
                        } else if( tagName == "AnchorNegativeDeltaY" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _printDataValuesSettings2._dataValuesAnchorNegativeDeltaY = value;
                        } else if( tagName == "NegativeRotation" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _printDataValuesSettings2._dataValuesNegativeRotation = value;
                        } else if( tagName == "AnchorPositivePosition" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _printDataValuesSettings2._dataValuesAnchorPositivePosition = KDChartEnums::stringToPositionFlag( value );
                        } else if( tagName == "AnchorPositiveAlign" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _printDataValuesSettings2._dataValuesAnchorPositiveAlign = value;
                        } else if( tagName == "AnchorPositiveDeltaX" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _printDataValuesSettings2._dataValuesAnchorPositiveDeltaX = value;
                        } else if( tagName == "AnchorPositiveDeltaY" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _printDataValuesSettings2._dataValuesAnchorPositiveDeltaY = value;
                        } else if( tagName == "PositiveRotation" ) {
                            int value;
                            if( KDXML::readIntNode( element, value ) )
                                _printDataValuesSettings2._dataValuesPositiveRotation = value;
                        } else if( tagName == "LayoutPolicy" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _printDataValuesSettings2._dataValuesLayoutPolicy = KDChartEnums::stringToLayoutPolicy( value );
                        } else if( tagName == "ShowInfinite" ) {
                            KDXML::readBoolNode( element, _printDataValuesSettings2._dataValuesShowInfinite );
                        } else {
                            qDebug( "Unknown subelement of DataValuesSettings2 found: %s", tagName.latin1() );
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "DataValuesGlobalSettings" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "allowOverlappingTexts" ) {
                            bool value;
                            if( KDXML::readBoolNode( element, value ) )
                                _allowOverlappingDataValueTexts = value;
                        }
                        else
                            qDebug( "Unknown subelement of DataValuesGlobalSettings found: %s", tagName.latin1() );
                        // do _not_ return false here (to enable future extentions)
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "AreaMap" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "FrameSettings" ) {
                            KDChartFrameSettings* frameSettings = new KDChartFrameSettings;
                            uint areaId = KDChartEnums::AreaUNKNOWN;
                            if( KDChartFrameSettings::readFrameSettingsNode(
                                        element, *frameSettings, areaId ) ) {
                                QString str;
                                if(areaId == KDChartEnums::AreaChartDataRegion)
                                    str = QString( "%1/%2/%3/%4" )
                                              .arg( areaId, 5 )
                                              .arg( frameSettings->dataRow(), 5 )
                                              .arg( frameSettings->dataCol(), 5 )
                                              .arg( 0, 5 );//frameSettings->data3rd(), 5 );
                                else
                                    str = QString( "%1/-----/-----/-----" ).arg( areaId, 5 );
                                _areaDict.insert( str, frameSettings );
                            }
                        }
                        else
                            qDebug( "Unknown tag in AreaMap found: %s", tagName.latin1() );
                        // do _not_ return false here (to enable future extentions)
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "CustomBoxMap" ) {
                QDomNode node = element.firstChild();
                int curNumber = -1;
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "Number" ) {
                            KDXML::readIntNode( element, curNumber );
                        } else if( tagName == "FrameSettings" ) {
                            Q_ASSERT( curNumber != -1 ); // there was a Dataset tag before
                            KDChartCustomBox customBox;
                            KDChartCustomBox::readCustomBoxNode( element,
                                    customBox );
                            _customBoxDict.insert( curNumber, customBox.clone() );
                        }
                        else
                            qDebug( "Unknown tag in CustomBoxMap found: %s", tagName.latin1() );
                    }
                    node = node.nextSibling();
                }
            } else {
                //qDebug( "Unknown second-level element found: %s", tagName.latin1() );
                // NOTE: We do *not* 'return false' here but continue normal operation
                //       since additional elements might have been added in future versions
            }
        }
        node = node.nextSibling();
    }
    return true;
}


/**
  Creates a DOM element node that represents a map of QColor objects
  for use in a DOM document.

  \param doc the DOM document to which the node will belong
  \param parent the parent node to which the new node will be appended
  \param elementName the name of the new node
  \param map the map of colors to be represented
  */
void KDChartParams::createColorMapNode( QDomDocument& doc, QDomNode& parent,
        const QString& elementName,
        const QMap< uint, QColor >& map )
{
    QDomElement mapElement =
        doc.createElement( elementName );
    parent.appendChild( mapElement );
    for( QMap<uint,QColor>::ConstIterator it = map.begin();
            it != map.end(); ++it ) {
        // Dataset element
        QDomElement datasetElement = doc.createElement( "Dataset" );
        mapElement.appendChild( datasetElement );
        QDomText datasetContent =
            doc.createTextNode( QString::number( it.key() ) );
        datasetElement.appendChild( datasetContent );
        // Color element
        KDXML::createColorNode( doc, mapElement, "Color", it.data() );
    }
}


/**
  Creates a DOM element node that represents a map of doubles
  for use in a DOM document.

  \param doc the DOM document to which the node will belong
  \param parent the parent node to which the new node will be appended
  \param elementName the name of the new node
  \param map the map of doubles to be represented
  */
void KDChartParams::createDoubleMapNode( QDomDocument& doc, QDomNode& parent,
        const QString& elementName,
        const QMap< int, double >& map )
{
    QDomElement mapElement =
        doc.createElement( elementName );
    parent.appendChild( mapElement );
    for( QMap<int,double>::ConstIterator it = map.begin();
            it != map.end(); ++it ) {
        // Dataset element
        QDomElement valueElement = doc.createElement( "Value" );
        mapElement.appendChild( valueElement );
        QDomText valueContent =
            doc.createTextNode( QString::number( it.key() ) );
        valueElement.appendChild( valueContent );
        // Color element
        KDXML::createDoubleNode( doc, mapElement, "Factor", it.data() );
    }
}


void dataCoordToElementAttr(const QVariant& val, QDomElement& element, const QString& postfix)
{
    if( QVariant::Double == val.type() )
        element.setAttribute( "DoubleValue"+postfix,
                              QString::number( val.toDouble() ) );
    else if( QVariant::String == val.type() )
        element.setAttribute( "StringValue"+postfix, val.toString() );
    else if( QVariant::DateTime == val.type() )
        element.setAttribute( "DateTimeValue"+postfix,
                              val.toDateTime().toString( Qt::ISODate ) );
    else
        element.setAttribute( "NoValue"+postfix, "true" );
}

/**
  Creates a DOM element node that represents a
  chart value for use in a DOM document.

  \param doc the DOM document to which the node will belong
  \param parent the parent node to which the new node will be appended
  \param elementName the name of the new node
  \param data the chart value to be represented
  */
void KDChartParams::createChartValueNode( QDomDocument& doc, QDomNode& parent,
        const QString& elementName,
        const QVariant& valY,
        const QVariant& valX,
        const int& propID )
{
    QDomElement element = doc.createElement( elementName );
    parent.appendChild( element );
    dataCoordToElementAttr( valY, element, "" ); // no postfix for Y value: backwards compat.
    dataCoordToElementAttr( valX, element, "X" );
    element.setAttribute( "PropertySetID",
                          QString::number( propID ) );
}



/**
  Creates a DOM element node that represents a font used in a
  chart for use in a DOM document.

  \param doc the DOM document to which the node will belong
  \param parent the parent node to which the new node will be appended
  \param elementName the name of the new node
  \param font the font to be resented
  \param useRelFont the specification whether the font size
  is relative
  \param relFont the relative font size
  \param minFont the minimal font size in points, leave this parameter out if not needed
  */
void KDChartParams::createChartFontNode( QDomDocument& doc, QDomNode& parent,
        const QString& elementName,
        const QFont& font, bool useRelFont,
        int relFont,
        int minFont )
{
    QDomElement chartFontElement = doc.createElement( elementName );
    parent.appendChild( chartFontElement );
    KDXML::createFontNode( doc, chartFontElement, "Font", font );
    KDXML::createBoolNode( doc, chartFontElement, "UseRelFontSize",
            useRelFont );
    KDXML::createIntNode( doc, chartFontElement, "RelFontSize", relFont );
    if( 0 <= minFont )
        KDXML::createIntNode( doc, chartFontElement, "MinFontSize", minFont );
}





/**
  Creates a DOM element node that represents a color map
  for use in a DOM document.

  \param doc the DOM document to which the node will belong
  \param parent the parent node to which the new node will be appended
  \param elementName the name of the new node
  \param map the color map to be represented
  */
bool KDChartParams::readColorMapNode( const QDomElement& element,
        QMap<uint,QColor>* value )
{
    QDomNode node = element.firstChild();
    int curDataset = -1;
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "Dataset" ) {
                KDXML::readIntNode( element, curDataset );
            } else if( tagName == "Color" ) {
                Q_ASSERT( curDataset != -1 ); // there was a Dataset tag before
                QColor color;
                KDXML::readColorNode( element, color );
                value->insert( curDataset, color );
            } else {
                qDebug( "Unknown tag in color map" );
            }
        }
        node = node.nextSibling();
    }

    return true;
}


/**
  Reads data from a DOM element node that represents a double
  map and fills a double map with the data.

  \param element the DOM element to read from
  \param map the frame settings object to read the data into
  */
bool KDChartParams::readDoubleMapNode( const QDomElement& element,
        QMap<int,double>* value )
{
    QDomNode node = element.firstChild();
    int curValue = -1;
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "Value" ) {
                KDXML::readIntNode( element, curValue );
            } else if( tagName == "Factor" ) {
                Q_ASSERT( curValue != -1 ); // there was a Value tag before
                double doubleValue;
                KDXML::readDoubleNode( element, doubleValue );
                value->insert( curValue, doubleValue );
            } else {
                qDebug( "Unknown tag in double map" );
            }
        }
        node = node.nextSibling();
    }

    return true;
}



/**
  Reads data from a DOM element node that represents a font
  uses in a chart and fills the reference parameters
  with the data.

  \param element the DOM element to read from
  \param font the represented font
  \param useRelFont whether the font size is relative
  \param relFontSize the relative font size
  */
bool KDChartParams::readChartFontNode( const QDomElement& element,
        QFont& font,
        bool& useRelFont,
        int& relFontSize,
        int* minFontSize )
{
    bool ok = true;
    QFont tempFont;
    bool tempRelFont;
    int tempRelFontSize;
    int tempMinFontSize=-1;
    QDomNode node = element.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "Font" ) {
                ok = ok & KDXML::readFontNode( element, tempFont );
            } else if( tagName == "UseRelFontSize" ) {
                ok = ok & KDXML::readBoolNode( element, tempRelFont );
            } else if( tagName == "RelFontSize" ) {
                ok = ok & KDXML::readIntNode( element, tempRelFontSize );
            } else if( tagName == "MinFontSize" ) {
                ok = ok & KDXML::readIntNode( element, tempMinFontSize );
            } else {
                qDebug( "Unknown tag in color map" );
            }
        }
        node = node.nextSibling();
    }

    if( ok ) {
        font = tempFont;
        useRelFont = tempRelFont;
        relFontSize = tempRelFontSize;
        if( minFontSize && 0 <= tempMinFontSize )
            *minFontSize = tempMinFontSize;
    }

    return ok;
}


bool foundCoordAttribute( const QDomElement& element, const QString& postfix,
                          QVariant& val )
{
    if( element.hasAttribute( "NoValue"+postfix ) )
        val = QVariant();
    else if( element.hasAttribute( "DoubleValue"+postfix ) )
        val = element.attribute(   "DoubleValue"+postfix );
    else if( element.hasAttribute( "DateTimeValue"+postfix ) )
        val = element.attribute(   "DateTimeValue"+postfix );
    else if( element.hasAttribute( "StringValue"+postfix ) )
        val = element.attribute(   "StringValue"+postfix );
    else
        return false;

    return true;
}

// PENDING(kalle) Support DateTime values, even when writing.
/**
  Reads data from a DOM element node that represents a chart
  value and fills a KDChartData object with the data.

  \param element the DOM element to read from
  \param value the chart data object to read the data into
  */
bool KDChartParams::readChartValueNode( const QDomElement& element,
        QVariant& valY,
        QVariant& valX,
        int& propID )
{
    if( foundCoordAttribute( element, "",  valY ) ||
        foundCoordAttribute( element, "Y", valY ) ){  // valY must be there
        if( !foundCoordAttribute( element, "X", valX ) ){
            valX = QVariant();
        }
        propID = 0;
        if( element.hasAttribute( "PropertySetID" ) ) {
            bool ok;
            int i = element.attribute( "PropertySetID" ).toInt( &ok );
            if( ok )
                propID = i;
        }
        return true;  // if Y value found everything is OK
    }

    return false;
}


/**
  Converts the specified chart type enum to a string representation.

  \param type the type enum to convert
  \return the string representation of the type enum
  */
QString KDChartParams::chartTypeToString( ChartType type )
{
    switch( type ) {
        case NoType:
            return "NoType";
        case Bar:
            return "Bar";
        case Line:
            return "Line";
        case Area:
            return "Area";
        case Pie:
            return "Pie";
        case HiLo:
            return "HiLo";
        case BoxWhisker:
            return "BoxWhisker";
        case Ring:
            return "Ring";
        case Polar:
            return "Polar";
        default: // should not happen
            return "NoType";
    }
}


/**
  Converts the specified string to a chart type enum value.

  \param string the string to convert
  \return the chart type enum value
  */
KDChartParams::ChartType KDChartParams::stringToChartType( const QString& string )
{
    if( string == "NoType" )
        return NoType;
    else if( string == "Bar" )
        return Bar;
    else if( string == "Line" )
        return Line;
    else if( string == "Area" )
        return Area;
    else if( string == "Pie" )
        return Pie;
    else if( string == "HiLo" )
        return HiLo;
    else if( string == "BoxWhisker" )
        return BoxWhisker;
    else if( string == "Ring" )
        return Ring;
    else if( string == "Polar" )
        return Polar;
    else // default, should not happen
        return NoType;
}



QString KDChartParams::markerStyleToString( int style )
{
    switch( style ) {
        case LineMarkerSquare:
            return "Square";
        case LineMarkerDiamond:
            return "Diamond";
        case LineMarkerCircle:
            return "Circle";
        case LineMarker1Pixel:
            return "one Pixel";
        case LineMarker4Pixels:
            return "four Pixels";
        case LineMarkerRing:
            return "Ring";
        case LineMarkerCross:
            return "Cross";
        case LineMarkerFastCross:
            return "fast Cross";
        default: // should not happen
            qDebug( "Unknown marker style" );
            return "Circle";
    }
}
QString KDChartParams::markerStyleToStringTr( int style )
{
    switch( style ) {
        case LineMarkerSquare:
            return tr( "Square" );
        case LineMarkerDiamond:
            return tr( "Diamond" );
        case LineMarkerCircle:
            return tr( "Circle" );
        case LineMarker1Pixel:
            return tr( "One pixel" );
        case LineMarker4Pixels:
            return tr( "Four pixels" );
        case LineMarkerRing:
            return tr( "Ring" );
        case LineMarkerCross:
            return tr( "Cross" );
        case LineMarkerFastCross:
            return tr( "fast Cross" );
        default: // should not happen
            qDebug( "Unknown line marker style!" );
            return tr( "Circle" );
    }
}
int KDChartParams::stringToMarkerStyle( const QString& string )
{
    if( string == "Square" )
        return LineMarkerSquare;
    else if( string == "Diamond" )
        return LineMarkerDiamond;
    else if( string == "Circle" )
        return LineMarkerCircle;
    else if( string == "one Pixel" )
        return LineMarker1Pixel;
    else if( string == "four Pixels" )
        return LineMarker4Pixels;
    else if( string == "Ring" )
        return LineMarkerRing;
    else if( string == "Cross" )
        return LineMarkerCross;
    else if( string == "fast Cross" )
        return LineMarkerFastCross;
    else // default, should not happen
        return LineMarkerCircle;
}
int KDChartParams::stringToMarkerStyleTr( const QString& string )
{
    if( string == tr( "Square" ) )
        return LineMarkerSquare;
    else if( string == tr( "Diamond" ) )
        return LineMarkerDiamond;
    else if( string == tr( "Circle" ) )
        return LineMarkerCircle;
    else if( string == tr( "One pixel" ) )
        return LineMarker1Pixel;
    else if( string == tr( "Four pixels" ) )
        return LineMarker4Pixels;
    else if( string == tr( "Ring" ) )
        return LineMarkerRing;
    else if( string == tr( "Cross" ) )
        return LineMarkerCross;
    else if( string == tr( "fast Cross" ) )
        return LineMarkerFastCross;
    else // default, should not happen
        return LineMarkerCircle;
}



/**
  Converts the specified bar chart subtype enum to a string representation.

  \param type the type enum to convert
  \return the string representation of the type enum
  */
QString KDChartParams::barChartSubTypeToString( BarChartSubType type ) {
    switch( type ) {
        case BarNormal:
            return "BarNormal";
        case BarStacked:
            return "BarStacked";
        case BarPercent:
            return "BarPercent";
        case BarMultiRows:
            return "BarMultiRows";
        default: // should not happen
            qDebug( "Unknown bar type" );
            return "BarNormal";
    }
}


/**
  Converts the specified string to a bar chart subtype enum value.

  \param string the string to convert
  \return the bar chart subtype enum value
  */
KDChartParams::BarChartSubType KDChartParams::stringToBarChartSubType( const QString& string ) {
    if( string == "BarNormal" )
        return BarNormal;
    else if( string == "BarStacked" )
        return BarStacked;
    else if( string == "BarPercent" )
        return BarPercent;
    else if( string == "BarMultiRows" )
        return BarMultiRows;
    else // should not happen
        return BarNormal;
}



/**
  Converts the specified string to a line chart subtype enum value.

  \param string the string to convert
  \return the line chart subtype enum value
  */
KDChartParams::LineChartSubType KDChartParams::stringToLineChartSubType( const QString& string ) {
    if( string == "LineNormal" )
        return LineNormal;
    else if( string == "LineStacked" )
        return LineStacked;
    else if( string == "LinePercent" )
        return LinePercent;
    else // should not happen
        return LineNormal;
}



/**
  Converts the specified line chart subtype enum to a string representation.

  \param type the type enum to convert
  \return the string representation of the type enum
  */
QString KDChartParams::lineChartSubTypeToString( LineChartSubType type ) {
    switch( type ) {
        case LineNormal:
            return "LineNormal";
        case LineStacked:
            return "LineStacked";
        case LinePercent:
            return "LinePercent";
        default: // should not happen
            qDebug( "Unknown bar type" );
            return "LineNormal";
    }
}


/**
  Converts the specified line marker style enum to a string
  representation.

  \param type the type enum to convert
  \return the string representation of the type enum
  */
QString KDChartParams::lineMarkerStyleToString( LineMarkerStyle style )
{
    return markerStyleToString( style );
}


/**
  Converts the specified line marker style enum to a localized
  string representation that can be used for string output.

  \param type the type enum to convert
  \return the localized string representation of the type enum
  */
QString KDChartParams::lineMarkerStyleToStringTr( LineMarkerStyle style )
{
    return markerStyleToStringTr( style );
}


/**
  Converts the specified string to a line marker style value.

  \param string the string to convert
  \return the line marker style enum value
  */
KDChartParams::LineMarkerStyle KDChartParams::stringToLineMarkerStyle( const QString& string )
{
    return static_cast<KDChartParams::LineMarkerStyle>(stringToMarkerStyle( string ));
}

/**
  Converts the specified localized string to a line marker style
  value.

  \param string the string to convert
  \return the line marker style enum value
  */
KDChartParams::LineMarkerStyle KDChartParams::stringToLineMarkerStyleTr( const QString& string )
{
    return static_cast<KDChartParams::LineMarkerStyle>(stringToMarkerStyleTr( string ));
}


/**
  Converts the specified area chart subtype enum to a string representation.

  \param type the subtype enum to convert
  \return the string representation of the type enum
  */
QString KDChartParams::areaChartSubTypeToString( AreaChartSubType type ) {
    switch( type ) {
        case AreaNormal:
            return "AreaNormal";
        case AreaStacked:
            return "AreaStacked";
        case AreaPercent:
            return "AreaPercent";
        default: // should not happen
            qDebug( "Unknown area chart subtype" );
            return "AreaNormal";
    }
}


/**
  Converts the specified string to a area chart subtype enum value.

  \param string the string to convert
  \return the area chart subtype enum value
  */
KDChartParams::AreaChartSubType KDChartParams::stringToAreaChartSubType( const QString& string ) {
    if( string == "AreaNormal" )
        return AreaNormal;
    else if( string == "AreaStacked" )
        return AreaStacked;
    else if( string == "AreaPercent" )
        return AreaPercent;
    else // should not happen
        return AreaNormal;
}


/**
  Converts the specified area location enum to a string representation.

  \param type the location enum to convert
  \return the string representation of the type enum
  */
QString KDChartParams::areaLocationToString( AreaLocation type ) {
    switch( type ) {
        case AreaAbove:
            return "Above";
        case AreaBelow:
            return "Below";
        default: // should not happen
            qDebug( "Unknown area location" );
            return "Below";
    }
}


/**
  Converts the specified string to an area location enum value.

  \param string the string to convert
  \return the aration location enum value
  */
KDChartParams::AreaLocation KDChartParams::stringToAreaLocation( const QString& string ) {
    if( string == "Above" )
        return AreaAbove;
    else if( string == "Below" )
        return AreaBelow;
    else // default, should not happen
        return AreaBelow;
}


/**
  Converts the specified string to a polar chart subtype enum value.

  \param string the string to convert
  \return the polar chart subtype enum value
  */
KDChartParams::PolarChartSubType KDChartParams::stringToPolarChartSubType( const QString& string ) {
    if( string == "PolarNormal" )
        return PolarNormal;
    else if( string == "PolarStacked" )
        return PolarStacked;
    else if( string == "PolarPercent" )
        return PolarPercent;
    else // should not happen
        return PolarNormal;
}


/**
  Converts the specified polar chart subtype enum to a string representation.

  \param type the type enum to convert
  \return the string representation of the type enum
  */
QString KDChartParams::polarChartSubTypeToString( PolarChartSubType type ) {
    switch( type ) {
        case PolarNormal:
            return "PolarNormal";
        case LineStacked:
            return "PolarStacked";
        case LinePercent:
            return "PolarPercent";
        default: // should not happen
            qDebug( "Unknown polar type" );
            return "PolarNormal";
    }
}


/**
  Converts the specified polar marker style enum to a string
  representation.

  \param type the type enum to convert
  \return the string representation of the type enum
  */
QString KDChartParams::polarMarkerStyleToString( PolarMarkerStyle style )
{
    return markerStyleToString( style );
}


/**
  Converts the specified polar marker style enum to a localized
  string representation that can be used for string output.

  \param type the type enum to convert
  \return the localized string representation of the type enum
  */
QString KDChartParams::polarMarkerStyleToStringTr( PolarMarkerStyle style )
{
    return markerStyleToStringTr( style );
}


/**
  Converts the specified string to a polar marker style value.

  \param string the string to convert
  \return the polar marker style enum value
  */
KDChartParams::PolarMarkerStyle KDChartParams::stringToPolarMarkerStyle( const QString& string )
{
    return static_cast<KDChartParams::PolarMarkerStyle>(stringToMarkerStyle( string ));
}


/**
  Converts the specified localized string to a polar marker style
  value.

  \param string the string to convert
  \return the polar marker style enum value
  */
KDChartParams::PolarMarkerStyle KDChartParams::stringToPolarMarkerStyleTr( const QString& string )
{
    return static_cast<KDChartParams::PolarMarkerStyle>(stringToMarkerStyle( string ));
}


/**
  Converts the specified HiLo chart subtype enum to a string representation.

  \param type the subtype enum to convert
  \return the string representation of the type enum
  */
QString KDChartParams::hiLoChartSubTypeToString( HiLoChartSubType type ) {
    switch( type ) {
        case HiLoSimple:
            return "HiLoSimple";
        case HiLoClose:
            return "HiLoClose";
        case HiLoOpenClose:
            return "HiLoOpenClose";
        default: // should not happen
            qDebug( "Unknown HiLo chart subtype" );
            return "HiLoNormal";
    }
}


/**
  Converts the specified string to a HiLo chart subtype enum value.

  \param string the string to convert
  \return the HiLo chart subtype enum value
  */
KDChartParams::HiLoChartSubType KDChartParams::stringToHiLoChartSubType( const QString& string ) {
    if( string == "HiLoSimple" )
        return HiLoSimple;
    else if( string == "HiLoClose" )
        return HiLoClose;
    else if( string == "HiLoOpenClose" )
        return HiLoOpenClose;
    else // should not happen
        return HiLoNormal;
}


/**
  Converts the specified string to a BoxWhisker chart subtype enum value.

  \param string the string to convert
  \return the BoxWhisker chart subtype enum value
  */
KDChartParams::BWChartSubType KDChartParams::stringToBWChartSubType( const QString& string ) {
    if( string == "BWSimple" )
        return BWSimple;
    else // should not happen
        return BWNormal;
}

/**
  Converts the specified BWStatVal enum to a string representation.

  \param type the BWStatVal enum to convert
  \return the string representation of the type BWStatVal
  */
QString KDChartParams::bWChartStatValToString( BWStatVal type ) {
    switch( type ) {
        case        UpperOuterFence:
            return "UpperOuterFence";
        case        UpperInnerFence:
            return "UpperInnerFence";
        case        Quartile3:
            return "Quartile3";
        case        Median:
            return "Median";
        case        Quartile1:
            return "Quartile1";
        case        LowerInnerFence:
            return "LowerInnerFence";
        case        LowerOuterFence:
            return "LowerOuterFence";
        case        MaxValue:
            return "MaxValue";
        case        MeanValue:
            return "MeanValue";
        case        MinValue:
            return "MinValue";
        default: // should not happen
            qDebug( "Unknown BoxWhisker statistical value type" );
            return "unknown";
    }
}

/**
  Converts the specified string to a BWStatVal enum value.

  \param string the string to convert
  \return the BWStatVal enum value
  */
KDChartParams::BWStatVal KDChartParams::stringToBWChartStatVal( const QString& string ) {
    if(      string == "UpperOuterFence" )
        return     UpperOuterFence;
    else if( string == "UpperInnerFence" )
        return     UpperInnerFence;
    else if( string == "Quartile3" )
        return     Quartile3;
    else if( string == "Median" )
        return     Median;
    else if( string == "Quartile1" )
        return     Quartile1;
    else if( string == "LowerInnerFence" )
        return     LowerInnerFence;
    else if( string == "LowerOuterFence" )
        return     LowerOuterFence;
    else if( string == "MaxValue" )
        return     MaxValue;
    else if( string == "MeanValue" )
        return     MeanValue;
    else if( string == "MinValue" )
        return     MinValue;
    else // should not happen
        return BWStatValUNKNOWN;
}


/**
  Converts the specified legend position enum to a string representation.

  \param type the legend position enum to convert
  \return the string representation of the type enum
  */
QString KDChartParams::legendPositionToString( LegendPosition pos ) {
    switch( pos ) {
        case NoLegend:
            return "NoLegend";
        case LegendTop:
            return "LegendTop";
        case LegendBottom:
            return "LegendBottom";
        case LegendLeft:
            return "LegendLeft";
        case LegendRight:
            return "LegendRight";
        case LegendTopLeft:
            return "LegendTopLeft";
        case LegendTopLeftTop:
            return "LegendTopLeftTop";
        case LegendTopLeftLeft:
            return "LegendTopLeftLeft";
        case LegendBottomLeft:
            return "LegendBottomLeft";
        case LegendBottomLeftBottom:
            return "LegendBottomLeftTop";
        case LegendBottomLeftLeft:
            return "LegendBottomLeftLeft";
        case LegendTopRight:
            return "LegendTopRight";
        case LegendTopRightTop:
            return "LegendTopRightTop";
        case LegendTopRightRight:
            return "LegendTopRightRight";
        case LegendBottomRight:
            return "LegendBottomRight";
        case LegendBottomRightBottom:
            return "LegendBottomRightTop";
        case LegendBottomRightRight:
            return "LegendBottomRightRight";
        default: // should not happen
            qDebug( "Unknown legend position" );
            return "LegendLeft";
    }
}


/**
  Converts the specified string to a legend position enum value.

  \param string the string to convert
  \return the legend position enum value
  */
KDChartParams::LegendPosition KDChartParams::stringToLegendPosition( const QString& string ) {
    if( string == "NoLegend" )
        return NoLegend;
    else if( string == "LegendTop" )
        return LegendTop;
    else if( string == "LegendBottom" )
        return LegendBottom;
    else if( string == "LegendLeft" )
        return LegendLeft;
    else if( string == "LegendRight" )
        return LegendRight;
    else if( string == "LegendTopLeft" )
        return LegendTopLeft;
    else if( string == "LegendTopLeftTop" )
        return LegendTopLeftTop;
    else if( string == "LegendTopLeftLeft" )
        return LegendTopLeftLeft;
    else if( string == "LegendBottomLeft" )
        return LegendBottomLeft;
    else if( string == "LegendBottomLeftBottom" )
        return LegendBottomLeftBottom;
    else if( string == "LegendBottomLeftLeft" )
        return LegendBottomLeftLeft;
    else if( string == "LegendTopRight" )
        return LegendTopRight;
    else if( string == "LegendTopRightTop" )
        return LegendTopRightTop;
    else if( string == "LegendTopRightRight" )
        return LegendTopRightRight;
    else if( string == "LegendBottomRight" )
        return LegendBottomRight;
    else if( string == "LegendBottomRightBottom" )
        return LegendBottomRightBottom;
    else if( string == "LegendBottomRightRight" )
        return LegendBottomRightRight;
    else // default, should not happen
        return LegendLeft;
}

/**
  Converts the specified legend source enum to a string representation.

  \param source the legend source enum to convert
  \return the string representation of the type enum
  */
QString KDChartParams::legendSourceToString( LegendSource source ) {
    switch( source ) {
        case LegendManual:
            return "Manual";
        case LegendFirstColumn:
            return "FirstColumn";
        case LegendAutomatic:
            return "Automatic";
        default: // should not happen
            qDebug( "Unknown legend source" );
            return "Automatic";
    }
}


/**
  Converts the specified string to a legend source enum value.

  \param string the string to convert
  \return the legend source enum value
  */
KDChartParams::LegendSource KDChartParams::stringToLegendSource( const QString& string ) {
    if( string == "Manual" )
        return LegendManual;
    else if( string == "FirstColumn" )
        return LegendFirstColumn;
    else if( string == "Automatic" )
        return LegendAutomatic;
    else // default, should not happen
        return LegendAutomatic;
}


/**
  Converts the specified chart source mode enum value to a string.

  \param mode the chart source mode enum value to convert
  \return the string
  */
QString KDChartParams::chartSourceModeToString( const SourceMode& mode )
{
    switch( mode ){
    case UnknownMode:
        return "UnknownMode";
    case DontUse:
        return "DontUse";
    case DataEntry:
        return "DataEntry";
    case AxisLabel:
        return "AxisLabel";
    case LegendText:
        return "LegendText";
    case ExtraLinesAnchor:
        return "ExtraLinesAnchor";
    default: // should not happen
        return "UnknownMode";
    }
}


/**
  Converts the specified string to a chart source mode enum value.

  \param string the string to convert
  \return the chart source mode enum value
  */
KDChartParams::SourceMode KDChartParams::stringToChartSourceMode( const QString& string )
{
    if( string.isEmpty() )
        return UnknownMode;
    // compatibility with pre-1.0 KDChart stream format:
    bool bOk;
    int mode = string.toInt( &bOk );
    if( bOk && mode >= 0 && mode <= Last_SourceMode )
        return (KDChartParams::SourceMode)mode;
    // new (KDChart 1.0...) stream format:
    if( string == "UnknownMode" )
        return UnknownMode;
    if( string == "DontUse" )
        return DontUse;
    if( string == "DataEntry" )
        return DataEntry;
    if( string == "AxisLabel" )
        return AxisLabel;
    if( string == "LegendText" )
        return LegendText;
    if( string == "ExtraLinesAnchor" )
        return ExtraLinesAnchor;
    // should not happen
    return UnknownMode;
}


/**
  Converts the specified BoxWhisker chart subtype enum to a string
  representation.

  \param type the subtype enum to convert
  \return the string representation of the type enum
  */
QString KDChartParams::bWChartSubTypeToString( BWChartSubType type ) {
    switch( type ) {
        case BWSimple:
            return "BWSimple";
        default: // should not happen
            qDebug( "Unknown BoxWhisker chart subtype" );
            return "BWNormal";
    }
}

