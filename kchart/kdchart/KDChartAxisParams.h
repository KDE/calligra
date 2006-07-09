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
#ifndef __KDCHARTAXISPARAMS_H__
#define __KDCHARTAXISPARAMS_H__

#include <qfont.h>
#include <qcolor.h>
#include <qmap.h>
#include <qobject.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include <qvariant.h>

#include "KDChartGlobal.h"

class KDChartParams;

/**
  Use this to specify that the axis label limits
  are to be calculated automatically.

  \sa setAxisValues
  */
#define KDCHART_AXIS_LABELS_AUTO_LIMIT QVariant( 140319.64 )
// If you edit the above, also edit KD Chart.cpp

/**
  Use this to specify that the step-width from one label
  to the other shall be calculated automatically.

  \sa setAxisValues
  */
#define KDCHART_AXIS_LABELS_AUTO_DELTA 140319.64

/**
  Use this to specify that an automatically computed amount of
  axis labels are to be skipped if there is not enough space
  for displaying all of them.
  This is usefull in case you have lots of entries in one dataset.

  \sa setAxisValues
  */
#define KDCHART_AXIS_LABELS_AUTO_LEAVEOUT 14364

/**
  Use this to specify that date/time format to be used for
  displaying the axis labels is to be determined automatically.
  */
#define KDCHART_AXIS_LABELS_AUTO_DATETIME_FORMAT "AUTO"

/**
  Use this to specify that the number of digits to be shown
  on the axis labels behind the comma is to be calculated
  automatically.

  \sa setAxisValues
  */
#define KDCHART_AXIS_LABELS_AUTO_DIGITS 14364

/**
  Use this to reset the grid colour to its default value
  for re-doing changes made by calling \c setAxisGridColor().

  \sa setAxisGridColor
  */


#define KDCHART_DEFAULT_AXIS_GRID_COLOR QColor( 0xA0, 0xA0, 0xA0 )

/**
  Use this to specify that the width of the axis grid lines
  shall be equal to the width of the axis line.

  \sa setAxisGridLineWidth
  */
#define KDCHART_AXIS_GRID_AUTO_LINEWIDTH 14364

/**
  Use this to specify that the zero-line shall NOT be omitted
  in case all of the data are grouped far away from zero.

  \sa setAxisValues
  */
#define KDCHART_AXIS_IGNORE_EMPTY_INNER_SPAN 1

/**
  Use this to prevent \c setAxisValues from changing the current
  setting of _axisMaxEmptyInnerSpan.

  \sa setAxisValues
  */

#define KDCHART_DONT_CHANGE_EMPTY_INNER_SPAN_NOW -2




/** \file KDChartAxisParams.h
  \brief Provide access to the chart axis parameters.

  Use the KDChartAxisParams class to modify parameters of one axis each.
  */

class KDCHART_EXPORT KDChartAxisParams : public QObject
{
    Q_OBJECT
    Q_ENUMS(AxisType)
    Q_ENUMS(AxisPos)
    Q_ENUMS(AxisAreaMode)
    Q_ENUMS(AxisCalcMode)
    Q_ENUMS(LabelsFromDataRow)
    Q_ENUMS(ValueScale)

    // Neede by QSA
    Q_ENUMS( PenStyle )

public:

    enum AxisType { AxisTypeUnknown,
                    AxisTypeEAST,
                    AxisTypeNORTH,
                    AxisUP };

public slots:  // PENDING(blackie) Go through this file and reorder, so all slots are in one section
    void setAxisType( AxisType axisType );


    AxisType axisType() const { return _axisType; }
    static QString axisTypeToString( AxisType type );
    static AxisType stringToAxisType( const QString& type );

public:

    // Attention: In case you extent AxisPos, make sure to also
    //            increase MAX_AXES and AxisPosEND.
    enum AxisPos { AxisPosSTART = 0,

