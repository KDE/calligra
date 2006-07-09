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
#include <KDChartAxisParams.h>
#ifndef KDCHART_MASTER_CVS
#include "KDChartAxisParams.moc"
#endif


/**
  \class KDChartAxisParams KDChartAxisParams.h
  \brief access the chart axis parameters.

  To be used in combination with the axis access methods in
  class KDChartParams.


  \see KDChartParams.h
  \see KDChartParams.cpp

  Bundles all parameters of one axis including the type except the
  actual data displayed. Serializing an object of this type plus the
  data displayed is enough to be able to recreate the chart later.

*/





/**
  \c  enum KDChartAxisParams::Axistype { AxisTypeUnknown,
  AxisTypeEAST,
  AxisTypeNORTH,
  AxisUP };

  The axis type - the kind of the axis (x-, y- or z-axis).

  \li \c AxisTypeEAST axis of abscisses, the so-called \em X-axis.
  In world coordinates this is the EAST axis as used
  on topographical maps.
  This axis normally is assigned to the 1st dimension
  of the represented data, to change this use
  KDChartAxesPainter::setAxisAssignment().

  \li \c AxisTypeNORTH axis of ordinates, the so-called \em Y-axis.
  when are represented by the chart.
  In world coordinates this is the NORTH axis as used
  on topographical maps for 2-dimensional data,
  or the NORTH axis as used on bird's-eys views of
  buildings etc for 3-dimensional data, resp..
  This axis normally is assigned to the 2nd dimension
  of the represented data.

  \li \c AxisTypeUP axis of the 3rd dimension, the so-called \em Z-axis.
  In world coordinates this is the UP axis as used
  on bird's-eys views of buildings etc.
  This axis normally is assigned to the 3rd dimension
  of the represented data.

  The following picture shows the default positions of the
  different axes:
  \verbatim

  +-----------+
  /|           |
  n  +------------+                / |           |
  .  |            |               /  |           |
  NORTH, y .  |   2-dim.   |              /   |           |
  .  |            |             /    +-----------+
  .  |   chart    |            /    /           /  n
  .  |            |        n  +    /  3-dim.   /  .
  .  |            |        .  |   /           /  .  NORTH, y
  .  |            |  UP, z .  |  /  chart    /  .
  .  |            |        .  | /           /  .
  .  |            |        .  |/           /  .
  0  +------------+        0  +-----------+  .
  0
  0............n           0...........n
  EAST, x                  EAST, x
  \endverbatim

  \note Normally you will not have to specify the AxisType since it
  will be set automatically in KDChartAxesPainter::setDefaultAxesTypes()
  called by c'tor of class KDChartAxesPainter

  \sa setAxisType, axisType, KDChartAxesPainter::setAxisAssignment()
  */


/**
  Constructor. Define default values:

  - do not show this axis at all,
  - use circa 100 per thousand of the printable area
    size for drawing this axis,
  - calc mode linear,
  - line is visible,
  - line width: 3 per mille of the printable area size
  - line color: black,
  - labels are visible,
  - labels font:      helvetica
  - labels font size: 20 / 1000 of the printable area size
  - labels color:     black,
  - this axis will be used for an ordinate
  - label value limits will be calculated automatically,
  */
KDChartAxisParams::KDChartAxisParams()
{
    _axisType = AxisTypeUnknown;
    _axisVisible = false;
    _axisAreaMode = AxisAreaModeAutoSize;
    _axisUseAvailableSpaceFrom = 0;
    _axisUseAvailableSpaceTo = -1000;
    _axisAreaMin = -100;
    _axisAreaMax = 0;
    _axisCalcMode = AxisCalcLinear;
    _axisIsoRefAxis = UINT_MAX; // == KDCHART_NO_AXIS, see KDChartParams.cpp
    _axisTrueAreaSize = 0;
    _axisZeroLineStartX = 0.0;
    _axisZeroLineStartY = 0.0;
    _axisDtLowPosX = 0.0;
    _axisDtLowPosY = 0.0;
    _axisDtHighPosX = 0.0;
    _axisDtHighPosY = 0.0;

    _axisLineWidth = -3; // == 3/1000
    _axisTrueLineWidth = 1;
    _axisLineVisible = true;
    _axisLineColor = QColor( 0, 0, 0 );

    _axisShowFractionalValuesDelimiters = true;
    _axisShowGrid      = false;
    _axisGridColor     = KDCHART_DEFAULT_AXIS_GRID_COLOR;
    _axisGridLineWidth = KDCHART_AXIS_GRID_AUTO_LINEWIDTH;
    _axisGridStyle     = Qt::SolidLine;

    _axisShowSubDelimiters = true;
    _axisGridSubColor      = KDCHART_DEFAULT_AXIS_GRID_COLOR;
    _axisGridSubLineWidth  = KDCHART_AXIS_GRID_AUTO_LINEWIDTH;
    _axisGridSubStyle      = Qt::DotLine;

    _axisZeroLineColor = QColor( 0x00, 0x00, 0x80 );

    _axisLabelsVisible = true;
    _axisLabelsFont = QFont( "helvetica", 12,
                             QFont::Normal, false );
    _axisLabelsFontUseRelSize = true;
    _axisLabelsFontRelSize = 17;
    _axisLabelsFontMinSize = 10;
    _axisLabelsDontShrinkFont = false;
    _axisLabelsDontAutoRotate = false;
    _axisLabelsRotation = 0;
    _axisLabelsColor = QColor( 0, 0, 0 );

    _axisSteadyValueCalc = true;
    _axisValueStartIsExact = true;
    _axisValueStart        = KDCHART_AXIS_LABELS_AUTO_LIMIT;
    _axisValueEnd          = KDCHART_AXIS_LABELS_AUTO_LIMIT;
    _axisValueDelta = KDCHART_AXIS_LABELS_AUTO_DELTA;
    _axisValueDeltaScale = ValueScaleNumerical;
    _axisValueLeaveOut = KDCHART_AXIS_LABELS_AUTO_LEAVEOUT;
    _axisValuesDecreasing = false;

    // First we used "h:mm:ss\nd.MM.yyyy" but now we calculate the
    // format automatically - based on the time span to be displayed.
    _axisLabelsDateTimeFormat = KDCHART_AXIS_LABELS_AUTO_DATETIME_FORMAT;

    _axisMaxEmptyInnerSpan = 90;
    _takeLabelsFromDataRow = LabelsFromDataRowNo;
    _labelTextsDataRow = 0;
    _axisLabelStringList.clear();
    _axisShortLabelsStringList.clear();

    setAxisLabelTexts( 0 );
    setAxisFirstLabelText();
    setAxisLastLabelText();
    setTrueAxisDeltaPixels( 0.0 );
    setTrueAxisLowHighDelta( 0.0, 0.0, 0.0 );
    setTrueAxisDtLowHighDeltaScale( QDateTime(), QDateTime(), ValueScaleDay );

    _axisLabelsDivPow10 = 0;
    _axisDigitsBehindComma = KDCHART_AXIS_LABELS_AUTO_DIGITS;
    _axisLabelsDecimalPoint = ".";
    _axisLabelsThousandsPoint = ",";
    _axisLabelsPrefix = "";
    _axisLabelsPostfix = "";
    _axisLabelsTotalLen = 0;
    _axisLabelsPadFill = ' ';
    _axisLabelsBlockAlign = true;
}

/**
  Destructor. Only defined to have it virtual.

*/

KDChartAxisParams::~KDChartAxisParams()
{
    // Intentionally left blank for now.
}


/**
  Specifies the axis type. The default is unknown (AxisTypeUnknown).

  \note Normally you will not have to specify the AxisType since it
  will be set automatically in KDChartAxesPainter::setDefaultAxesTypes()
  called by c'tor of class KDChartAxesPainter

  \param axisType the axis type to use
  \sa axisType, AxisType
  */
void KDChartAxisParams::setAxisType( AxisType axisType )
{
    _axisType = axisType;
    emit changed();
}

/**
  \fn AxisType  KDChartAxisParams::axisType() const

  \return the axis type configured in this params object.

  \sa setAxisType, AxisType
  */


/**

  \c enum KDChartAxisParams::AxisPos {  AxisPosSTART = 0,

  AxisPosBottom         = 0,
  AxisPosSagittal            = 0,  //   <-- for POLAR charts
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

Since the chart area is defined by subtracting the space consumed by
the axes from the printable area there are two options that are
mutually exclusive.

- Define the chart by precisely specifying (in absolute or in
relative numbers) how much space may be used by each axis.
and/or define start and end values for the axes' labels.

-> This produces results that are comparable to each other
since the chart will have an exactly fixed size and its position
on the paper will remain the same even if the labels of its axes
become wider or more narrow.
Also the graphical representation of the date will be comparable
since all your charts will use the same scale if you wish this.

- Let the program calculate the chart layout based upon the kind
of and width of the labels to be drawn at the axes.
Also the program can calculate the labels for you and find the
boundary values for start and end of the scale automatically.
-> This will produce good looking results without you having
to fiddle around with the settings.
Unfortunately these charts will not allways be comparable to
each other if the axes' labels change since both the size of
the area reserved for the chart and the scaling of the
scales will vary if the data values do so.

With KDChart both ways are possible, see hints given with
setAxisAreaMode(), setAxisAreaMin() and setAxisAreaMax().

To optimize your results you might want to use setAxisLineWidth() and
setAxisLabelsFontPointSize(): negative values will specify it
in per thousand  of the printable area size - producing a result
looking fine even if the chart is
printed at very large dimensions.

\sa setAxisAreaMode, setAxisAreaMin, setAxisAreaMax
\sa setAxisLineWidth(), setAxisLabelsFontPointSize()
\sa setAxisLabelsLimitsAuto(), setAxisLabelsLimitsLow()
\sa setAxisLabelsLimitsHeigh()
*/

/**
  The axis position.
  Use AxisPosBottom for X-axis and AxisPosLeft for traditional Y-axis.
  When showing 3-dimensional charts you may use AxisPosLowerRightEdge
  to have an Y-axis on the lower right side going into the 'depth' of
  the picture and a z-axis going 'upwards' at the left edge.
  The other, optional values (AxisPosRight, AxisPosTop and
  AxisPosLowerLeftEdge, resp.) could be used to show a second
  axis on the other side of the picture - useful in case you want
  to show two data sets in one chart, like two curves showing data sets
  sharing the same abscissa axis but having differently scaled ordinates.
  The additional values (AxisPosBottom2, AxisPosTop2 ..) may be used
  to specify composed charts having two ordinates at the same side of
  the drawing and the like...

  These AxisPos values are used to specify which axis' settings are
  to be modified or retrieved, resp.

  \note Use special values \c AxisPossagittal and \c AxisPosCircular
  to set delimiter/label/grid parameters for polar charts.

  \sa setAxisAreaMode, axisAreaMode, setAxisCalcMode, axisCalcMode
  \sa KDChartParams::setAxisParams, KDChartParams::axisParams
  */


/**
  The basic axis type.

  \param pos the axis type to be analyzed
  \return the basic axis type (Bottom, Left, Top or Right)
  */
KDChartAxisParams::AxisPos KDChartAxisParams::basicAxisPos( uint pos )
{
    AxisPos ret = AxisPos( pos );
    switch ( pos ) {
        case AxisPosBottom:
        case AxisPosLeft:
        case AxisPosTop:
        case AxisPosRight:
            break;
        case AxisPosLowerRightEdge:
            ret = AxisPosRight;
            break;
        case AxisPosLowerLeftEdge:
            ret = AxisPosLeft;
            break;
        case AxisPosBottom2:
            ret = AxisPosBottom;
            break;
        case AxisPosLeft2:
            ret = AxisPosLeft;
            break;
        case AxisPosTop2:
            ret = AxisPosTop;
            break;
        case AxisPosRight2:
            ret = AxisPosRight;
            break;
        case AxisPosLowerRightEdge2:
            ret = AxisPosRight;
            break;
        case AxisPosLowerLeftEdge2:
            ret = AxisPosLeft;
            break;
        default: {
                     qDebug( "IMPLEMENTATION ERROR: type missing in KDChartAxisParams::basicAxisPos()" );
                     Q_ASSERT( ret != AxisPos( pos ) );
                 }
    }
    return ret;
}

/**
  Specifies if the axis will be drawn. The default is false.

  \param axisVisible set true to make visible the respective axis.
  \sa axisVisible, AxisVisible
  */
void KDChartAxisParams::setAxisVisible( bool axisVisible )
{
    _axisVisible = axisVisible;
    emit changed();
}

/**
  Returns weither the axis will be drawn or not.

  \return if the axis is visible or not.
  \sa setAxisVisible, AxisVisible
  */
bool  KDChartAxisParams::axisVisible() const
{
    return _axisVisible;
}


/**
  \c  enum AxisAreaMode { AxisAreaModeFixedSize,
  AxisAreaModeAutoSize,
  AxisAreaModeMinMaxSize };

  The axis size, determines how to calculate the size of area used
  by the axis - i.e. the width of left axis area (or the right
  one, resp.) or the height of the top axis area (or the bottom one,
  resp.).
  \li \c AxisAreaModeFixedSize (default) value will be taken
  from \c AxisAreaMinSize() or \c AxisAreaMaxSize() - whichever
  returns the smaller value
  \li \c AxisAreaModeAutoSize (default) will be calculated
  automatically based on the size of the labels to be drawn
  \li \c AxisAreaModeMinMaxSize will be calculated automatically but
  bearing user defined limits in mind (this is not implemented yet)

  \sa setAxisAreaMode, axisAreaMode, AxisAreaMode
  \sa setAxisAreaMin, setAxisAreaMax, setAxisArea
  */

