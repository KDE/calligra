/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KDCHARTPARAMS_H__
#define __KDCHARTPARAMS_H__

#include <qapplication.h>
#include <qfont.h>
#include <qcolor.h>
#include <qmap.h>
#include <qobject.h>
#include <qtextstream.h>
#include <qdom.h>

#if defined( SUN7 )
#include <math.h>
#else
#include <cmath>
#endif
#ifdef __WINDOWS__
#define M_PI 3.14159265358979323846
#endif

#include "KDChartData.h"
#include "KDChartAxisParams.h"

/** \file KDChartParams.h
    \brief Header for all common chart parameters.

    This file is used to access all chart parameters except of the
    axis settings which you will find in \c KDChartAxisParams.h

    \see KDChartAxisParams.h
*/

/**
   Bundles all parameters of a chart including the type except the
   actual data displayed. Serializing an object of this type plus the
   data displayed is enough to be able to recreate the chart later.
*/
class KDChartParams : public QObject
{
    Q_OBJECT

public:
    //// GENERAL

    /**
       Our charts will have up to 12 axes:
       up to two axes on both sides for X, Y
       and Z (if data is 3-dimensional)
       KHZ: This isn't in KDChartParamsAxis because KDChartParamsAxis deals
       only with settings of *one* axis
    */
    static const uint KDCHART_MAX_AXES;

    static const uint KDCHART_ALL_AXES;
    static const uint KDCHART_NO_AXIS;
    static const uint KDCHART_ALL_DATASETS;
    static const uint KDCHART_NO_DATASET;
    static const uint KDCHART_UNKNOWN_CHART;
    static const uint KDCHART_ALL_CHARTS;
    static const uint KDCHART_NO_CHART;


    /** \def KDCHART_DEFAULT_DATETIME_FORMAT
    \brief The default datetime format is used by methods printing a date
    and a time when no user defined format was given.

    Time and Date formating is controlled by specifying
    a QString containing one or more time (or date, resp.) placeholder
    tags and any number of other text.
    Each placeholder tag consists of a leading percentage sign and a
    trailing alphabetic case-signficant character.


    <b>Time format tags:<b>
    \li \%H - The hour according to a 24-hour clock, using two digits (00 to 23).
    \li \%k - The hour according to a 24-hour clock, using one or two digits (0 to 23).
    \li \%I - (an UPPERcase i) - The hour according to a 12-hour clock, using two digits (01 to 12).
    \li \%l - (a LOWERcase L) - The hour according to a 12-hour clock, using one or two digits (1 to 12).
    \li \%M - The minute using two digits (00 to 59).
    \li \%S - The second using two digits (00 to 59).
    \li \%p - Either "am" or "pm" depending on the hour. Useful with %I or %l.

    <b>Date format tags:</b>
    \li \%Y - The year, using 4 digits.
    \li \%y - The year, using 2 digits.
    \li \%m - The month, using 2 digits (01 to 12).
    \li \%n - The month, using 1 or 2 digits (1 to 12).
    \li \%B - The name of the month.
    \li \%b - The abbreviated name of the month.
    \li \%d - The day, using 2 digits (01 to 31).
    \li \%e - The day, using 1 or 2 digits (1 to 31).
    \li \%A - The name of the weekday.
    \li \%a - The abbreviated name of the weekday.

    \sa KDCHART_DEFAULT_DATE_FORMAT, KDCHART_DEFAULT_TIME_FORMAT
    \sa <br>KDChartParams::setGanttChartPrintStartValues
    \sa KDChartParams::setGanttChartPrintEndValues
    \sa KDChartParams::setGanttChartPrintDurations
    */
    #define KDCHART_DEFAULT_DATETIME_FORMAT "%e. %b %Y, %H:%M:%S"
    /** \def KDCHART_DEFAULT_DATE_FORMAT
    \brief The default date format is used by methods printing a date
    when no user defined date format was given.

    Explanation of date and time formating, see \c KDCHART_DEFAULT_DATETIME_FORMAT

    \sa KDCHART_DEFAULT_DATETIME_FORMAT
    */
    #define KDCHART_DEFAULT_DATE_FORMAT "%e. %b %Y"
    /** \def KDCHART_DEFAULT_TIME_FORMAT
    \brief The default time format is used by methods printing a time when
    no user defined date format was given.

    Explanation of date and time formating, see \c KDCHART_DEFAULT_DATETIME_FORMAT

    \sa KDCHART_DEFAULT_DATETIME_FORMAT
    */
    #define KDCHART_DEFAULT_TIME_FORMAT "%H:%M:%S"


    /**
        Our charts may have up to 4 ordinate axes:
        2 left ones and 2 right ones
    */
    #define KDCHART_CNT_ORDINATES 4


    /**
       The chart type. Covers only built-in chart types.

       \sa setChartType, chartType
       \sa setAdditionalChartType, additionalChartType,
       \sa setChartSourceMode
    */
    enum ChartType { NoType, Bar, Line, Area, Pie, HiLo, Gantt, Ring };

    /**
       Specifies the chart type. The default is bar charts (Bar).

       \note Allways call this <b>before</b> specifying other
       axis properties and before calling \c setAdditionalChartType().<br>
       \c setChartType() deactivates the right axis and also sets the
       axisLabelsTouchEdges back to their default for all bottom and
       top axes.


       \param chartType the chart type to use
       \sa chartType, ChartType
       \sa setAdditionalChartType, additionalChartType,
       \sa setBarChartSubType, barChartSubType
       \sa setLineChartSubType, lineChartSubType
       \sa setChartSourceMode, chartSourceMode
       \sa setAxisLabelsTouchEdges
    */
    void setChartType( ChartType chartType )
    {
        _chartType = chartType;

        // de-activate 2nd axis that might have been set automatically
        // by a previous call of \c setAdditionalChartType()
        setAxisVisible( KDChartAxisParams::AxisPosRight, false );

        // Make sure abscissa axes start their labeling at the very
        // first position and end at the last position when drawing
        // area charts.
        // Bar charts and line charts look better with their abscissa labels
        // in the respective middle positions below each bar (or point, resp.)
        bool bAbscissaAxisLabelsTouchEdges = ( Area == chartType );
        setAxisLabelsTouchEdges( KDChartAxisParams::AxisPosBottom,
                                 bAbscissaAxisLabelsTouchEdges );
        setAxisLabelsTouchEdges( KDChartAxisParams::AxisPosTop,
                                 bAbscissaAxisLabelsTouchEdges );
        setAxisLabelsTouchEdges( KDChartAxisParams::AxisPosBottom2,
                                 bAbscissaAxisLabelsTouchEdges );
        setAxisLabelsTouchEdges( KDChartAxisParams::AxisPosTop2,
                                 bAbscissaAxisLabelsTouchEdges );
        emit changed();
    }

    /**
       Returns the chart type configured in this params object.

       \return the chart type configured in this params object.
       \sa setChartType, ChartType
       \sa setAdditionalChartType, additionalChartType,
       \sa setBarChartSubType, barChartSubType
       \sa setLineChartSubType, lineChartSubType
       \sa setChartSourceMode, chartSourceMode
    */
    ChartType chartType() const
    {
        return _chartType;
    }