                   AxisPosBottom         = 0,
                   AxisPosSagittal            = 0,  //   <-- for POLAR charts
                   AxisPosSaggital            = 0,  //   <-- wrong spelling kept for backward compatibility
                   AxisPosLeft           = 1,
                   AxisPosCircular            = 1,  //   <-- for POLAR charts
                   AxisPosLowerRightEdge = 2,

                   AxisPosTop = 3,
                   AxisPosRight = 4,
                   AxisPosLowerLeftEdge = 5,

                   // diese Markierung muss jeweils mitgepflegt werden,
                   // wenn AxisPos erweitert werden sollte.
                   AxisPosAdditionalsSTART = 6,

                   AxisPosBottom2 = 6,
                   AxisPosLeft2 = 7,
                   AxisPosLowerRightEdge2 = 8,

                   AxisPosTop2 = 9,
                   AxisPosRight2 = 10,
                   AxisPosLowerLeftEdge2 = 11,
                   // auch diese Markierung muss jeweils mitgepflegt werden,
                   // wenn AxisPos erweitert werden sollte.
                   AxisPosEND = 11 };

public slots:
    static AxisPos basicAxisPos( uint pos );
    void setAxisVisible( bool axisVisible );
    bool axisVisible() const;

public:
    enum AxisAreaMode { AxisAreaModeFixedSize,
                        AxisAreaModeAutoSize,
                        AxisAreaModeMinMaxSize };

public slots:
    void setAxisAreaMode( AxisAreaMode axisAreaMode );
    AxisAreaMode axisAreaMode() const;
    static QString axisAreaModeToString( AxisAreaMode mode );
    static AxisAreaMode stringToAxisAreaMode( const QString& type );
    void setAxisAreaMin( int axisAreaMin );

    //Returns the axis area minimum width (or height, resp.).
    int axisAreaMin() const { return _axisAreaMin; }
    void setAxisUseAvailableSpace( int axisUseAvailableSpaceFrom,
                                   int axisUseAvailableSpaceTo );
    int axisUseAvailableSpaceFrom() const { return _axisUseAvailableSpaceFrom; }
    int axisUseAvailableSpaceTo() const { return _axisUseAvailableSpaceTo; }
    void setAxisAreaMax( int axisAreaMax );
    int axisAreaMax() const { return _axisAreaMax; }
    void setAxisArea( AxisAreaMode axisAreaMode,
                      int axisAreaMin,
                      int axisAreaMax );

public:
    enum AxisCalcMode { AxisCalcLinear, AxisCalcLogarithmic };

public slots:
    void setAxisCalcMode( AxisCalcMode axisCalcMode );
    AxisCalcMode axisCalcMode() const { return _axisCalcMode; }
    static QString axisCalcModeToString( AxisCalcMode mode );
    static AxisCalcMode stringToAxisCalcMode( const QString& type );
    void setIsometricReferenceAxis( uint isoRefAxis );
    uint isometricReferenceAxis() const { return _axisIsoRefAxis; }
    void setAxisTrueAreaSize( int axisTrueAreaSize ) { _axisTrueAreaSize = axisTrueAreaSize; }
    int axisTrueAreaSize() const { return _axisTrueAreaSize; }
    void setAxisTrueAreaRect( const QRect& axisTrueAreaRect ) { _axisTrueAreaRect = axisTrueAreaRect; }
    QRect axisTrueAreaRect() const { return _axisTrueAreaRect; }
    void setAxisLineVisible( bool axisLineVisible );
    bool axisLineVisible() const { return _axisLineVisible; }
    void setAxisLineWidth( int axisLineWidth );
    int axisLineWidth() const { return _axisLineWidth; }
    void setAxisTrueLineWidth( int axisTrueLineWidth ) { _axisTrueLineWidth = axisTrueLineWidth; }
    int axisTrueLineWidth() const { return _axisTrueLineWidth; }
    void setAxisLineColor( QColor axisLineColor );
    QColor axisLineColor() const { return _axisLineColor; }
    // main grid:
    void setAxisShowFractionalValuesDelimiters( bool axisShowFractValDelim );
    bool axisShowFractionalValuesDelimiters() const { return _axisShowFractionalValuesDelimiters; }
    void setAxisShowGrid( bool axisShowGrid );
    bool axisShowGrid() const { return _axisShowGrid; }
    void setAxisGridStyle( PenStyle axisGridStyle );
    PenStyle axisGridStyle() const { return _axisGridStyle; }
    void setAxisGridColor( QColor axisGridColor );
    QColor axisGridColor() const { return _axisGridColor; }
    void setAxisGridLineWidth( int axisGridLineWidth );
    int axisGridLineWidth() const { return _axisGridLineWidth; }
    // sub grid:
    void setAxisShowSubDelimiters( bool axisShowSubDelimiters );
    bool axisShowSubDelimiters() const { return _axisShowSubDelimiters; }
    void setAxisGridSubStyle( PenStyle axisGridSubStyle );
    PenStyle axisGridSubStyle() const { return _axisGridSubStyle; }
    void setAxisGridSubColor( QColor axisGridSubColor );
    QColor axisGridSubColor() const { return _axisGridSubColor; }
    void setAxisGridSubLineWidth( int axisGridSubLineWidth );
    int axisGridSubLineWidth() const { return _axisGridSubLineWidth; }