/**
  Specifies the axis size mode.
  The default is auto size (AxisAreaModeAutoSize).

  \param axisAreaMode the way how to find out the
  size of the area where the axis shall be drawn.
  \sa axisAreaMode, AxisAreaMode
  \sa setAxisAreaMin, setAxisAreaMax, setAxisArea
  */
void  KDChartAxisParams::setAxisAreaMode( AxisAreaMode axisAreaMode )
{
    _axisAreaMode = axisAreaMode;
    emit changed();
}



/**
  Returns the axis size mode configured in this params object.

  \return the axis size mode configured in this params object.
  \sa setAxisAreaMode, AxisAreaMode, setAxisAreaMin, setAxisAreaMax
  \sa setAxisArea
  */
KDChartAxisParams::AxisAreaMode  KDChartAxisParams::axisAreaMode() const
{
    return _axisAreaMode;
}

/**
  Converts the specified axis area mode enum to a string representation.

  \param mode the axis area mode enum to convert
  \return the string representation of the mode enum
  */
QString KDChartAxisParams::axisAreaModeToString( AxisAreaMode mode ) {
    switch( mode ) {
        case AxisAreaModeFixedSize:
            return "FixedSize";
        case AxisAreaModeAutoSize:
            return "AutoSize";
        case AxisAreaModeMinMaxSize:
            return "MinMaxSize";
        default: // should not happen
            qDebug( "Unknown axis area mode" );
            return "AxisAreaMode";
    }
}

/**
  Converts the specified string to an axis area mode enum value.

  \param string the string to convert
  \return the axis area mode enum value
  */
KDChartAxisParams::AxisAreaMode KDChartAxisParams::stringToAxisAreaMode( const QString& type ) {
    if( type == "FixedSize" )
        return AxisAreaModeFixedSize;
    else if( type == "AutoSize" )
        return AxisAreaModeAutoSize;
    else if( type == "MinMaxSize" )
        return AxisAreaModeMinMaxSize;
    else // should not happen
        return AxisAreaModeAutoSize;
}

/**
  Specifies the axis area minimum width (or height, resp.).

  \param axisAreaMin the axis area minimum width (or height, resp.)
  If value is negative, the absolute value is per thousand
  of the size of the printable area to
  be used. This will make the axis look the same even if scaled
  to very different size.

  Note: It AxisAreaModeFixedSize is set the smaller value of
  axisAreaMax and axisAreaMin is used for the area size.

\sa axisAreaMin, axisAreaMax, setAxisAreaMode, setAxisAreaMax
\sa setAxisArea
*/
void KDChartAxisParams::setAxisAreaMin( int axisAreaMin )
{
    _axisAreaMin = axisAreaMin;
    emit changed();
}

/**
  \fn int  KDChartAxisParams::axisAreaMin() const
  Returns the axis area minimum width (or height, resp.).

  \return the axis area minimum width (or height, resp.).
  \sa setAxisAreaMin, setAxisAreaMax, setAxisArea
  */



/**
  Specifies how the axis will make use of the available space.

  Use this function to specify how large the area of the axis
  will be and where it will be positioned.

  \note This function is to be used for (vertical) ordinate axes <b>only</b>,
  available space usage specified for abscissa axes will be ignored.

example: setAxisUseAvailableSpace( 0, -499 ) will make the
axis occupy the first half of the available space,
so there could be a second axis (for chart #2 having the same
x-axis as chart #1) using the remaining
part of the available space, this one would be specified
by setAxisUseAvailableSpace( -500, -1000 ).

See also the examples given with \c KDChartParams::setAdditionalChartType().

\param axisUseAvailableSpaceFrom the beginning offset
of the space to be covered by this axis.
Set this to 0 to have your axis start at the very beginning
of the avaliable space - this is the default setting.
Use negative values to specify an offset in per mil
of the available space, or use positive values to specify
it in pixels.

\param axisUseAvailableSpaceTo the ending offset
of the space to be covered by this axis.
Set this to -1000 (== 1000 per mille) to let the axis
end at the very end of the available space, this is the default setting.
to a smaller negative value to specify the percantage
Use negative values to specify an offset in per mil
of the available space, or use positive values to specify
it in pixels.

\sa axisUseAvailableSpaceFrom, axisUseAvailableSpaceTo
*/
void  KDChartAxisParams::setAxisUseAvailableSpace( int axisUseAvailableSpaceFrom,
        int axisUseAvailableSpaceTo )
{
    _axisUseAvailableSpaceFrom = axisUseAvailableSpaceFrom;
    _axisUseAvailableSpaceTo   = axisUseAvailableSpaceTo;
    emit changed();
}


/**
  \fn  int  KDChartAxisParams::axisUseAvailableSpaceFrom() const
  Returns the beginning offset of the space used by this
  axis in comparison to the available space that could
  be used by this axis.

  \sa setAxisUseAvailableSpace, axisUseAvailableSpaceTo
  */


/**
  \fn  int  KDChartAxisParams::axisUseAvailableSpaceTo() const
  Returns the ending offset of the space used by this
  axis in comparison to the available space that could
  be used by this axis.

  \sa setAxisUseAvailableSpace, axisUseAvailableSpaceFrom
  */

/**
  Specifies the axis area maximum width (or height, resp.).

  \param axisAreaMax the axis area maximum width (or height, resp.)
  If value is negative, the absolute value is per thousand
  of the size of the printable area to
  be used. This will make the axis look the same even if scaled
  to very different size.

  Note: If AxisAreaModeFixedSize is set the smaller value of
  axisAreaMax and axisAreaMin is used for the area size.

\sa axisAreaMax, axisAreaMin, setAxisAreaMode, setAxisAreaMin
\sa setAxisArea
*/
void  KDChartAxisParams::setAxisAreaMax( int axisAreaMax )
{
    _axisAreaMax = axisAreaMax;
    emit changed();
}
/**
  \fn int  KDChartAxisParams::axisAreaMax() const
  Returns the axis area maximum width (or height, resp.).

  \return the axis area maximum width (or height, resp.).
  \sa setAxisAreaMax, setAxisAreaMin, setAxisArea
  */

/**
  Specifies the axis area size mode and the
  minimum and maximum width (or height, resp.) of the area.
  This method is here for convenience, see \c setAxisAreaMode,
  \c setAreaMin and \c setAreaMax for details.

  Note: Is AxisAreaModeFixedSize is set the smaller value of
  axisAreaMax and axisAreaMin is used for the area size.

  \param axisAreaMode the way how to find out the
  size of the area where the axis shall be drawn.
  \param axisAreaMin the axis area minimum width (or height, resp.)
  \param axisAreaMax the axis area maximum width (or height, resp.)

  \sa setAxisAreaMode, setAxisAreaMin, setAxisAreaMax
  */
void  KDChartAxisParams::setAxisArea( AxisAreaMode axisAreaMode,
        int axisAreaMin,
        int axisAreaMax )
{
    _axisAreaMode = axisAreaMode;
    _axisAreaMin = axisAreaMin;
    _axisAreaMax = axisAreaMax;
    emit changed();
}

/**
  \c enum AxisCalcMode { AxisCalcLinear, AxisCalcLogarithmic };
  The axis calculation mode.

  \sa setAxisCalcMode, axisCalcMode, AxisCalcMode
  */

/**
  Specifies the axis calculation mode.
  The default is linear calculation (AxisCalcLinear).

  \note Specifying an AxisCalcLogarithmic calculation mode
  also results in the axis's label parameters being adjusted,
  in detail these settings will become valid:

  \li steady-value-calculation mode is activated
  \li automatic limit calculation will be set for the begin and the end of the axis
  \li logarithmical step width will be calculated automatically: 0.001, 0.01, 0.1, 1, 10, ..
  \li number of Digits shown behind the comma will be calculated automatically.

  This is done by implicitely calling setAxisValues(),
  so you don't need to explicitely call that function,
  actually the following is what happens inside:

  \verbatim
  if( AxisCalcLogarithmic == axisCalcMode )
  setAxisValues( true,
                 KDCHART_AXIS_LABELS_AUTO_LIMIT,
                 KDCHART_AXIS_LABELS_AUTO_LIMIT,
                 1.0,
                 KDCHART_AXIS_LABELS_AUTO_DIGITS );
  \endverbatim

  You may override these setting by making an extra setAxisValues()
  call AFTER calling setAxisCalcMode( AxisCalcLogarithmic ).

  \param axisCalcMode the axis calculation mode to be used.
  \sa axisCalcMode, AxisCalcMode
  \sa setAxisSteadyValueCalc
  */
void  KDChartAxisParams::setAxisCalcMode( AxisCalcMode axisCalcMode )
{
    _axisCalcMode = axisCalcMode;
    if( AxisCalcLogarithmic == axisCalcMode ){
        setAxisValues( true,
                KDCHART_AXIS_LABELS_AUTO_LIMIT,
                KDCHART_AXIS_LABELS_AUTO_LIMIT,
                1.0,
                KDCHART_AXIS_LABELS_AUTO_DIGITS );
    }
    emit changed();
}
/**
  \fn  AxisCalcMode  KDChartAxisParams::axisCalcMode() const
  Returns the axis calculation mode configured in this params object.

  \return the axis calculation mode configured in this params object.
  \sa setAxisCalcMode, AxisCalcMode
  */

/**
  Converts the specified axis calc mode enum to a string representation.

  \param mode the axis calc mode enum to convert
  \return the string representation of the mode enum
  */
QString  KDChartAxisParams::axisCalcModeToString( AxisCalcMode mode ) {
    switch( mode ) {
        case AxisCalcLinear:
            return "Linear";
        case AxisCalcLogarithmic:
            return "Logarithmic";
        default: // should not happen
            qDebug( "Unknown axis calc mode" );
            return "Linear";
    }
}

/**
  Converts the specified string to an axis calc mode enum value.

  \param string the string to convert
  \return the axis calc mode enum value
  */
KDChartAxisParams::AxisCalcMode KDChartAxisParams::stringToAxisCalcMode( const QString& type ) {
    if( type == "Linear" )
        return AxisCalcLinear;
    else if( type == "Logarithmic" )
        return AxisCalcLogarithmic;
    else // should not happen
        return AxisCalcLinear;
}

/**
  Specifies another axis which this axis shall be isometric with.

  \param isoRefAxis axis which this axis shall be isometric with.

  Normally all axes' step widths are calculated independently from
  each other. By specifying a reference axis for one axis you make
  KDChart use the same scale for both axes.

  For example to have the left axis using the same scale as the
  right axis you could invoke this:

  \verbatim
  KDChartAxisParams pa(
  _p->axisParams( KDChartAxisParams::AxisPosLeft ) );
  pa.setIsometricReferenceAxis( KDChartAxisParams::AxisPosBottom );
  _p->setAxisParams( KDChartAxisParams::AxisPosLeft, pa );
  \endverbatim

  These commands are equivalent to the following ones:

  \verbatim
  KDChartAxisParams pa(
  _p->axisParams( KDChartAxisParams::AxisPosBottom ) );
  pa.setIsometricReferenceAxis( KDChartAxisParams::AxisPosLeft );
  _p->setAxisParams( KDChartAxisParams::AxisPosBottom, pa );
  \endverbatim

  In any case both axes will use the same scale so - unless you are
  using more axes than these two ones - the resulting chart will be true
  to scale.

  \note Use special value KDCHART_ALL_AXES if your chart
  has got more than two axes and all of them shall use the same scale, specifying
  this for one of the axes is enough, there is no need to set it several times.

  Use special value KDCHART_NO_AXIS to undo any previous setting
  that was specified for this axis, this has to be called for any axis that was
  modified by previous calls.

  \sa isometricReferenceAxis
  */
void  KDChartAxisParams::setIsometricReferenceAxis( uint isoRefAxis )
{
    _axisIsoRefAxis = isoRefAxis;
    emit changed();
}

/**
  \fn uint  KDChartAxisParams::isometricReferenceAxis() const
  Returns which axis this axis shall be isometric with, this will
  be either the axis position - see KDChartAxisParams::AxisPos - or one of the special
  values KDCHART_ALL_AXES and KDCHART_NO_AXIS.

  \return which axis this axis shall be isometric with.

  \sa setIsometricReferenceAxis
  */


/**
  \fn void  KDChartAxisParams::setAxisTrueAreaSize( int axisTrueAreaSize )
  Specifies the true axis area width (or height, resp.).

  \param axisAreaMax the true axis area width (or height, resp.)
  as it was calculated and drawn.
  This is allways an absolute value.

  \note Do <b>not call</b> this function unless you are knowing
  exactly what you are doing. \c setAxisTrueAreaSize is normally
  reserved for internal usage by methods calculating the area
  size based upon \c axisAreaMin and \c axisAreaMax. Thus the
  signal \c changed() is not sended here.

  \sa axisAreaMax, axisAreaMin, setAxisAreaMode, setAxisAreaMin
  \sa setAxisArea
  */

/**
  \fn  int  KDChartAxisParams::axisTrueAreaSize() const
  Returns the true axis area width (or height, resp.)
  as calculated and drawn.

  \return the true axis area width (or height, resp.).
  \sa setAxisAreaMax, setAxisAreaMin, setAxisArea
  */

/**
  \fn void  KDChartAxisParams::setAxisTrueAreaRect( const QRect& axisTrueAreaRect )
  Specifies the true axis area rectangle.

  \param axisAreaMax the true axis area rectangle
  as it was calculated and drawn.

  \note Do <b>not call</b> this function unless you are knowing
  exactly what you are doing. \c setAxisTrueAreaRect is normally
  reserved for internal usage by methods calculating the area
  size based upon \c axisAreaMin and \c axisAreaMax. Thus the
  signal \c changed() is not sended here.

  \sa axisAreaMax, axisAreaMin, setAxisAreaMode, setAxisAreaMin
  \sa setAxisArea
  */