    /**
    Converts the specified chart type enum to a string representation.

    \param type the type enum to convert
    \return the string representation of the type enum
    */
    static QString chartTypeToString( ChartType type ) {
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
        case Gantt:
            return "Gantt";
        case Ring:
            return "Ring";
        default: // should not happen
            return "NoType";
        }
    }


    /**
    Converts the specified string to a chart type enum value.

    \param string the string to convert
    \return the chart type enum value
    */
    static ChartType stringToChartType( const QString& string ) {
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
        else if( string == "Gantt" )
            return Gantt;
        else if( string == "Ring" )
            return Ring;
        else // default, should not happen
            return NoType;
    }

    /**
       Specifies the additional chart type.
       The default is no additional chart (NoType).
       The additional chart will have <b>no grid</b> so make sure you
       specify the chart that should have a grid via \c setChartType() and
       the other one as additional chart.
       At the moment only combinations of bar charts
       and line charts are supported so the following combinations
       are possible:<br>

       <tt>setChartType( KDChartParams::Bar );<br>
       setAdditionalChartType( KDChartParams::Line );</tt><br>
       or<br>
       <tt>setChartType( KDChartParams::Line );<br>
       setAdditionalChartType( KDChartParams::Bar );</tt>

       \note When defining multiple charts it is mandatory to
       call \c setChartSourceMode() for specifying the dataset(s)
       and their respective charts after calling \c setAdditionalChartType().

       \param chartType the additional chart type to use

       \sa additionalChartType, setChartSourceMode, chartSourceMode
       \sa <br>chartType, ChartType
       \sa setBarChartSubType, barChartSubType
       \sa setLineChartSubType, lineChartSubType
    */
    void setAdditionalChartType( ChartType chartType )
    {
        _additionalChartType = chartType;
        if ( KDChartParams::NoType == chartType ) {
            setAxisVisible( KDChartAxisParams::AxisPosRight, false );
            setAxisDatasets( KDChartAxisParams::AxisPosRight,
                             KDCHART_NO_DATASET );
        } else {
            setAxisVisible( KDChartAxisParams::AxisPosRight, true );
            setAxisDatasets( KDChartAxisParams::AxisPosRight,
                             KDCHART_ALL_DATASETS,
                             KDCHART_ALL_DATASETS,
                             1 );
        }
        emit changed();
    }

    /**
       Returns the additional chart type configured in this params object.

       \return the additional chart type configured in this params object.
       \sa setAdditionalChartType, setChartSourceMode, chartSourceMode
       \sa <br>setChartType, ChartType
       \sa setBarChartSubType, barChartSubType
       \sa setLineChartSubType, lineChartSubType
    */
    ChartType additionalChartType() const
    {
        return _additionalChartType;
    }


    /**
       The chart source mode.
       Use \c setChartSourceMode to specify if and how any data
       should be taken into account for a given chart.

       \sa setChartSourceMode, chartSourceMode, setAdditionalChartType
    */
    enum SourceMode { UnknownMode, DontUse, DataEntry
                      , AxisLabel, LegendText };

    /**
       Specifies the chart source mode of one or more datasets.
       Use this to specify if and how any data should be taken
       into account for a given chart. You may call this function
       multiple times to specify how the different datasets are to
       be used. Number of datasets is only limited by the data that
       you actually provide to the chart widget. Usage of this method
       is mandatory when specifying combinations of more than one
       chart (e.g. via \c setAdditionalChartType() ).

       \note If specifying <b>more than one dataset</b> for the very
       same chart and the same source mode make sure they form
       a contiguous <b>series</b>.<br>
       It is not possible to use dataset 0..3 as DataEntry, dataset
       4 as AxisLabel and again dataset 5..6 as DataEntry for the
       very same chart!<br>
       Of course you could specify 0..3 as DataEntry for chart 0
       while 5..6 would contain the data entries for chart 1
       when specifying a widget showing two charts.

       <b>Also note:</b><br>
       Whenever you use \c setChartSourceMode() make sure not to
       forget any of the datasets which you want to define: not
       specifying a dataset would result in it being ignored.<br>
       So the rule is: either don't call setChartSourceMode() at all
       or call it for each dataset!

       To quickly clear all such settings just might want to call
       <b>setChartSourceMode( UnknownMode, KDCHART_ALL_DATASETS );</b>
       without specifying a dataset number and without specifying
       a chart number.<br>
       However this KDCHART_ALL_DATASETS may only be used to
       <b>clear</b> SourceMode settings - but not for defining the
       SourceMode for all datasets - the above mentioned combination
       with <b>UnknownMode</b> is the only way how to use
       <b>KDCHART_ALL_DATASETS</b> here!

       <b>Examples:</b>

       \verbatim
       setChartSourceMode( DataEntry, 2 );
       \endverbatim
       This would specify a simple chart obtaining its values from dataset 2.

       \verbatim
       setChartSourceMode( DataEntry, 0, 0, 0 );
       setChartSourceMode( DataEntry, 3, 3, 1 );
       \endverbatim
       This would specify a two chart widget - the 1st obtaining its values
       from dataset 0 and the 2nd taking the values from dataset 3.

       \verbatim
       setChartSourceMode( LegendText, 1,  1, 0 );
       setChartSourceMode( AxisLabel,  2,  2, 0 );
       setChartSourceMode( DataEntry,  3,  6, 0 );
       setChartSourceMode( LegendText, 7,  7, 1 );
       setChartSourceMode( AxisLabel,  8,  8, 1 );
       setChartSourceMode( DataEntry,  9, 12, 1 );
       \endverbatim
       This would specify a two chart widget - the 1st obtaining its values
       from datasets 3..6 and the 2nd taking the values from datasets 9..12.
       Their respective legends and axes would take their values from
       datasets 1 and 2 (or 7 and 8, resp.).

       <P>

       \param mode the way how to use information stored in this dataset.
       \param dataset the dataset number to be used (or ignored, resp.).
       Use \c KDCHART_ALL_DATASETS to indicate that information given
       applies to all of the datasets.
       \param dataset2 a second dataset number to be taken into account
       (or ignored, resp.). Use this one together with the \dataset
       param to specify a range of datasets.
       \param chart the chart taking into account this dataset.
       Use this parameter only if more than one chart is to be displayed.
       By using KDCHART_ALL_CHARTS here you may specify that the dataset
       (or the range of datasets, resp.) is to be taken into account for
       all of the charts displayed.

       \sa SourceMode, chartSourceMode, findDataset
       \sa maxDatasetSourceMode, setAdditionalChartType
    */
    void setChartSourceMode( SourceMode mode,
                             uint dataset,
                             uint dataset2 = KDCHART_NO_DATASET,
                             uint chart = 0 );

    /**
    Converts the specified string to a chart source mode enum value.

    \param string the string to convert
    \return the chart source mode enum value
    */
    static SourceMode stringToChartSourceMode( const QString& string ) {
        if( string == "UnknownMode" )
            return UnknownMode;
        else if( string == "DontUse" )
            return DontUse;
        else if( string == "DataEntry" )
            return DataEntry;
        else if( string == "AxisLabel" )
            return AxisLabel;
        else if( string == "LegendText" )
            return LegendText;
        else // should not happen
            return UnknownMode;
    }


    /**
       Retrieves whether setChartSourceMode() was called or not
    */
    bool neverUsedSetChartSourceMode() const
    {
        return !_setChartSourceModeWasUsed;
    }

    /**
       Retrieves usage information of a given dataset or a range of
       datasets.

       \note If you specify a range of datasets and not all of them
       share the same SourceMode the return value will be \c UnknownMode
       whereas in case of the dataset(s) not being used it will be \c DontUse.
       Accordingly if you provide a chart pointer and not all the
       datasets share the same chart the chart pointer will receive
       the value KDCHART_UNKNOWN_CHART.<br>

       <b>Also note:</b>
       If \c setChartSourceMode() was not used at all
       chartSourceMode() will return <b>DataEntry</b> no matter what
       dataset number is specified.

       <p>

       \param dataset the dataset number to be looked at.
       Use \c KDCHART_ALL_DATASETS to find out if all of the datasets share
       the very same SourceMode.
       \param dataset2 a second dataset number.
       Use this one together with the \dataset param to look at a range of
       datasets and find out if all of them share the very same SourceMode.
       \param chart If this parameter points to an int the method will
       provide you with the chart number that this dataset (or this range
       of datasets, resp.) is taken into account for.

       \sa SourceMode, setChartSourceMode, setAdditionalChartType
       \sa maxDatasetSourceMode, findDataset
    */
    SourceMode chartSourceMode( uint dataset,
                                uint dataset2 = KDCHART_NO_DATASET,
                                uint* chart = 0 ) const;

    /**
       Retrieves the dataset number or a range of datasets
       being used with a given SourceMode for a given chart
       or KDCHART_NO_DATASET if no dataset was specified for
       that mode and that chart.

       \note If \c setChartSourceMode() was not used at all
       findDataset() will return true and set the dataset
       to KDCHART_ALL_DATASETS when called for SourceMode
       <b>DataEntry</b>. All other SourceModes will return false then.

       \return True if at least one dataset was found.

       \param mode the SourceMode to search for.
       \param dataset the dataset number found (or the first
       dataset number in case a series of datasets was found, resp.)
       \param dataset2 the dataset number found (or the last
       dataset number in case a series of datasets was found, resp.)
       <b>Hint:</b> You may give the same variable both for
       dataset and for dataset2, e.g. this makes sense in case you
       are sure that findDataset will find only one dataset anyway.
       \param chart the chart number that the dataset (or this range
       of datasets, resp.) must have been defined for. If this is missing
       findDataset assumes you are looking for datasets of the first chart.

       \sa SourceMode, setChartSourceMode, chartSourceMode
       \sa maxDatasetSourceMode, setAdditionalChartType
    */
    bool findDataset( SourceMode mode,
                      uint& dataset,
                      uint& dataset2,
                      uint chart = 0 ) const;
    /**
       Returns the number of the highest dataset for which a SourceMode has been
       defined.

       \return the number of the highest dataset for which a SourceMode has been
       defined.
       \sa setChartSourceMode, chartSourceMode, findDataset
    */
    uint maxDatasetSourceMode() const
    {
        return _maxDatasetSourceMode;
    }


    /**
       Set the default axis types for all the axes the user might activate for this chart:
       \li Pie charts by default have no axes at all
       \li Bar/Line/Area charts may have up to 8 axes (up to 2 at each side of the chart)
       \li charts representing 3-dimensional data may have up to 12 axes

       \note This function also specifies the default way how to calculate
       the axis labels: abscissa starts with zero and counts by 1, ordinate
       is calculating the labels automatically based upon the values found in
       the associated dataset(s).
    */
    void setDefaultAxesTypes();

    /**
       activate the default axis types for this chart:
       \li Pie charts by default have no axes at all
       \li Bar/Line/Area by default have 2 axes: the abscissa and the ordinate
       \li charts representing 3-dimensional data by default have 3 axes
    */
    void activateDefaultAxes();


    /**
       Specifies how many of the values should be shown. -1 means all
       available values.

       \param numValues the number of values to be shown
       \sa numValues
    */
    void setNumValues( uint numValues )
    {
        _numValues = numValues;
        emit changed();
    }


    /**
       Returns how many of the values should be shown. -1 means all
       available values.

       \return the number of values to be shown
       \sa setNumValues
    */
    uint numValues() const
    {
        return _numValues;
    }


    /**
       Specifies a color for a dataset. Note that if you define a color for a
       dataset but not for a dataset with a lower number (and there is neither
       a default value), the color for that dataset with the lower number will
       be undefined. If you do not want any color, pass an invalid color
       (i.e. a default-constructed QColor object). This is only useful
       for chart types that have an outline (like bars or pies),
       because you would not see anything otherwise.<br>
       In charts that can only display one dataset (like pies), the specified
       colors are used for the different values instead.

       \param dataset the number of the dataset (or value for pies and similar
       charts) for which the color is specified
       \param color the color to use for this dataset/value
       \sa dataColor, maxDataColor
    */
    void setDataColor( uint dataset, QColor color );


    /**
       Returns the color for a dataset. If no color has been defined for this
       dataset and none for a higher dataset either, the number will be mapped
       to the range of defined colors. If no color has been defined for this
       dataset, but at least one for a higher dataset, the return value of
       this method is undefined.

       \param dataset the number of the dataset for which to return the color
       \return the color for this dataset
       \sa setDataColor, maxDataColor
    */
    QColor dataColor( uint dataset ) const;


    /**
       Specifies a factor to be used to adjust the
       built-in brightness of shadow colors in
       3-dimensional drawings like e.g. 3D Bar charts.

       \param factor a factor that is to be taken into account for internal
       calculation of shadow colors. By specifying values greater 1.0 you
       may lighten the shadows until the most intensive brightness is
       reached, while values smaller then 1.0 will darken the shadows until
       the respective colors are black.

       \sa shadowBrightnessFactor
    */
    void setShadowBrightnessFactor( double factor )
    {
        _shadowBrightnessFactor = factor;
        emit changed();
    }

    /**
       Returns a factor to be used to adjust the
       built-in brightness of shadow colors in
       3-dimensional drawings like e.g. 3D Bar charts.

       \return a factor that is to be taken into account for internal
       calculation of shadow colors.

       \sa setShadowBrightnessFactor
    */
    double shadowBrightnessFactor() const
    {
        return _shadowBrightnessFactor;
    }


    /**
       Specifies a color for the outlines of data displays. The default is
       black.

       \param color the color to use for the outlines
       \sa outlineDataColor
    */
    void setOutlineDataColor( QColor color )
    {
        _outlineDataColor = color;
        emit changed();
    }


    /**
       Returns the color for the outlines of data displays.

       \return the outline color
       \sa setOutlineDataColor
    */
    QColor outlineDataColor() const
    {
        return _outlineDataColor;
    }


    /**
       Specifies the width of the outlines of data displays. The default is 1
       pixel.

       \param width the line width to use for the outlines
       \sa outlineDataLineWidth
    */
    void setOutlineDataLineWidth( uint width )
    {
        _outlineDataLineWidth = width;
        emit changed();
    }

    /**
       Returns the width of the outlines of data displays.

       \param style the line width that is used for outlines
       \sa setOutlineDataLineWidth
    */
    uint outlineDataLineWidth() const
    {
        return _outlineDataLineWidth;
    }


    /**
       Specifies the line style of the outlines of data displays. The default
       is a solid line. Warning: On Windows 95/98, the style setting (other
       than NoPen and SolidLine) has no effect for lines with width greater
       than 1.

       \param width the line style to use for the outlines
       \sa outlineDataLineStyle
    */
    void setOutlineDataLineStyle( PenStyle style )
    {
        _outlineDataLineStyle = style;
        emit changed();
    }

    /**
       Returns the style of the outlines of data displays.

       \param style the line style that is used for outlines
       \sa setOutlineDataLineStyle
    */
    PenStyle outlineDataLineStyle() const
    {
        return _outlineDataLineStyle;
    }


    /**
       Returns the number of the highest dataset for which a color has been
       defined.

       \return the number of the highest dataset for which a color has been
       defined.
       \sa setDataColor, dataColor
    */
    uint maxDataColor() const
    {
        return _maxDatasetColor;
    }


    /**
       Specifies whether the engine should draw the sides and tops of 3D effects
       in shadowed versions of the data colors or in the data colors
       themselves. Only used with 3D effects in charts that support these.

       \param shadow true for shadowed colors, false for original colors
       \sa threeDShadowColors
    */
    void setThreeDShadowColors( bool shadow )
    {
        _threeDShadowColors = shadow;
        emit changed();
    }


    /**
       Returns whether the engine should draw the sides and tops of 3D effects in
       shadowed versions of the data colors or in the data colors
       themselves. Only used with 3D effects in charts that
       support these. The default is true.

       \return true if shadowed colors are used for 3D effects
       \sa setThreeDShadowColors
    */
    bool threeDShadowColors() const
    {
        return _threeDShadowColors;
    }


    static int roundVal( double d )
    {
        double fr;
        double i;
        fr = modf( d, &i );
        int ret = static_cast < int > ( i );
        if( 0.49999 <= fabs( fr ) )
            ret += ( 0.0 < d ) ? 1 : -1;
        return ret;
    }


    /// END GENERAL



    /// BAR CHART-SPECIFIC
    /**
       The bar subtype. Only used when chartType == Bar

       \sa setBarChartSubType, barChartSubType
    */
    enum BarChartSubType { BarNormal, BarStacked, BarPercent };

    /**
       Specifies the bar chart subtype. Only used if chartType() ==
       Bar

       \param barChartSubType the bar chart subtype
       \sa barChartSubType, BarChartSubType, setChartType, chartType
    */
    void setBarChartSubType( BarChartSubType barChartSubType )
    {
        _barChartSubType = barChartSubType;
        emit changed();
    }

    /**
       Returns the bar chart subtype. Only used if chartType() ==
       Bar.

       \return the bar chart sub type
       \sa setBarChartSubType, BarChartSubType, setChartType, chartType
    */
    BarChartSubType barChartSubType() const
    {
        return _barChartSubType;
    }


    /**
    Converts the specified bar chart subtype enum to a string representation.

    \param type the type enum to convert
    \return the string representation of the type enum
    */
    static QString barChartSubTypeToString( BarChartSubType type ) {
        switch( type ) {
        case BarNormal:
            return "BarNormal";
        case BarStacked:
            return "BarStacked";
        case BarPercent:
            return "BarPercent";
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
    static BarChartSubType stringToBarChartSubType( const QString& string ) {
        if( string == "BarNormal" )
            return BarNormal;
        else if( string == "BarStacked" )
            return BarStacked;
        else if( string == "BarPercent" )
            return BarPercent;
        else // should not happen
            return BarNormal;
    }


    /**
       Specifies whether the engine should draw the bars in 3D. Only
       used if chartType() == Bar.

       \param threeDBars true if bars should be drawn with a 3D effect
       \sa threeDBars, setThreeDBarAngle, threeDBarAngle
    */
    void setThreeDBars( bool threeDBars )
    {
        _threeDBars = threeDBars;
        emit changed();
    }

    /**
       Returns whether the engine should draw any bars in 3D. Only
       used if chartType() == Bar.

       \return true if bars should be draws with a 3D effect, false
       otherwise
       \sa setThreeDBars, threeDBarAngle, setThreeDBarAngle
    */
    bool threeDBars() const
    {
        return _threeDBars;
    }


    /**
       \obsolete
       Specifies whether the engine should draw the sides and tops of 3D bars
       in shadowed versions of the data colors or in the data colors
       themselves. Only used if chartType() == Bar and threeDBars() ==
       true. The default is true.

       This method is obsolete; use setThreeDShadowColors instead

       \param shadow true for shadowed colors, false for original colors
       \sa setThreeDShadowColors
    */
    void setThreeDBarsShadowColors( bool shadow )
    {
        _threeDShadowColors = shadow;
        emit changed();
    }


    /**
       \obsolete
       Returns whether the engine should draw the sides and tops of 3D bars in
       shadowed versions of the data colors or in the data colors
       themselves. Only used if chartType() == Bar and threeDBars() ==
       true. The default is true.

       This method is obsolete; use threeDShadowColors instead

       \return true if bars use shadowed colors for 3D effects
       \sa setThreeDBarsShadowColors
    */
    bool threeDBarsShadowColors() const
    {
        return _threeDShadowColors;
    }


    /**
       Returns the first shadow color for a dataset. This is the color that
       is used to draw the top bars with 3D effects. It is somewhat
       darker than the original data color.  If no color has been defined for this
       dataset and none for a higher dataset either, the number will be mapped
       to the range of defined colors. If no color has been defined for this
       dataset, but at least one for a higher dataset, the return value of
       this method is undefined.

       \param dataset the number of the dataset for which to return the color
       \return the color for this dataset
       \sa setDataColor, maxDataColor, dataShadow2Color
    */
    QColor dataShadow1Color( uint dataset ) const;


    /**
       Returns the second shadow color for a dataset. This is the color that
       is used to draw the sides of bars with 3D effects. It is
       darker than the original data color.  If no color has been defined for this
       dataset and none for a higher dataset either, the number will be mapped
       to the range of defined colors. If no color has been defined for this
       dataset, but at least one for a higher dataset, the return value of
       this method is undefined.

       \param dataset the number of the dataset for which to return the color
       \return the color for this dataset
       \sa setDataColor, maxDataColor, dataShadow1Color
    */
    QColor dataShadow2Color( uint dataset ) const;


    /**
       Specifies the angle used for 3D bars. Only used if chartType()
       == Bar and threeDBars() == true.

       \param angle the angle in degrees. The default (and most useful
       value) is 45. Angle can be between 0 and 90, all other values
       are ignored. Values close to 0 or close to 90 may look
       strange, depending on the resolution of the output device.
       \sa setThreeDBars, threeDBars
       \sa threeDBarAngle, cosThreeDBarAngle
       \sa setThreeDBarDepth, threeDBarDepth
    */
    void setThreeDBarAngle( uint angle )
    {
        if ( angle > 90 )  /* since angle is an uint, we do not need to
                                        test for < 0 */
            return ;
        _threeDBarAngle = angle;

        // cache the cosine of this value
        _cosThreeDBarAngle = cos( static_cast < double > ( _threeDBarAngle ) * M_PI / 180.0 );
        emit changed();
    }
    /**
       Returns the angle in degrees used for 3D bars. Only used if chartType() ==
       Bar and threeDBars() == true.

       \return the angle in degrees used for 3D bars, always between 0 and 90.
       \sa setThreeDBars, threeDBars
       \sa setThreeDBarAngle, cosThreeDBarAngle
       \sa setThreeDBarDepth, threeDBarDepth
    */
    uint threeDBarAngle() const
    {
        return _threeDBarAngle;
    }


    /**
       Returns the cosine in rad of the angle used for 3D bars. Only used if chartType() ==
       Bar and threeDBars() == true.

       \return the cosine in rad of the angle used for 3D bars, always between 0 and 90.
       \sa setThreeDBars, threeDBars
       \sa setThreeDBarAngle, threeDBarAngle
       \sa setThreeDBarDepth, threeDBarDepth
    */
    double cosThreeDBarAngle() const
    {
        return _cosThreeDBarAngle;
    }


    /**
       Specifies the depth of the 3D Effect used for 3D bars.
       Only used if chartType() == Bar and threeDBars() == true.

       \param depth the depth of the 3D Effect in relation to
       the bar width. The default (and most useful) value of
       1.0 may be used to specify bars with square basis.
       \sa threeDBarDepth
       \sa setThreeDBars,     threeDBars
       \sa setThreeDBarAngle, threeDBarAngle, cosThreeDBarAngle
    */
    void setThreeDBarDepth( double depth )
    {
        _threeDBarDepth = depth;
        emit changed();
    }


    /**
       Returns the depth of the 3D Effect used for 3D bars.
       Only used if chartType() == Bar and threeDBars() == true.

       \return the depth of the 3D Effect in relation to the bar width.
       \sa setThreeDBarDepth
       \sa setThreeDBars,     threeDBars
       \sa setThreeDBarAngle, threeDBarAngle, cosThreeDBarAngle
    */
    double threeDBarDepth() const
    {
        return _threeDBarDepth;
    }


    /**
       Specifies the number of pixels between two dataset values.

       \note Use negative values for overlaps, use \c
       setDatasetGapIsRelative to specify that the \gap
       value is a per mille value of the chart data area width.

       \param gap the number of pixels between two dataset values.
       \sa datasetGap
       \sa datasetGapIsRelative, setDatasetGapIsRelative
    */
    void setDatasetGap( int gap )
    {
        _datasetGap = gap;
        emit changed();
    }

    /**
       Returns the number of pixels between two dataset values.

       \note Negative values signify overlaps, use \c datasetGapIsRelative
       to find out if the \datasetGap value is a per mille value of the
       chart data area width.

       \return the number of pixels between two dataset values.
       \sa setDatasetGap
       \sa datasetGapIsRelative, setDatasetGapIsRelative
    */
    int datasetGap() const
    {
        return _datasetGap;
    }


    /**
       Specifies if the value set by \c setDatasetGap is a
       per mille value of the chart data area width.

       \param gapIsRelative specifies if the value set by \c setDatasetGap
       is a per mille value of the chart data area width.
       \sa datasetGapIsRelative, datasetGap, setDatasetGap
    */
    void setDatasetGapIsRelative( bool gapIsRelative )
    {
        _datasetGapIsRelative = gapIsRelative;
        emit changed();
    }

    /**
       Returns if the value set by \c setDatasetGap
       is a per mille value of the chart data area width.

       \return if the value set by \c setDatasetGap
       is a per mille value of the chart data area width.
       \sa setDatasetGap, setDatasetGapIsRelative, datasetGap, setDatasetGap
    */
    bool datasetGapIsRelative() const
    {
        return _datasetGapIsRelative;
    }


    /**
       Specifies the number of pixels between each value block.

       \note Use negative values for overlaps (which might look strange),
       use \c setValueBlockGapIsRelative to specify that the \gap
       value is a per mille value of the chart data area width.

       \param gap the number of pixels between each value block.
       \sa valueBlockGap
       \sa valueBlockGapIsRelative, setValueBlockGapIsRelative
    */
    void setValueBlockGap( int gap )
    {
        _valueBlockGap = gap;
        emit changed();
    }

    /**
       Returns the number of pixels between each value block.

       \note Negative values signify overlaps, use \c valueBlockGapIsRelative
       to find out if the \valueBlockGap value is a per mille value of the
       chart data area width.

       \return the number of pixels between each value block.
       \sa valueBlockGap
       \sa valueBlockGapIsRelative, setValueBlockGapIsRelative
    */
    int valueBlockGap() const
    {
        return _valueBlockGap;
    }


    /**
       Specifies if the value set by \c setValueBlockGap is a
       per mille value of the chart data area width.

       \param gapIsRelative specifies if the value set by \c setValueBlockGap
       is a per mille value of the chart data area width.
       \sa valueBlockGapIsRelative, valueBlockGap, setValueBlockGap
    */
    void setValueBlockGapIsRelative( bool gapIsRelative )
    {
        _valueBlockGapIsRelative = gapIsRelative;
        emit changed();
    }

    /**
       Returns if the value set by \c setValueBlockGap
       is a per mille value of the chart data area width.

       \return if the value set by \c setValueBlockGap
       is a per mille value of the chart data area width.
       \sa setValueBlockGapIsRelative, setValueBlockGap, valueBlockGap
       \sa setValueBlockGap
    */
    bool valueBlockGapIsRelative() const
    {
        return _valueBlockGapIsRelative;
    }

    /// END BAR CHART-SPECIFIC


    /// LINE/AREA CHART-SPECIFIC
    /**
       The line subtype. Only used when chartType == Line

       \sa setLineChartSubType, lineChartSubType
    */
    enum LineChartSubType { LineNormal, LineStacked, LinePercent };

    /**
       Specifies the line chart subtype. Only used if chartType() ==
       Line. The default is LineNormal.

       \param lineChartSubType the line chart subtype
       \sa lineChartSubType, LineChartSubType, setChartType, chartType
    */
    void setLineChartSubType( LineChartSubType lineChartSubType )
    {
        _lineChartSubType = lineChartSubType;
        emit changed();
    }

    /**
       Returns the line chart subtype. Only used if chartType() ==
       Line.

       \return the line chart sub type
       \sa setLineChartSubType, LineChartSubType, setChartType, chartType
    */
    LineChartSubType lineChartSubType() const
    {
        return _lineChartSubType;
    }


    /**
    Converts the specified string to a line chart subtype enum value.

    \param string the string to convert
    \return the line chart subtype enum value
    */
    static LineChartSubType stringToLineChartSubType( const QString& string ) {
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
    static QString lineChartSubTypeToString( LineChartSubType type ) {
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
       Specifies whether there should be a marker at each data
       point. Only used if chartType() == Line. The default is not to
       draw markers.

       \param marker true if markers should be drawn
    */
    void setLineMarker( bool marker )
    {
        _lineMarker = marker;
        emit changed();
    }

    /**
       Returns whether line markers should be drawn at each data
       point. Only used if chartType() == Line.

       \return true if markers should be drawn.
    */
    bool lineMarker() const
    {
        return _lineMarker;
    }

    /**
       The available line marker styles.
    */
    enum LineMarkerStyle { LineMarkerCircle, LineMarkerSquare,
                           LineMarkerDiamond };

    /**
       Specifies the line marker to be used for a dataset. Only used if
       chartType() == Line and lineMarker() == true. If you specify a
       marker for a dataset, but not for a dataset with a lower
       value, then the marker for the dataset with the lower value
       will be undefined unless it was previously defined. The default
       is a circle for the first dataset, a square for the second, a
       diamond for the third and undefined for all subsequent
       datasets.

       \param dataset the dataset for which to set the line marker
       \param style the style to set for the specified dataset
       \sa LineMarkerStyle, lineMarkerStyle
    */
    void setLineMarkerStyle( uint dataset, LineMarkerStyle style )
    {
        _lineMarkerStyles[ dataset ] = style;
        _maxDatasetLineMarkerStyle = QMAX( dataset,
                                           _maxDatasetLineMarkerStyle );
        emit changed();
    }

    /**
       Returns the marker to be used for a dataset. Only used if
       chartType() == Line and lineMarker() == true.

       \param dataset the dataset for which to return the line marker
       \return the line marker for the specified data set
       \sa LineMarkerStyle, setLineMarkerStyle
    */
    LineMarkerStyle lineMarkerStyle( uint dataset ) const
    {
        return _lineMarkerStyles[ dataset ];
    }


    /**
    Converts the specified line marker style enum to a string representation.

    \param type the type enum to convert
    \return the string representation of the type enum
    */
    static QString lineMarkerStyleToString( LineMarkerStyle style ) {
        switch( style ) {
        case LineMarkerSquare:
            return "Square";
        case LineMarkerDiamond:
            return "Diamond";
        case LineMarkerCircle:
            return "Circle";
        default: // should not happen
            qDebug( "Unknown line marker style" );
            return "Circle";
        }
    }


    /**
    Converts the specified string to a line marker style value.

    \param string the string to convert
    \return the line marker style enum value
    */
    static LineMarkerStyle stringToLineMarkerStyle( const QString& string ) {
        if( string == "Square" )
            return LineMarkerSquare;
        else if( string == "Diamond" )
            return LineMarkerDiamond;
        else if( string == "Circle" )
            return LineMarkerCircle;
        else // default, should not happen
            return LineMarkerCircle;
    }


    /**
       Returns the highest dataset for which a line marker style has been
       defined. Not all datasets with a lower number necessarily have
       a defined line marker.

       \return the highest dataset with a defined line marker
       \sa LineMarkerStyle, setLineMarkerStyle, lineMarkerStyle
    */
    uint maxDatasetLineMarkerStyle() const
    {
        return _maxDatasetLineMarkerStyle;
    }


    /**
       Specifies the sizes of line markers. Only used if chartType() == Line
       and lineMarker() == true. The default is 6x6.

       \param size the size of the line marker in pixels
       \sa lineMarkerSize
    */
    void setLineMarkerSize( QSize size )
    {
        _lineMarkerSize = size;
        emit changed();
    }

    /**
       Returns the sizes of line markers. Only used if chartType() == Line and
       lineMarker() == true.

       \return the size of the line marker in pixels
       \sa setLineMarkerSize
    */
    QSize lineMarkerSize() const
    {
        return _lineMarkerSize;
    }

    /**
       Specifies the width for lines in line charts. Default is 1.

       \param width the new width
       \sa lineWidth
    */
    void setLineWidth( uint width )
    {
        _lineWidth = width;
        emit changed();
    }


    /**
       Returns the line width of the lines in line charts.

       \return the line width of lines in line charts
       \sa lineWidth
    */
    uint lineWidth() const
    {
        return _lineWidth;
    }


    /**
       The line subtype. Only used when chartType == Area

       \sa setAreaChartSubType, areaChartSubType
    */
    enum AreaChartSubType { AreaNormal, AreaStacked, AreaPercent };


    /**
       Specifies the area chart subtype. Only used if chartType() ==
       Area. The default is AreaNormal.

       \param areaChartSubType the area chart subtype
       \sa areaChartSubType, AreaChartSubType, setChartType, chartType
    */
    void setAreaChartSubType( AreaChartSubType areaChartSubType )
    {
        _areaChartSubType = areaChartSubType;
        emit changed();
    }


    /**
       Returns the area chart subtype. Only used if chartType() ==
       Area.

       \return the area chart sub type
       \sa setAreaChartSubType, AreaChartSubType, setChartType, chartType
    */
    AreaChartSubType areaChartSubType() const
    {
        return _areaChartSubType;
    }


    /**
    Converts the specified area chart subtype enum to a string representation.

    \param type the subtype enum to convert
    \return the string representation of the type enum
    */
    static QString areaChartSubTypeToString( AreaChartSubType type ) {
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
    static AreaChartSubType stringToAreaChartSubType( const QString& string ) {
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
       Specifies whether the area above the value points or below the
       value points should be filled. The default is to fill below the
       value points.

       \sa setAreaLocation(), areaLocation()
    */
    enum AreaLocation { AreaAbove, AreaBelow };


    /**
       Specifies whether the area above or below the value points
       should be filled. The default is to fill below the value
       points.

       This setting is <em>not</em> used with percent areas
       (i.e. areaChartSubType() == AreaPercent), because these always
       cover the whole chart.

       \param location whether to fill above or below the value points
       \sa AreaLocation, areaLocation()
    */
    void setAreaLocation( AreaLocation location )
    {
        _areaLocation = location;
        emit changed();
    }

    /**
       Returns whether the area above or below the value points is
       filled.

       \return whether the area above or below the value points is
       filled.
       \sa AreaLocation, setAreaLocation()
    */
    AreaLocation areaLocation() const
    {
        return _areaLocation;
    }


    /**
    Converts the specified area location enum to a string representation.

    \param type the location enum to convert
    \return the string representation of the type enum
    */
    static QString areaLocationToString( AreaLocation type ) {
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
    static AreaLocation stringToAreaLocation( const QString& string ) {
        if( string == "Above" )
            return AreaAbove;
        else if( string == "Below" )
            return AreaBelow;
        else // default, should not happen
            return AreaBelow;
    }



    /// END LINE/AREA CHART-SPECIFIC


    /// PIE/RING CHART-SPECIFIC

    /**
       Specifies whether the pie chart or ring chart should be
       exploding (all pies or ring segments are slightly displaced
       from the center) or not. The default is not to explode. Only
       used if chartType() == Pie or chartType() == Ring.

       Note that calling this method by passing true turns on
       exploding for all segments. You can turn exploding on and off
       for individual segments by calling setExplodeValues() and also
       change the explode factors by calling setExplodeFactor() or
       setExplodeFactors().

       Note that in ring charts, only the outermost ring is
       exploded. Also note that exploding rings are only useful if
       none of the values in the outermost dataset covers more than
       1/4 of the ring, otherwise the exploded ring will look funny.

       \param explode true if the pie or ring should be exploding,
       false otherwise
       \sa exploding(), setExplodeFactor(), explodeFactor(),
       setExplodeFactors(), explodeFactors(), setExplodeValues(),
       explodeValues()
    */
    void setExplode( bool explode )
    {
        _explode = explode;
        emit changed();
    }

    /**
       Returns whether the pie or ring chart should be exploding or not

       \return true if the pie should be exploding
       \sa setExplode, setExplodeFactor(), explodeFactor(),
       setExplodeFactors(), explodeFactors(), setExplodeValues(),
       explodeValues()
    */
    bool explode() const
    {
        return _explode;
    }


    /**
    Specifies which values to explode. Explosion of values only
    happens if setExplode( true ) has been called. Normally, all
    values are exploded (all values on the outer ring in the case of a
    ring chart). With this function, you have a finer control about
    the explosion. You can specify the values that should be exploded
    by their position. Passing an empty list here turns on explosion
    for all pies or ring segments (but only if explosion is turned on
    in general).

    To repeat: Setting anything here has no effect if setExplode( true )
    is not called. You can, however, set any values here even if explosion is
    not turned on; they will be saved in case explosion will be turned on in
    the future and then be valid automatically.

    The explode factor, i.e., the amount with which a segment is moved
    from the center can either be set for all segments with
    \a setExplodeFactor() or for individual segments with
    \a setExplodeFactors(). The default is 10%.

    \param explodeList the list of positions in the displayed dataset that
    should be drawn in an exploded position. Pass an empty list here to
    explode all values.
    \sa setExplode(), explode(), setExplodeFactor(), explodeFactor(),
    explodeValues(), setExplodeFactors(), explodeFactors()
    */
    void setExplodeValues( QValueList<int> explodeList ) {
        _explodeList = explodeList;
        emit changed();
    }


    /**
    Returns which values are exploded in a pie or ring chart. For a detailed
    explanation of this feature, please see \a setExplodeValues().

    \return the list of values that are exploded
    \sa setExplode(), explode(), setExplodeFactor(), explodeFactor(),
    setExplodeValues()
    */
    QValueList<int> explodeValues() const {
        return _explodeList;
    }


    /**
    Specifies the explode factors for each segment in percent,
    i.e. how much an exploded pie or ring segment is displaced from
    the center. The factor is given as a double value between 0 and 1;
    0.1 means 10%. Only used if chartType() == Pie or chartType() ==
    Ring and explode() == true.

    Segments that are not contained in the map specified here will
    have the default explode factor of 10%, if exploding is turned on
    for them at all. This also means that passing an empty list to
    this method does not turn off exploding in general; use \a
    setExplode( false ) for that.

    Note: This method has no immediate effect if setExplode( true )
    has not been called. It is, however, possible to preset explode
    factors and then turn on exploding later.

    Note: Besides giving a segment an explode factor and turning on
    exploding in general, you also need to specify to explode a
    certain segment by calling \a setExplodeValues(). This gives maximum
    flexibility as it allows you to preset explode factors and then
    explode or not not explode a segment at leisure - at the expense
    of one more method call.

    \param factors the list of explode factors
    \sa setExplode(), explode(), setExplodeValues(), explodeValues(),
    setExplodeFactor(), explodeFactor(), explodeFactors()
    */
    void setExplodeFactors( QMap<int,double> factors ) {
        _explodeFactors = factors;
        emit changed();
    }


    /**
    Returns the list of explode factors. Each explode factor in the
    list corresponds to the segment at the same position.

    \return the list of explode factors
    \sa setExplode(), explode(), setExplodeValues(), explodeValues(),
    setExplodeFactor(), explodeFactor(), setExplodeFactors()
    */
    QMap<int,double> explodeFactors() const {
        return _explodeFactors;
    }


    /**
       Specifies the explode factor in percent, i.e. how much an
       exploded pie or ring segment is displaced from the center. The
       factor is given as a double value between 0 and 1; 0.1 means
       10% and is the default. Only used if
       chartType() == Pie or chartType() == Ring and explode() == true.

       Note that this method sets the explode factor for all segments
       that are exploded but for which no more specific explode factor
       has been set with \a setExplodeFactors(). Thus, to make the value
       specified here the default value, you can pass an empty list to
       \a setExplodeFactors() (which also is the default).

       \param factor the explode factor
       \sa setExplode(), explode(), explodeFactor(),
       setExplodeValues(), explodeValues(), setExplodeFactors(),
       explodeFactors()
    */
    void setExplodeFactor( double factor )
    {
        _explodeFactor = factor;
        emit changed();
    }

    /**
       Returns the default explode factor, i.e., the explode factor
       used for those segments which should be exploded but for which
       no more specific explode factor has been set. See \a
       setExplodeFactor() for an explanation of the value.

       \return the explode factor
       \sa setExplodeFactor(), setExplode(), explode()
    */
    double explodeFactor() const
    {
        return _explodeFactor;
    }


    /**
       Specifies whether the engine should draw the pies in 3D. Only
       used if chartType() == Pie.

       \param threeDPies true if pies should be drawn with a 3D effect
       \sa threeDPies(), setThreeDPieHeight(), threeDPieHeight()
    */
    void setThreeDPies( bool threeDPies )
    {
        _threeDPies = threeDPies;
        emit changed();
    }

    /**
       Returns whether the engine should draw any pies in 3D. Only
       used if chartType() == Pie.

       \return true if pies should be drawn with a 3D effect, false
       otherwise
       \sa setThreeDPies(), threeDPieHeight(), setThreeDPieHeight()
    */
    bool threeDPies() const
    {
        return _threeDPies;
    }


    /**
       Specifies the height of the 3D effect for pies in pixels. Only used if
       chartType() == Pie and threeDPies() == true. Negative values are
       interpreted relative to the height of the pie where -100 means
       that the 3D effect has the same height as the pie itself; values
       smaller than -25 are hardly useful. Positive values are
       interpreted as absolute sizes in pixels. The default is an
       absolute value of 20 pixels.

       \param height the height of the 3D effect in pixels
       \sa threeDHeight(), setThreeDPies(), threeDPies()
    */
    void setThreeDPieHeight( int pixels )
    {
        _threeDPieHeight = pixels;
        emit changed();
    }

    /**
       Returns the height of the 3D effect for pies in pixels. See \a
       setThreeDPieHeight for the interpretation of the value.

       \return the height of the 3D effect in pixels
       \sa setThreeDPieHeight(), setThreeDPies(), threeDPies()
    */
    int threeDPieHeight() const
    {
        return _threeDPieHeight;
    }

    /**
       Specifies the starting point of the pie circle. The default is the 3 o'
       clock position which equals to 0. Positive values are angles in degrees
       counterclockwise, negative values are angles in degrees
       clockwise. All values will be normalized to [0;360[.

       \param degrees the starting point in degrees
       \sa pieStart()
    */
    void setPieStart( int degrees )
    {
        while ( degrees < 0 )
            degrees += 360;
        while ( degrees >= 360 )
            degrees -= 360;
        _pieStart = degrees;

        emit changed();
    }


    /**
       Returns the starting point of the pie circle. See \a setPieStart() for
       the interpretation of these values. The return value is
       always in the interval [0;360[

       \return the starting point of the pie circle in degrees
       \sa setPieStart()
    */
    int pieStart() const
    {
        return _pieStart;
    }

    /**
       Specifies the starting point of the ring circle. The default is the 3 o'
       clock position which equals to 0. Positive values are angles in degrees
       counterclockwise, negative values are angles in degrees
       clockwise. All values will be normalized to [0;360[.

       \param degrees the starting point in degrees
       \sa pieStart()
    */
    void setRingStart( int degrees )
    {
        while ( degrees < 0 )
            degrees += 360;
        while ( degrees >= 360 )
            degrees -= 360;
        _ringStart = degrees;

        emit changed();
    }


    /**
       Returns the starting point of the ring circle. See \a
       setRingStart() for the interpretation of these values. The
       return value is always in the interval [0;360[

       \return the starting point of the ring circle in degrees
       \sa setRingStart()
    */
    int ringStart() const
    {
        return _ringStart;
    }


    /**
    Specifies whether the ring thickness should be relative to the sum of the
    values in the dataset that the ring represents. The default is to have all
    the rings with the same thickness which itself depends on the size of the
    chart and the number of rings.

    \param relativeThickness if true, ring thickness is relative, if false, it
    is constant.
    \sa relativeThickness()
    */
    void setRelativeRingThickness( bool relativeThickness ) {
        _relativeRingThickness = relativeThickness;

        emit changed();
    }


    /**
    Returns whether the ring thickness is relative to the sum of values in the
    dataset that the ring represents, or whether the ring thickness should be
    constant.

    \return true if thickness is relative, false if it is constant
    \sa setRelativeRingThickness()
    */
    bool relativeRingThickness() const {
        return _relativeRingThickness;
    }

    /// END PIE/RING CHART-SPECIFIC


    /// HI/LO CHART-SPECIFIC
    /**
       The HiLo subtype. Only used when chartType == HiLo

       \sa setHiLoChartSubType, hiLoChartSubType
    */
    enum HiLoChartSubType { HiLoNormal, HiLoSimple = HiLoNormal,
                            HiLoClose, HiLoOpenClose };

    /**
       Specifies the HiLo chart subtype. Only used if chartType() ==
       HiLo

       \param hiLoChartSubType the HiLo chart subtype
       \sa hiLoChartSubType, HiLoChartSubType, setChartType, chartType
    */
    void setHiLoChartSubType( HiLoChartSubType hiLoChartSubType )
    {
        _hiLoChartSubType = hiLoChartSubType;
        emit changed();
    }

    /**
       Returns the HiLo chart subtype. Only used if chartType() ==
       HiLo.

       \return the HiLo chart sub type
       \sa setHiLoChartSubType, HiLoChartSubType, setChartType, chartType
    */
    HiLoChartSubType hiLoChartSubType() const
    {
        return _hiLoChartSubType;
    }

    /**
    Converts the specified HiLo chart subtype enum to a string representation.

    \param type the subtype enum to convert
    \return the string representation of the type enum
    */
    static QString hiLoChartSubTypeToString( HiLoChartSubType type ) {
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
    static HiLoChartSubType stringToHiLoChartSubType( const QString& string ) {
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
       Specifies if and how a HiLo chart will print the Low
       values their respective entries. Only used if chartType() ==
       HiLo

       \note <b>setHiLoChartPrintLowValues( false )</b> will
       deactivate printing of Low values.

       \param active specifies whether the values are to be printed or not.
       \param font a Pointer to the font to be used.
       \param size (in per mille of the chart width) the dynamic size of
       the font to be used. If this parameter is zero the size of the
       \c font is used instead - regardless of the size of the chart!
       \param color the color to be used when printing the values.
       \sa setChartType, chartType
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setHiLoChartPrintHighValues
       \sa hiLoChartPrintLowValues, HiLoChartPrintLowValues
       \sa hiLoChartLowValuesFontColor, HiLoChartLowValuesFontColor
       \sa hiLoChartLowValuesFontUseRelSize, HiLoChartLowValuesFontUseRelSize
       \sa hiLoChartLowValuesFontRelSize, HiLoChartLowValuesFontRelSize
       \sa hiLoChartPrintHighValues, HiLoChartPrintHighValues
       \sa hiLoChartHighValuesFontColor, HiLoChartHighValuesFontColor
       \sa hiLoChartHighValuesFontUseRelSize, HiLoChartHighValuesFontUseRelSize
       \sa hiLoChartHighValuesFontRelSize, HiLoChartHighValuesFontRelSize
    */
    void setHiLoChartPrintLowValues( bool active,
                                     QFont* font = 0,
                                     uint size = 12,
                                     QColor* color = 0 )
    {
        _hiLoChartPrintLowValues = active;
        if ( font )
            _hiLoChartLowValuesFont = *font;
        else
            _hiLoChartLowValuesFont = QFont( "helvetica", 8,
                                             QFont::Normal, false );
        _hiLoChartLowValuesUseFontRelSize = ( 0 < size );
        _hiLoChartLowValuesFontRelSize = size;
        if ( 0 == color )
            _hiLoChartLowValuesColor = QColor( 0, 0, 0 );
        else
            _hiLoChartLowValuesColor = *color;
        emit changed();
    }

    /**
       Returns whether the low values
       will be printed under their respective entries.

       \return whether the low values
       will be printed under their respective entries.

       \sa setHiLoChartPrintLowValues, setHiLoChartPrintHighValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
    */
    bool hiLoChartPrintLowValues() const
    {
        return _hiLoChartPrintLowValues;
    }
    /**
       Returns the font to be used for printing the
       low values

       \returns the font to be used for printing the
       low values

       \sa setHiLoChartPrintLowValues, setHiLoChartPrintHighValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
    */
    QFont hiLoChartLowValuesFont() const
    {
        return _hiLoChartLowValuesFont;
    }
    /**
       Returns whether the font size to be used for printing the
       low values is calculated dynamically.

       \return whether the font size to be used for printing the
       low values is calculated dynamically.

       \sa setHiLoChartPrintLowValues, setHiLoChartPrintHighValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
    */
    bool hiLoChartLowValuesUseFontRelSize() const
    {
        return _hiLoChartLowValuesUseFontRelSize;
    }
    /**
       Returns the relative size (in per mille of the chart width)
       of font size to be used for printing the
       low values.

       \return the relative size (in per mille of the chart width)
       of font size to be used for printing the
       low values.

       \sa setHiLoChartPrintLowValues, setHiLoChartPrintHighValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
    */
    int hiLoChartLowValuesFontRelSize() const
    {
        return _hiLoChartLowValuesFontRelSize;
    }
    /**
       Returns the colour of the font size to be used for printing the
       low values.

       \return the colour of the font size to be used for printing the
       low values.

       \sa setHiLoChartPrintLowValues, setHiLoChartPrintHighValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
    */
    QColor hiLoChartLowValuesColor() const
    {
        return _hiLoChartLowValuesColor;
    }

    /**
       Specifies if and how a HiLo chart will print the High
       values below their respective entries.
       Only used if chartType() == HiLo

       \note <b>setHiLoChartPrintHighValues( false )</b> will
       deactivate printing of High values.

       \param active specifies whether the values are to be printed or not.
       \param font specifies a Pointer to the font to be used.
       \param size (in per mille of the chart width) specifies the
       dynamic size of the font to be used. If this parameter is zero
       the size of the
       \c font is used instead - regardless of the size of the chart!
       \param specifies the color the color to be used when printing
       the values.
       \sa setHiLoChartPrintLowValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
       \sa hiLoChartPrintHighValues, HiLoChartPrintHighValues
       \sa hiLoChartHighValuesFontColor, HiLoChartHighValuesFontColor
       \sa hiLoChartHighValuesFontUseRelSize, HiLoChartHighValuesFontUseRelSize
       \sa hiLoChartHighValuesFontRelSize, HiLoChartHighValuesFontRelSize
       \sa hiLoChartPrintLowValues, HiLoChartPrintLowValues
       \sa hiLoChartLowValuesFontColor, HiLoChartLowValuesFontColor
       \sa hiLoChartLowValuesFontUseRelSize, HiLoChartLowValuesFontUseRelSize
       \sa hiLoChartLowValuesFontRelSize, HiLoChartLowValuesFontRelSize
    */
    void setHiLoChartPrintHighValues( bool active,
                                      QFont* font = 0,
                                      int size = 12,
                                      QColor* color = 0 )
    {
        _hiLoChartPrintHighValues = active;
        if ( font )
            _hiLoChartHighValuesFont = *font;
        else
            _hiLoChartHighValuesFont = QFont( "helvetica", 8,
                                              QFont::Normal, false );
        _hiLoChartHighValuesUseFontRelSize = ( 0 < size );
        _hiLoChartHighValuesFontRelSize = size;
        if ( 0 == color )
            _hiLoChartHighValuesColor = QColor( 0, 0, 0 );
        else
            _hiLoChartHighValuesColor = *color;
        emit changed();
    }

    /**
       Returns whether the high values
       will be printed under their respective entries.

       \return whether the high values
       will be printed under their respective entries.

       \sa setHiLoChartPrintHighValues, setHiLoChartPrintLowValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
    */
    bool hiLoChartPrintHighValues() const
    {
        return _hiLoChartPrintHighValues;
    }
    /**
       Returns the font to be used for printing the
       high values.

       \returns the font to be used for printing the
       high values.

       \sa setHiLoChartPrintHighValues, setHiLoChartPrintLowValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
    */
    QFont hiLoChartHighValuesFont() const
    {
        return _hiLoChartHighValuesFont;
    }
    /**
       Returns whether the font size to be used for printing the
       high values is calculated dynamically.

       \return whether the font size to be used for printing the
       high values is calculated dynamically.

       \sa setHiLoChartPrintHighValues, setHiLoChartPrintLowValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
    */
    bool hiLoChartHighValuesUseFontRelSize() const
    {
        return _hiLoChartHighValuesUseFontRelSize;
    }
    /**
       Returns the relative size (in per mille of the chart width)
       of font size to be used for printing the
       high values.

       \return the relative size (in per mille of the chart width)
       of font size to be used for printing the
       high values.

       \sa setHiLoChartPrintHighValues, setHiLoChartPrintHighValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
    */
    int hiLoChartHighValuesFontRelSize() const
    {
        return _hiLoChartHighValuesFontRelSize;
    }
    /**
       Returns the colour of the font size to be used for printing the
       high values.

       \return the colour of the font size to be used for printing the
       high values.

       \sa setHiLoChartPrintHighValues, setHiLoChartPrintLowValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
    */
    QColor hiLoChartHighValuesColor() const
    {
        return _hiLoChartHighValuesColor;
    }

    /**
       Specifies if and how a HiLo chart will print the Open values
       below their respective entries.
       Only used if chartType() == HiLo and if the HiLo chart sub type
       shows open values.

       \note <b>setHiLoChartPrintOpenValues( false )</b> will
       deactivate printing of Open values.

       \param active specifies whether the values are to be printed or not.
       \param font a Pointer to the font to be used.
       \param size (in per mille of the chart width) the dynamic size of
       the font to be used. If this parameter is zero the size of the
       \c font is used instead - regardless of the size of the chart!
       \param color the color to be used when printing the values.
       \sa setHiLoChartPrintCloseValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
       \sa hiLoChartPrintOpenValues, HiLoChartPrintOpenValues
       \sa hiLoChartOpenValuesFontColor, HiLoChartOpenValuesFontColor
       \sa hiLoChartOpenValuesFontUseRelSize, HiLoChartOpenValuesFontUseRelSize
       \sa hiLoChartOpenValuesFontRelSize, HiLoChartOpenValuesFontRelSize
       \sa hiLoChartPrintCloseValues, HiLoChartPrintCloseValues
       \sa hiLoChartCloseValuesFontColor, HiLoChartCloseValuesFontColor
       \sa hiLoChartCloseValuesFontUseRelSize, HiLoChartCloseValuesFontUseRelSize
       \sa hiLoChartCloseValuesFontRelSize, HiLoChartCloseValuesFontRelSize
    */
    void setHiLoChartPrintOpenValues( bool active,
                                       QFont* font = 0,
                                       uint size = 12,
                                       QColor* color = 0 )
    {
        _hiLoChartPrintOpenValues = active;
        if ( font )
            _hiLoChartOpenValuesFont = *font;
        else
            _hiLoChartOpenValuesFont = QFont( "helvetica", 8,
                                               QFont::Normal, false );
        _hiLoChartOpenValuesUseFontRelSize = ( 0 < size );
        _hiLoChartOpenValuesFontRelSize = size;
        if ( 0 == color )
            _hiLoChartOpenValuesColor = QColor( 0, 0, 0 );
        else
            _hiLoChartOpenValuesColor = *color;
        emit changed();
    }

    /**
       Returns whether the open values will be printed under their
       respective entries.

       \return whether the open values will be printed under their
       respective entries.

       \sa setHiLoChartPrintOpenValues, setHiLoChartPrintCloseValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
    */
    bool hiLoChartPrintOpenValues() const
    {
        return _hiLoChartPrintOpenValues;
    }
    /**
       Returns the font to be used for printing the
       open values.

       \returns the font to be used for printing the
       open values.

       \sa setHiLoChartPrintOpenValues, setHiLoChartPrintCloseValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
    */
    QFont hiLoChartOpenValuesFont() const
    {
        return _hiLoChartOpenValuesFont;
    }
    /**
       Returns whether the font size to be used for printing the
       open values is calculated dynamically.

       \return whether the font size to be used for printing the
       open values is calculated dynamically.

       \sa setHiLoChartPrintOpenValues, setHiLoChartPrintCloseValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
    */
    bool hiLoChartOpenValuesUseFontRelSize() const
    {
        return _hiLoChartOpenValuesUseFontRelSize;
    }
    /**
       Returns the relative size (in per mille of the chart width)
       of font size to be used for printing the open values.

       \return the relative size (in per mille of the chart width) of
       font size to be used for printing the open values.

       \sa setHiLoChartPrintOpenValues, setHiLoChartPrintCloseValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
    */
    int hiLoChartOpenValuesFontRelSize() const
    {
        return _hiLoChartOpenValuesFontRelSize;
    }
    /**
       Returns the colour of the font size to be used for printing the
       open values.

       \return the colour of the font size to be used for printing the
       open values.

       \sa setHiLoChartPrintOpenValues, setHiLoChartPrintCloseValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
    */
    QColor hiLoChartOpenValuesColor() const
    {
        return _hiLoChartOpenValuesColor;
    }

    /**
       Specifies if and how a HiLo chart will print the Close
       values next to their respective entries.
       Only used if chartType() == HiLo and the HiLo chart sub type
       contains Close values.

       \note <b>setHiLoChartPrintCloseValues( false )</b> will
       deactivate printing of Close values.

       \param active specifies whether the values are to be printed or not.
       \param font specifies a Pointer to the font to be used.
       \param size (in per mille of the chart width) specifies the dynamic size of
       the font to be used. If this parameter is zero the size of the
       \c font is used instead - regardless of the size of the chart!
       \param specifies the color the color to be used when printing the values.
       \sa setHiLoChartPrintOpenValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
       \sa hiLoChartPrintCloseValues, HiLoChartPrintCloseValues
       \sa hiLoChartCloseValuesFontColor, HiLoChartCloseValuesFontColor
       \sa hiLoChartCloseValuesFontUseRelSize, HiLoChartCloseValuesFontUseRelSize
       \sa hiLoChartCloseValuesFontRelSize, HiLoChartCloseValuesFontRelSize
       \sa hiLoChartPrintOpenValues, HiLoChartPrintOpenValues
       \sa hiLoChartOpenValuesFontColor, HiLoChartOpenValuesFontColor
       \sa hiLoChartOpenValuesFontUseRelSize, HiLoChartOpenValuesFontUseRelSize
       \sa hiLoChartOpenValuesFontRelSize, HiLoChartOpenValuesFontRelSize
    */
    void setHiLoChartPrintCloseValues( bool active,
                                      QFont* font = 0,
                                      int size = 12,
                                      QColor* color = 0 )
    {
        _hiLoChartPrintCloseValues = active;
        if ( font )
            _hiLoChartCloseValuesFont = *font;
        else
            _hiLoChartCloseValuesFont = QFont( "helvetica", 8,
                                              QFont::Normal, false );
        _hiLoChartCloseValuesUseFontRelSize = ( 0 < size );
        _hiLoChartCloseValuesFontRelSize = size;
        if ( 0 == color )
            _hiLoChartCloseValuesColor = QColor( 0, 0, 0 );
        else
            _hiLoChartCloseValuesColor = *color;
        emit changed();
    }

    /**
       Returns whether the close values will be printed under their
       respective entries.

       \return whether the close values will be printed under their
       respective entries.

       \sa setHiLoChartPrintCloseValues, setHiLoChartPrintOpenValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
    */
    bool hiLoChartPrintCloseValues() const
    {
        return _hiLoChartPrintCloseValues;
    }
    /**
       Returns the font to be used for printing the close values.

       \returns the font to be used for printing the close values.

       \sa setHiLoChartPrintCloseValues, setHiLoChartPrintOpenValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
    */
    QFont hiLoChartCloseValuesFont() const
    {
        return _hiLoChartCloseValuesFont;
    }
    /**
       Returns whether the font size to be used for printing the close
       values is calculated dynamically.

       \return whether the font size to be used for printing the close
       values is calculated dynamically.

       \sa setHiLoChartPrintCloseValues, setHiLoChartPrintOpenValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
    */
    bool hiLoChartCloseValuesUseFontRelSize() const
    {
        return _hiLoChartCloseValuesUseFontRelSize;
    }
    /**
       Returns the relative size (in per mille of the chart width) of
       font size to be used for printing the close values.

       \return the relative size (in per mille of the chart width) of
       font size to be used for printing the close values.

       \sa setHiLoChartPrintCloseValues, setHiLoChartPrintOpenValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
    */
    int hiLoChartCloseValuesFontRelSize() const
    {
        return _hiLoChartCloseValuesFontRelSize;
    }
    /**
       Returns the colour of the font size to be used for printing the
       close values.

       \return the colour of the font size to be used for printing the
       close values.

       \sa setHiLoChartPrintCloseValues, setHiLoChartPrintOpenValues
       \sa setHiLoChartSubType, hiLoChartSubType
       \sa setChartType, chartType
    */
    QColor hiLoChartCloseValuesColor() const
    {
        return _hiLoChartCloseValuesColor;
    }


    /**
       \obsolete

       This method is obsolete; use \a setHiLoChartPrintOpenValues().
    */
    void setHiLoChartPrintFirstValues( bool active,
                                       QFont* font = 0,
                                       uint size = 12,
                                       QColor* color = 0 )
    {
        setHiLoChartPrintOpenValues( active, font, size, color );
    }

    /**
       \obsolete

       This method is obsolete; use \a hiLoChartPrintOpenValues()
       instead.
    */
    bool hiLoChartPrintFirstValues() const
    {
        return hiLoChartPrintOpenValues();
    }
    /**
       \obsolete

       This method is obsolete; use \a hiLoChartOpenValuesFont()
       instead.
    */
    QFont hiLoChartFirstValuesFont() const
    {
        return hiLoChartOpenValuesFont();
    }
    /**
       \obsolete

       This method is obsolete; use \a
       hiLoChartOpenValuesUseFontRelSize() instead.
    */
    bool hiLoChartFirstValuesUseFontRelSize() const
    {
        return hiLoChartOpenValuesUseFontRelSize();
    }

    /**
       \obsolete

       This method is obsolete; use \a
       hiLoChartOpenValuesFontRelSize() instead.
    */
    int hiLoChartFirstValuesFontRelSize() const
    {
        return hiLoChartOpenValuesFontRelSize();
    }
    /**
       \obsolete

       This method is obsolete; use \a hiLoChartOpenValuesColor()
       instead.
    */
    QColor hiLoChartFirstValuesColor() const
    {
        return hiLoChartOpenValuesColor();
    }

    /**
       \obsolete

       This method is obsolete; use \a setHiLoChartPrintCloseValues()
       instead.
    */
    void setHiLoChartPrintLastValues( bool active,
                                      QFont* font = 0,
                                      int size = 12,
                                      QColor* color = 0 )
    {
        setHiLoChartPrintCloseValues( active, font, size, color );
    }

    /**
       \obsolete

       This method is obsolete; use \a hiLoChartPrintCloseValues()
       instead.
    */
    bool hiLoChartPrintLastValues() const
    {
        return hiLoChartPrintCloseValues();
    }
    /**
       \obsolete

       This method is obsolete; use \a hiLoChartCloseValuesFont()
       instead.
    */
    QFont hiLoChartLastValuesFont() const
    {
        return hiLoChartCloseValuesFont();
    }
    /**
       \obsolete

       This method is obsolete; use \a hiLoChartCloseValuesUseFontRelSize()
       instead.
    */
    bool hiLoChartLastValuesUseFontRelSize() const
    {
        return hiLoChartCloseValuesUseFontRelSize();
    }
    /**
       \obsolete

       This method is obsolete; use \a hiLoChartCloseValuesFontRelSize()
       instead.
    */
    int hiLoChartLastValuesFontRelSize() const
    {
        return hiLoChartCloseValuesFontRelSize();
    }
    /**
       \obsolete

       This method is obsolete; use \a hiLoChartCloseValuesColor()
       instead.
    */
    QColor hiLoChartLastValuesColor() const
    {
        return hiLoChartCloseValuesColor();
    }


    /// END HI/LO CHART-SPECIFIC


    /// GANTT CHART-SPECIFIC
    /**
       The Gantt subtype. Only used when chartType == Gantt

       \sa setGanttChartSubType, ganttChartSubType
    */
    enum GanttChartSubType { GanttNormal };
    /**
       Specifies the Gantt chart subtype. Only used if chartType() ==
       Gantt

       \param ganttChartSubType the Gantt chart subtype
       \sa ganttChartSubType, GanttChartSubType, setChartType, chartType
    */
    void setGanttChartSubType( GanttChartSubType ganttChartSubType )
    {
        _ganttChartSubType = ganttChartSubType;
        emit changed();
    }
    /**
       Returns the Gantt chart subtype. Only used if chartType() ==
       Gantt.

       \return the Gantt chart sub type
       \sa setGanttChartSubType, GanttChartSubType, setChartType, chartType
    */
    GanttChartSubType ganttChartSubType() const
    {
        return _ganttChartSubType;
    }

    /**
    Converts the specified gantt chart subtype enum to a string representation.

    \param type the subtype enum to convert
    \return the string representation of the type enum
    */
    static QString ganttChartSubTypeToString( GanttChartSubType type ) {
        switch( type ) {
        case GanttNormal:
            return "GanttNormal";
        default: // should not happen
            qDebug( "Unknown Gantt chart subtype" );
            return "GanttNormal";
        }
    }


    /**
    Converts the specified string to a Gantt chart subtype enum value.

    \param string the string to convert
    \return the Gantt chart subtype enum value
    */
    static GanttChartSubType stringToGanttChartSubType( const QString& string ) {
        if( string == "GanttNormal" )
            return GanttNormal;
        else // should not happen
            return GanttNormal;
    }


    /**
       The temporal resolution type. Only used when chartType == Gantt

       \sa setGanttChartTemporalResolution, ganttChartTemporalResolution
    */
    enum GanttChartTemporalResolution { GanttTempResSecond,
                                        GanttTempResMinute,
                                        GanttTempResHour,
                                        GanttTempResDay,
                                        GanttTempResWeek,
                                        GanttTempResMonth,
                                        GanttTempResQuarterYear,
                                        GanttTempResYear };
    /* besser so:

    enum GanttChartTemporalResolution { GanttTempResSecond,
    GanttTempResMinute,
    GanttTempRes5Minutes,
    GanttTempRes10Minutes,
    GanttTempRes15Minutes,
    GanttTempRes20Minutes,
    GanttTempRes30Minutes,
    GanttTempResHour,
    GanttTempRes2Hours,
    GanttTempRes3Hours,
    GanttTempRes4Hours,
    GanttTempRes6Hours,
    GanttTempRes8Hours,
    GanttTempRes12Hours,
    GanttTempResDay,
    GanttTempResWeek,
    GanttTempResMonth,
    GanttTempResQuarterYear,
    GanttTempResYear };
    */

    /**
       Specifies the temporal resolution of the gantt chart.
       Only used if chartType() == Gantt

       The grid will be drawn according to the resolution specified here
       and the time line (the abcissa-axis) will show the respective scaling.

       \param ganttChartTemporalResolution the temporal resolution
       of the gantt chart.
       \sa ganttChartSubType, GanttChartSubType, setChartType, chartType
    */
    void setGanttChartTemporalResolution(
        GanttChartTemporalResolution resolution )
    {
        _ganttChartTemporalResolution = resolution;
        emit changed();
    }
    /**
       Returns the temporal resolution of the gantt chart.
       Only used if chartType() == Gantt.

       \return the Gantt chart temporal resolution
       \sa setGanttChartTemporalResolution, GanttChartTemporalResolution
       \sa setChartType, chartType
    */
    GanttChartTemporalResolution ganttChartTemporalResolution() const
    {
        return _ganttChartTemporalResolution;
    }

    /**
    Converts the specified temporal resolution enum to a
    string representation.

    \param type the temporal resolution enum to convert
    \return the string representation of the type enum
    */
    static QString ganttTemporalResolutionToString( GanttChartTemporalResolution type ) {
        switch( type ) {
        case GanttTempResSecond:
            return "Second";
        case GanttTempResMinute:
            return "Minute";
        case GanttTempResHour:
            return "Hour";
        case GanttTempResDay:
            return "Day";
        case GanttTempResWeek:
            return "Week";
        case GanttTempResMonth:
            return "Month";
        case GanttTempResQuarterYear:
            return "QuarterYear";
        case GanttTempResYear:
            return "Year";
        default: // should not happen
            qDebug( "Unknown Gantt chart temporal resolution" );
            return "Second";
        }
    }


    /**
    Converts the specified string to an temporal resolution enum value.

    \param string the string to convert
    \return the temporal resolution enum value
    */
    static GanttChartTemporalResolution stringToGanttChartTemporalResolution( const QString& string ) {
        if( string == "Second" )
            return GanttTempResSecond;
        else if( string == "Minute" )
            return GanttTempResMinute;
        else if( string == "Hour" )
            return GanttTempResHour;
        else if( string == "Day" )
            return GanttTempResDay;
        else if( string == "Week" )
            return GanttTempResWeek;
        else if( string == "Month" )
            return GanttTempResMonth;
        else if( string == "QuarterYear" )
            return GanttTempResQuarterYear;
        else if( string == "Year" )
            return GanttTempResYear;
        else // default, should not happen
            return GanttTempResDay;
    }


    /**
       The position and alignment of texts to be printed at or
       inside of Gantt chart bars.
       Only used when chartType == Gantt

       The following picture shows the different possibilities:
       \verbatim

                    GanttTopInL      GanttTopC      GanttTopInR
                   +-------------------------------------------+
       GanttTopOutL|                                           |GanttTopOutR
                   |                                           |
                   |                                           |
          GanttOutL|GanttInL         GanttInC          GanttInR|GanttOutR
                   |                                           |
                   |                                           |
       GanttBotOutL|                                           |GanttBotOutR
                   +-------------------------------------------+
                    GanttBotInL      GanttBotC      GanttBotInR

       \endverbatim

       \sa setGanttChartPrintStartValues
       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintDurationValues
    */
    enum GanttChartTextAlign {
        GanttTopInL, GanttTopC, GanttTopInR,
        GanttTopOutL, GanttTopOutR,
        GanttOutL, GanttInL, GanttInC, GanttInR, GanttOutR,
        GanttBotOutL, GanttBotOutR,
        GanttBotInL, GanttBotC, GanttBotInR };


    /**
    Converts the specified text align enum to a
    string representation.

    \param type the text align to convert
    \return the string representation of the type enum
    */
    static QString ganttTextAlignToString( GanttChartTextAlign type ) {
        switch( type ) {
        case GanttTopInL:
            return "TopInL";
        case GanttTopC:
            return "TopC";
        case GanttTopInR:
            return "TopInR";
        case GanttTopOutL:
            return "TopOutL";
        case GanttTopOutR:
            return "TopOutR";
        case GanttOutL:
            return "OutL";
        case GanttInL:
            return "InL";
        case GanttInC:
            return "InC";
        case GanttInR:
            return "InR";
        case GanttOutR:
            return "OutR";
        case GanttBotOutL:
            return "BotOutL";
        case GanttBotOutR:
            return "BotOutR";
        case GanttBotInL:
            return "BotInL";
        case GanttBotC:
            return "BotC";
        case GanttBotInR:
            return "BotInR";
        default: // should not happen
            qDebug( "Unknown Gantt text align" );
            return "TopInL";
        }
    }


    /**
    Converts the specified string to a gantt align enum value.

    \param string the string to convert
    \return the gantt align enum value
    */
    static GanttChartTextAlign stringToGanttChartTextAlign( const QString& string ) {
        if( string == "TopInL" )
            return GanttTopInL;
        else if( string == "TopC" )
            return GanttTopC;
        else if( string == "TopInR" )
            return GanttTopInR;
        else if( string == "TopOutL" )
            return GanttTopOutL;
        else if( string == "TopOutR" )
            return GanttTopOutR;
        else if( string == "OutL" )
            return GanttOutL;
        else if( string == "InL" )
            return GanttInL;
        else if( string == "InC" )
            return GanttInC;
        else if( string == "InR" )
            return GanttInR;
        else if( string == "OutR" )
            return GanttOutR;
        else if( string == "BotOutL" )
            return GanttBotOutL;
        else if( string == "BotOutR" )
            return GanttBotOutR;
        else if( string == "BotInL" )
            return GanttBotInL;
        else if( string == "BotC" )
            return GanttBotC;
        else if( string == "BotInR" )
            return GanttBotInR;
        else // default, should not happen
            return GanttTopInL;
    }


    /**
       Specifies if and how a Gantt chart will print the Start Time values
       at or inside of their respective bars.
       Only used if chartType() == Gantt

       \note <b>setGanttChartPrintStartValues( false )</b> will deactivate
       printing of Start Time values.

       \param active specifies whether the values are to be printed or not.
       \param align specifies where to print the texts and how to align them.
       \param dtf specifies the format to be used when printing the values.
       \param font specifies a Pointer to the font to be used.
       \param size (in per mille of the chart width) specifies the dynamic size of
       the font to be used. If this parameter is zero the size of the
       \c font is used instead - regardless of the size of the chart!
       \param color specifies the color to be used when printing the values.

       \sa GanttChartTextAlign, KDCHART_DEFAULT_DATETIME_FORMAT
       \sa <br>setGanttChartPrintEndValues, setGanttChartPrintDurations
       \sa <br>setGanttChartSubType, ganttChartSubType
       \sa setChartType, chartType
       \sa ganttChartPrintStartValues, GanttChartPrintStartValues
       \sa ganttChartStartValuesFontColor, GanttChartStartValuesFontColor
       \sa ganttChartStartValuesFontUseRelSize, GanttChartStartValuesFontUseRelSize
       \sa ganttChartStartValuesFontRelSize, GanttChartStartValuesFontRelSize
       \sa ganttChartPrintEndValues, GanttChartPrintEndValues
       \sa ganttChartEndValuesFontColor, GanttChartEndValuesFontColor
       \sa ganttChartEndValuesFontUseRelSize, GanttChartEndValuesFontUseRelSize
       \sa ganttChartEndValuesFontRelSize, GanttChartEndValuesFontRelSize
       \sa ganttChartPrintDurations, GanttChartPrintDurations
       \sa ganttChartDurationsFontColor, GanttChartDurationsFontColor
       \sa ganttChartDurationsFontUseRelSize, GanttChartDurationsFontUseRelSize
       \sa ganttChartDurationsFontRelSize, GanttChartDurationsFontRelSize
    */
    void setGanttChartPrintStartValues( bool active,
                                        GanttChartTextAlign align = GanttOutL,
                                        QString dtf = KDCHART_DEFAULT_DATETIME_FORMAT,
                                        QFont* font = 0,
                                        int size = 12,
                                        QColor* color = 0 )
    {
        _ganttChartPrintStartValues = active;
        _ganttChartStartValuesAlign = align;
        _ganttChartStartValuesDateTimeFormat = dtf;
        if ( font )
            _ganttChartStartValuesFont = *font;
        else
            _ganttChartStartValuesFont = QFont( "helvetica", 8,
                                                QFont::Normal, false );
        _ganttChartStartValuesUseFontRelSize = ( 0 < size );
        _ganttChartStartValuesFontRelSize = size;
        if ( 0 == color )
            _ganttChartStartValuesColor = QColor( 0, 0, 0 );
        else
            _ganttChartStartValuesColor = *color;
        emit changed();
    }

    /**
       Returns whether the Start Time values
       will be printed at or inside of their respective entries.

       \return whether the Start Time values
       will be printed at or inside of their respective entries.

       \sa setGanttChartPrintStartValues
       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintDurations
       \sa setChartType, chartType
    */
    bool ganttChartPrintStartValues() const
    {
        return _ganttChartPrintStartValues;
    }
    /**
       Returns where to print the Start Time values and how to align them.

       \returns where to print the Start Time values and how to align them.

       \sa setGanttChartPrintStartValues
       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintDurations
       \sa setChartType, chartType
    */
    GanttChartTextAlign ganttChartStartValuesAlign() const
    {
        return _ganttChartStartValuesAlign;
    }
    /**
       Returns the datetime format to be used for printing the
       Start Time values.

       \returns the datetime format to be used for printing the
       Start Time values.

       \sa setGanttChartPrintStartValues
       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintDurations
       \sa setChartType, chartType
    */
    QString ganttChartStartValuesDateTimeFormat() const
    {
        return _ganttChartStartValuesDateTimeFormat;
    }
    /**
       Returns the font to be used for printing the
       Start Time values.

       \returns the font to be used for printing the
       Start Time values.

       \sa setGanttChartPrintStartValues
       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintDurations
       \sa setChartType, chartType
    */
    QFont ganttChartStartValuesFont() const
    {
        return _ganttChartStartValuesFont;
    }
    /**
       Returns whether the font size to be used for printing the
       Start Time values is calculated
       dynamically.

       \return whether the font size to be used for printing the
       Start Time values is calculated
       dynamically.

       \sa setGanttChartPrintStartValues
       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintDurations
       \sa setChartType, chartType
    */
    bool ganttChartStartValuesUseFontRelSize() const
    {
        return _ganttChartStartValuesUseFontRelSize;
    }
    /**
       Returns the relative size (in per mille of the chart width)
       of font size to be used for printing the
       Start Time values.

       \return the relative size (in per mille of the chart width)
       of font size to be used for printing the
       Start Time values.

       \sa setGanttChartPrintStartValues
       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintDurations
       \sa setChartType, chartType
    */
    int ganttChartStartValuesFontRelSize() const
    {
        return _ganttChartStartValuesFontRelSize;
    }
    /**
       Returns the colour of the font size to be used for printing the
       Start Time values.

       \return the colour of the font size to be used for printing the
       Start Time values.

       \sa setGanttChartPrintStartValues
       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintDurations
       \sa setChartType, chartType
    */
    QColor ganttChartStartValuesColor() const
    {
        return _ganttChartStartValuesColor;
    }

    /**
       Specifies if and how a Gantt chart will print the End Time values
       at or inside of their respective bars.
       Only used if chartType() == Gantt

       \note <b>setGanttChartPrintEndValues( false )</b> will deactivate
       printing of End Time values.

       \param active specifies whether the values are to be printed or not.
       \param align specifies where to print the texts and how to align them.
       \param dtf specifies the format to be used when printing the values.
       \param font a Pointer to the font to be used.
       \param size (in per mille of the chart width) the dynamic size of
       the font to be used. If this parameter is zero the size of the
       \c font is used instead - regardless of the size of the chart!
       \param color the color to be used when printing the values.

       \sa GanttChartTextAlign, KDCHART_DEFAULT_DATETIME_FORMAT
       \sa <br>setGanttChartPrintStartValues, setGanttChartPrintDurations
       \sa <br>setGanttChartSubType, ganttChartSubType
       \sa setChartType, chartType
       \sa ganttChartPrintStartValues, GanttChartPrintStartValues
       \sa ganttChartStartValuesFontColor, GanttChartStartValuesFontColor
       \sa ganttChartStartValuesFontUseRelSize, GanttChartStartValuesFontUseRelSize
       \sa ganttChartStartValuesFontRelSize, GanttChartStartValuesFontRelSize
       \sa ganttChartPrintEndValues, GanttChartPrintEndValues
       \sa ganttChartEndValuesFontColor, GanttChartEndValuesFontColor
       \sa ganttChartEndValuesFontUseRelSize, GanttChartEndValuesFontUseRelSize
       \sa ganttChartEndValuesFontRelSize, GanttChartEndValuesFontRelSize
       \sa ganttChartPrintDurations, GanttChartPrintDurations
       \sa ganttChartDurationsFontColor, GanttChartDurationsFontColor
       \sa ganttChartDurationsFontUseRelSize, GanttChartDurationsFontUseRelSize
       \sa ganttChartDurationsFontRelSize, GanttChartDurationsFontRelSize
    */
    void setGanttChartPrintEndValues( bool active,
                                      GanttChartTextAlign align = GanttOutR,
                                      QString dtf = KDCHART_DEFAULT_DATETIME_FORMAT,
                                      QFont* font = 0,
                                      int size = 12,
                                      QColor* color = 0 )
    {
        _ganttChartPrintEndValues = active;
        _ganttChartEndValuesAlign = align;
        _ganttChartEndValuesDateTimeFormat = dtf;
        if ( font )
            _ganttChartEndValuesFont = *font;
        else
            _ganttChartEndValuesFont = QFont( "helvetica", 8,
                                              QFont::Normal, false );
        _ganttChartEndValuesUseFontRelSize = ( 0 < size );
        _ganttChartEndValuesFontRelSize = size;
        if ( 0 == color )
            _ganttChartEndValuesColor = QColor( 0, 0, 0 );
        else
            _ganttChartEndValuesColor = *color;
        emit changed();
    }

    /**
       Returns whether the End Time values
       will be printed at or inside of their respective entries.

       \return whether the End Time values
       will be printed under their respective entries.

       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintStartValues
       \sa setGanttChartPrintDurations
       \sa setChartType, chartType
    */
    bool ganttChartPrintEndValues() const
    {
        return _ganttChartPrintEndValues;
    }
    /**
       Returns where to print the End Time values and how to align them.

       \returns where to print the End Time values and how to align them.

       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintStartValues
       \sa setGanttChartPrintDurations
       \sa setChartType, chartType
    */
    GanttChartTextAlign ganttChartEndValuesAlign() const
    {
        return _ganttChartEndValuesAlign;
    }
    /**
       Returns the datetime format to be used for printing the
       End Time values.

       \returns the datetime format to be used for printing the
       End Time values.

       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintStartValues
       \sa setGanttChartPrintDurations
       \sa setChartType, chartType
    */
    QString ganttChartEndValuesDateTimeFormat() const
    {
        return _ganttChartEndValuesDateTimeFormat;
    }
    /**
       Returns the font to be used for printing the
       End Time values.

       \returns the font to be used for printing the
       End Time values.

       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintStartValues
       \sa setGanttChartPrintDurations
       \sa setChartType, chartType
    */
    QFont ganttChartEndValuesFont() const
    {
        return _ganttChartEndValuesFont;
    }
    /**
       Returns whether the font size to be used for printing the
       End Time values is calculated
       dynamically.

       \return whether the font size to be used for printing the
       End Time values is calculated
       dynamically.

       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintStartValues
       \sa setGanttChartPrintDurations
       \sa setChartType, chartType
    */
    bool ganttChartEndValuesUseFontRelSize() const
    {
        return _ganttChartEndValuesUseFontRelSize;
    }
    /**
       Returns the relative size (in per mille of the chart width)
       of font size to be used for printing the
       End Time values.

       \return the relative size (in per mille of the chart width)
       of font size to be used for printing the
       End Time values.

       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintStartValues
       \sa setGanttChartPrintDurations
       \sa setChartType, chartType
    */
    int ganttChartEndValuesFontRelSize() const
    {
        return _ganttChartEndValuesFontRelSize;
    }
    /**
       Returns the colour of the font size to be used for printing the
       End Time values.

       \return the colour of the font size to be used for printing the
       End Time values.

       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintStartValues
       \sa setGanttChartPrintDurations
       \sa setChartType, chartType
    */
    QColor ganttChartEndValuesColor() const
    {
        return _ganttChartEndValuesColor;
    }

    /**
       Specifies if and how a Gantt chart will print the Durations
       at or inside of their respective bars.
       Only used if chartType() == Gantt

       \note <b>setGanttChartPrintDurations( false )</b> will deactivate
       printing of Durations.

       \param active specifies whether the values are to be printed or not.
       \param align specifies where to print the texts and how to align them.
       \param dtf specifies the format to be used when printing the values.
       \param font a Pointer to the font to be used.
       \param size (in per mille of the chart width) the dynamic size of
       the font to be used. If this parameter is zero the size of the
       \c font is used instead - regardless of the size of the chart!
       \param color the color to be used when printing the values.

       \sa GanttChartTextAlign, KDCHART_DEFAULT_DATETIME_FORMAT
       \sa <br>setGanttChartPrintStartValues, setGanttChartPrintEndValues
       \sa <br>setGanttChartSubType, ganttChartSubType
       \sa setChartType, chartType
       \sa ganttChartPrintStartValues, GanttChartPrintStartValues
       \sa ganttChartStartValuesFontColor, GanttChartStartValuesFontColor
       \sa ganttChartStartValuesFontUseRelSize, GanttChartStartValuesFontUseRelSize
       \sa ganttChartStartValuesFontRelSize, GanttChartStartValuesFontRelSize
       \sa ganttChartPrintEndValues, GanttChartPrintEndValues
       \sa ganttChartEndValuesFontColor, GanttChartEndValuesFontColor
       \sa ganttChartEndValuesFontUseRelSize, GanttChartEndValuesFontUseRelSize
       \sa ganttChartEndValuesFontRelSize, GanttChartEndValuesFontRelSize
       \sa ganttChartPrintDurations, GanttChartPrintDurations
       \sa ganttChartDurationsFontColor, GanttChartDurationsFontColor
       \sa ganttChartDurationsFontUseRelSize, GanttChartDurationsFontUseRelSize
       \sa ganttChartDurationsFontRelSize, GanttChartDurationsFontRelSize
    */
    void setGanttChartPrintDurations( bool active,
                                      GanttChartTextAlign align = GanttInC,
                                      QString dtf = KDCHART_DEFAULT_DATETIME_FORMAT,
                                      QFont* font = 0,
                                      int size = 12,
                                      QColor* color = 0 )
    {
        _ganttChartPrintDurations = active;
        _ganttChartDurationsAlign = align;
        _ganttChartDurationsDateTimeFormat = dtf;
        if ( font )
            _ganttChartDurationsFont = *font;
        else
            _ganttChartDurationsFont = QFont( "helvetica", 8,
                                              QFont::Normal, false );
        _ganttChartDurationsUseFontRelSize = ( 0 < size );
        _ganttChartDurationsFontRelSize = size;
        if ( 0 == color )
            _ganttChartDurationsColor = QColor( 0, 0, 0 );
        else
            _ganttChartDurationsColor = *color;
        emit changed();
    }

    /**
       Returns whether the Durations
       will be printed at or inside of their respective entries.

       \return whether the Durations
       will be printed under their respective entries.

       \sa setGanttChartPrintDurations
       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintStartValues
       \sa setChartType, chartType
    */
    bool ganttChartPrintDurations() const
    {
        return _ganttChartPrintDurations;
    }
    /**
       Returns where to print the Durations and how to align them.

       \returns where to print the Durations and how to align them.

       \sa setGanttChartPrintDurations
       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintStartValues
       \sa setChartType, chartType
    */
    GanttChartTextAlign ganttChartDurationsAlign() const
    {
        return _ganttChartDurationsAlign;
    }
    /**
       Returns the datetime format to be used for printing the
       Durations.

       \returns the datetime format to be used for printing the
       Durations.

       \sa setGanttChartPrintDurations
       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintStartValues
       \sa setChartType, chartType
    */
    QString ganttChartDurationsDateTimeFormat() const
    {
        return _ganttChartDurationsDateTimeFormat;
    }
    /**
       Returns the font to be used for printing the
       Durations.

       \returns the font to be used for printing the
       Durations.

       \sa setGanttChartPrintDurations
       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintStartValues
       \sa setChartType, chartType
    */
    QFont ganttChartDurationsFont() const
    {
        return _ganttChartDurationsFont;
    }
    /**
       Returns whether the font size to be used for printing the
       Durations is calculated dynamically.

       \return whether the font size to be used for printing the
       Durations is calculated dynamically.

       \sa setGanttChartPrintDurations
       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintStartValues
       \sa setChartType, chartType
    */
    bool ganttChartDurationsUseFontRelSize() const
    {
        return _ganttChartDurationsUseFontRelSize;
    }
    /**
       Returns the relative size (in per mille of the chart width)
       of font size to be used for printing the
       Durations.

       \return the relative size (in per mille of the chart width)
       of font size to be used for printing the
       Durations.

       \sa setGanttChartPrintDurations
       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintStartValues
       \sa setChartType, chartType
    */
    int ganttChartDurationsFontRelSize() const
    {
        return _ganttChartDurationsFontRelSize;
    }
    /**
       Returns the colour of the font size to be used for printing the
       Durations.

       \return the colour of the font size to be used for printing the
       Durations.

       \sa setGanttChartPrintDurations
       \sa setGanttChartPrintEndValues
       \sa setGanttChartPrintStartValues
       \sa setChartType, chartType
    */
    QColor ganttChartDurationsColor() const
    {
        return _ganttChartDurationsColor;
    }

    /// END GANTT CHART-SPECIFIC




    /// LEGENDS

    /**
       The possible positions for a legend. NoLegend turns legend display
       off.

       \sa setLegendPosition, legendPosition
    */
    enum LegendPosition { NoLegend, LegendTop, LegendBottom,
                          LegendLeft, LegendRight,
                          LegendTopLeft,
                          LegendTopLeftTop,
                          LegendTopLeftLeft,
                          LegendTopRight,
                          LegendTopRightTop,
                          LegendTopRightRight,
                          LegendBottomLeft,
                          LegendBottomLeftBottom,
                          LegendBottomLeftLeft,
                          LegendBottomRight,
                          LegendBottomRightBottom,
                          LegendBottomRightRight
                        };


    /**
       Specifies where the legend should be shown. NoLegend turns legend
       display off. The default is LegendRight which positions the legend
       right to the data display.

       \param the position for the legend
       \sa LegendPosition, legendPosition
    */
    void setLegendPosition( LegendPosition position )
    {
        _legendPosition = position;
        emit changed();
    }

    /**
       Returns where the legend will be shown.

       \return where the legend will be shown
       \sa LegendPosition, setLegendPosition
    */
    LegendPosition legendPosition() const
    {
        return _legendPosition;
    }


    /**
    Converts the specified legend position enum to a string representation.

    \param type the legend position enum to convert
    \return the string representation of the type enum
    */
    static QString legendPositionToString( LegendPosition pos ) {
        switch( pos ) {
        case NoLegend:
            return "NoLegend";
        case LegendTop:
            return "Top";
        case LegendBottom:
            return "Bottom";
        case LegendLeft:
            return "Left";
        case LegendRight:
            return "Right";
        case LegendTopLeft:
            return "TopLeft";
        case LegendTopLeftTop:
            return "TopLeftTop";
        case LegendTopLeftLeft:
            return "TopLeftLeft";
        case LegendBottomLeft:
            return "BottomLeft";
        case LegendBottomLeftBottom:
            return "BottomLeftTop";
        case LegendBottomLeftLeft:
            return "BottomLeftLeft";
        case LegendTopRight:
            return "TopRight";
        case LegendTopRightTop:
            return "TopRightTop";
        case LegendTopRightRight:
            return "TopRightRight";
        case LegendBottomRight:
            return "BottomRight";
        case LegendBottomRightBottom:
            return "BottomRightTop";
        case LegendBottomRightRight:
            return "BottomRightRight";
        default: // should not happen
            qDebug( "Unknown legend position" );
            return "Right";
        }
    }


    /**
    Converts the specified string to a legend position enum value.

    \param string the string to convert
    \return the legend position enum value
    */
    static LegendPosition stringToLegendPosition( const QString& string ) {
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
       The possible ways for getting the legend text. Available are:
       <ul>
       <li>Manual - Uses texts set with setLegendText(); if no text is set for
       a dataset, the legend text will be empty.
       <li>FirstColumn - Uses values from the first column, no matter what
       this contains.
       <li>Automatic - Tries first to use values from the first column; if
       this does not contain any string values, tries to use values set
       manually with setLegendText(). If there are no values set manually
       either, resolves to standard texts like Series 1, Series 2, .... This
       is the default.
       </ul>
       \sa setLegendSource, legendSource, setLegendText, legendText
    */
    enum LegendSource { LegendManual, LegendFirstColumn, LegendAutomatic };

    /**
       Specifies where the legend text is coming from. See LegendSource for an
       explanation of the possible values.

       \param source the source of the legend texts
       \sa legendSource, LegendSource, setLegendText, legendText
    */
    void setLegendSource( LegendSource source )
    {
        _legendSource = source;
        emit changed();
    }

    /**
       Returns where the legend text is coming from. See LegendSource for an
       explanation of the possible return values.

       \return where the legend text is coming from
       \sa LegendSource, setLegendSource, setLegendText, legendText
    */
    LegendSource legendSource() const
    {
        return _legendSource;
    }

    /**
    Converts the specified legend source enum to a string representation.

    \param source the legend source enum to convert
    \return the string representation of the type enum
    */
    static QString legendSourceToString( LegendSource source ) {
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
    static LegendSource stringToLegendSource( const QString& string ) {
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
       Manually specifies a legend text for a certain dataset. This is only
       used if legendSource() == Manual or if legendSource() == Automatic and
       it is not possible to retrieve values from the first column.

       \param dataset the dataset for which to set the legend text
       \param the legend text to be set for a given dataset
       \sa LegendSource, setLegendSource, legendSource, legendText
    */
    void setLegendText( uint dataset, const QString& text )
    {
        _legendText[ dataset ] = text;
        emit changed();
    }

    /**
       Returns the manually specified legend text for a given dataset.

       \return the manually specified legend text for the specified dataset
       \sa setLegendText, LegendSource, setLegendSource, legendSource
    */
    QString legendText( uint dataset ) const
    {
        return _legendText[ dataset ];
    }


    /**
       Specifies the color to be used for the legend texts. The
       default is black.

       \param color the color to be used for the legend texts
       \sa legendTextColor(), setLegendTitleTextColor()
    */
    void setLegendTextColor( const QColor& color )
    {
        _legendTextColor = color;
        emit changed();
    }


    /**
       Returns the color in which the legend texts are drawn. The
       default is black.

       \return the color in which the legend texts are drawn
       \sa setLegendTextColor(), legendTitleTextColor()
    */
    QColor legendTextColor() const
    {
        return _legendTextColor;
    }


    /**
       Specifies the font in which the legend texts are drawn. The default is a
       10pt helvetica font.

       \note The font size will be ignored if \c useFontSize is false,
       in this case the font size will be calculated dynamically using
       the value stored by you calling setLegendFontRelSize().

       \param font the font to draw the legend texts in
       \sa setLegendFontUseRelSize, legendFont
    */
    void setLegendFont( const QFont& font, bool useFontSize )
    {
        _legendFont = font;
        _legendFontUseRelSize = ! useFontSize;
        emit changed();
    }

    /**
       Returns the font in which the legend texts are drawn.

       \return the font in which the legend texts are drawn
       \sa setLegendFont, setLegendFontUseRelSize
    */
    QFont legendFont() const
    {
        return _legendFont;
    }


    /**
       Specifies whether the legend shall be drawn
       using relative font size.

       \param legendFontUseRelSize whether legend shall be drawn
       using relative font size.
       If true the absolute value of the value set by \c
       setLegendFontRelSize is per thousand
       of of the average value of the printable area height and width.
       This will make the legend look nice even if scaled to very
       different size.

       \sa setLegendFontRelSize, setAxisLabelsFont
    */
    void setLegendFontUseRelSize( bool legendFontUseRelSize )
    {
        _legendFontUseRelSize = legendFontUseRelSize;
        emit changed();
    }

    /**
       Returns whether the legend font size is used.

       \return whether the fix legend font size is used.
       \sa setLegendFontRelSize, setLegendFont
    */
    bool legendFontUseRelSize() const
    {
        return _legendFontUseRelSize;
    }

    /**
       Specifies the legend relative font size.

       \param legendFontRelSize the relative legend size.
       If this value unequals zero the absolute value is per thousand
       of of the average value of the printable area height and width.
       This will make the legend look nice even if scaled to very
       different size.

       \sa setLegendFontUseRelSize, setLegendFont
    */
    void setLegendFontRelSize( int legendFontRelSize )
    {
        _legendFontRelSize = legendFontRelSize;
        emit changed();
    }

    /**
       Returns the relative legend font size.

       \return the relative legend font size.
       \sa setLegendFontRelSize, setLegendFontUseRelSize
    */
    int legendFontRelSize() const
    {
        return _legendFontRelSize;
    }


    /**
       Specifies a text to be shown as the title of the legend. The
       default is "Legend" (possibly localized). To turn off the
       legend title completely, pass an empty string.

       \param text the text to use as the legend title
       \sa legendTitleText(), setLegendTitleFont(),
       setLegendTitleFontUseRelSize()
    */
    void setLegendTitleText( const QString& text )
    {
        _legendTitleText = text;
        emit changed();
    }


    /** Returns the text that is shown as the title of the legend.

    \return the text that is used as the legend title
    \sa setLegendTitleText(), legendTitleFont(),
    legendTitleFontUseRelSize()
    */
    QString legendTitleText() const
    {
        return _legendTitleText;
    }


    /**
       Specifies the color to be used for the legend title. The
       default is black. Note that this color is only the default when
       rich text is used; tags in the rich text might change the color.

       \param color the color to be used for the legend title
       \sa legendTitleTextColor(), setLegendTextColor()
    */
    void setLegendTitleTextColor( const QColor& color )
    {
        _legendTitleTextColor = color;
        emit changed();
    }


    /**
       Returns the color in which the legend title is drawn. The
       default is black.

       \return the color in which the legend title is drawn
       \sa setLegendTitleTextColor(), legendTextColor()
    */
    QColor legendTitleTextColor() const
    {
        return _legendTitleTextColor;
    }



    /**
       Specifies the font in which the legend title (if any) is drawn. The
       default is a 12pt helvetica font.

       \note The font size will be ignored if \c useFontSize is false,
       in this case the font size will be calculated dynamically using
       the value stored by you calling setLegendFontRelSize().

       \param font the font to draw the legend texts in
       \sa setLegendTitleFontUseRelSize, legendTitleFont
    */
    void setLegendTitleFont( const QFont& font, bool useFontSize )
    {
        _legendTitleFont = font;
        _legendTitleFontUseRelSize = ! useFontSize;
        emit changed();
    }

    /**
       Returns the font in which the legend title (if any) is drawn.

       \return the font in which the legend texts are drawn
       \sa setLegendTitleFont, setLegendTitleFontUseRelSize
    */
    QFont legendTitleFont() const
    {
        return _legendTitleFont;
    }


    /**
       Specifies whether the legend title (if any) shall be drawn using
       relative font size.

       \param legendTitleFontUseRelSize whether the legend title shall
       be drawn using relative font size.
       If true the absolute value of the value set by \c
       setLegendTitleFontRelSize is per thousandth of the average
       value of the printable area height and width.
       This will make the legend title look nice even if scaled to a very
       different size.

       \sa setLegendTitleFontRelSize, setAxisLabelsFont
    */
    void setLegendTitleFontUseRelSize( bool legendTitleFontUseRelSize )
    {
        _legendTitleFontUseRelSize = legendTitleFontUseRelSize;
        emit changed();
    }

    /**
       Returns whether the legend title font size is used.

       \return whether the fixed legend title font size is used.
       \sa setLegendTitleFontRelSize, setLegendTitleFont
    */
    bool legendTitleFontUseRelSize() const
    {
        return _legendTitleFontUseRelSize;
    }

    /**
       Specifies the legend title relative font size.

       \param legendTitleFontRelSize the relative legend size.
       If this value unequals zero the absolute value is per thousand
       of of the average value of the printable area height and width.
       This will make the legend title look nice even if scaled to a very
       different size.

       \sa setLegendTitleFontUseRelSize, setLegendTitleFont
    */
    void setLegendTitleFontRelSize( int legendTitleFontRelSize )
    {
        _legendTitleFontRelSize = legendTitleFontRelSize;
        emit changed();
    }

    /**
       Returns the relative legend title font size.

       \return the relative legend title font size.
       \sa setLegendTitleFontRelSize, setLegendTitleFontUseRelSize
    */
    int legendTitleFontRelSize() const
    {
        return _legendTitleFontRelSize;
    }


    /**
       Specifies how much space in pixels there should be between the legend
       and the data display. The default is 20 pixels.

       \param space the space in pixels between the legend and the data
       display
       \sa legendSpacing
    */
    void setLegendSpacing( uint space )
    {
        _legendSpacing = space;
    }


    /**
       Returns how much space in pixels there is between the legend and the
       data display.

       \return the space in pixels between the legend and the data display
       \sa setLegendSpacing
    */
    uint legendSpacing() const
    {
        return _legendSpacing;
    }

    /// END LEGENDS



    /// AXES

    /**
       Specifies the axis type.

       \param n the number of the axis settings to be modified
       \param axisType the type of the axis.
       \sa axisVisible
    */
    void setAxisType( uint n, const KDChartAxisParams::AxisType axisType )
    {
        if ( n < KDCHART_MAX_AXES ) {
            _axisSettings[ n ].params.setAxisType( axisType );
            emit changed();
        }
    }

    /**
       Specifies if the axis will be drawn. The default is false.

       \param n the number of the axis settings to be modified
       \param axisVisible set true to make visible the respective axis.
       \sa axisVisible
    */
    void setAxisVisible( uint n,
                         const bool axisVisible )
    {
        if ( n < KDCHART_MAX_AXES ) {
            _axisSettings[ n ].params.setAxisVisible( axisVisible );
            emit changed();
        }
    }

    /**
       Returns if the axis will be drawn.

       \return if the axis is visible or not.
       \sa setAxisVisible
    */
    bool axisVisible( uint n ) const
    {
        return n < KDCHART_MAX_AXES ? _axisSettings[ n ].params.axisVisible()
               : false;
    }

    /**
       Specifies whether a grid should be drawn at the chart data area.

       The grid will show a solid line for each delimiter.
       (or a line with a pattern defined by \c KDChartAxisParams::setAxisGridStyle(), resp.)

       \note If true and axisShowSubDelimiters is also true the grid
       will show a thin dotted line for each sub-delimiter.
       (or a line with a pattern defined by \c KDChartAxisParams::setAxisGridSubStyle(), resp.)

       \param n the number of the axis for which to specify whether a grid should be drawn.
       \param axisShowGrid if true a grid will be drawn on the chart data area.
       \sa axisShowGrid, setAxisGridStyle, setAxisGridSubStyle
    */
    void setAxisShowGrid( uint n,
                          bool axisShowGrid )
    {
        if ( n < KDCHART_MAX_AXES ) {
            _axisSettings[ n ].params.setAxisShowGrid( axisShowGrid );
            emit changed();
        }
    }

    /**
       Returns if any grid will be drawn at the data area.

       \return if any grid will be drawn at the data area.
       \sa KDChartAxisParams::setAxisShowGrid
    */
    bool showGrid() const
    {
        for ( uint i = 0; i < KDCHART_MAX_AXES; ++i ) {
            if ( _axisSettings[ i ].params.axisVisible()
                        && _axisSettings[ i ].params.axisShowGrid() )
                return true;
        }
        return false;
    }

    /**
       Specifies which data are to be represented by a given axis.<br>
       If you specify a valid dataset-number for parameter dataset
       you may use the same number or a greater number for
       parameter dataset2 to specify a range of contiguous series
       of datasets.<br>
       If you specify <b>KDCHART_ALL_DATASETS</b> for dataset
       the value of dataset2 does not matter.<br>
       By setting the \c chart parameter you may specify which chart is
       represented by the axis, you may use this when displaying
       more than one chart in the same widget.

       \note If you specify \c KDCHART_ALL_DATASETS the axis will
       represent all the datasets with SourceMode DataEntry
       (see \c setChartSourceMode for explanation) that have been
       specified for this chart.<br>

       \param n the number of the axis for which to specify the dataset(s)
       \param dataset the dataset represented by axis \c n

       \sa axisDatasets, chartAxes
    */
    void setAxisDatasets( uint n,
                          uint dataset,
                          uint dataset2 = KDCHART_NO_DATASET,
                          uint chart = 0 )
    {
        uint a1 = ( KDCHART_ALL_AXES == n )
                ? 0
                : QMIN( n, KDCHART_MAX_AXES-1 );
        uint a2 = ( KDCHART_ALL_AXES == n )
                ? KDCHART_MAX_AXES-1
                : QMIN( n, KDCHART_MAX_AXES-1 );
        for( uint i = a1;  i <= a2;  ++i ) {
            _axisSettings[ i ].dataset = dataset;
            _axisSettings[ i ].dataset2 =
                ( KDCHART_ALL_DATASETS == dataset
                  || KDCHART_NO_DATASET == dataset
                  || KDCHART_ALL_DATASETS == dataset2
                  || KDCHART_NO_DATASET == dataset2 )
                ? dataset
                : dataset2;
            _axisSettings[ i ].chart = chart;
        }
        emit changed();
    }

    /**
       Returns which data are to be represented by a given axis.
       ( see explanation given with \c setAxisDatasets() )

       \param n the axis for which to return the dataset number(s)
       \param dataset the starting number of the series of datasets represented by axis \c n or KDCHART_ALL_DATASETS
       \param dataset2 the end number of the series of datasets
       \param chart the chart represented by axis \c n
       \return true if a valid axis number was specified by parameter n
       \sa setAxisDataset, chartAxes
    */
    bool axisDatasets( uint n,
                       uint& dataset,
                       uint& dataset2,
                       uint& chart ) const
    {
        bool bOk = ( n < KDCHART_MAX_AXES );
        if ( bOk ) {
            dataset  = _axisSettings[ n ].dataset;
            dataset2 = _axisSettings[ n ].dataset2;
            chart    = _axisSettings[ n ].chart;
        }
        return bOk;
    }


    typedef uint Ordinates[ KDCHART_CNT_ORDINATES ];
    /**
       Returns which ORDINATE axis(axes) is(are) representing a given chart.
       This will look for the following axes: AxisPosLeft, AxisPosRight,
       AxisPosLeft2, AxisPosRight2.

       It is allowed to specify the same variable as axis1, axis2, axis3,
       axis4 - when doing this you will get the last axis number that was
       found for this chart.  In case you are sure there is only one axis
       per chart you may act that way.

       \param chart the chart for which to return the axis number(s)
       \param cnt the count of axis numbers returned
       \param axis1 an axis representing the chart \c chart or KDCHART_NO_AXIS
       if no axis was found for this chart
       \param axis2 an 2nd axis representing the chart \c chart or KDCHART_NO_AXIS
       \param axis3 an 3rd axis representing the chart \c chart or KDCHART_NO_AXIS
       \param axis4 an 4th axis representing the chart \c chart or KDCHART_NO_AXIS
       \return true if at least one axis ist representing the chart \c chart
       \sa setAxisDataset, axisDataset
    */
    bool chartAxes( uint chart, uint& cnt, Ordinates& axes ) const
                                //uint& axis1, uint& axis2,
                                //uint& axis3, uint& axis4 ) const
    {
        cnt = 0;
        for ( uint i2 = 0; i2 < KDCHART_CNT_ORDINATES; ++i2 ) {
            axes[ i2 ] = KDCHART_NO_AXIS;
        }
        for ( uint i = 0; i < KDCHART_MAX_AXES; ++i ) {
            if (    chart == _axisSettings[ i ].chart
                 && (    KDChartAxisParams::AxisPosLeft   == i
                      || KDChartAxisParams::AxisPosRight  == i
                      || KDChartAxisParams::AxisPosLeft2  == i
                      || KDChartAxisParams::AxisPosRight2 == i ) ) {
                for( uint j = 0;  j < KDCHART_CNT_ORDINATES;  ++j ) {
                    if( KDCHART_NO_AXIS == axes[ j ] || axes[ j ] == i ) {
                        if( KDCHART_NO_AXIS == axes[ j ] ) {
                            ++cnt;
                            axes[ j ] = i;
                        }
                        break;
                    }
                }
            }
        }
        return (0 < cnt);
    }


    /**
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
    void setAxisArea( const uint n, const QRect& areaRect )
    {
        if ( n < KDCHART_MAX_AXES ) {
            _axisSettings[ n ].params.setAxisTrueAreaRect( areaRect );
            // Do not do emit changed() here!
        }
    }

    /**
       Specifies whether the axis labels start and end at the
       edges of the charts instead being positioned in the
       middle of the first data point (or the last one, resp.)

       \note If you decide to call setAxisLabelsTouchEdges() to
       to override the default settings of abscissa axes
       (AxisPosBottom, AxisPosTop, AxisPosBottom2, AxisPosTop2)
       make sure to call it again each time you have called
       setChartType() since your settings will be overwritten
       when changing the chart type.

       \param axisLabelsTouchEdges if the axis labels start and end at
       the edges of the charts instead being positioned in the
       middle of the first data point (or the last one, resp.)

       \sa axisLabelsTouchEdges
    */
    void setAxisLabelsTouchEdges( uint n, bool axisLabelsTouchEdges )
    {
        _axisSettings[ n ].params.setAxisLabelsTouchEdges(
            axisLabelsTouchEdges );
        emit changed();
    }

    /**
       Specifies the label text parameters for one axis.

       \sa KDChartAxisParams::setAxisLabelTextParams
    */
    void setAxisLabelTextParams( uint n,
                                 bool axisSteadyValueCalc = true,
                                 KDChartData axisValueStart =
                                     KDChartAxisParams::AXIS_LABELS_AUTO_LIMIT,
                                 KDChartData axisValueEnd =
                                     KDChartAxisParams::AXIS_LABELS_AUTO_LIMIT,
                                 double axisValueDelta =
                                     KDChartAxisParams::AXIS_LABELS_AUTO_DELTA,
                                 int axisDigitsBehindComma =
                                     KDChartAxisParams::AXIS_LABELS_AUTO_DIGITS,
                                 int axisMaxEmptyInnerSpan = 67,
                                 KDChartAxisParams::LabelsFromDataRow takeLabelsFromDataRow
                                 = KDChartAxisParams::LabelsFromDataRowNo,
                                 int labelTextsDataRow = 0,
                                 QStringList* axisLabelStringList = 0,
                                 QStringList* axisShortLabelsStringList = 0 )
    {
        _axisSettings[ n ].params.setAxisValues( axisSteadyValueCalc,
                axisValueStart,
                axisValueEnd,
                axisValueDelta,
                axisDigitsBehindComma,
                axisMaxEmptyInnerSpan,
                takeLabelsFromDataRow,
                labelTextsDataRow,
                axisLabelStringList,
                axisShortLabelsStringList );
        emit changed();
    }

    /**
       Specifies the parameters for one axis.

       \param axisParams the axis parameters to be used.
       \sa axisParams, AxisParams
    */
    void setAxisParams( uint n,
                        const KDChartAxisParams& axisParams )
    {
        if ( n < KDCHART_MAX_AXES ) {
            _axisSettings[ n ].params = axisParams;
            emit changed();
        }
    }

    /**
       Returns the parameters that are currently defined for axis n.

       \return all the parameters defined for axis n. If called with invalid
       value for \c n it returns the default axis parameters.
       \sa setAxisParams, AxisParams
    */
    const KDChartAxisParams& axisParams( uint n ) const
    {
        return n < KDCHART_MAX_AXES ? _axisSettings[ n ].params
               : _axisSettings[ KDCHART_MAX_AXES ].params;
    }
    /// END AXES


    /// HEADERS/FOOTERS

    /**
       The names of the Header and Footer sections.

       You may use up to 9 header and/or up to 9 footer sections.

       \li Up to three <b>leading headers</b> ( \c HdFtPosHeader0 , \c
       HdFtPosHeader0A , \c HdFtPosHeader0B ) may be drawn at the very upper
       edge of the printable area. <br>
       You might want to use those sections to show some marginal information
       like department name, print date, page number... <br>
       <b>Note:</b> Those headers <b>share the same area</b> so make sure to
       specify propper horizontal alignment for each section when using more than
       one of them. By default \c HdFtPosHeader0 has centered alignement,
       \c HdFtPosHeader0A is aligned to the left and \c HdFtPosHeader0B to the
       right side. All of them are vertically aligned to the bottom, you may
       change this at your own risk: the resulting output might look strange. <br>
       <b>also note:</b> Alignment to bottom here means their baselines are
       horizontally adjusted to each other when more than one of them is used - it
       does <b>not</b> mean the <b>lowest pixel positions</b> of their output will
       be at the very same y coordinate since those might vary depending from the
       font size and or the font size you have specified for each of the sections.

       \li Up to three <b>main headers</b> ( \c HdFtPosHeader , \c
       HdFtPosHeaderA , \c HdFtPosHeaderB ) may be drawn under the
       leading header(s). <br>
       You could use this headers to show the main information such as project name,
       chart title or period of time (e.g. census year). <br>
       Like their counterparts they share the same part of the printable area so the
       restrictions regarding alignment mentioned above apply also to these three
       sections.

       \li Up to three <b>additional headers</b> ( \c HdFtPosHeader2 , \c
       HdFtPosHeader2A , \c HdFtPosHeader2B ) may be drawn under the main
       header(s). <br>
       This headers could show additional information such as project phase, chart
       sub-title or sub-period of time (e.g. census quarter-year). <br>
       Like their counterparts they share the same part of the printable area so the
       restrictions regarding alignment mentioned above apply also to these three
       sections.

       <p>

       \li Up to three <b>main footers</b> ( \c HdFtPosFooter , \c
       HdFtPosFooterA , \c HdFtPosFooterB ) may be drawn under the chart data
       area (or the legend that might be shown below that area, resp.). <br>
       You might want to use these footers instead of (or additional to) the
       main header(s) for showing the main information... <br>
       Like their header-counterparts they share the same part of the printable area
       so the restrictions regarding alignment mentioned above apply also to these
       three sections.

       \li Up to three <b>additional footers</b> ( \c HdFtPosFooter2 , \c
       HdFtPosFooter2A , \c HdFtPosHeader2B ) may be drawn under the main
       footer(s). <br>
       This footers could show additional information instead of (or additional to)
       the additional header(s). <br>
       Like their counterparts they share the same part of the printable area so the
       restrictions regarding alignment mentioned above apply also to these three
       sections.

       \li Up to three <b>trailing footers</b> ( \c HdFtPosFooter0 , \c
       HdFtPosFooter0A , \c HdFtPosFooter0B ) may be drawn at the very lower
       edge of the printable area: <b>under</b> the other footer sections <br>
       You might want to use those sections to show some marginal information
       instead of (or additional to) the leading header(s). <br>
       Like their counterparts they share the same part of the printable area so the
       restrictions regarding alignment mentioned above apply also to these three
       sections.

       \note The names \c HdFtPosHeader or \c HdFtPosFooter are the basic names also returned by \c basicAxisPos.

       <p>
       <b>Usage:</b> Please see example code at \c setHeaderFooterText function explanation.

       \sa setHeaderFooterText,           headerFooterText
       \sa setHeaderFooterFont,           headerFooterFont
       \sa setHeaderFooterFontUseRelSize, headerFooterFontUseRelSize
       \sa setHeaderFooterFontRelSize,    headerFooterFontRelSize
    */

    /*
      Hinweise zur AEnderung/Ergaenzung des folgenden enum
      ====================================================

      Bitte beachten: dieser enum ist bewusst so spezifiziert,
      dass die Sektionen mit kleinerer 'Hoehe'
      auch kleinere Nummern haben.
      So ist es moeglich, eine Schleife zu bauen,
      die allmaehlich steigende Y-Werte fuer
      die Bereichsgrenzen liefert.

      Auch ist es wichtig, bei Ergaenzungen des enum die Gruppierung
      in Trippel beizubehalten, da die Algorithmen in
      KDChartPainter::setupGeometry() und
      KDChartPainter::paintHeaderFooter() darauf beruhen.
      Auch die Hilfs-Werte HdFtPosSTART, HdFtPosHeadersSTART,
      HdFtPosHeadersEND, HdFtPosFootersSTART,
      HdFtPosFootersEND und HdFtPosEND muessen stets auf Stand
      gehalten werden, damit huebsch alle Sektionen gezeichnet werden.
    */
    enum HdFtPos{ /// start of all sections
        HdFtPosSTART = 0,
        /// end of header sections
        HdFtPosHeadersSTART = 0,


        /// leading header (at the very top of the printable area)
        HdFtPosHeader0 = 0,
        /// another leading header, by default left-aligned
        HdFtPosHeader0A = 1,
        /// yet another leading header, by default right-aligned <br>
        HdFtPosHeader0B = 2,

        /// main header (below the leading header)
        HdFtPosHeader = 3,
        /// another main header, by default left-aligned
        HdFtPosHeaderA = 4,
        /// yet another main header, by default right-aligned <br>
        HdFtPosHeaderB = 5,

        /// additional header (below the main header)
        HdFtPosHeader2 = 6,
        /// another additional header, by default left-aligned
        HdFtPosHeader2A = 7,
        /// yet another additional header, by default right-aligned <br>
        HdFtPosHeader2B = 8,


        /// end of header sections
        HdFtPosHeadersEND = 9,
        /// start of footer sections
        HdFtPosFootersSTART = 9,


        /// main footer (below the data area or the bottom legend (if any), resp.)
        HdFtPosFooter = 9,
        /// another main footer, by default left-aligned
        HdFtPosFooterA = 10,
        /// yet another main footer, by default right-aligned <br>
        HdFtPosFooterB = 11,

        /// additional footer (below the main footer)
        HdFtPosFooter2 = 12,
        /// another additional footer, by default left-aligned
        HdFtPosFooter2A = 13,
        /// yet another additional footer, by default right-aligned <br>
        HdFtPosFooter2B = 14,

        /// trailing footer  (below the additional footer)
        HdFtPosFooter0 = 15,
        /// another trailing footer, by default left-aligned
        HdFtPosFooter0A = 16,
        /// yet another trailing footer, by default right-aligned
        HdFtPosFooter0B = 17,


        /// end of footer sections
        HdFtPosFootersEND = 17,
        /// end of all sections
        HdFtPosEND = 17 };


    /**
       Specifies the text to be displayed in the header (or footer, resp.)
       section specified by \c pos.
       The default is not to display any header text nor footer text.

       \note To learn about the different header (or footer, resp.)
       sections please see information given at \c HdFtPos enum explanation.

       \param pos the section in which the text is to be shown
       \param text the text to be shown
       \sa HdFtPos, headerFooterText()
       \sa setHeaderFooterFont,           headerFooterFont
       \sa setHeaderFooterFontUseRelSize, headerFooterFontUseRelSize
       \sa setHeaderFooterFontRelSize,    headerFooterFontRelSize
       \sa setHeader1Text(), header1Text()
       \sa setHeader2Text(), header2Text()
       \sa setFooterText(),  footerText()
    */
    void setHeaderFooterText( uint pos, const QString& text )
    {
        if ( HdFtPosEND >= pos ) {
            _hdFtParams[ pos ]._text = text;
            emit changed();
        }
    }


    /**
       Returns the text that is displayed in the header (or footer, resp.)
       section specified by \c pos.

       \note To learn about the different header (or footer, resp.)
       sections please see information given at \c HdFtPos enum explanation.

       \param pos the section for which to return the text
       \return the displayed text
       \sa HdFtPos, setHeaderFooterText()
       \sa setHeaderFooterFont,           headerFooterFont
       \sa setHeaderFooterFontUseRelSize, headerFooterFontUseRelSize
       \sa setHeaderFooterFontRelSize,    headerFooterFontRelSize
       \sa setHeader1Text(), header1Text()
       \sa setHeader2Text(), header2Text()
       \sa setFooterText(),  footerText()
    */
    QString headerFooterText( uint pos ) const
    {
    if ( HdFtPosEND >= pos )
        return _hdFtParams[ pos ]._text;
    else
        return QString::null;
    }


    /**
       Specifies the font to be used in the header (or footer,
       resp.) section specified by \c pos..
       The default is a 14pt Helvetica font.

       \note To learn about the different header (or footer, resp.)
       sections please see information given at \c HdFtPos enum explanation.

       \param pos the section in which the font is to be used
       \param font the font to use
       \sa HdFtPos, setHeaderFooterText(), headerFooterText()
       \sa headerFooterFont()
       \sa setHeaderFooterFontUseRelSize, headerFooterFontUseRelSize
       \sa setHeaderFooterFontRelSize,    headerFooterFontRelSize
       \sa setHeader1Font(), header1Font()
       \sa setHeader2Font(), header2Font()
       \sa setFooterFont(),  footerFont()
       \sa setHeader1Text(), header1Text()
       \sa setHeader2Text(), header2Text()
       \sa setFooterText(),  footerText()
    */
    void setHeaderFooterFont( uint pos, const QFont& font,
                              bool fontUseRelSize,
                              int fontRelSize )
    {
        if ( HdFtPosEND >= pos ) {
            _hdFtParams[ pos ]._font = font;
            _hdFtParams[ pos ]._fontUseRelSize = fontUseRelSize;
            _hdFtParams[ pos ]._fontRelSize = fontRelSize;
            emit changed();
        }
    }


    /**
       Returns the font used in the header (or footer, resp.)
       section specified by \c pos.

       \note To learn about the different header (or footer, resp.)
       sections please see information given at \c HdFtPos enum explanation.

       \param pos the section for which to return the font
       \return the used font
       \sa HdFtPos, setHeaderFooterText(), headerFooterText()
       \sa setHeaderFooterFont()
       \sa setHeaderFooterFontUseRelSize, headerFooterFontUseRelSize
       \sa setHeaderFooterFontRelSize,    headerFooterFontRelSize
       \sa setHeader1Font(), header1Font()
       \sa setHeader2Font(), header2Font()
       \sa setFooterFont(),  footerFont()
       \sa setHeader1Text(), header1Text()
       \sa setHeader2Text(), header2Text()
       \sa setFooterText(),  footerText()
    */
    QFont headerFooterFont( uint pos ) const
    {
        if ( HdFtPosEND >= pos )
            return _hdFtParams[ pos ]._font;
        else
            return QApplication::font();
    }


    /**
       Returns is the font size used in the header (or footer, resp.)
       section specified by \c pos will be calculated dynamically.

       \note To learn about the different header (or footer, resp.)
       sections please see information given at \c HdFtPos enum explanation.

       \param pos the section for which to return the font calculation flag
       \return the font calculation flag
       \sa HdFtPos, setHeaderFooterText(), headerFooterText()
       \sa setHeaderFooterFont()
       \sa setHeaderFooterFontUseRelSize, headerFooterFontUseRelSize
       \sa setHeaderFooterFontRelSize,    headerFooterFontRelSize
       \sa setHeader1Font(), header1Font()
       \sa setHeader2Font(), header2Font()
       \sa setFooterFont(),  footerFont()
       \sa setHeader1Text(), header1Text()
       \sa setHeader2Text(), header2Text()
       \sa setFooterText(),  footerText()
    */
    bool headerFooterFontUseRelSize( uint pos ) const
    {
        if ( HdFtPosEND >= pos )
        return _hdFtParams[ pos ]._fontUseRelSize;
        else
            return false;
    }


    /**
       Returns the factor used for dynamical calculation of
       the font size of the text in the header (or footer, resp.)
       section specified by \c pos.

       \note To learn about the different header (or footer, resp.)
       sections please see information given at \c HdFtPos enum explanation.

       \param pos the section for which to return the font size factor
       \return the font size factor
       \sa HdFtPos, setHeaderFooterText(), headerFooterText()
       \sa setHeaderFooterFont()
       \sa setHeaderFooterFontUseRelSize, headerFooterFontUseRelSize
       \sa setHeaderFooterFontRelSize,    headerFooterFontRelSize
       \sa setHeader1Font(), header1Font()
       \sa setHeader2Font(), header2Font()
       \sa setFooterFont(),  footerFont()
       \sa setHeader1Text(), header1Text()
       \sa setHeader2Text(), header2Text()
       \sa setFooterText(),  footerText()
    */
    int headerFooterFontRelSize( uint pos ) const
    {
        if ( HdFtPosEND >= pos )
        return _hdFtParams[ pos ]._fontRelSize;
        else
            return 10;
    }


    // quick&dirty functions: access Header, Header2 and Footer
    //                        without having to specify the HdFtPos
    //
    // This may be convenient for specifying simple charts with
    // up to two centered header(s) and up to one centered footer.


    /**
       Specifies the text to be displayed as the first header line. The
       default is not to display any header text.

       \note This function provided for convenience only. <br>
       To gain the <b>full</b> benefits of the header/footer management
       please consider learning about the different header (or footer,
       resp.) sections, see information given at \c HdFtPos enum
       explanation. <br>
       The 'better' way to specify content and settings of headers or
       footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
       , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
       resp.) while specifying the appropriate \c HdFtPos. <br>
       The canonical way to retrieve settings and content of headers or
       footers is to call \c headerFooterText (or \c headerFooterFont ,
       \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

       \param text the text to display
       \sa HdFtPos, setHeaderFooterText(), headerFooterText()
       \sa header2Text(), setHeader1Text(), header1Text(), setFooterText(),
       footerText()
    */
    void setHeader1Text( const QString& text )
    {
        _hdFtParams[ HdFtPosHeader ]._text = text;
        emit changed();
    }


    /**
       Returns the text that is displayed as the first header line.

       \note This function provided for convenience only. <br>
       To gain the <b>full</b> benefits of the header/footer management
       please consider learning about the different header (or footer,
       resp.) sections, see information given at \c HdFtPos enum
       explanation. <br>
       The 'better' way to specify content and settings of headers or
       footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
       , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
       resp.) while specifying the appropriate \c HdFtPos. <br>
       The canonical way to retrieve settings and content of headers or
       footers is to call \c headerFooterText (or \c headerFooterFont ,
       \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

       \return the displayed text
       \sa HdFtPos, setHeaderFooterText(), headerFooterText()
       \sa setHeader2Text(), setHeader1Text(), header1Text()
       \sa setFooterText(), footerText()
    */
    QString header1Text() const
    {
        return _hdFtParams[ HdFtPosHeader ]._text;
    }

    /**
       Specifies the font to be used for displaying the first header line.
       The default is a 14pt Helvetica font.

       \note This function provided for convenience only. <br>
       To gain the <b>full</b> benefits of the header/footer management
       please consider learning about the different header (or footer,
       resp.) sections, see information given at \c HdFtPos enum
       explanation. <br>
       The 'better' way to specify content and settings of headers or
       footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
       , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
       resp.) while specifying the appropriate \c HdFtPos. <br>
       The canonical way to retrieve settings and content of headers or
       footers is to call \c headerFooterText (or \c headerFooterFont ,
       \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

       \param font the font to use
       \sa HdFtPos, setHeaderFooterFont(), headerFooterFont()
       \sa setHeaderFooterFontUseRelSize(), headerFooterFontUseRelSize()
       \sa setHeaderFooterFontRelSize(), headerFooterFontRelSize()
       \sa header2Font(), setHeader2Text(), header2Text()
       \sa setHeader1Font(), header1Font()
       \sa setFooterFont(), footerFont()
    */
    void setHeader1Font( const QFont& font )
    {
        _hdFtParams[ HdFtPosHeader ]._font = font;
        emit changed();
    }


    /**
       Returns the font used for displaying the second header line.

       \note This function provided for convenience only. <br>
       To gain the <b>full</b> benefits of the header/footer management
       please consider learning about the different header (or footer,
       resp.) sections, see information given at \c HdFtPos enum
       explanation. <br>
       The 'better' way to specify content and settings of headers or
       footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
       , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
       resp.) while specifying the appropriate \c HdFtPos. <br>
       The canonical way to retrieve settings and content of headers or
       footers is to call \c headerFooterText (or \c headerFooterFont ,
       \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

       \return the used font
       \sa HdFtPos, setHeaderFooterFont(), headerFooterFont()
       \sa setHeaderFooterFontUseRelSize(), headerFooterFontUseRelSize()
       \sa setHeaderFooterFontRelSize(), headerFooterFontRelSize()
       \sa setHeader2Font(), setHeader2Text(), header2Text(),
       setHeader1Font(), header1Font(), setFooterFont(), footerFont()
    */
    QFont header1Font() const
    {
        return _hdFtParams[ HdFtPosHeader ]._font;
    }

    /**
       Specifies the text to be displayed as the second header line. The
       default is not to display any header text.

       \note This function provided for convenience only. <br>
       To gain the <b>full</b> benefits of the header/footer management
       please consider learning about the different header (or footer,
       resp.) sections, see information given at \c HdFtPos enum
       explanation. <br>
       The 'better' way to specify content and settings of headers or
       footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
       , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
       resp.) while specifying the appropriate \c HdFtPos. <br>
       The canonical way to retrieve settings and content of headers or
       footers is to call \c headerFooterText (or \c headerFooterFont ,
       \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

       \param text the text to display
       \sa HdFtPos, setHeaderFooterText(), headerFooterText()
       \sa header2Text(), setHeader1Text(), header1Text(), setFooterText(),
       footerText()
    */
    void setHeader2Text( const QString& text )
    {
        _hdFtParams[ HdFtPosHeader2 ]._text = text;
        emit changed();
    }


    /**
       Returns the text that is displayed as the second header line.

       \note This function provided for convenience only. <br>
       To gain the <b>full</b> benefits of the header/footer management
       please consider learning about the different header (or footer,
       resp.) sections, see information given at \c HdFtPos enum
       explanation. <br>
       The 'better' way to specify content and settings of headers or
       footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
       , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
       resp.) while specifying the appropriate \c HdFtPos. <br>
       The canonical way to retrieve settings and content of headers or
       footers is to call \c headerFooterText (or \c headerFooterFont ,
       \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

       \return the displayed text
       \sa HdFtPos, setHeaderFooterText(), headerFooterText()
       \sa setHeader2Text(), setHeader1Text(), header1Text(), setFooterText(),
       footerText()
    */
    QString header2Text() const
    {
        return _hdFtParams[ HdFtPosHeader2 ]._text;
    }

    /**
       Specifies the font to be used for displaying the second header line. The
       default is a 12pt Helvetica font.

       \note This function provided for convenience only. <br>
       To gain the <b>full</b> benefits of the header/footer management
       please consider learning about the different header (or footer,
       resp.) sections, see information given at \c HdFtPos enum
       explanation. <br>
       The 'better' way to specify content and settings of headers or
       footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
       , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
       resp.) while specifying the appropriate \c HdFtPos. <br>
       The canonical way to retrieve settings and content of headers or
       footers is to call \c headerFooterText (or \c headerFooterFont ,
       \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

       \param font the font to use
       \sa HdFtPos, setHeaderFooterFont(), headerFooterFont()
       \sa setHeaderFooterFontUseRelSize(), headerFooterFontUseRelSize()
       \sa setHeaderFooterFontRelSize(), headerFooterFontRelSize()
       \sa header2Font(), setHeader2Text(), header2Text(), setHeader1Font(), header1Font(),
       setFooterFont(), footerFont()
    */
    void setHeader2Font( const QFont& font )
    {
        _hdFtParams[ HdFtPosHeader2 ]._font = font;
        emit changed();
    }


    /**
       Returns the font used for displaying the second header line.

       \note This function provided for convenience only. <br>
       To gain the <b>full</b> benefits of the header/footer management
       please consider learning about the different header (or footer,
       resp.) sections, see information given at \c HdFtPos enum
       explanation. <br>
       The 'better' way to specify content and settings of headers or
       footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
       , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
       resp.) while specifying the appropriate \c HdFtPos. <br>
       The canonical way to retrieve settings and content of headers or
       footers is to call \c headerFooterText (or \c headerFooterFont ,
       \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

       \return the used font
       \sa HdFtPos, setHeaderFooterFont(), headerFooterFont()
       \sa setHeaderFooterFontUseRelSize(), headerFooterFontUseRelSize()
       \sa setHeaderFooterFontRelSize(), headerFooterFontRelSize()
       \sa setHeader2Font(), setHeader2Text(), header2Text(),
       setHeader1Font(), header1Font(), setFooterFont(), footerFont()
    */
    QFont header2Font() const
    {
        return _hdFtParams[ HdFtPosHeader2 ]._font;
    }



    /**
       Specifies the text to be displayed as the footer line. The
       default is not to display any footer text.

       \note This function provided for convenience only. <br>
       To gain the <b>full</b> benefits of the header/footer management
       please consider learning about the different header (or footer,
       resp.) sections, see information given at \c HdFtPos enum
       explanation. <br>
       The 'better' way to specify content and settings of headers or
       footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
       , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
       resp.) while specifying the appropriate \c HdFtPos. <br>
       The canonical way to retrieve settings and content of headers or
       footers is to call \c headerFooterText (or \c headerFooterFont ,
       \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

       \param text the text to display
       \sa HdFtPos, setHeaderFooterText(), headerFooterText()
       \sa footerText(), setHeader1Text(), header1Text(), setHeader2Text(),
       header2Text()
    */
    void setFooterText( const QString& text )
    {
        _hdFtParams[ HdFtPosFooter ]._text = text;
        emit changed();
    }


    /**
       Returns the text that is displayed as the footer line.

       \note This function provided for convenience only. <br>
       To gain the <b>full</b> benefits of the header/footer management
       please consider learning about the different header (or footer,
       resp.) sections, see information given at \c HdFtPos enum
       explanation. <br>
       The 'better' way to specify content and settings of headers or
       footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
       , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
       resp.) while specifying the appropriate \c HdFtPos. <br>
       The canonical way to retrieve settings and content of headers or
       footers is to call \c headerFooterText (or \c headerFooterFont ,
       \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

       \return the displayed text
       \sa HdFtPos, setHeaderFooterText(), headerFooterText()
       \sa setFooterText(), setHeader1Text(), header1Text(), setHeader2Text(),
       header2Text()
    */
    QString footerText() const
    {
        return _hdFtParams[ HdFtPosFooter ]._text;
    }

    /**
       Specifies the font to be used for displaying the footer line. The
       default is a 12pt Helvetica font.

       \note This function provided for convenience only. <br>
       To gain the <b>full</b> benefits of the header/footer management
       please consider learning about the different header (or footer,
       resp.) sections, see information given at \c HdFtPos enum
       explanation. <br>
       The 'better' way to specify content and settings of headers or
       footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
       , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
       resp.) while specifying the appropriate \c HdFtPos. <br>
       The canonical way to retrieve settings and content of headers or
       footers is to call \c headerFooterText (or \c headerFooterFont ,
       \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

       \param font the font to use
       \sa HdFtPos, setHeaderFooterFont(), headerFooterFont()
       \sa setHeaderFooterFontUseRelSize(), headerFooterFontUseRelSize()
       \sa setHeaderFooterFontRelSize(), headerFooterFontRelSize()
       \sa footerFont(), setFooterText(), footerText(), setHeader1Font(), header1Font(),
       setHeader2Font(), header2Font()
    */
    void setFooterFont( const QFont& font )
    {
        _hdFtParams[ HdFtPosFooter ]._font = font;
        emit changed();
    }


    /**
       Returns the font used for displaying the footer line.

       \note This function provided for convenience only. <br>
       To gain the <b>full</b> benefits of the header/footer management
       please consider learning about the different header (or footer,
       resp.) sections, see information given at \c HdFtPos enum
       explanation. <br>
       The 'better' way to specify content and settings of headers or
       footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
       , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
       resp.) while specifying the appropriate \c HdFtPos. <br>
       The canonical way to retrieve settings and content of headers or
       footers is to call \c headerFooterText (or \c headerFooterFont ,
       \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

       \return the used font
       \sa HdFtPos, setHeaderFooterFont(), headerFooterFont()
       \sa setHeaderFooterFontUseRelSize(), headerFooterFontUseRelSize()
       \sa setHeaderFooterFontRelSize(), headerFooterFontRelSize()
       \sa setFooterFont(), setFooterText(), footerText(),
       setHeader1Font(), header1Font(), setHeader2Font(), header2Font()
    */
    QFont footerFont() const
    {
        return _hdFtParams[ HdFtPosFooter ]._font;
    }

    /// END HEADERS/FOOTERS




    /**
       Constructor. Defines default values.
    */
    KDChartParams();

    /**
       Destructor. Only defined to have it virtual.
    */
    virtual ~KDChartParams();


    /**
       Reads the parameters from an XML document.
       \param doc the XML document to read from
       \return true if the parameters could be read, false if a file
       format error occurred
       \sa saveXML
    */
    bool loadXML( const QDomDocument& doc );


    /**
       Saves the parameters to an XML document.

       \return the XML document that represents the parameters
       \sa loadXML
    */
    QDomDocument saveXML( bool withPI = true ) const;


    friend QTextStream& operator<<( QTextStream& s, const KDChartParams& p );
    friend QTextStream& operator>>( QTextStream& s, KDChartParams& p );

signals:
    /**
       This signal is emitted when any of the chart parameters has changed.
    */
    void changed();

private:
    /**
       Specifies the chart type.
    */
    ChartType _chartType;

    /**
       Specifies the additional chart type.
    */
    ChartType _additionalChartType;

    /**
       Specifies how many of the values should be shown. -1 means all
       available values.
    */
    int _numValues;


    /**
       Stores the SourceMode and the chart(s) for one dataset.
    */
    class ModeAndChart
    {
public:
ModeAndChart() : _mode( UnknownMode ),
        _chart( KDCHART_NO_CHART )
        {}
        ModeAndChart( SourceMode mode, uint chart )
: _mode( mode ), _chart( chart )
        {}

        SourceMode mode() const
        {
            return _mode;
        };
        uint chart() const
        {
            return _chart;
        };

        void setMode( SourceMode mode )
        {
            _mode = mode;
        };
        void setChart( uint chart )
        {
            _chart = chart;
        };
private:
        SourceMode _mode;
        uint _chart;
    };

    /**
       Stores the SourceMode and the chart(s) for the datasets.
    */
    typedef QMap < uint, ModeAndChart > ModeAndChartMap;
    ModeAndChartMap _dataSourceModeAndChart;

    /**
       Stores whether setChartSourceMode() was called or not
    */
    bool _setChartSourceModeWasUsed;

    /**
       Stores the colors for the datasets.
    */
    QMap < uint, QColor > _dataColors;

    /**
       Stores the highest dataset number for which a SourceMode has been
       defined. Note that if you define a value for a dataset but not for a
       dataset with a lower number (and there is neither a default value), the
       source mode for that dataset with the lower number will be undefined.
    */
    uint _maxDatasetSourceMode;

    /**
       Stores a factor to be used to adjust the
       built-in brightness of shadow colors in
       3-dimensional drawings like e.g. 3D Bar charts.
    */
    double _shadowBrightnessFactor;

    /**
       Specifies whether shadowed colors are used for 3D effects. Only used
       for 3D effects in charts that support these.
    */
    bool _threeDShadowColors;


    /**
       Stores the highest dataset number for which a color has been
       defined. Note that if you define a value for a dataset but not for a
       dataset with a lower number (and there is neither a default value), the
       color for that dataset with the lower number will be undefined.
    */
    uint _maxDatasetColor;

    /**
       Stores the shadow colors that are somewhat darker than the colors in
       _dataColors.
    */
    QMap < uint, QColor > _dataColorsShadow1;

    /**
       Stores the shadow colors that are even more darker than the colors in
       _dataColorsShadow1;
    */
    QMap < uint, QColor > _dataColorsShadow2;

    /**
       Stores the color to be used for outlines of data displays.
    */
    QColor _outlineDataColor;

    /**
       Stores the line width to be used for outlines of data displays.
    */
    uint _outlineDataLineWidth;

    /**
       Stores the line style to be used for outlines of data displays.
    */
    PenStyle _outlineDataLineStyle;


    /**
       Specifies the bar chart subtype. Only used when chartType() ==
       Bar
    */
    BarChartSubType _barChartSubType;

    /**
       Specifies whether the engine should draw the bars in 3D. Only
       used if chartType() == Bar.
    */
    bool _threeDBars;

    /**
       Specifies the angle used for 3D display. Only used if
       threeDBars == true.
    */
    int _threeDBarAngle;

    /**
       Specifies the depth of the 3D Effect used for 3D bars
       in relation to the bar width.
       Only used if chartType() == Bar and threeDBars() == true.
    */
    double _threeDBarDepth;


    /**
       The cosine in rad of the angle for 3D display. Only used if
       threeDBars == true.
    */
    double _cosThreeDBarAngle;

    /**
       Specifies the number of pixels between two dataset values.
    */
    int _datasetGap;

    /**
       Specifies if the value set by \c setDatasetGap
       is a per mille value of the chart data area width.
    */
    bool _datasetGapIsRelative;

    /**
       Specifies the number of pixels between each value block.
    */
    int _valueBlockGap;

    /**
       Specifies if the value set by \c setValueBlockGap
       is a per mille value of the chart data area width.
    */
    bool _valueBlockGapIsRelative;

    /// LINES/AREAS-specific
    /**
       Specifies the line chart subtype. Only used when chartType() ==
       Line
    */
    LineChartSubType _lineChartSubType;

    /**
       Specifies whether line markers should be drawn. Only used when
       chartType() == Line.
    */
    bool _lineMarker;

    QMap < uint, LineMarkerStyle > _lineMarkerStyles;

    /**
       Stores the highest dataset number for which a line marker has been
       defined. Note that if you define a value for a dataset but not for a
       dataset with a lower number (and there is neither a default value), the
       line marker for that dataset with the lower number will be undefined.
    */
    uint _maxDatasetLineMarkerStyle;

    /**
       Stores the size of the line markers.
    */
    QSize _lineMarkerSize;


    /**
       Stores the line width of lines in line charts.
    */
    uint _lineWidth;


    /**
       Specifies the area chart subtype. Only used when chartType() ==
       Area
    */
    AreaChartSubType _areaChartSubType;


    /**
       Specifies whether the area above or below the value points should
       be filled.
    */
    AreaLocation _areaLocation;



    // PIES/RINGS
    /**
       Stores whether pies or ring segments are drawn exploding.
    */
    bool _explode;

    /**
    Stores the default explode factor of pie or ring segments.
    */
    double _explodeFactor;

    /**
    Stores the list of segment-specific explode factors of pie or
    ring segments.
    */
    QMap<int,double> _explodeFactors;

    /**
    Stores the list of pies or ring segments to explode.
    */
    QValueList<int> _explodeList;

    /**
       Stores whether pies will have a 3D effect.
    */
    bool _threeDPies;

    /**
       Stores the height of the pie 3D effect.
    */
    int _threeDPieHeight;

    /**
       Stores the starting point of the pie circle.
    */
    int _pieStart;

    /**
       Stores the starting point of the ring circle.
    */
    int _ringStart;

    /**
       Stores whether the ring thickness is relativ or constant.
    */
    bool _relativeRingThickness;


    // HI-LO CHARTS

    /**
       Stores the HiLo subtype.
    */
    HiLoChartSubType _hiLoChartSubType;


    /**
       Stores whether the low values
       will be printed under their respective entries.
    */
    bool _hiLoChartPrintLowValues;


    /**
       Stores the font to be used for printing the low values.
    */
    QFont _hiLoChartLowValuesFont;


    /**
       Stores whether the font size to be used for printing the low
       values is calculated dynamically.
    */
    bool _hiLoChartLowValuesUseFontRelSize;


    /**
       Stores the relative size (in per mille of the chart width) of
       font size to be used for printing the low values.
    */
    int _hiLoChartLowValuesFontRelSize;


    /**
       Stores the colour of the font size to be used for printing the
       low values.
    */
    QColor _hiLoChartLowValuesColor;

    /**
       Stores whether the high values will be printed under their
       respective entries.
    */
    bool _hiLoChartPrintHighValues;


    /**
       Stores the font to be used for printing the high values.
    */
    QFont _hiLoChartHighValuesFont;


    /**
       Stores whether the font size to be used for printing the high
       values is calculated dynamically.
    */
    bool _hiLoChartHighValuesUseFontRelSize;


    /**
       Stores the relative size (in per mille of the chart width) of
       font size to be used for printing the high values.
    */
    int _hiLoChartHighValuesFontRelSize;


    /**
       Stores the colour of the font size to be used for printing the
       high values.
    */
    QColor _hiLoChartHighValuesColor;

    /**
       Stores whether the open values
       will be printed under their respective entries.
    */
    bool _hiLoChartPrintOpenValues;


    /**
       Stores the font to be used for printing the open values.
    */
    QFont _hiLoChartOpenValuesFont;


    /**
       Stores whether the font size to be used for printing the open
       values is calculated dynamically.
    */
    bool _hiLoChartOpenValuesUseFontRelSize;


    /**
       Stores the relative size (in per mille of the chart width) of
       font size to be used for printing the open values.
    */
    int _hiLoChartOpenValuesFontRelSize;


    /**
       Stores the colour of the font size to be used for printing the
       open values.
    */
    QColor _hiLoChartOpenValuesColor;

    /**
       Stores whether the close values will be printed under their
       respective entries.
    */
    bool _hiLoChartPrintCloseValues;


    /**
       Stores the font to be used for printing the close values.
    */
    QFont _hiLoChartCloseValuesFont;


    /**
       Stores whether the font size to be used for printing the close
       values is calculated dynamically.
    */
    bool _hiLoChartCloseValuesUseFontRelSize;


    /**
       Stores the relative size (in per mille of the chart width) of
       font size to be used for printing the close values.
    */
    int _hiLoChartCloseValuesFontRelSize;


    /**
       Stores the colour of the font size to be used for printing the
       close values.
    */
    QColor _hiLoChartCloseValuesColor;


    // GANTT CHARTS
    /**
       Stores the Gantt chart subtype.
    */
    GanttChartSubType _ganttChartSubType;
    /**
       Stores the temporal resolution of the gantt chart.
    */
    GanttChartTemporalResolution _ganttChartTemporalResolution;
    /**
       Stores whether the Start Time values
       will be printed at or inside of their respective entries.
    */
    bool _ganttChartPrintStartValues;
    /**
       Stores where to print the Start Time values and how to align them.
    */
    GanttChartTextAlign _ganttChartStartValuesAlign;
    /**
       Stores the datetime format to be used for printing the
       Start Time values.
    */
    QString _ganttChartStartValuesDateTimeFormat;
    /**
       Stores the font to be used for printing the
       Start Time values.
    */
    QFont _ganttChartStartValuesFont;
    /**
       Stores  whether the font size to be used for printing the
       Start Time values is calculated dynamically.
    */
    bool _ganttChartStartValuesUseFontRelSize;
    /**
       Stores the relative size (in per mille of the chart width)
       of font size to be used for printing the
       Start Time values.
    */
    int _ganttChartStartValuesFontRelSize;
    /**
       Stores the colour of the font size to be used for printing the
       Start Time values.
    */
    QColor _ganttChartStartValuesColor;

    /**
       Stores whether the End Time values
       will be printed at or inside of their respective entries.
    */
    bool _ganttChartPrintEndValues;
    /**
       Stores where to print the End Time values and how to align them.
    */
    GanttChartTextAlign _ganttChartEndValuesAlign;
    /**
       Stores the datetime format to be used for printing the
       End Time values.
    */
    QString _ganttChartEndValuesDateTimeFormat;
    /**
       Stores the font to be used for printing the
       End Time values.
    */
    QFont _ganttChartEndValuesFont;
    /**
       Stores  whether the font size to be used for printing the
       End Time values is calculated dynamically.
    */
    bool _ganttChartEndValuesUseFontRelSize;
    /**
       Stores the relative size (in per mille of the chart width)
       of font size to be used for printing the
       End Time values.
    */
    int _ganttChartEndValuesFontRelSize;
    /**
       Stores the colour of the font size to be used for printing the
       End Time values.
    */
    QColor _ganttChartEndValuesColor;

    /**
       Stores whether the Durations
       will be printed at or inside of their respective entries.
    */
    bool _ganttChartPrintDurations;
    /**
       Stores where to print the Durations and how to align them.
    */
    GanttChartTextAlign _ganttChartDurationsAlign;
    /**
       Stores the datetime format to be used for printing the
       Durations.
    */
    QString _ganttChartDurationsDateTimeFormat;
    /**
       Stores the font to be used for printing the
       Durations.
    */
    QFont _ganttChartDurationsFont;
    /**
       Stores  whether the font size to be used for printing the
       Durations is calculated dynamically.
    */
    bool _ganttChartDurationsUseFontRelSize;
    /**
       Stores the relative size (in per mille of the chart width)
       of font size to be used for printing the
       Durations.
    */
    int _ganttChartDurationsFontRelSize;
    /**
       Stores the colour of the font size to be used for printing the
       Durations.
    */
    QColor _ganttChartDurationsColor;



    // LEGENDS

    /**
       Stores the legend position.
    */
    LegendPosition _legendPosition;

    /**
       Stores the legend source.
    */
    LegendSource _legendSource;

    /**
       Stores the manually set legend texts.
    */
    QMap < int, QString > _legendText;


    /**
       Stores the color for drawing the legend texts.
    */
    QColor _legendTextColor;

    /**
       Stores the font for drawing the legends.
    */
    QFont _legendFont;

    /**
       Specifies whether the size of the legend font is to be calculated
       on a relative basis.

       \sa setLegendFontUseRelSize, setLegendFontRelSize
       \sa setLegendFont
    */
    bool _legendFontUseRelSize;

    /**
       Specifies the per mille basis for calculating the relative
       legend font size.

       \sa setLegendFontRelSize, setLegendFontUseRelSize
       \sa setLegendFont
    */
    int _legendFontRelSize;


    /**
       Stores the legend title text.
    */
    QString _legendTitleText;


    /**
       Stores the color for drawing the legend title.
    */
    QColor _legendTitleTextColor;


    /**
       Stores the font for drawing the legend title.
    */
    QFont _legendTitleFont;

    /**
       Specifies whether the size of the legend title font is to be
       calculated on a relative basis.

       \sa setLegendTitleFontUseRelSize, setLegendTitleFontRelSize
       \sa setLegendTitleFont
    */
    bool _legendTitleFontUseRelSize;

    /**
       Specifies the per mille basis for calculating the relative
       legend title font size.

       \sa setLegendTitleFontRelSize, setLegendTitleFontUseRelSize
       \sa setLegendTitleFont
    */
    int _legendTitleFontRelSize;


    /**
       Stores the number of pixels between the legend and the data display
    */
    uint _legendSpacing;


    /// AXES (private)
    /**
       Specifies all the settings of all the axis \em plus one more
       parameter set containing the build-in defauls axis settings.
    */
    struct AxisSettings
    {
        uint dataset;
        uint dataset2;
        uint chart;
        KDChartAxisParams params;
        AxisSettings()
        {
            dataset = KDCHART_NO_DATASET;
            dataset2 = KDCHART_NO_DATASET;
            chart = KDCHART_NO_CHART;
        }
    };

    // 13 == KDCHART_MAX_AXES + 1
    AxisSettings _axisSettings[ 13 ];
    /// END AXES (private)


    // HEADER/FOOTER (private)

    /**
       Specifies all the settings of all the header sections
       and all the footer sections.
    */
    struct HdFtParams
    {
        QString _text;
        QFont _font;
        bool _fontUseRelSize;
        int _fontRelSize;
        HdFtParams()
        {
            _font = QFont( "helvetica", 10,
                           QFont::Normal, false );
            _fontUseRelSize = true;
            _fontRelSize = 8; // per default quite small
        }
    };

    HdFtParams _hdFtParams[ HdFtPosEND + 1 ];
    // END HEADER/FOOTER (private)
};


/**
   Writes the KDChartParams object p as an XML document to the text stream s.

   \param s the text stream to write to
   \param p the KDChartParams object to write
   \return the text stream after the write operation
*/
QTextStream& operator<<( QTextStream& s, const KDChartParams& p );


/**
   Reads the an XML document from the text stream s into the
   KDChartParams object p

   \param s the text stream to read from
   \param p the KDChartParams object to read into
   \return the text stream after the read operation
*/
QTextStream& operator>>( QTextStream& s, KDChartParams& p );


#endif