    void setAxisZeroLineColor( QColor axisZeroLineColor );
    QColor axisZeroLineColor() const { return _axisZeroLineColor; }

    void setAxisLabelsVisible( bool axisLabelsVisible );
    bool axisLabelsVisible() const { return _axisLabelsVisible; }
    void setAxisLabelsTouchEdges( bool axisLabelsTouchEdges );
    bool axisLabelsTouchEdges() const { return _axisLabelsTouchEdges; }
    void setAxisLabelsFont( QFont axisLabelsFont, bool useFontSize );
    QFont axisLabelsFont() const { return _axisLabelsFont; }
    void setAxisLabelsFontUseRelSize( bool axisLabelsFontUseRelSize );
    bool axisLabelsFontUseRelSize() const { return _axisLabelsFontUseRelSize; }
    void setAxisLabelsFontRelSize( int axisLabelsFontRelSize );
    int axisLabelsFontRelSize() const { return _axisLabelsFontRelSize;  }
    void setAxisLabelsFontMinSize( int axisLabelsFontMinSize ) { _axisLabelsFontMinSize = axisLabelsFontMinSize; }
    int axisLabelsFontMinSize() const { return _axisLabelsFontMinSize;  }
    void setAxisLabelsDontShrinkFont( bool labelsDontShrinkFont ) { _axisLabelsDontShrinkFont = labelsDontShrinkFont; }
    bool axisLabelsDontShrinkFont() const { return _axisLabelsDontShrinkFont; }
    void setAxisLabelsDontAutoRotate( bool labelsDontAutoRotate ) { _axisLabelsDontAutoRotate = labelsDontAutoRotate; }
    bool axisLabelsDontAutoRotate() const { return _axisLabelsDontAutoRotate; }
    void setAxisLabelsRotation( int rotation ) { _axisLabelsRotation = rotation; }
    int axisLabelsRotation() const { return _axisLabelsRotation; }
    void setAxisLabelsColor( QColor axisLabelsColor );
    QColor axisLabelsColor() const { return _axisLabelsColor; }