/**
  \fn  QRect  KDChartAxisParams::axisTrueAreaRect() const
  Returns the true axis area rectangle
  as calculated and drawn.

  \return the true axis area rectangle
  \sa setAxisAreaMax, setAxisAreaMin, setAxisArea
  */

/**
  Specifies whether the axis sub-delimiters should be drawn.

  \note If true and axisShowGrid is also true the grid on the
  chart data area will show a thin dotted line for each sub-delimiter
  (or a line with a pattern defined by \c setAxisGridSubStyle, resp.)

  \param axisShowSubDelimiters if true sub-delimiters will be drawn at this axis.
  \sa axisShowSubDelimiters, setAxisShowGrid, setAxisGridSubStyle
  */
void  KDChartAxisParams::setAxisShowSubDelimiters( bool axisShowSubDelimiters )
{
    _axisShowSubDelimiters = axisShowSubDelimiters;
    emit changed();
}

/**
  Specifies whether the delimiters should be drawn at the position for the fractional values.
  
  \note This can be useful if you want to force only painting non fractional values on the axis and 
  do not want the delimiters and grid lines to be drawn at the position where some fractional values 
  (auto calculation) were meant to be displayed. It will have no effect in case fractional values labels 
  are painted.
  In order to force painting only non fractional values you need to call setAxisBehindDigitsComma(0).
  \param axisShowFracValDelim if false delimiters and grid line will not be drawn on this axis at the positon 
  where fractional values were meant to be drawn.
  \sa setAxisDigitBehindComma
*/

void  KDChartAxisParams::setAxisShowFractionalValuesDelimiters( bool axisShowFracValDelim )
{
    _axisShowFractionalValuesDelimiters = axisShowFracValDelim;
    emit changed();
}


/**
  \fn bool  KDChartAxisParams::axisShowSubDelimiters() const
  Returns whether the axis sub-delimiters will be drawn.

  \return whether the axis sub-delimiters will be drawn.
  \sa setAxisShowSubDelimiters
  */

/**
  Specifies whether the axis line should be drawn.

  \param axisLineVisible if true the line of this axis will be drawn.
  \sa axisLineVisible
  */
void  KDChartAxisParams::setAxisLineVisible( bool axisLineVisible )
{
    _axisLineVisible = axisLineVisible;
    emit changed();
}


/**
  \fn bool  KDChartAxisParams::axisLineVisible() const
  Returns whether the axis line should be drawn.

  \return whether the axis line should be drawn.
  \sa setAxisLineVisible
  */

/**
  Specifies the axis line width.

  \param axisLineWidth the axis line width.
  If value is negative, the absolute value is per thousand
  of the printable area size to be used. This will make the
  axis look the same even if scaled to very different size.

  \sa axisLineWidth
  */
void  KDChartAxisParams::setAxisLineWidth( int axisLineWidth )
{
    _axisLineWidth = axisLineWidth;
    emit changed();
}


/**
  \fn  int  KDChartAxisParams::axisLineWidth() const
  Returns the axis line width.

  \return the axis line width.
  \sa setAxisLineWidth
  */


/**
  \fn void s KDChartAxisParams::setAxisTrueLineWidth( int axisTrueLineWidth )
  Specifies the actual axis line width, as calculated and drawn.

  \Note You may not use this internal function.

  \param axisTrueLineWidth the actual axis line width,
  as calculated and drawn.

  \sa axisTrueLineWidth
  */

/**
  \fn  int  KDChartAxisParams::axisTrueLineWidth() const
  Returns the axis true line width, as calculated and drawn.

  \return the axis true line width, as calculated and drawn.
  \sa setAxisTrueLineWidth
  */


/**
  Specifies the axis line colour.

  \param axisLineColor the axis line colour.
  \sa axisLineColor
  */
void  KDChartAxisParams::setAxisLineColor( QColor axisLineColor )
{
    _axisLineColor = axisLineColor;
    emit changed();
}

/**
  \fn QColor  KDChartAxisParams::axisLineColor() const
  Returns the axis line colour.

  \return the axis line colour.
  \sa setAxisLineColor
  */


/**
  Specifies whether a grid should be drawn at the chart data area.
  By default the grid will be drawn based on the left
  ordinate axis and on the bottom abscissa axis.

  The grid will show a solid line for each delimiter.
  (or a line with a pattern defined by \c setAxisGridStyle, resp.)

  \note If true and axisShowSubDelimiters is also true the grid
  will show a thin dotted line for each sub-delimiter.
  (or a line with a pattern defined by \c setAxisGridSubStyle, resp.)

  \param axisShowGrid if true a grid will be drawn on the chart data area.
  \sa axisShowGrid, setAxisGridStyle, setAxisGridSubStyle
  */
void  KDChartAxisParams::setAxisShowGrid( bool axisShowGrid )
{
    _axisShowGrid = axisShowGrid;
    emit changed();
}

/**
  \fn bool  KDChartAxisParams::axisShowGrid() const
  Returns whether a grid should be drawn at the chart data area.

  \return whether a grid should be drawn at the chart data area.
  \sa setAxisShowGrid, setAxisShowSubDelimiters
  */



/**
  Specifies the axis grid colour.

  To reset the color to the built-in default value
  please call \c setAxisGridColor( KDCHART_DEFAULT_AXIS_GRID_COLOR )

  \param axisGridColor the axis grid color.
  \sa axisGridColor, setAxisShowGrid
  */
void  KDChartAxisParams::setAxisGridColor( QColor axisGridColor )
{
    _axisGridColor = axisGridColor;
    emit changed();
}

/**
  \fn QColor  KDChartAxisParams::axisGridColor() const
  Returns the axis grid color.

  \return the axis grid color.
  \sa setAxisGridColor, setAxisShowGrid
  */


/**
  Specifies the colour to be used for the thin lines between the
  normal axis grid lines.

  To reset the color to the built-in default value
  please call \c setAxisGridSubColor( KDCHART_DEFAULT_AXIS_GRID_COLOR )

  \param axisGridSubColor the axis grid sub color.
  \sa axisGridSubColor, setAxisGridColor, setAxisShowGrid, setAxisShowSubDelimiters
  */
void  KDChartAxisParams::setAxisGridSubColor( QColor axisGridSubColor )
{
    _axisGridSubColor = axisGridSubColor;
    emit changed();
}


/**
  \fn QColor  KDChartAxisParams::axisGridSubColor() const
  Returns the axis grid sub color.

  \return the axis grid sub color.
  \sa setAxisGridSubColor
  */

/**
  Specifies the width of the axis grid lines.

  \note Normally you would <b>not</b> call this function since
  grid lines in most cases look best in their default line
width: the same width as the axis line they belong to.
However when combining multiple datasets or multiple charts
sharing the same abscissa axes but having their ordinates
differently scaled you might want to reduce the line width
of the respective grid lines and use different grid colours
to show two grids at the same time.  In such cases it might
also be a good idea to deactivate \c setAxisShowSubDelimiters
thus avoiding the dotted sub-grid lines or to set their
style to Qt::NoPen to get sub-delimiters on the axis
but no sub-grid lines.

You may use setAxisGridLineWidth( KDCHART_AXIS_GRID_AUTO_LINEWIDTH )
to reset the value to its default: being automatically
adjusted to the width of the axis line.

\param axisGridLineWidth the width of the axis grid lines.
If value is negative, the absolute value is per thousand
of the printable area size to be used. This will make the
grid look the same even if scaled to very different size.
\sa axisGridLineWidth, setAxisGridColor, setAxisGridStyle
\sa setAxisShowGrid, setAxisShowSubDelimiters
*/
void  KDChartAxisParams::setAxisGridLineWidth( int axisGridLineWidth )
{
    _axisGridLineWidth = axisGridLineWidth;
    emit changed();
}

/**
  \fn int  KDChartAxisParams::axisGridLineWidth() const
  Returns the width of the axis grid lines.
  (see explanation given with \c setAxisGridLineWidth )

  \return the width of the axis grid lines.
  \sa setAxisGridLineWidth, setAxisShowGrid
  */

/**
  Specifies the width of the thin lines between the
  normal axis grid lines.

  You may use setAxisGridSubLineWidth( KDCHART_AXIS_GRID_AUTO_LINEWIDTH )
  to reset the value to its default: being automatically
  adjusted to the width of the axis line.

  \param axisGridSubLineWidth the axis grid sub line width.
  \sa axisGridSubLineWidth, setAxisGridLineWidth, setAxisShowGrid, setAxisShowSubDelimiters
  */
void  KDChartAxisParams::setAxisGridSubLineWidth( int axisGridSubLineWidth )
{
    _axisGridSubLineWidth = axisGridSubLineWidth;
    emit changed();
}


/**
  \fn QColor  KDChartAxisParams::axisGridSubLineWidth() const
  Returns the axis grid sub line width.

  \return the axis grid sub line width.
  \sa setAxisGridSubLineWidth
  */


/**
  Specifies the axis grid line pattern.

  \param axisGridStyle the axis grid line pattern.
  \sa axisGridStyle, setAxisShowGrid
  */
void  KDChartAxisParams::setAxisGridStyle( PenStyle axisGridStyle )
{
    _axisGridStyle = axisGridStyle;
    emit changed();
}

/**
  \fn PenStyle  KDChartAxisParams::axisGridStyle() const
  Returns the axis grid line pattern.

  \return the axis grid line pattern.
  \sa setAxisGridStyle, setAxisShowGrid
  */


/**
  Specifies the axis grid line pattern for the thin lines
  showing the sub-delimiter values.

  \param axisGridStyle the axis grid line pattern for the thin lines
  showing the sub-delimiter values.
  \sa axisGridSubStyle, setAxisGridStyle, axisGridStyle
  \sa setAxisShowGrid
  */
void  KDChartAxisParams::setAxisGridSubStyle( PenStyle axisGridSubStyle )
{
    _axisGridSubStyle = axisGridSubStyle;
    emit changed();
}

/**
  \fn  PenStyle  KDChartAxisParams::axisGridSubStyle() const
  Returns the axis grid line pattern for the thin lines
  showing the sub-delimiter values.

  \return the axis grid line pattern for the thin lines
  showing the sub-delimiter values.
  \sa setAxisGridSubStyle
  \sa setAxisGridStyle, axisGridStyle, setAxisShowGrid
  */

/**
  Specifies the colour of the zero-line
  that is drawn if zero is not at the lower
  edge of the chart.

  \param axisZeroLineColor the zero-line colour.
  \sa axisZeroLineColor
  */
void  KDChartAxisParams::setAxisZeroLineColor( QColor axisZeroLineColor )
{
    _axisZeroLineColor = axisZeroLineColor;
    emit changed();
}

/**
  \fn  QColor  KDChartAxisParams::axisZeroLineColor() const
  Returns the colour used for the zero-value line
  that is drawn if zero is not at the lower
  edge of the chart.

  \return the zero-line colour.
  \sa setAxisZeroLineColor
  */

/**
  Specifies whether the axis labels should be drawn.

  \param axisLabelsVisible if true the labels of this axis will be
  drawn.
  \sa axisLabelsVisible
  */
void  KDChartAxisParams::setAxisLabelsVisible( bool axisLabelsVisible )
{
    _axisLabelsVisible = axisLabelsVisible;
    emit changed();
}


/**
  \fn bool  KDChartAxisParams::axisLabelsVisible() const
  Returns whether the axis labels should be drawn.

  \return whether the axis labels should be drawn.
  \sa setAxisLabelsVisible
  */


/**
  \fn void KDChartAxisParams::setAxisLabelsFontMinSize( int axisLabelsFontMinSize )

  Specifies the minimal font size to be used for displaying the axis labels.

  Use this to specify the minimal font size to be used for axis labels,
  in case KD Chart is calculating the axis labels fonts dynamically. Default value is 10 points.

  \sa setAxisLabelsFont, setAxisLabelsFontUseRelSize, setAxisLabelsFontRelSize
  */



/**
  Specifies whether the axis labels start and end at the
  edges of the charts instead being positioned in the
  middle of the first data point (or the last one, resp.)

  \param axisLabelsTouchEdges if the axis labels start and end at the
  edges of the charts instead being positioned in the
  middle of the first data point (or the last one, resp.)

  \sa axisLabelsTouchEdges
  */
void  KDChartAxisParams::setAxisLabelsTouchEdges( bool axisLabelsTouchEdges )
{
    _axisLabelsTouchEdges = axisLabelsTouchEdges;
    emit changed();
}

/**
  \fn  bool  KDChartAxisParams::axisLabelsTouchEdges() const
  Returns whether the axis labels start and end at the
  edges of the charts instead being positioned in the
  middle of the first data point (or the last one, resp.)

  \return whether the axis labels start and end at the
  edges of the charts instead being positioned in the
  middle of the first data point (or the last one, resp.)
  \sa setAxisLabelsTouchEdges
  */

/**
  Specifies the axis labels font.

  \note The font size will be ignored if \c useFontSize is false,
  in this case the font size will be calculated dynamically using
  the value stored by you calling setAxisLabelsFontRelSize().

  \param axisLabelsFont the font to be used for the axis' labels.
  \param useFontSize set ti true if the fixed font size of
  the \c axisLabelsFont is to be used, otherwise the font size
  will be calculated dynamically.

  \sa setAxisLabelsFontRelSize, setAxisLabelsFontUseRelSize
  \sa axisLabelsFont, axisLabelsFontRelSize
  */
void  KDChartAxisParams::setAxisLabelsFont( QFont axisLabelsFont, bool useFontSize )
{
    _axisLabelsFont = axisLabelsFont;
    _axisLabelsFontUseRelSize = ! useFontSize;
    emit changed();
}
/**
  \fn QFont  KDChartAxisParams::axisLabelsFont() const
  Returns the axis labels font.

  \return the axis labels font.
  \sa setAxisLabelsFont, setAxisLabelsFontRelSize
  \sa axisLabelsFontRelSize
  */

/**
  Specifies whether axis labels shall be drawn
  using relative font size.

  \param axisLabelsFontUseRelSize whether axis labels shall be drawn
  using relative font size.
  If true the absolute value of the value set by \c
  setAxisLabelsFontRelSize is per thousand
  of of the printable area size
  to be used. This will make the axis look the same even if scaled
  to very different size.

  \sa setAxisLabelsFontRelSize, setAxisLabelsFont
  */
void  KDChartAxisParams::setAxisLabelsFontUseRelSize( bool axisLabelsFontUseRelSize )
{
    _axisLabelsFontUseRelSize = axisLabelsFontUseRelSize;
    emit changed();
}

/**
  \fn  bool  KDChartAxisParams::axisLabelsFontUseRelSize() const
  Returns whether the fix axis font size is used.

  \return whether the fix axis labels font size is used.
  \sa setAxisLabelsFontRelSize, setAxisLabelsFont
  */


/**
  Specifies the axis labels relative font size.

  \param axisLabelsFontRelSize the relative axis font size.
  If this value unequals zero the absolute value is per thousand
  of the printable area width size
  to be used. This will make the axis look the same even if scaled
  to very different size.

  \sa setAxisLabelsFontUseRelSize, setAxisLabelsFont
  */
void  KDChartAxisParams::setAxisLabelsFontRelSize( int axisLabelsFontRelSize )
{
    _axisLabelsFontRelSize = axisLabelsFontRelSize;
    emit changed();
}


/**
  \fn int  KDChartAxisParams::axisLabelsFontRelSize() const
  Returns the axis labels relative font size.

  \return the axis labels relative font size.
  \sa setAxisLabelsFontRelSize, setAxisLabelsFontUseRelSize
  */

/**
  \fn bool  KDChartAxisParams::axisLabelsDontShrinkFont() const
  Specifies whether the axis labels' font size may be shrinked
  to avoid overwriting neighboring areas.

  \sa axisLabelsDontShrinkFont
  \sa setAxisLabelsDontAutoRotate, setAxisLabelsRotation
  \sa setAxisLabelsFontUseRelSize, setAxisLabelsFont
  */


/**
  \fn  void  KDChartAxisParams::setAxisLabelsDontAutoRotate( bool labelsDontAutoRotate )

  Specifies whether the axis labels may be rotated
  to avoid overwriting neighboring areas.

  \sa axisLabelsDontAutoRotate
  \sa setAxisLabelsDontShrinkFont, setAxisLabelsRotation
  \sa setAxisLabelsFontUseRelSize, setAxisLabelsFont
  */

/**
  \fn bool  KDChartAxisParams::axisLabelsDontAutoRotate() const
  Returns whether the axis labels may not be rotated
  to avoid overwriting neighboring areas.

  \return whether the axis labels may not be rotated
  to avoid overwriting neighboring areas.
  \sa setAxisLabelsDontAutoRotate
  \sa axisLabelsDontShrinkFont, setAxisLabelsRotation
  \sa setAxisLabelsFontRelSize, setAxisLabelsFont
  */

/**
  \fn void  KDChartAxisParams::setAxisLabelsRotation( int rotation )
  Specifies by how many degrees the axis labels shall be rotated.

  \param rotation The rotation of the labels - value must be either
  zero or between 360 and 270. 360 degrees means don't rotate.

  \note This setting specifies the <b>minimum</b> rotation of
  the labels. Rotation may be increased to fit labels into
  available space unless you explicitely called the
  setAxisLabelsDontAutoRotate() function.

  \sa axisLabelsDontAutoRotate
  \sa setAxisLabelsDontShrinkFont, setAxisLabelsRotation
  \sa setAxisLabelsFontUseRelSize, setAxisLabelsFont
  */


/**
  \fn  int  KDChartAxisParams::axisLabelsRotation() const
  Returns by how many degrees the axis labels will be rotated.

  \sa setAxisLabelsDontAutoRotate
  \sa axisLabelsDontShrinkFont, setAxisLabelsRotation
  \sa setAxisLabelsFontRelSize, setAxisLabelsFont
  */


/**
  Specifies the axis labels colour.

  \param axisLabelsColor the axis labels colour.
  \sa axisLabelsColor
  */
void KDChartAxisParams::setAxisLabelsColor( QColor axisLabelsColor )
{
    _axisLabelsColor = axisLabelsColor;
    emit changed();
}

/**
  \fn  QColor  KDChartAxisParams::axisLabelsColor() const
  Returns the axis labels colour.

  \return the axis labels colour.
  \sa setAxisLabelsColor
  */

/**
  Specifies the calculations to be applied to the axis labels.

  \param divPow10  The power of 10 which the data value is to be divided by.
    A value of 2 means divide by 100, a value of  -3 means multiply by 1000,
    and 0 (by definition) would result in multiplying by 1.
  \param digitsBehindComma The number of digits to show behind the comma,
    to have this calculated automatically just use the default value
    KDCHART_DATA_VALUE_AUTO_DIGITS.
  \sa setAxisLabelsFormat, axisLabelsDivPow10, axisLabelsDigitsBehindComma
  */
void KDChartAxisParams::setAxisLabelsCalc( int divPow10,
                                           int digitsBehindComma )
{
  _axisLabelsDivPow10 = divPow10;
  _axisDigitsBehindComma = digitsBehindComma;
  emit changed();
}

/**
  Specifies the way how the axis label strings will be formatted. Will be ignored for non-numerical axis labels.

  \param decimalPoint The 'radix character' (or text, resp.) to be inserted
    into the string (default is '.').
  \param thousandsPoint The character (or text, resp.) to be used as delimiter
    between the hundred digit and the thousand digit and between the
    100.000 and the 1.000.000 digit (default is ',').

  \sa setAxisLabelsFormat, setAxisLabelsCalc
  \sa axisLabelsDecimalPoint, axisLabelsThousandsPoint
  */
void KDChartAxisParams::setAxisLabelsRadix( const QString& decimalPoint,
                                            const QString& thousandsPoint )
{
  _axisLabelsDecimalPoint = decimalPoint;
  _axisLabelsThousandsPoint = thousandsPoint;
}

/**
  Specifies the way how the axis label strings will be formatted. Will be ignored for non-numerical axis labels.

  \param prefix The character (or text, resp.) to be prepended before the string
    after inserting the decimalPoint and the thousandsPoint.
  \param postfix The character (or text, resp.) to be appended to the string
    after adding decimalPoint, thousandsPoint, prefix.
  \param totalLen The forced size of the string after adding decimalPoint,
    thousandsPoint, prefix, postfix. If this parameter is set to zero
    (default) no padding will be performed.
    If the string has more characters than the (non-zero) value of totalLen,
    the respective number of characters will be cut off at the right side.
  \param padFill The padding character to be prepended before the string
    (or inserted into the string, resp.) when increasing its length until
    the totalLen is reached.
  \param blockAlign If set to true (default) the padFill character(s) will be
    inserted between the number and the prefix, if set to false they will be
    prepended before the prefix.

  \sa setAxisLabelsRadix, setAxisLabelsCalc
  \sa axisLabelsPrefix, axisLabelsPostfix, axisLabelsTotalLen
  \sa axisLabelsPadFill, axisLabelsBlockAlign
  */
void KDChartAxisParams::setAxisLabelsFormat( const QString& prefix,
                                             const QString& postfix,
                                             const int&     totalLen,
                                             const QChar&   padFill,
                                             const bool&    blockAlign )
{
  _axisLabelsPrefix = prefix;
  _axisLabelsPostfix = postfix;
  _axisLabelsTotalLen = totalLen;
  _axisLabelsPadFill = padFill;
  _axisLabelsBlockAlign = blockAlign;
}



/**
  \c enum LabelsFromDataRow { LabelsFromDataRowYes,
  LabelsFromDataRowNo,
  LabelsFromDataRowGuess };

  Are the axis labels stored in a data row?
  If \c LabelsFromDataRowGuess we assume yes only if
  all the entries of that data row contain strings - no numbers.

  \li \c LabelsFromDataRowYes = label texts are to be taken from data row
  \li \c LabelsFromDataRowNo  = do not take label texts from data row
  \li \c LabelsFromDataRowGuess = take label texts from data row if all
  entries in that row are strings (no numerical values!)

  \sa setAxisValues, setAxisValueStart, setAxisValueEnd, setAxisValueDelta
  \sa setLabelTextsFormDataRow
  \sa axisLabelTextsFormDataRow
  */


/**
  Converts the specified labels from data row enum to a string
  representation.

  \param mode the enum to convert
  \return the string representation of the mode enum
  */
QString  KDChartAxisParams::labelsFromDataRowToString( LabelsFromDataRow mode ) {
    switch( mode ) {
        case LabelsFromDataRowYes:
            return "Yes";
        case LabelsFromDataRowNo:
            return "No";
        case LabelsFromDataRowGuess:
            return "Guess";
        default: // should not happen
            qDebug( "Unknown labels from data row mode" );
            return "Guess";
    }
}

/**
  Converts the specified string to a data row enum value.

  \param string the string to convert
  \return the data row mode enum value
  */
KDChartAxisParams::LabelsFromDataRow  KDChartAxisParams::stringToLabelsFromDataRow( const QString& type ) {
    if( type == "Yes" )
        return LabelsFromDataRowYes;
    else if( type == "No" )
        return LabelsFromDataRowNo;
    else if( type == "Guess" )
        return LabelsFromDataRowGuess;
    else // should not happen
        return LabelsFromDataRowGuess;
}

/**

  \c enum ValueScale { ValueScaleNumerical =   0, ( have gaps here to allow specifying of additional scaling steps in between )
  ValueScaleSecond    =  20,
  ValueScaleMinute    =  30,
  ValueScaleHour      =  40,
  ValueScaleDay       =  50,
  ValueScaleWeek      =  60,
  ValueScaleMonth     =  70,
  ValueScaleQuarter   =  80,
  ValueScaleYear      =  90 };

  Are axis labels scaled mumerically or fixing a time period?
  If \c ValueScaleNumerical axis labels are just numbers like "item 1", "item 2"...

  \li \c ValueScaleSecond  = seconds
  \li \c ValueScaleMinute = minutes
  ..
  \li \c ValueScaleYear = years

  \sa setAxisValues, setAxisValueStart, setAxisValueEnd, setAxisValueDelta
  \sa setLabelTextsFormDataRow
  \sa axisLabelTextsFormDataRow
  */


/**

Note: The following text is to become part of the setAxisValues() doku
you see below.
It shall be added once the automatic string support has been added!


Currently the following strings are supported by the automatical
numbering feature. (These strings are implemented for your convenience,
you may specify any other set of label texts by passing a \c QStringList
pointer to \c axisLabelStringList.)

\li Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday
\li January, February, March, May, June, July, August, September, October, November, December
\li Spring, Summer, Autumn, Winter

...

To specify the start value and the way the label values
are calculated you may

\li \b either pass one of the <b>above strings</b> as start value
<br>
In this case the label texts will be taken from the respective
list restarting at the beginnig whenever the end is reached.
<br>
In case you specify a non-zero integer value for
\c axisValueDelta the width and direction
of steps will be performed accordingly:
<br>
A value of <b>1.0</b>
(specifying a step-length of 1 ) will cause every entry to be
taken.
<br>
A <b>4.0</b> would take the start entry, skip 3 entries, take
the 4th entry and so forth...
<br>
By using negative numbers you can specify backward stepping.
<br>
(Of course all non-integer values will be ignored here.)
*/