    void setAxisLabelsCalc( int divPow10 = 0,
                            int digitsBehindComma = KDCHART_AXIS_LABELS_AUTO_DIGITS );
    int axisLabelsDivPow10()          const { return _axisLabelsDivPow10; }
    int axisLabelsDigitsBehindComma() const { return _axisDigitsBehindComma; }
    void setAxisLabelsRadix( const QString& decimalPoint,
                             const QString& thousandsPoint );
    QString axisLabelsDecimalPoint()   const { return _axisLabelsDecimalPoint; }
    QString axisLabelsThousandsPoint() const { return _axisLabelsThousandsPoint; }
    void setAxisLabelsFormat( const QString& prefix = "",
                              const QString& postfix = "",
                              const int&     totalLen = 0,
                              const QChar&   padFill = ' ',
                              const bool&    blockAlign = true );
    QString axisLabelsPrefix()         const { return _axisLabelsPrefix; }
    QString axisLabelsPostfix()        const { return _axisLabelsPostfix; }
    int axisLabelsTotalLen()           const { return _axisLabelsTotalLen; }
    QChar axisLabelsPadFill()          const { return _axisLabelsPadFill; }
    bool axisLabelsBlockAlign()        const { return _axisLabelsBlockAlign; }

public:
    enum LabelsFromDataRow { LabelsFromDataRowYes,
                             LabelsFromDataRowNo,
                             LabelsFromDataRowGuess };

public slots:
    static QString labelsFromDataRowToString( LabelsFromDataRow mode );
    static LabelsFromDataRow stringToLabelsFromDataRow( const QString& type );

public:
    enum ValueScale { ValueScaleNumerical =   0, // have gaps here to allow specifying of additional scaling steps in between
                      ValueScaleSecond    =  20,
                      ValueScaleMinute    =  30,
                      ValueScaleHour      =  40,
                      ValueScaleDay       =  50,
                      ValueScaleWeek      =  60,
                      ValueScaleMonth     =  70,
                      ValueScaleQuarter   =  80,
                      ValueScaleYear      =  90 };

public slots:
    void setAxisValues( bool axisSteadyValueCalc = true,
                        const QVariant& axisValueStart = KDCHART_AXIS_LABELS_AUTO_LIMIT,
                        const QVariant& axisValueEnd = KDCHART_AXIS_LABELS_AUTO_LIMIT,
                        double axisValueDelta = KDCHART_AXIS_LABELS_AUTO_DELTA,
                        int axisDigitsBehindComma = KDCHART_AXIS_LABELS_AUTO_DIGITS,
                        int axisMaxEmptyInnerSpan = KDCHART_DONT_CHANGE_EMPTY_INNER_SPAN_NOW,
                        LabelsFromDataRow takeLabelsFromDataRow = LabelsFromDataRowNo,
                        int labelTextsDataRow = 0,
                        QStringList* axisLabelStringList = 0,
                        QStringList* axisShortLabelsStringList = 0,
                        int axisValueLeaveOut = KDCHART_AXIS_LABELS_AUTO_LEAVEOUT,
                        ValueScale axisValueDeltaScale = ValueScaleNumerical );