/**
  <B>General axis texts setup routine.</B><BR>
  Specifies how the axis limits shall be calculated
  and specifies the lower and the upper limit,
  the step-width and the list of strings
  (if such is to be used).

  \note Start and end values are \c KDChartData so you may either
  specify numerical data or pass a string. <br>
  However if passing a string make sure to also specify a
  valid QStringList* for \c axisLabelStringList.
  In that case it could be wise to also also specify a second
  QStringList* for \c axisShortLabelsStringsList to be used in
  case the axes area is not wide enough to display the
  label texts in their full length.

  <P>

  To specify the start value and the way the label values
  are calculated you may

  \li \b either pass a <b>string</b> as start value
  <br>
  In this case the label texts will be taken from the string
  list passed to the function via \c axisLabelStringList
  starting with the string you specified as start value and
  restarting at the beginnig whenever the end is reached.
  In case the start value is not contained in the
  \c axisLabelStringList list it will be ignored and labelling
  will be done as if you had specified
  KDCHART_AXIS_LABELS_AUTO_LIMIT as start value (see below).
  <br>
  In case you specify a non-zero integer value for
  \c axisValueDelta the width and direction
  of steps will be performed accordingly:
  <br>
  A value of <b>1.0</b>
  (specifying a step-length of 1 ) will cause every entry
  of the list to be taken.
  <br>
  A <b>4.0</b> would take the start entry, skip 3 entries, take
  the 4th entry and so forth...
  <br>
  By using negative numbers you can specify backward stepping.
  <br>
  (Of course all non-integer values will be ignored here.)
  \li \b or pass <b>KDCHART_AXIS_LABELS_AUTO_LIMIT</b> as start value
  <br>
  In this case the first label text <br>
  <i>either</i> will be calculated
  based upon the lowest value of the associated
  datasets row number \c labelTextsDataRow if
  \c axisLabelsFromDataRow is set to true, <br>
  <i>or</i> the first entry of the texts list will be taken
  if \c axisLabelStringList is set properly, <br>
  <i>otherwise</i> it will be set to "1".

  \li \b or pass a <b>numerical value</b> as start value
  <br>
  In this case the first label text will be set to that value.
  <br>
  <b>In either case</b> (KDCHART_AXIS_LABELS_AUTO_LIMIT or numerical value)
  the \c axisValueDelta may be used to specify the value to be
  added to obtain the next values:
  <br>
  A value of <b>1.0</b> will cause 1.0 to be added to the current
  value to make the next value.
  <br>
  A <b>-0.25</b> would cause 0.25 to be subtracted from the current value.
  <br>
  (Negative or non-integer numbers are allowed here.)

<P>

Examples:

\verbatim
setAxisValues();
\endverbatim
This would specify a default ordinate-axis obtaining its values from the
attached dataset. Looks nice but it is difficult to compare it to another
chart representing slightly different min/max values since it neither starts
at zero nor ends at an allways same value.

\verbatim
setAxisValues( true, KDChartData( 0.0 ) );
\endverbatim
This would specify a half-automatical ordinate-axis obtaining its values from
the attached dataset. Looks nice and can be slightly easier compared to
another chart since it allways starts at zero, causing negative values to be
ignored completely.

\verbatim
setAxisValues( true, KDChartData( -2.0 ),
        KDChartData(  7.5 ),
        0.5, 1 );
\endverbatim
This would specify an ordinate-axis \not obtaining its values
from the attached dataset.
<b>Since both the start value and the end value are specified the range
of the dataset values are ignored completely.</b>
It will show one digit behind the comma.
The resulting chart can be perfectly compared to
another chart since it allways starts and ends at the very same level - no
matter what the dataset values are about actually.

\verbatim
setAxisValues( false, KDChartData( 0.0 ),
        KDChartData( 3.5),
        0.25, 2 );
\endverbatim
This would specify a default abscissa-axis starting with value 0.0 and
adding 0.25 to get the next value, will count until 3.5.
It will show two digits behind the comma.

\verbatim
setAxisValues( false,
        KDChartData( 1964.0 ), KDCHART_AXIS_LABELS_AUTO_LIMIT, 1, 0 );
\endverbatim
This would specify a default abscissa-axis starting with value 1964 and
adding 1 to get the next value.
It will show no digits behind the comma.

\verbatim
KDChartParams p;
KDChartAxisParams pa( p.axisParams( KDChartAxisParams::AxisPosBottom ) );

QStringList abscissaNames;
abscissaNames << "Sunday" << "Monday" << "Tuesday" << "Wednesday"
<< "Thursday" << "Friday" << "Saturday";

QStringList abscissaShortNames;
abscissaShortNames << "Sun" << "Mon" << "Tue" << "Wed"
<< "Thu" << "Fri" << "Sat";

pa.setAxisValues( false,
        KDChartData( "Monday" ),
        KDCHART_AXIS_LABELS_AUTO_LIMIT,
        KDCHART_AXIS_LABELS_AUTO_DELTA,
        KDCHART_AXIS_LABELS_AUTO_DIGITS,
        KDChartAxisParams::LabelsFromDataRowNo,
        0,
        &abscissaNames,
        &abscissaShortNames );
p.setAxisParams( KDChartAxisParams::AxisPosBottom, pa );
\endverbatim
This would specify a default abscissa-axis starting with 'Monday' and
counting the days of the week as far as neccessary to represent
all the entries in the associated dataset.
Note this \c LabelsFromDataRowNo indicating that the texts are <b>not</b> to
be taken from a data row and note \c &abscissaNames indicating the
\c QStringList where to take the texts from instead. (In case the axis area
        is not wide enough to display the strings in their full length their
        counterparts stored in abscissaShortNames will be displayed instead.)
<b>Note also:</b> The strings in those two QStringList are <b>not</b>
copied into the \c KDChartAxisParams nor into the \c KDChartParams
so please make sure the Lists are alive and valid till the end of
the param objects. Otherwise you will not be able to display the
texts.
<P>

\param axisSteadyValueCalc specifies whether label values shall be calculataed based upon the associated dataset values (normally this is true for ordinate axes) or based upon some string list (as you might expect it for abscissa labels).
\param axisValueStart specifies first label value to be written.
\param axisValueEnd specifies the last label value to be written.
\param axisValueDelta specifies the length of the steps to be taken from one label text to the next one.
\param axisDigitsBehindComma specifies how many digits are to be shown behind the axis label texts comma.
\param axisMaxEmptyInnerSpan specifies the percentage of the y-axis range that may to contain NO data entries, if - and only if - axisValueStart (or axisValueEnd, resp.) is set to KDCHART_AXIS_LABELS_AUTO_LIMIT. To prevent \c setAxisValues from changing the current setting you may specify KDCHART_DONT_CHANGE_EMPTY_INNER_SPAN_NOW here, to deactivate taking into account the inner span entirely just use KDCHART_AXIS_IGNORE_EMPTY_INNER_SPAN.
\param takeLabelsFromDataRow specifies whether the labels texts shall be taken from a special row (reserved for this in each dataset) or not.
\param axisLabelStringList points to a \c QStringList containing the label texts to be used.
\param axisShortLabelsStringList points to a \c QStringList containing the label texts to be used in case their full-size counterparts cannot be shown due to limited axis area size.
\param axisValueLeaveOut is used for horizontal (top or bottom) axes only; it specifies whether some of the axis labels are to be skipped if there is not enough room for drawing them all without overlapping - this parameter may be set to KDCHART_AXIS_LABELS_AUTO_LEAVEOUT or to zero or to another positive value.
\param axisValueDeltaScale is used to specify the scaling mode of \c axisValueDelta - either just ValueScaleNumbers of a special time scale indicator.
\sa setAxisValueStart, setAxisValueEnd, setAxisValueDelta, setAxisValuesDecreasing
\sa axisValueStart axisValueEnd, axisValueDelta, ValueScale
\sa LabelsFromDataRow, axisLabelTextsFormDataRow, axisLabelTexts
\sa axisSteadyValueCalc, setAxisValueLeaveOut
*/
void  KDChartAxisParams::setAxisValues( bool axisSteadyValueCalc,
        const QVariant& axisValueStart,
        const QVariant& axisValueEnd,
        double axisValueDelta,
        int axisDigitsBehindComma,
        int axisMaxEmptyInnerSpan,
        LabelsFromDataRow takeLabelsFromDataRow,
        int labelTextsDataRow,
        QStringList* axisLabelStringList,
        QStringList* axisShortLabelsStringList,
        int axisValueLeaveOut,
        ValueScale axisValueDeltaScale )
{
    _axisSteadyValueCalc = axisSteadyValueCalc;
    _axisValueStart = axisValueStart;
    _axisValueEnd   = axisValueEnd;
    _axisValueLeaveOut = axisValueLeaveOut;
    _axisValueDelta = axisValueDelta;
    _axisValueDeltaScale = axisValueDeltaScale;
    _axisDigitsBehindComma = axisDigitsBehindComma;
    if ( KDCHART_DONT_CHANGE_EMPTY_INNER_SPAN_NOW != axisMaxEmptyInnerSpan ) {
        if ( 100 < axisMaxEmptyInnerSpan
                || 1 > axisMaxEmptyInnerSpan )
            _axisMaxEmptyInnerSpan = KDCHART_AXIS_IGNORE_EMPTY_INNER_SPAN;
        else
            _axisMaxEmptyInnerSpan = axisMaxEmptyInnerSpan;
    }
    _takeLabelsFromDataRow = takeLabelsFromDataRow;
    _labelTextsDataRow = labelTextsDataRow;
    if( axisLabelStringList )
        _axisLabelStringList = *axisLabelStringList;
    else
        _axisLabelStringList.clear();
    if( axisShortLabelsStringList )
        _axisShortLabelsStringList = *axisShortLabelsStringList;
    else
        _axisShortLabelsStringList.clear();
    // label texts must be recalculated?
    setAxisLabelTexts( 0 );
    setTrueAxisDeltaPixels( 0.0 );
    setTrueAxisLowHighDelta( 0.0, 0.0, 0.0 );
    setTrueAxisDtLowHighDeltaScale( QDateTime(), QDateTime(), ValueScaleDay );
    emit changed();
}

/**
  \fn  void  KDChartAxisParams::setAxisValueStart( const KDChartData axisValueStart )
  Specifies the lower limit for the axis labels: the start value.

  \param axisValueStart the lower limit for the axis labels: the start
  value.
  \sa setAxisValues, setAxisValueStartIsExact
  \sa setAxisValueEnd, setAxisValueDelta
  \sa axisValueStart, axisValueEnd, axisValueDelta
  \sa axisLabelsFromDataRow, axisLabelTexts
  \sa axisLabelStringList, axisShortLabelsStringList
  */

/**
  \fn  KDChartData  KDChartAxisParams::axisValueStart() const
  Returns the lower limit for the axis labels: the start value.

  \return the lower limit for the axis labels: the start value.
  \sa setAxisValues, setAxisValueStart, setAxisValueStartIsExact
  \sa setAxisValueEnd, setAxisValueDelta
  \sa axisValueEnd, axisValueDelta
  \sa axisLabelsFromDataRow, axisLabelTexts
  \sa axisLabelStringList, axisShortLabelsStringList
  */

/**
  \fn  void setAxisValueStartIsExact( bool isExactValue )
  Specifies whether the lower limit for the axis labels that
  is specified via setAxisValueStart() is to be used as the
  axis start value even if this will not look very nice: this is
  the default, KD Chart just takes the value specified by you, e.g.
  if you specify 75003.00 as start value you will get exactly this.

  By setting this flag to FALSE you let KD Chart find a better
  value for you: if your value is not Zero the axis will start
  with the next value lower than your start value that can be
  divided by the delta factor.

  \param isExactValue set this to FALSE if KD Chart shall find
  a better value than the one you have specified by setAxisValueStart()
  \sa setAxisValues, setAxisValueEnd, setAxisValueDelta
  \sa axisValueStartIsExact, axisValueStart
  \sa axisValueEnd, axisValueDelta
  \sa axisLabelsFromDataRow, axisLabelTexts
  \sa axisLabelStringList, axisShortLabelsStringList
  */

/**
  \fn  bool axisValueStartIsExact() const
  Returns whether the lower limit for the axis labels that
  is specified via setAxisValueStart() is to be used as the
  axis start value even if this will not look very nice: this is
  the default, KD Chart just takes the value specified by you, e.g.
  if you specify 75003.00 as start value you will get exactly this.

  \return whether the lower limit for the axis labels that
  is specified via setAxisValueStart() is to be used as the
  axis start value even if this will not look very nice.
  \sa setAxisValues, setAxisValueStartIsExact, setAxisValueStart
  \sa setAxisValueEnd, setAxisValueDelta
  \sa axisValueEnd, axisValueDelta
  \sa axisLabelsFromDataRow, axisLabelTexts
  \sa axisLabelStringList, axisShortLabelsStringList
  */


/**
  \fn  void  KDChartAxisParams::setAxisValueEnd( const KDChartData axisValueEnd )
  Specifies the upper limit for the axis labels: the end value.

  \param axisValueStart the upper limit for the axis labels: the end
  value.
  \sa setAxisValues, setAxisValueStart, setAxisValueStartIsExact
  \sa setAxisValueDelta
  \sa axisValueStart, axisValueEnd, axisValueDelta
  \sa axisLabelsFromDataRow, axisLabelTexts
  \sa axisLabelStringList, axisShortLabelsStringList
  */

/**
  \fn  KDChartData  KDChartAxisParams::axisValueEnd() const
  Returns the upper limit for the axis labels: the end value.

  \return the upper limit for the axis labels: the end value.
  \sa setAxisValues, setAxisValueStart, setAxisValueStartIsExact
  \sa setAxisValueEnd, setAxisValueDelta
  \sa axisValueStart, axisValueDelta
  \sa axisLabelsFromDataRow, axisLabelTexts
  \sa axisLabelStringList, axisShortLabelsStringList
  */


/**
  Specifies the DELTA value for the axis labels: the distance
  between two labels.

  \param axisValueDelta the DELTA value for the axis labels: the distance
  between two labels.
  \param scale the scaling of the DELTA value
  \sa ValueScale
  \sa setAxisValues
  \sa setAxisValueStart, setAxisValueStartIsExact
  \sa setAxisValueEnd, setAxisValueDelta
  \sa axisValueStart, axisValueEnd, axisValueDelta
  \sa axisLabelsFromDataRow, axisLabelTexts
  \sa axisLabelStringList, axisShortLabelsStringList
  */
void  KDChartAxisParams::setAxisValueDelta( const double axisValueDelta,
        ValueScale scale )
{
    _axisValueDelta = axisValueDelta;
    _axisValueDeltaScale = scale;
}

/**
  \fn  double KDChartAxisParams::axisValueDelta() const
  Returns the DELTA value for the axis labels: the distance
  between two labels.

  \return the DELTA value for the axis labels: the distance
  between two labels.
  \sa setAxisValueDelta
  */


/**
  \fn ValueScale  KDChartAxisParams::axisValueDeltaScale() const
  Returns the DELTA value scaling mode for the axis labels

  \sa setAxisValueDelta
  */



/**
  \fn  void  KDChartAxisParams::setAxisValueLeaveOut( const int leaveOut )
  Specifies how many axis labels are to be skipped
  if there is not enough space for displaying all of them.
  This is usefull in case you have lots of entries in one dataset.

  \sa setAxisValues
  */
/**
  \fn int  KDChartAxisParams::axisValueLeaveOut() const
  Returns how many axis labels are to be skipped
  if there is not enough space for displaying all of them.

  \sa setAxisValueLeaveOut
  */

/**
  \fn  void  KDChartAxisParams::setAxisValuesDecreasing( bool valuesDecreasing )
  Specifies whether axis values should be printed in reverted order: starting
  with the highest label and decreasing until the lowest label is reached.

  \note This feature is supported for LINE charts only.

  \sa setAxisValues
*/

/**
  \fn bool  KDChartAxisParams::axisValuesDecreasing() const
  Returns whether axis values should be printed in reverted order: starting
  with the highest label and decreasing until the lowest label is reached.

  \note This feature is supported for LINE charts only.

  \sa setAxisValuesDecreasing
  */

/**
  \fn  void  KDChartAxisParams::setTrueAxisDeltaPixels( double nDeltaPixels )
  Specifies the true axis pixel distance between two label delimiters.

  \param nDeltaPixels the true value as it was calculated.

  \note Do <b>not call</b> this function unless you are knowing
  exactly what you are doing. \c setTrueAxisDeltaPixels is normally
  reserved for internal usage by methods calculating the axis
  label texts. Thus the signal \c changed() is not sended here.

  \sa trueAxisDeltaPixels, trueAxisLow, trueAxisHigh, trueAxisDelta
  \sa setAxisArea
  */

/**
  \fn  double  KDChartAxisParams::trueAxisDeltaPixels() const
  Returns the <b>true</b> delimiter delta pixel value of the axis labels
  as is was calculated and set by \c setTrueAxisDeltaPixels.

  \return the true delimiter delta pixel value of the axis labels
  \sa setAxisValues
  \sa trueAxisLow, trueAxisHigh, trueAxisDelta
  */



/**
  Specifies the true axis lower and upper limit values of the axis
  and the exact Delta value between the axis delimiters.

  \param nLow/nHigh/nDelta the true values as they were calculated.

  \note Do <b>not call</b> this function unless you are knowing
  exactly what you are doing. \c setAxisTrueAreaSize is normally
  reserved for internal usage by methods calculating the axis
  label texts. Thus the signal \c changed() is not sended here.

  \sa trueAxisLow, trueAxisHigh, trueAxisDelta, trueAxisDeltaPixels
  \sa setAxisArea
  */
void  KDChartAxisParams::setTrueAxisLowHighDelta( double nLow, double nHigh, double nDelta )
{
    _trueLow = nLow;
    _trueHigh = nHigh;
    _trueDelta = nDelta;

}


/**
  \fn double  KDChartAxisParams::trueAxisLow() const
  Returns the <b>true</b> start value of the ordinate axis labels
  as is was calculated and set by \c setTrueAxisLowHighDelta.

  \return the true lower limit of the axis labels
  \sa setAxisValues
  \sa trueAxisHigh, trueAxisDelta, trueAxisDeltaPixels
  */

/**
  \fn  double  KDChartAxisParams::trueAxisHigh() const
  Returns the <b>true</b> end value of the ordinate axis labels
  as is was calculated and set by \c setTrueAxisLowHighDelta.

  \return the true upper limit of the axis labels
  \sa setAxisValues
  \sa trueAxisLow, trueAxisDelta, trueAxisDeltaPixels
  */

/**
  \fn  double  KDChartAxisParams::trueAxisDelta() const
  Returns the <b>true</b> delta value of the ordinate axis labels
  as is was calculated and set by \c setTrueAxisLowHighDelta.

  \return the true delta value of the axis labels
  \sa setAxisValues
  \sa trueAxisLow, trueAxisHigh, trueAxisDeltaPixels
  */



void  KDChartAxisParams::setTrueAxisDtLowHighDeltaScale( QDateTime dtLow, QDateTime dtHigh,
        ValueScale dtDeltaScale )
{
    _trueDtLow        = dtLow;
    _trueDtHigh       = dtHigh;
    _trueDtDeltaScale = dtDeltaScale;
}

/**
  \fn  void  KDChartAxisParams::setTrueAxisDtLow( QDateTime dtLow )

*/

/**
  \fn  void  KDChartAxisParams::void setTrueAxisDtHigh( QDateTime dtHigh )

*/

/**
  \fn  void  KDChartAxisParams::void setTrueAxisDtScale( ValueScale scale )

*/

/**
  \fn  void  KDChartAxisParams::QDateTime trueAxisDtLow() const

*/

/**
  \fn  void  KDChartAxisParams::QDateTime trueAxisDtHigh() const

*/

/**
  \fn  void  KDChartAxisParams::ValueScale trueAxisDtDeltaScale() const

*/


/**
  Specifies the not-rounded screen positions where drawing of
  this axis zero line started.

  \Note You may not call this internal function - it is reserved for
  internal usage of methodes needing to know the zero-line offsets

  \param Pt the not-rounded screen positions where drawing of
  this axis zero line started.

  \sa axisZeroLineStartX, axisZeroLineStartY
  */
void  KDChartAxisParams::setAxisZeroLineStart( double x, double y )
{
    _axisZeroLineStartX = x;
    _axisZeroLineStartY = y;
}

/**
  \fn  double  KDChartAxisParams::axisZeroLineStartX() const
  Returns the not-rounded x-position where drawing of
  this axis zero line started. This function needed
  when painting the data of isometric axes (bars, lines, dots...).

  \return the unrounded x-position where drawing of
  this axis zero line started.

  \sa setAxisZeroLineStart, axisZeroLineStartY
  */

/**
  \fn  double  KDChartAxisParams::axisZeroLineStartY() const
  Returns the not-rounded y-position where drawing of
  this axis zero line started. This function needed
  when painting the data of isometric axes (bars, lines, dots...).

  \return the unrounded y-position where drawing of
  this axis zero line started.

  \sa setAxisZeroLineStart, axisZeroLineStartX
  */

/**
  Specifies the not-rounded screen positions where drawing of
  this axis low date/time value could be done.

  \Note You may not call this internal function - it is reserved for
  internal usage of methodes needing to know the zero-line offsets

  \param Pt the not-rounded screen positions where drawing of
  this axis low date/time value could be done.

  \sa axisDtLowPosX, axisDtLowPosY
  */
void  KDChartAxisParams::setAxisDtLowPos( double x, double y )
{
    _axisDtLowPosX = x;
    _axisDtLowPosY = y;
}

/**
  \fn double void  KDChartAxisParams::axisDtLowPosX() const
  Returns the not-rounded x-position where drawing of
  this axis low date/time value could be done. This function needed
  when painting the data of isometric axes (bars, lines, dots...).

  \return the unrounded x-position where drawing of
  this axis low date/time value could be done.

  \sa setAxisDtLowPos, axisDtLowPosY
  */

/**
  \fn  double  KDChartAxisParams::axisDtLowPosY() const
  Returns the not-rounded y-position where drawing of
  this axis low date/time value could be done. This function needed
  when painting the data of isometric axes (bars, lines, dots...).

  \return the unrounded y-position where drawing of
  this axis low date/time value could be done.

  \sa setAxisDtLowPos, axisDtLowPosX
  */


/**
  Specifies the not-rounded screen positions where drawing of
  this axis high date/time value could be done.

  \Note You may not call this internal function - it is reserved for
  internal usage of methodes needing to know the zero-line offsets

  \param Pt the not-rounded screen positions where drawing of
  this axis high date/time value could be done.

  \sa axisDtHighPosX, axisDtHighPosY
  */
void  KDChartAxisParams::setAxisDtHighPos( double x, double y )
{
    _axisDtHighPosX = x;
    _axisDtHighPosY = y;
}

/**
  \fn  double  KDChartAxisParams::axisDtHighPosX() const
  Returns the not-rounded x-position where drawing of
  this axis high date/time value could be done. This function needed
  when painting the data of isometric axes (bars, lines, dots...).

  \return the unrounded x-position where drawing of
  this axis high date/time value could be done.

  \sa setAxisDtHighPos, axisDtHighPosY
  */

/**
  \fn  double  KDChartAxisParams::axisDtHighPosY() const
  Returns the not-rounded y-position where drawing of
  this axis high date/time value could be done. This function needed
  when painting the data of isometric axes (bars, lines, dots...).

  \return the unrounded y-position where drawing of
  this axis high date/time value could be done.

  \sa setAxisDtHighPos, axisDtHighPosX
  */


/**
  \fn void  KDChartAxisParams::setAxisDigitsBehindComma( const int digits )
  Specifies the number of digits to be printed behind the comma
  on the axis labels.

  \param digits the number of digits to be printed behind the comma
  on the axis labels.

  \sa axisDigitsBehindComma
  */

/**
  \fn  int  KDChartAxisParams::axisDigitsBehindComma() const
  Returns the number of digits to be printed behind the comma
  on the axis labels.

  \return the number of digits to be printed behind the comma
  on the axis labels.
  \sa setAxisValues
  \sa axisValueStart
  \sa axisLabelsFromDataRow, axisLabelTexts
  \sa axisLabelStringList, axisShortLabelsStringList
  \sa setAxisLabelStringLists
  */

/**
  \fn  void  KDChartAxisParams::setAxisLabelsDateTimeFormat( const QString& format )
  Specifies the format to be used for displaying abscissa axis
  QDateTime item labels.

  \note This feature is only available when using Qt 3.0 or newer,
  previous versions use a non changable format.

  To calculate the format automatically (based on the
  time span to be displayed) use the special value
  \c KDCHART_AXIS_LABELS_AUTO_DATETIME_FORMAT - this is the default setting.

  See Qt documentation on the format to be used here:

  $QTDIR/doc/html/qdatetime.html#toString-2

  \note Insert a '\n' character if you want to print the labels in two rows, e.g. "h:mm:ss\nd.MM.yyyy" would do that.

  \sa axisLabelsDateTimeFormat
  */

/**
  \fn  QString  KDChartAxisParams::axisLabelsDateTimeFormat() const
  Returns the format to be used for displaying abscissa axis
  QDateTime item labels.

  \sa setAxisLabelsDateTimeFormat
  */


/**
  \fn  void  KDChartAxisParams::setAxisMaxEmptyInnerSpan( const int maxEmpty )
  Specifies the percentage of the y-axis range that may to contain NO
  data entries, if - and only if - axisValueStart (or axisValueEnd,
  resp.) is set to KDCHART_AXIS_LABELS_AUTO_LIMIT.

  \param maxEmpty the percentage of the y-axis range that may to contain NO
  data entries, if - and only if - axisValueStart (or axisValueEnd,
  resp.) is set to KDCHART_AXIS_LABELS_AUTO_LIMIT.

  \sa axisMaxEmptyInnerSpan
  */

/**
  \fn  int  KDChartAxisParams::axisMaxEmptyInnerSpan() const
  Returns the percentage of the y-axis range that may to contain NO
  data entries, if - and only if - axisValueStart (or axisValueEnd,
  resp.) is set to KDCHART_AXIS_LABELS_AUTO_LIMIT.

  \note If more space is empty the zero-line will not be included info the chart but the lowest (or highest, resp.) entry of the axis will be shifted accordingly.
  \sa setAxisValues
  */


/**
  Specifies whether the axis labels are stored in a data row.
  If \c LabelsFromDataRowGuess we assume yes only if
  all the entries of that data row contain strings - no numbers.

  \note Calling this function results in overwriting the information
  that you might have set by previous calls of that function.
  Only <b>one</b> data row can be specified as containing label texts.
  To specify a data row that contains (or might contain) axis label texts just
  call this function with \c LabelsFromDataRowYes (or \c LabelsFromDataRowGuess,
  resp.) specifying this row but do <b>not</b> call the function n times with
  the \c LabelsFromDataRowNo parameter to 'deactivate' the other rows.
  The \c LabelsFromDataRowNo should be used to specify that <b>none</b> of
  the data rows is containing the axis label texts (this is the default
  setting).

  \param row the data row number that contains (or might contain, resp.) the labels
  \param mode the state of our information concerning that row (see: \c LabelsFromDataRow)

  \sa LabelsFromDataRow, axisLabelTextsFormDataRow, setAxisValues
  */
void  KDChartAxisParams::setLabelTextsFormDataRow( int row, LabelsFromDataRow mode )
{
    _labelTextsDataRow = row;
    _takeLabelsFromDataRow = mode;
}


/**
  \fn  void void  KDChartAxisParams::setLabelTextsFormDataRow( int row, LabelsFromDataRow mode );
  Returns whether the axis labels will be taken from the associated dataset.

  \return whether the axis limits will be taken from the associated dataset.
  \sa setAxisValues
  \sa axisValueStart, axisValueEnd
  \sa axisLabelsFromDataRow, axisLabelTexts
  */


/**
  \fn  int  KDChartAxisParams::labelTextsDataRow() const
  Returns the number of the data row that contains (or might contain,
  resp.) the texts to be taken for the axis labels.
  <br>
  Use \c axisLabelTextsFormDataRow to make sure the texts are
  to be taken from that row.
  <br>
  Use \c axisLabelStringList to get a QStringList* of texts from
  which the texts to be drawn will be taken.

  Use \c axisShortLabelsStringList to get a QStringList* of texts from
  which the texts to be drawn will be taken in case the axis area size
  is too small to display their full-size counterparts stored in
  \c axisLabelStringList.

  Use \c axisLabelTexts to get a QStringList* containing the label
  texts that are <b>actually</b> drawn at the axis.

  \return the number of the data row that contains (or might contain,
  resp.) the texts to be taken for the axis labels.
  \sa setAxisValues
  \sa axisValueStart, axisValueEnd
  \sa axisLabelsFromDataRow, axisLabelTexts
  \sa axisLabelStringList, axisShortLabelsStringList
  */