    void setAxisValueStartIsExact( bool isExactValue=true ) { _axisValueStartIsExact = isExactValue; }
    bool axisValueStartIsExact() const { return _axisValueStartIsExact; }
    void setAxisValueStart( const QVariant& axisValueStart ) { _axisValueStart = axisValueStart; }
    QVariant axisValueStart() const { return _axisValueStart; }
  void setAxisValueEnd( const QVariant& axisValueEnd ) {_axisValueEnd = axisValueEnd; }
    QVariant axisValueEnd() const { return _axisValueEnd; }
    void setAxisValueDelta( const double axisValueDelta,
                            ValueScale scale = ValueScaleNumerical );
    double axisValueDelta() const { return _axisValueDelta; }
    ValueScale axisValueDeltaScale() const { return _axisValueDeltaScale; }
    void setAxisValueLeaveOut( const int leaveOut ) { _axisValueLeaveOut = leaveOut; }
    int axisValueLeaveOut() const { return _axisValueLeaveOut; }
    void setAxisValuesDecreasing( bool valuesDecreasing = true )
    {
        _axisValuesDecreasing = valuesDecreasing;
    }
    bool axisValuesDecreasing() const { return _axisValuesDecreasing; }
    void setTrueAxisDeltaPixels( double nDeltaPixels ) { _trueAxisDeltaPixels = nDeltaPixels; }
    double trueAxisDeltaPixels() const { return _trueAxisDeltaPixels; }
    void setTrueAxisLowHighDelta( double nLow, double nHigh, double nDelta );
    double trueAxisLow() const { return _trueLow; }
    double trueAxisHigh() const { return _trueHigh; }
    double trueAxisDelta() const { return _trueDelta; }
    void setTrueAxisDtLowHighDeltaScale( QDateTime dtLow, QDateTime dtHigh,
                                         ValueScale dtDeltaScale );
    void setTrueAxisDtLow( QDateTime dtLow ) {  _trueDtLow        = dtLow; }
    void setTrueAxisDtHigh( QDateTime dtHigh ) { _trueDtHigh       = dtHigh; }
    void setTrueAxisDtScale( ValueScale scale ) { _trueDtDeltaScale = scale; }
    QDateTime trueAxisDtLow() const { return _trueDtLow; }
    QDateTime trueAxisDtHigh() const { return _trueDtHigh; }
    ValueScale trueAxisDtDeltaScale() const { return _trueDtDeltaScale; }
    void setAxisZeroLineStart( double x, double y );
    double axisZeroLineStartX() const { return _axisZeroLineStartX; }
    double axisZeroLineStartY() const { return _axisZeroLineStartY; }
    void setAxisDtLowPos( double x, double y );
    double axisDtLowPosX() const { return _axisDtLowPosX; }
    double axisDtLowPosY() const { return _axisDtLowPosY; }
    void setAxisDtHighPos( double x, double y );
    double axisDtHighPosX() const { return _axisDtHighPosX; }
    double axisDtHighPosY() const { return _axisDtHighPosY; }
    void setAxisDigitsBehindComma( const int digits ) { _axisDigitsBehindComma = digits; }
    int axisDigitsBehindComma() const { return _axisDigitsBehindComma; }
    void setAxisLabelsDateTimeFormat( const QString& format ) { _axisLabelsDateTimeFormat = format; }
    QString axisLabelsDateTimeFormat() const { return _axisLabelsDateTimeFormat; }
    void setAxisMaxEmptyInnerSpan( const int maxEmpty ) { _axisMaxEmptyInnerSpan = maxEmpty; }
    int axisMaxEmptyInnerSpan() const { return _axisMaxEmptyInnerSpan; }
    void setLabelTextsFormDataRow( int row, LabelsFromDataRow mode );
    LabelsFromDataRow axisLabelTextsFormDataRow() const { return _takeLabelsFromDataRow; }
    int labelTextsDataRow() const { return _labelTextsDataRow; }
    void setAxisLabelStringLists( QStringList*   axisLabelStringList,
                                  QStringList*   axisShortLabelStringList,
                                  const QString& valueStart = QString::null,
                                  const QString& valueEnd   = QString::null );
    QStringList axisLabelStringList() const { return _axisLabelStringList; }
    uint axisLabelStringCount() const { return _axisLabelStringList.count(); }
    QStringList axisShortLabelsStringList() const { return _axisShortLabelsStringList; }
    uint axisShortLabelsStringCount() const { return _axisShortLabelsStringList.count(); }
    const QStringList* axisLabelTexts() const { return _axisLabelTextsDirty ? 0 : &_axisLabelTexts; }
    void setAxisLabelTexts( const QStringList* axisLabelTexts );
    void setAxisLabelTextsDirty( bool axisLabelTextsDirty ) { _axisLabelTextsDirty = axisLabelTextsDirty; }
    bool axisLabelTextsDirty() const { return _axisLabelTextsDirty; }
    void setAxisFirstLabelText( const QString& axisFirstLabelText = QString() );
    void setAxisLastLabelText(  const QString& axisLastLabelText  = QString() );
    QString axisFirstLabelText() const { return _axisFirstLabelText; }
    QString axisLastLabelText()  const { return _axisLastLabelText; }
    bool hasAxisFirstLabelText() const { return !_axisFirstLabelText.isNull(); }
    bool hasAxisLastLabelText()  const { return !_axisLastLabelText.isNull(); }
    void setAxisSteadyValueCalc( bool axisSteadyValueCalc ) { _axisSteadyValueCalc = axisSteadyValueCalc; }
    bool axisSteadyValueCalc() const { return _axisSteadyValueCalc; }

public:
    KDChartAxisParams();
    virtual ~KDChartAxisParams();
    KDChartAxisParams& operator=( const KDChartAxisParams& R );
    static void deepCopy( KDChartAxisParams& D, const KDChartAxisParams& R );
    friend QTextStream& operator<<( QTextStream& s, const KDChartParams& p );
    friend QTextStream& operator>>( QTextStream& s, KDChartParams& p );
    friend class KDChartParams;

signals:

    // emitted when any of the chart axis parameters have changed.
    void changed();

private:

    AxisType _axisType;
    bool _axisVisible;
    bool _axisLabelsTouchEdges;
    AxisAreaMode _axisAreaMode;
    int _axisUseAvailableSpaceFrom;
    int _axisUseAvailableSpaceTo;
    int _axisAreaMin;
    int _axisAreaMax;
    AxisCalcMode _axisCalcMode;
    uint _axisIsoRefAxis;
    int _axisTrueAreaSize;
    QRect _axisTrueAreaRect;
    bool _axisLineVisible;
    int _axisLineWidth;
    int _axisTrueLineWidth;
    QColor _axisLineColor;

    bool     _axisShowFractionalValuesDelimiters;
    bool     _axisShowGrid;
    QColor   _axisGridColor;
    int      _axisGridLineWidth;
    PenStyle _axisGridStyle;

    bool     _axisShowSubDelimiters;
    QColor   _axisGridSubColor;
    int      _axisGridSubLineWidth;
    PenStyle _axisGridSubStyle;

    QColor _axisZeroLineColor;
    bool _axisLabelsVisible;
    QFont _axisLabelsFont;
    bool _axisLabelsFontUseRelSize;
    int _axisLabelsFontRelSize;
    int _axisLabelsFontMinSize;
    bool _axisLabelsDontShrinkFont;
    bool _axisLabelsDontAutoRotate;
    int _axisLabelsRotation;
    QColor _axisLabelsColor;

    bool     _axisSteadyValueCalc;
    bool     _axisValueStartIsExact;
    QVariant _axisValueStart;
    QVariant _axisValueEnd;
    double   _axisValueDelta;
    ValueScale _axisValueDeltaScale;
    int _axisValueLeaveOut;
    bool _axisValuesDecreasing;
    double _trueAxisDeltaPixels;
    double _trueLow;
    double _trueHigh;
    double _trueDelta;
    QDateTime _trueDtLow;
    QDateTime _trueDtHigh;
    ValueScale _trueDtDeltaScale;
    double _axisZeroLineStartX;
    double _axisZeroLineStartY;
    double _axisDtLowPosX;
    double _axisDtLowPosY;
    double _axisDtHighPosX;
    double _axisDtHighPosY;
    int _axisDigitsBehindComma;
    QString _axisLabelsDateTimeFormat;
    int _axisMaxEmptyInnerSpan;
    LabelsFromDataRow _takeLabelsFromDataRow;
    int _labelTextsDataRow;
    QStringList _axisLabelStringList;
    QStringList _axisShortLabelsStringList;
    QStringList _axisLabelTexts;
    bool _axisLabelTextsDirty;
    QString _axisFirstLabelText;
    QString _axisLastLabelText;

    int _axisLabelsDivPow10;
    QString _axisLabelsDecimalPoint;
    QString _axisLabelsThousandsPoint;
    QString _axisLabelsPrefix;
    QString _axisLabelsPostfix;
    int     _axisLabelsTotalLen;
    QChar   _axisLabelsPadFill;
    bool    _axisLabelsBlockAlign;
};

#endif