/**
  Specifies a \c QStringList which the axis label texts are to
  be taken from, the second parameter (if not zero) specifies an
  alternative list of strings that are to be displayed in case
  the axis area size is too small for showing the full-length names.

  \note Normally axis labeling starts with the list's first string
  and end with its last string, but by specifying a start and an
  end value as additional parameters you can make KDChart repeat
  the strings between these two values only, as shown here:

  \verbatim
  QStringList abscissaNames;
  abscissaNames << "Sunday" << "Monday" << "Tuesday" << "Wednesday"
  << "Thursday" << "Friday" << "Saturday";
  QStringList abscissaShortNames;
  abscissaShortNames << "Sun" << "Mon" << "Tue" << "Wed"
  << "Thu" << "Fri" << "Sat";

  KDChartAxisParams pa( _p->axisParams(
  KDChartAxisParams::AxisPosBottom ) );

  setAxisLabelStringParams( &abscissaNames,
  &abscissaShortNames,
  "Monday",
  "Friday")

  _p->setAxisParams( KDChartAxisParams::AxisPosBottom, pa );
  \endverbatim


  \param QStringList* axisLabelStringList points to the list of labels to be displayed
  \param  QStringList* axisShortLabelStringList points to
  an alternative list of short names to be displayed if the long labels take too much place
  \param QString valueStart ( default null ) the label to begin with
  \param QString valueEnd ( default null ) the label to end with

  \sa KDChartParams::setAxisLabelStringParams
  \sa axisLabelStringList, axisShortLabelsStringList
  \sa setAxisValues, setLabelTextsFormDataRow
  \sa axisLabelTexts
  */
void  KDChartAxisParams::setAxisLabelStringLists( QStringList*   axisLabelStringList,
        QStringList*   axisShortLabelStringList,
        const QString& valueStart,
        const QString& valueEnd )
{
    QVariant axisValueStart, axisValueEnd;

    if( valueStart.isNull() )
        axisValueStart = KDCHART_AXIS_LABELS_AUTO_LIMIT;
    else
        axisValueStart = valueStart;

    if( valueEnd.isNull() )
        axisValueEnd = KDCHART_AXIS_LABELS_AUTO_LIMIT;
    else
        axisValueEnd = valueEnd;

    setAxisValues( false,
            axisValueStart,
            axisValueEnd,
            KDCHART_AXIS_LABELS_AUTO_DELTA,
            KDCHART_AXIS_LABELS_AUTO_DIGITS,
            KDCHART_AXIS_IGNORE_EMPTY_INNER_SPAN,
            LabelsFromDataRowNo,
            0,
            axisLabelStringList,
            axisShortLabelStringList,
            KDCHART_AXIS_LABELS_AUTO_LEAVEOUT );

}


/**
  \fn  QStringList  KDChartAxisParams::axisLabelStringList() const
  Returns a \c QStringList containing the label texts to be used.

  Calling \c axisShortLabelsStringList() instead will return
  another \c QStringList containing the label texts to be displayed
  in case the axis area size is too small to show the full-size names.

  \note This is the list of texts you specified by \c setAxisValues
  or by \c setAxisLabelStringLists.
  The texts <b>actually</b> drawn at the axis are <b>not neccessarily</b> the
  same as the ones in this list  since (regarding Start and/or End and/or
  Delta value) they might be only a subset of this list. Whenever label texts
  are calculated automatically the resulting labels are also stored in a
  second list that you may access via \c axisLabelTexts().

  \return a \c QStringList containing the label texts to be used.
  \sa axisShortLabelsStringList
  \sa setAxisLabelStringLists
  \sa setAxisValues
  \sa axisValueStart, axisValueEnd, axisLabelTexts
  \sa axisLabelsFromDataRow, setLabelTextsFormDataRow
  */

/**
  \fn uint  KDChartAxisParams::axisLabelStringCount() const
  Returns the number of strings stored as label texts,
  the texts can be retrieved by calling \c axisLabelStringList().

  \sa axisShortLabelsStringCount
  \sa axisLabelStringList, axisShortLabelsStringList
  */

/**
  \fn  QStringList  KDChartAxisParams::axisShortLabelsStringList() const
  Returns a \c QStringList containing the label texts to be used
  in case the axis area size is too small to show the full-size
  label texts.

  Calling \c axisLabelStringList() instead will return
  another \c QStringList containing their full-size counterparts.

  \note This is the list of texts you specified by \c setAxisValues
  or by \c setAxisShortLabelsStringList.
  The texts <b>actually</b> drawn at the axis are <b>not neccessarily</b> the
  same as the ones in this list  since (regarding Start and/or End and/or
  Delta value) they might be only a subset of this list. Whenever label texts
  are calculated automatically the resulting labels are also stored in a
  second list that you may access via \c axisLabelTexts().

  \return a \c QStringList containing the label texts to be used
  in case the axis area size is too small to show the full-size
  label texts.
  \sa axisLabelStringList
  \sa setAxisLabelStringLists
  \sa setAxisValues
  \sa axisValueStart, axisValueEnd, axisLabelTexts
  \sa axisLabelsFromDataRow, setLabelTextsFormDataRow
  */

/**
  \fn  uint  KDChartAxisParamsaxisShortLabelsStringCount() const
  Returns the number of strings stored as label texts,
  the texts can be retrieved by calling \c axisLabelStringList().

  \sa axisLabelStringCount
  \sa axisLabelStringList, axisShortLabelsStringList
  */


/**
  \fn  const QStringList*  KDChartAxisParamsaxisLabelTexts() const
  Returns a \c QStringList containing the label texts
  that are <b>actually</b> drawn at the axis.

  In case the texts are unknown returns zero.

  \note This is the list of texts <b>actually</b> drawn at the axis.
  This strings are not neccessarily the same as the ones in the list given by
  \c setAxisValues since (regarding Start and/or End and/or Delta value) it
  might be only a subset of that list. Whenever labels text are calculated
  automatically the resulting labels also stored in this list - it will
  allways be a true copy of the texts painted at the axis.

  \return a \c QStringList containing the label texts actually being used.
  \sa setAxisValues
  \sa axisValueStart, axisValueEnd, axisLabelStringList
  \sa axisLabelsFromDataRow, setLabelTextsFormDataRow
  \sa setAxisLabelStringLists, setAxisValues
  */



/**
  Specifies the label texts that are <b>actually</b> drawn
  at the axis labels.

  \note Do not call this function unless you know what you are
  doing. It is used internally whenever the texts to be drawn
  have been re-calculated or the charts parameters have changed.
  For specifying another list of strings to be used as label texts
  you might rather want to call \c setAxisLabelStringLists() or
  \c setLabelTextsFormDataRow() depending from whether your texts
  are stored in a \c QStringList or in a data row.

  \param axisLabelTexts specifies the texts that are
  <b>actually</b> drawn at the axis labels are unknown.

  \sa setAxisLabelStringLists, setLabelTextsFormDataRow, setAxisValues
  \sa setAxisFirstLabelText, setAxisLastLabelText
  */
void  KDChartAxisParams::setAxisLabelTexts( const QStringList* axisLabelTexts )
{
    _axisLabelTexts.clear();
    _axisLabelTextsDirty = ( 0 == axisLabelTexts );
    if ( !_axisLabelTextsDirty )
        _axisLabelTexts = *axisLabelTexts;
}

/**
  \fn  void  KDChartAxisParamssetAxisLabelTextsDirty( bool axisLabelTextsDirty )
  Specifies whether the label texts are <b>actually</b> drawn
  at the axis labels are unknown. If \c false, they could
  successfully be retrieved by \c axisLabelTexts().

  \note Do not call this function unless you know what you are
  doing. It is used internally whenever the texts to be drawn
  have been re-calculated or the charts parameters have changed.
  For specifying another list of strings to be used as label texts
  you might rather want to call \c setAxisLabelStringLists() or
  \c setLabelTextsFormDataRow() depending from whether your texts
  are stored in a \c QStringList or in a data row.

  \param axisLabelTextsDirty specifies whether the texts are
  <b>actually</b> drawn at the axis labels are unknown.

  \sa setAxisLabelStringLists, setLabelTextsFormDataRow, setAxisValues
  */

/**
  \fn  bool  KDChartAxisParams::axisLabelTextsDirty() const
  Returns whether the label texts that are <b>actually</b> drawn
  at the axis labels are unknown. If \c false, they could
  successfully be retrieved by \c axisLabelTexts().

  \return whether the texts that are <b>actually</b> drawn at
  the axis labels are unknown.

  \sa setAxisLabelStringLists, setLabelTextsFormDataRow, setAxisValues
  */


/**
  Sets a special text that is to be displayed _instead_of_
  the first axis label IF the parameter is not a NULL string.

  \note This function does not affect axis label and axis range
  calculation but it replaces the first label after all calculation
  is done.  This may be used in case you want to replace the first
  label by some special text string, e.g. you might want to display
  the text "origo" instead of the start value.

  To remove a first label string that was set by a previous call of this
  function just call it again, with no parameter.

  \param axisFirstLabelText specifies the text that is
  <b>actually</b> drawn as the first label: a NULL string
  ( as produced by QString() ) will be ignored, to suppress
  the first label please specify an EMPTY but NOT NULL string
  by passing "" as parameter.


  \sa setAxisLastLabelText
  \sa setAxisLabelStringLists, setLabelTextsFormDataRow, setAxisValues
  */
void  KDChartAxisParams::setAxisFirstLabelText( const QString& axisFirstLabelText )
{
    _axisFirstLabelText = axisFirstLabelText;
}

/**
  Sets a special text that is to be displayed _instead_of_
  the last axis label IF the parameter is not a NULL string.

  \note This function does not affect axis label and axis range
  calculation but it replaces the last label after all calculation
  is done.  This may be used in case you want to replace the last
  label by some special text string, e.g. you might want to display
  the text "maximum" instead of the end value.

  To remove a last label string that was set by a previous call of this
  function just call it again, with no parameter.

  \param axisFirstLabelText specifies the text that is
  <b>actually</b> drawn as the last label: a NULL string
  ( as produced by QString() ) will be ignored, to suppress
  the first label please specify an EMPTY but NOT NULL string
  by passing "" as parameter.

  \sa setAxisFirstLabelText
  \sa setAxisLabelStringLists, setLabelTextsFormDataRow, setAxisValues
  */
void  KDChartAxisParams::setAxisLastLabelText( const QString& axisLastLabelText )
{
    _axisLastLabelText = axisLastLabelText;
}



/**
  \fn  void  KDChartAxisParams::setAxisSteadyValueCalc( bool axisSteadyValueCalc )
  Specifies whether label values shall be calculated based upon the
  associated dataset values (normally this is true for ordinate axes)
  or based upon some string list (as you might expect it for abscissa
  labels).

  \sa setAxisValues
  */

/**
  \fn  bool  KDChartAxisParams::axisSteadyValueCalc() const
  Returns whether label values shall be calculataed based upon the associated
  dataset values (normally this is true for ordinate axes) or based upon some
  string list (as you might expect it for abscissa labels).

  \sa setAxisValues
  */

/**
  \fn   KDChartAxisParams::KDChartAxisParams( const KDChartAxisParams& R ) : QObject()
  Copy-constructor: By calling the copy method,
  see also the assignment operator.
  */


/**
  Assignment operator: By calling the copy method,
  see also the copy constructor.
  */

KDChartAxisParams& KDChartAxisParams::operator=( const KDChartAxisParams& R )
{
    if ( this != &R )
        deepCopy( *this, R );
    return *this;
}

void KDChartAxisParams::deepCopy( KDChartAxisParams& D, const KDChartAxisParams& R )
{
    D._axisType = R._axisType;
    D._axisVisible = R._axisVisible;
    D._axisAreaMode = R._axisAreaMode;
    D._axisUseAvailableSpaceFrom = R._axisUseAvailableSpaceFrom;
    D._axisUseAvailableSpaceTo   = R._axisUseAvailableSpaceTo;
    D._axisAreaMin = R._axisAreaMin;
    D._axisAreaMax = R._axisAreaMax;
    D._axisCalcMode = R._axisCalcMode;
    D._axisIsoRefAxis = R._axisIsoRefAxis;
    D._axisTrueAreaSize = R._axisTrueAreaSize;
    D._axisTrueAreaRect = R._axisTrueAreaRect;
    D._axisZeroLineStartX = R._axisZeroLineStartX;
    D._axisZeroLineStartY = R._axisZeroLineStartY;
    D._axisDtLowPosX = R._axisDtLowPosX;
    D._axisDtLowPosY = R._axisDtLowPosY;
    D._axisDtHighPosX = R._axisDtHighPosX;
    D._axisDtHighPosY = R._axisDtHighPosY;
    D._axisLineVisible = R._axisLineVisible;
    D._axisLineWidth = R._axisLineWidth;
    D._axisTrueLineWidth = R._axisTrueLineWidth;
    D._axisLineColor = R._axisLineColor;
    // main grid:
    D._axisShowFractionalValuesDelimiters = R._axisShowFractionalValuesDelimiters;
    D._axisShowGrid      = R._axisShowGrid;
    D._axisGridColor     = R._axisGridColor;
    D._axisGridLineWidth = R._axisGridLineWidth;
    D._axisGridStyle     = R._axisGridStyle;
    // sub grid:
    D._axisShowSubDelimiters = R._axisShowSubDelimiters;
    D._axisGridSubColor      = R._axisGridSubColor;
    D._axisGridSubLineWidth  = R._axisGridSubLineWidth;
    D._axisGridSubStyle      = R._axisGridSubStyle;

    D._axisZeroLineColor = R._axisZeroLineColor;
    D._axisLabelsVisible = R._axisLabelsVisible;
    D._axisLabelsFont = R._axisLabelsFont;
    D._axisLabelsFontUseRelSize = R._axisLabelsFontUseRelSize;
    D._axisLabelsDontShrinkFont = R._axisLabelsDontShrinkFont;
    D._axisLabelsDontAutoRotate = R._axisLabelsDontAutoRotate;
    D._axisLabelsRotation = R._axisLabelsRotation;
    D._axisLabelsFontRelSize = R._axisLabelsFontRelSize;
    D._axisLabelsFontMinSize = R._axisLabelsFontMinSize;
    D._axisLabelsColor = R._axisLabelsColor;

    D._axisSteadyValueCalc   = R._axisSteadyValueCalc;
    D._axisValueStartIsExact = R._axisValueStartIsExact;
    D._axisValueStart        = R._axisValueStart;
    D._axisValueEnd          = R._axisValueEnd;
    D._axisValueDelta        = R._axisValueDelta;
    D._axisValueDeltaScale   = R._axisValueDeltaScale;
    D._axisValueLeaveOut     = R._axisValueLeaveOut;
    D._axisValuesDecreasing  = R._axisValuesDecreasing;
    D._axisDigitsBehindComma = R._axisDigitsBehindComma;
    D._axisLabelsDateTimeFormat = R._axisLabelsDateTimeFormat;
    D._axisMaxEmptyInnerSpan = R._axisMaxEmptyInnerSpan;
    D._takeLabelsFromDataRow = R._takeLabelsFromDataRow;
    D._labelTextsDataRow     = R._labelTextsDataRow;
    D._axisLabelStringList   = R._axisLabelStringList;
    D._axisShortLabelsStringList = R._axisShortLabelsStringList;
    D._axisLabelTextsDirty   = R._axisLabelTextsDirty;

    D._axisLabelsDivPow10      = R._axisLabelsDivPow10;
    D._axisLabelsDecimalPoint  = R._axisLabelsDecimalPoint;
    D._axisLabelsThousandsPoint= R._axisLabelsThousandsPoint;
    D._axisLabelsPrefix        = R._axisLabelsPrefix;
    D._axisLabelsPostfix       = R._axisLabelsPostfix;
    D._axisLabelsTotalLen      = R._axisLabelsTotalLen;
    D._axisLabelsPadFill       = R._axisLabelsPadFill;
    D._axisLabelsBlockAlign    = R._axisLabelsBlockAlign;

    D._axisFirstLabelText = R._axisFirstLabelText;
    D._axisLastLabelText = R._axisLastLabelText;

    D._axisLabelTexts = R._axisLabelTexts;
    D._trueAxisDeltaPixels = R._trueAxisDeltaPixels;
    D._trueHigh = R._trueHigh;
    D._trueLow = R._trueLow;
    D._trueDelta = R._trueDelta;
    D._trueDtLow = R._trueDtLow;
    D._trueDtHigh = R._trueDtHigh;
    D._trueDtDeltaScale = R._trueDtDeltaScale;
}


/**
  Converts the specified axis type enum to a string representation.

  \param type the axis type enum to convert
  \return the string representation of the axis type enum
  */
QString  KDChartAxisParams::axisTypeToString( AxisType type ) {
    switch( type ) {
        case AxisTypeUnknown:
            return "Unknown";
        case AxisTypeEAST:
            return "East";
        case AxisTypeNORTH:
            return "North";
        case AxisUP:
            return "Up";
        default: // should not happen
            qDebug( "Unknown axis type" );
            return "Unknown";
    }
}

/**
  Converts the specified string to an axis type enum value.

  \param string the string to convert
  \return the axis type enum value
  */
KDChartAxisParams::AxisType KDChartAxisParams::stringToAxisType( const QString& type ) {
    if( type == "Unknown" )
        return AxisTypeUnknown;
    else if( type == "East" )
        return AxisTypeEAST;
    else if( type == "North" )
        return AxisTypeNORTH;
    else if( type == "Up" )
        return AxisUP;
    else // should not happen
        return AxisTypeUnknown;
}



/**
  \fn   void  KDChartAxisParams::changed()
  \c Signals:
  This signal is emitted when any of the chart axis
  parameters have changed.
  */

/**
  \var AxisType _axisType
  \c private:

  Specifies the axis type.

  \sa setAxisType
  */

/**
  \var bool  _axisVisible
  \c private:
  Specifies whether this axis is to be drawn. False by default.

  \sa setAxisVisible
  */


/**
  \var bool  _axisLabelsTouchEdges
  \private:
  Specifies whether the axis labels start and end at the
  edges of the charts instead being positioned in the
  middle of the first data point (or the last one, resp.)

  \sa axisLabelsTouchEdges
  */


/**
  \var  AxisAreaMode _axisAreaMode
  \private:
  Specifies how to find out the size of the area to be
  used by this axis.

  \sa setAxisAreaMode, setAxisAreaMin, setAxisAreaMax, setAxisArea
  */

/**
  \var int _axisUseAvailableSpaceFrom
  \private:
  Specifies the beginning offset of the space used by this axis
  in comparison to the space that could be used by this axis.

  \sa setAxisUseAvailableSpace
  */

/**
  \var int _axisUseAvailableSpaceTo
  \private:
  Specifies the ending offset of the space used by this axis
  in comparison to the space that could be used by this axis.

  \sa setAxisUseAvailableSpace
  */

/**
  \var int _axisAreaMin
  Specifies the minimum axis area width (or height, resp.).
  \sa setAxisAreaMin, setAxisAreaMode, setAxisAreaMax, setAxisArea
  */

/**
  \var  int _axisAreaMax
  Specifies the maximum axis area width (or height, resp.).

  \sa setAxisAreaMax, setAxisAreaMode, setAxisAreaMin, setAxisArea
  */

/**
  \var AxisCalcMode _axisCalcMode
  Specifies the axis calculation mode.

  \sa setAxisCalcMode
  */
/**
  \var  uint _axisIsoRefAxis
  Specifies which axis this axis shall be isometric with.

  \sa setIsometricReferenceAxis
  */

/**
  \var int _axisTrueAreaSize
  Specifies the axis area width (or height, resp.)
  as it was calculated and drawn.

  \sa setAxisAreaMax, setAxisAreaMode, setAxisAreaMin, setAxisArea
  */

/**
  \var  QRect _axisTrueAreaRect
  Specifies the true axis area rectangle
  as it was calculated and drawn.

  \sa setAxisAreaMax, setAxisAreaMin, setAxisArea
  */

/**
  \var  bool _axisShowSubDelimiters
  Specifies whether the axis sub-delimiters will be drawn.

  \sa setAxisShowSubDelimiters
  */
/**
  \var  bool _axisLineVisible
  Specifies whether the axis line is visible or not.

  \sa setAxisLineVisible
  */

/**
  \var  int _axisLineWidth
  Specifies the axis line width.

  \sa setAxisLineWidth
  */
/**
  \var int _axisTrueLineWidth
  Specifies the actual axis line width, as calculated and drawn.

  \sa setAxisTrueLineWidth
  */

/**
  \var QColor _axisLineColor
  Specifies the axis line colour.

  \sa setAxisLineColor
  */

/**
  \var  bool _axisShowGrid
  Specifies whether a grid will be drawn at the chart data area.

  \sa setAxisShowGrid
  */

/**
  \var  QColor _axisGridColor
  Specifies the axis grid colour.

  \sa setAxisGridColor, setAxisShowGrid
  */


/**
  \var  int _axisGridLineWidth
  Specifies the width of the axis grid lines.

  \sa setAxisGridLineWidth
  */

/**
  \var  QColor _axisGridSubColor
  Specifies the axis grid sub colour.

  \sa setAxisGridSubColor, setAxisShowGrid, setAxisShowSubDelimiters
  */


/**
  \var  int _axisGridSubLineWidth
  Specifies the width of the axis grid sub lines.

  \sa setAxisGridSubLineWidth, setAxisShowGrid, setAxisShowSubDelimiters
  */

/**
  \var  PenStyle _axisGridStyle
  Specifies the axis grid line pattern for main grid lines.

  \sa setAxisGridStyle, setAxisShowGrid
  */

/**
  \var PenStyle _axisGridSubStyle
  Specifies the axis grid line pattern for sub-delimiter grid lines.

  \sa setAxisGridSubStyle, setAxisGridStyle, setAxisShowGrid
  */

/**
  \var QColor _axisZeroLineColor
  Specifies the zero-line colour.

  \sa setAxisZeroLineColor
  */

/**
  \var bool _axisLabelsVisible
  Specifies whether the axis' labels are visible or not.

  \sa setAxisLabelsVisible
  */

/**
  \var QFont _axisLabelsFont
  Specifies the axis labels font.

  \sa setAxisLabelsFont
  \sa setAxisLabelsFontUseRelSize, setAxisLabelsFontRelSize
  */

/**
  \var bool _axisLabelsFontUseRelSize
  Specifies whether the size of the label font is to be calculated
  on a relative basis.

  \sa setAxisLabelsFontUseRelSize, setAxisLabelsFontRelSize
  \sa setAxisLabelsFont
  */

/**
  \var  int _axisLabelsFontRelSize
  Specifies the per mille basis for calculating the relative
  axis labels font size.

  \sa setAxisLabelsFontRelSize, setAxisLabelsFontUseRelSize
  \sa setAxisLabelsFont
  */

/**
  \var QColor _axisLabelsColor
  Specifies the axis labels color.

  \sa setAxisLabelsColor
  */

/**
  \var  bool _axisSteadyValueCalc
  Specifies whether label values shall be calculataed based upon the associated dataset values (normally this is true for ordinate axes) or based upon some string list (as you might expect it for abscissa labels).

  \sa setAxisValues
  */

/**
  \var  KDChartData _axisValueStart;
  Specifies the lower limit for the axis labels: the start value.

  \sa setAxisValues
  */

/**
  \var  bool _axisValueStartIsExact;
  Specifies whether the lower limit for the axis labels is
  to be used the start value even if this might not look good.

  \sa setAxisValueStartIsExact
  */

/**
  \var KDChartData _axisValueEnd
  Specifies the higher limit for the axis labels: the end value.

  \sa setAxisValues
  */

/**
  \var double _axisValueDelta
  Stores the DELTA value for the axis labels: the distance
  between two labels.

  \sa setAxisValues
  */

/**
  \var ValueScale _axisValueDeltaScale
  Stores the scaling mode for axis labels: either just numbers
  or a specified time scale (e.g. milliseconds or hours or months...)
  */

/**
  \var  int _axisValueLeaveOut
  Stores how many axis labels are to be skipped after drawing one.
  This is usefull in case you have lots of entries in one dataset.

  \sa setAxisValues
  */

/**
  \var  bool _axisValuesDecreasing;
  Stores whether the axis labels are printed in reverted order.

  \sa setAxisValuesDecreasing
  */

/**
  \var double _trueAxisDeltaPixels
  Stores the <b>true</b> delimiter delta pixel value of the axis labels
  as is was calculated and set by \c setTrueAxisDeltaPixels.

  \sa setAxisValues
  \sa trueAxisDeltaPixels
  */

/**
  \var  double _trueLow
  Specifies the <b>true</b> start value of the axis labels
  as is was calculated and set by \c setTrueAxisLowHighDelta.

  \sa setAxisValues
  \sa trueAxisLow, trueAxisHigh, trueAxisDelta
  */

/**
  \var double _trueHigh
  Specifies the <b>true</b> end value of the axis labels
  as is was calculated and set by \c setTrueAxisLowHighDelta.

  \sa setAxisValues
  \sa trueAxisLow, trueAxisHigh, trueAxisDelta
  */

/**
  \var  double _trueDelta
  Specifies the <b>true</b> delta value of the axis labels
  as is was calculated and set by \c setTrueAxisLowHighDelta.

  \sa setAxisValues
  \sa trueAxisLow, trueAxisHigh, trueAxisDelta
  */
/**
  \var double _axisZeroLineStartX
  Specifies the not-rounded screen x-position where drawing of
  this axis zero line started.
  */

/**
  \var double _axisZeroLineStartY
  Specifies the not-rounded screen y-position where drawing of
  this axis zero line started.
  */

/**
  \var int _axisDigitsBehindComma
  Specifies the number of digits to be printed behind the comma
  on the axis labels.

  \sa setAxisValues
  */

/**
  \var  int _axisMaxEmptyInnerSpan
  Specifies the percentage of the y-axis range that may to contain NO
  data entries, if - and only if - axisValueStart (or axisValueEnd,
  resp.) is set to KDCHART_AXIS_LABELS_AUTO_LIMIT.

  \sa setAxisValues
  */
/**
  \var LabelsFromDataRow _takeLabelsFromDataRow
  Specifies whether the axis labels shall be taken directly
  from the entries of a data row.

  \sa setAxisValues
  */


/**
  \var int _labelTextsDataRow
  Specifies a data row which the axis labels shall be taken from.

  \sa setAxisValues
  */

/**
  \var  QStringList _axisLabelStringList
  Specifies a QStringList containing the label texts to be used.

  \sa _axisShortLabelsStringList
  \sa setAxisValues, _axisLabelTexts, _axisLabelTextsDirty
  */


/**
  \var  QStringList _axisShortLabelsStringList
  Specifies a QStringList containing the label texts to be used
  in case the axis area is not wide enough to show their full-size
  counterparts.

  \sa _axisLabelStringList
  \sa setAxisValues, _axisLabelTexts, _axisLabelTextsDirty
  */

/**
  \var QStringList _axisLabelTexts
  Contains the label texts <b>actually</b> being used.

  \sa setAxisValues, _axisLabelStringList, _axisLabelTextsDirty
  */

/**
  \var bool _axisLabelTextsDirty
  Specifies whether the QStringList _axisLabelTexts content is invalid.

  \sa setAxisValues, _axisLabelTexts
  */
