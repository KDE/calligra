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
   Library General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KDCHARTAXISPARAMS_H__
#define __KDCHARTAXISPARAMS_H__

#include <qfont.h>
#include <qcolor.h>
#include <qmap.h>
#include <qobject.h>
#include <qtextstream.h>

#include "KDChartData.h"
class KDChartParams;

/** \file KDChartAxisParams.h
    \brief Header for chart axis parameters.

    This file is used to access chart axis parameters
    and should be looked at in combination with the
    axis access methods in \c KDChartParams.h

    \see KDChartParams.h
*/



/**
   Bundles all parameters of one axis including the type except the
   actual data displayed. Serializing an object of this type plus the
   data displayed is enough to be able to recreate the chart later.
*/
class KDChartAxisParams : public QObject
{
    Q_OBJECT

public:
    /**
        Use this to specify that the axis label limits
        are to be calculated automatically.

        \sa setAxisValues
    */
    static KDChartData AXIS_LABELS_AUTO_LIMIT;

    /**
        Use this to specify that the step-width from one label
        to the other shall be calculated automatically.

        \sa setAxisValues
    */
    static const double AXIS_LABELS_AUTO_DELTA;
    /**
        Use this to specify that the number of digits to be shown
        on the axis labels behind the comma is to be calculated
        automatically.

        \sa setAxisValues
    */
    static const int AXIS_LABELS_AUTO_DIGITS;
    /**
        Use this to reset the grid colour to its default value
        for re-doing changes made by calling \c setAxisGridColor().

        \sa setAxisGridColor
    */
    static const QColor DEFAULT_AXIS_GRID_COLOR;
    /**
        Use this to specify that the width of the axis grid lines
        shall be equal to the width of the axis line.

        \sa setAxisGridLineWidth
    */
    static const int AXIS_GRID_AUTO_LINEWIDTH;
    /**
        Use this to specify that the zero-line shall NOT be omitted
        in case all of the data are grouped far away from zero.

        \sa setAxisValues
    */
    static const int AXIS_IGNORE_EMPTY_INNER_SPAN;

    /**
        Use this to prevent \c setAxisValues from changing the current
        setting of _axisMaxEmptyInnerSpan.

        \sa setAxisValues
    */
    static const int DONT_CHANGE_EMPTY_INNER_SPAN_NOW;


    /**
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
    enum AxisType { AxisTypeUnknown,
                    AxisTypeEAST,
                    AxisTypeNORTH,
                    AxisUP };

    /**
       Specifies the axis type. The default is unknown (AxisTypeUnknown).

       \note Normally you will not have to specify the AxisType since it
       will be set automatically in KDChartAxesPainter::setDefaultAxesTypes()
       called by c'tor of class KDChartAxesPainter

       \param axisType the axis type to use
       \sa axisType, AxisType
    */
    void setAxisType( AxisType axisType )
    {
        _axisType = axisType;
        emit changed();
    }

    /**
       Returns the axis type configured in this params object.

       \return the axis type configured in this params object.
       \sa setAxisType, AxisType
    */
    AxisType axisType() const
    {
        return _axisType;
    }


    /**
    Converts the specified axis type enum to a string representation.

    \param type the axis type enum to convert
    \return the string representation of the axis type enum
    */
    static QString axisTypeToString( AxisType type ) {
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
    static AxisType stringToAxisType( const QString& type ) {
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
        in per thousand  of the average value of the printable area height
        and width - producing a result looking fine even if the chart is
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

       \sa setAxisAreaMode, axisAreaMode, setAxisCalcMode, axisCalcMode
       \sa setAxisSettings, axisSettings
    */

    // Achtung: Wird AxisPos erweitert, sind auch MAX_AXES
    //          und AxisPosEND zu erhoehen
    enum AxisPos { AxisPosSTART = 0,

                   AxisPosBottom = 0,
                   AxisPosLeft = 1,
                   AxisPosLowerRightEdge = 2,

                   AxisPosTop = 3,
                   AxisPosRight = 4,
                   AxisPosLowerLeftEdge = 5,

                   // diese Markierung muss jeweils mitgepflegt werden,
                   // wenn AxisPos erweitert werden sollte.
                   AxisPosAdditionalsSTART = 5,

                   AxisPosBottom2 = 6,
                   AxisPosLeft2 = 7,
                   AxisPosLowerRightEdge2 = 8,

                   AxisPosTop2 = 9,
                   AxisPosRight2 = 10,
                   AxisPosLowerLeftEdge2 = 11,

                   // auch diese Markierung muss jeweils mitgepflegt werden,
                   // wenn AxisPos erweitert werden sollte.
                   AxisPosEND = 11 };


    /**
        The basic axis type.

       \param pos the axis type to be analyzed
       \return the basic axis type (Bottom, Left, Top or Right)
    */
    static AxisPos basicAxisPos( uint pos )
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
        case AxisPosLowerLeftEdge:
            ret = AxisPosLeft;
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
        case AxisPosLowerLeftEdge2:
            ret = AxisPosLeft;
        default: {
                qDebug( "IMPLEMENTATION ERROR: type missing in KDChartAcisParams::basicAxisPos()" );
                ASSERT( ret != AxisPos( pos ) );
            }
        }
        return ret;
    }

    /**
       Specifies if the axis will be drawn. The default is false.

       \param axisVisible set true to make visible the respective axis.
       \sa axisVisible, AxisVisible
    */
    void setAxisVisible( bool axisVisible )
    {
        _axisVisible = axisVisible;
        emit changed();
    }

    /**
       Returns if the axis will be drawn.

       \return if the axis is visible or not.
       \sa setAxisVisible, AxisVisible
    */
    bool axisVisible() const
    {
        return _axisVisible;
    }


    /**
    The axis size, determines how to calculate the size of area used
       by the axis - i.e. the width of left axis area (or the right
       one, resp.) or the height of the top axis area (or the bottom one,
       resp.).
       \li \c AxisAreaModeFixedSize (default) value will be taken
              from \c AxisAreaMinSize()
       \li \c AxisAreaModeAutoSize will be calculated automatically based on
              the size of the labels to be drawn
       \li \c AxisAreaModeMinMaxSize will be calculated automatically but
              bearing user defined limits in mind

       \sa setAxisAreaMode, axisAreaMode, AxisAreaMode
       \sa setAxisAreaMin, setAxisAreaMax, setAxisArea
    */
    enum AxisAreaMode { AxisAreaModeFixedSize,
                        AxisAreaModeAutoSize,
                        AxisAreaModeMinMaxSize };

    /**
       Specifies the axis size mode.
       The default is auto size (AxisAreaModeAutoSize).

       \param axisAreaMode the way how to find out the
            size of the area where the axis shall be drawn.
       \sa axisAreaMode, AxisAreaMode
       \sa setAxisAreaMin, setAxisAreaMax, setAxisArea
    */
    void setAxisAreaMode( AxisAreaMode axisAreaMode )
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
    AxisAreaMode axisAreaMode() const
    {
        return _axisAreaMode;
    }

    /**
    Converts the specified axis area mode enum to a string representation.

    \param mode the axis area mode enum to convert
    \return the string representation of the mode enum
    */
    static QString axisAreaModeToString( AxisAreaMode mode ) {
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
    static AxisAreaMode stringToAxisAreaMode( const QString& type ) {
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
        of the width (or the height, resp.) of the printable area to
        be used. This will make the axis look the same even if scaled
        to very different size.
        Note: This value will be used for AxisAreaModeFixedSize charts
              as well.

       \sa axisAreaMin, axisAreaMax, setAxisAreaMode, setAxisAreaMax
       \sa setAxisArea
    */
    void setAxisAreaMin( int axisAreaMin )
    {
        _axisAreaMin = axisAreaMin;
        emit changed();
    }

    /**
       Returns the axis area minimum width (or height, resp.).

       \return the axis area minimum width (or height, resp.).
       \sa setAxisAreaMin, setAxisAreaMax, setAxisArea
    */
    int axisAreaMin() const
    {
        return _axisAreaMin;
    }

    /**
       Specifies the axis area maximum width (or height, resp.).

       \param axisAreaMax the axis area maximum width (or height, resp.)
        If value is negative, the absolute value is per thousand
        of the width (or the height, resp.) of the printable area to
        be used. This will make the axis look the same even if scaled
        to very different size.
        Note: This value will be ignored for AxisAreaModeFixedSize charts.

       \sa axisAreaMax, axisAreaMin, setAxisAreaMode, setAxisAreaMin
       \sa setAxisArea
    */
    void setAxisAreaMax( int axisAreaMax )
    {
        _axisAreaMax = axisAreaMax;
        emit changed();
    }

    /**
       Returns the axis area maximum width (or height, resp.).

       \return the axis area maximum width (or height, resp.).
       \sa setAxisAreaMax, setAxisAreaMin, setAxisArea
    */
    int axisAreaMax() const
    {
        return _axisAreaMax;
    }

    /**
       Specifies the axis area size mode and the
       minimum and maximum width (or height, resp.) of the area.
       This method is here for convenience, see \c setAxisAreaMode,
       \c setAreaMin and \c setAreaMax for details.

       \param axisAreaMode the way how to find out the
            size of the area where the axis shall be drawn.
       \param axisAreaMin the axis area minimum width (or height, resp.)
       \param axisAreaMax the axis area maximum width (or height, resp.)

       \sa setAxisAreaMode, setAxisAreaMin, setAxisAreaMax
    */
    void setAxisArea( AxisAreaMode axisAreaMode,
                      int axisAreaMin,
                      int axisAreaMax )
    {
        _axisAreaMode = axisAreaMode;
        _axisAreaMin = axisAreaMin;
        _axisAreaMax = axisAreaMax;
        emit changed();
    }

    /**
       The axis calculation mode.

       \sa setAxisCalcMode, axisCalcMode, AxisCalcMode
    */
    enum AxisCalcMode { AxisCalcLinear, AxisCalcLogarithmic };

    /**
       Specifies the axis calculation mode.
       The default is linear calculation (AxisCalcLinear).

       \param axisCalcMode the axis calculation mode to be used.
       \sa axisCalcMode, AxisCalcMode
    */
    void setAxisCalcMode( AxisCalcMode axisCalcMode )
    {
        _axisCalcMode = axisCalcMode;
        emit changed();
    }

    /**
       Returns the axis calculation mode configured in this params object.

       \return the axis calculation mode configured in this params object.
       \sa setAxisCalcMode, AxisCalcMode
    */
    AxisCalcMode axisCalcMode() const
    {
        return _axisCalcMode;
    }

    /**
    Converts the specified axis calc mode enum to a string representation.

    \param mode the axis calc mode enum to convert
    \return the string representation of the mode enum
    */
    static QString axisCalcModeToString( AxisCalcMode mode ) {
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
    static AxisCalcMode stringToAxisCalcMode( const QString& type ) {
        if( type == "Linear" )
            return AxisCalcLinear;
        else if( type == "Logarithmic" )
            return AxisCalcLogarithmic;
        else // should not happen
            return AxisCalcLinear;
    }


    /**
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
    void setAxisTrueAreaSize( int axisTrueAreaSize )
    {
        _axisTrueAreaSize = axisTrueAreaSize;
    }

    /**
       Returns the true axis area width (or height, resp.)
       as calculated and drawn.

       \return the true axis area width (or height, resp.).
       \sa setAxisAreaMax, setAxisAreaMin, setAxisArea
    */
    int axisTrueAreaSize() const
    {
        return _axisTrueAreaSize;
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
    void setAxisTrueAreaRect( const QRect& axisTrueAreaRect )
    {
        _axisTrueAreaRect = axisTrueAreaRect;
    }

    /**
       Returns the true axis area rectangle
       as calculated and drawn.

       \return the true axis area rectangle
       \sa setAxisAreaMax, setAxisAreaMin, setAxisArea
    */
    QRect axisTrueAreaRect() const
    {
        return _axisTrueAreaRect;
    }


    /**
       Specifies whether the axis sub-delimiters should be drawn.

       \note If true and axisShowGrid is also true the grid on the
        chart data area will show a thin dotted line for each sub-delimiter
        (or a line with a pattern defined by \c setAxisGridSubStyle, resp.)

       \param axisShowSubDelimiters if true sub-delimiters will be drawn at this axis.
       \sa axisShowSubDelimiters, setAxisShowGrid, setAxisGridSubStyle
    */
    void setAxisShowSubDelimiters( bool axisShowSubDelimiters )
    {
        _axisShowSubDelimiters = axisShowSubDelimiters;
        emit changed();
    }

    /**
       Returns whether the axis sub-delimiters will be drawn.

       \return whether the axis sub-delimiters will be drawn.
       \sa setAxisShowSubDelimiters
    */
    bool axisShowSubDelimiters() const
    {
        return _axisShowSubDelimiters;
    }


    /**
       Specifies whether the axis line should be drawn.

       \param axisLineVisible if true the line of this axis will be drawn.
       \sa axisLineVisible
    */
    void setAxisLineVisible( bool axisLineVisible )
    {
        _axisLineVisible = axisLineVisible;
        emit changed();
    }

    /**
       Returns whether the axis line should be drawn.

       \return whether the axis line should be drawn.
       \sa setAxisLineVisible
    */
    bool axisLineVisible() const
    {
        return _axisLineVisible;
    }

    /**
       Specifies the axis line width.

       \param axisLineWidth the axis line width.
        If value is negative, the absolute value is per thousand
        of average value of the height and the width of the
        the printable area height to be used. This will make the
        axis look the same even if scaled to very different size.

       \sa axisLineWidth
    */
    void setAxisLineWidth( int axisLineWidth )
    {
        _axisLineWidth = axisLineWidth;
        emit changed();
    }

    /**
       Returns the axis line width.

       \return the axis line width.
       \sa setAxisLineWidth
    */
    int axisLineWidth() const
    {
        return _axisLineWidth;
    }

    /**
       Specifies the actual axis line width, as calculated and drawn.

       \Note You may not use this internal function.

       \param axisTrueLineWidth the actual axis line width,
        as calculated and drawn.

       \sa axisTrueLineWidth
    */
    void setAxisTrueLineWidth( int axisTrueLineWidth )
    {
        _axisTrueLineWidth = axisTrueLineWidth;
    }

    /**
       Returns the axis true line width, as calculated and drawn.

       \return the axis true line width, as calculated and drawn.
       \sa setAxisTrueLineWidth
    */
    int axisTrueLineWidth() const
    {
        return _axisTrueLineWidth;
    }

    /**
       Specifies the axis line colour.

       \param axisLineColor the axis line colour.
       \sa axisLineColor
    */
    void setAxisLineColor( QColor axisLineColor )
    {
        _axisLineColor = axisLineColor;
        emit changed();
    }

    /**
       Returns the axis line colour.

       \return the axis line colour.
       \sa setAxisLineColor
    */
    QColor axisLineColor() const
    {
        return _axisLineColor;
    }

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
    void setAxisShowGrid( bool axisShowGrid )
    {
        _axisShowGrid = axisShowGrid;
        emit changed();
    }

    /**
       Returns whether a grid should be drawn at the chart data area.

       \return whether a grid should be drawn at the chart data area.
       \sa setAxisShowGrid, setAxisShowSubDelimiters
    */
    bool axisShowGrid() const
    {
        return _axisShowGrid;
    }

    /**
       Specifies the axis grid colour.

       To reset the color to the built-in default value
       please call \c setAxisGridColor( DEFAULT_AXIS_GRID_COLOR )

       \param axisGridColor the axis grid color.
       \sa axisGridColor, setAxisShowGrid
    */
    void setAxisGridColor( QColor axisGridColor )
    {
        _axisGridColor = axisGridColor;
        emit changed();
    }

    /**
       Returns the axis grid color.

       \return the axis grid color.
       \sa setAxisGridColor, setAxisShowGrid
    */
    QColor axisGridColor() const
    {
        return _axisGridColor;
    }


    /**
       Specifies the width of the axis grid lines.

       \note Normally you would <b>not</b> call this function since
       grid lines in most cases look best in their default line
       width: the same witdh as the axis line they belong to.
       However when combining multiple charts sharing the same
       abscissa axes but having their ordinates differently scaled
       you might want to reduce the line width of the respective
       grid lines and use different grid colours to show two
       grids at the same time.  In such cases it might also be a
       good idea to deactivate \c setAxisShowSubDelimiters thus
       avoiding the dotted sub-grid lines or to set their
       style to Qt::NoPen to get sub-delimiters on the axis
       but no sub-grid lines.

       You may use setAxisGridLineWidth( AXIS_GRID_AUTO_LINEWIDTH )
       to reset the value to its default: being automatically
       adjusted to the width of the axis line.

       \param axisGridLineWidth the width of the axis grid lines.
        If value is negative, the absolute value is per thousand
        of average value of the height and the width of the
        the printable area height to be used. This will make the
        grid look the same even if scaled to very different size.
       \sa axisGridLineWidth, setAxisGridColor, setAxisGridStyle
       \sa setAxisShowGrid, setAxisShowSubDelimiters
    */
    void setAxisGridLineWidth( int axisGridLineWidth )
    {
        _axisGridLineWidth = axisGridLineWidth;
        emit changed();
    }

    /**
       Returns the width of the axis grid lines.
       (see explanation given with \c setAxisGridLineWidth )

       \return the width of the axis grid lines.
       \sa setAxisGridLineWidth, setAxisShowGrid
    */
    int axisGridLineWidth() const
    {
        return _axisGridLineWidth;
    }


    /**
       Specifies the axis grid line pattern.

       \param axisGridStyle the axis grid line pattern.
       \sa axisGridStyle, setAxisShowGrid
    */
    void setAxisGridStyle( PenStyle axisGridStyle )
    {
        _axisGridStyle = axisGridStyle;
        emit changed();
    }

    /**
       Returns the axis grid line pattern.

       \return the axis grid line pattern.
       \sa setAxisGridStyle, setAxisShowGrid
    */
    PenStyle axisGridStyle() const
    {
        return _axisGridStyle;
    }


    /**
       Specifies the axis grid line pattern for the thin lines
       showing the sub-delimiter values.

       \param axisGridStyle the axis grid line pattern for the thin lines
       showing the sub-delimiter values.
       \sa axisGridSubStyle, setAxisGridStyle, axisGridStyle
       \sa setAxisShowGrid
    */
    void setAxisGridSubStyle( PenStyle axisGridSubStyle )
    {
        _axisGridSubStyle = axisGridSubStyle;
        emit changed();
    }

    /**
       Returns the axis grid line pattern for the thin lines
       showing the sub-delimiter values.

       \return the axis grid line pattern for the thin lines
       showing the sub-delimiter values.
       \sa setAxisGridSubStyle
       \sa setAxisGridStyle, axisGridStyle, setAxisShowGrid
    */
    PenStyle axisGridSubStyle() const
    {
        return _axisGridSubStyle;
    }


    /**
       Specifies the colour of the zero-line
       that is drawn if zero is not at the lower
       edge of the chart.

       \note The zero line will only be drawn for the left
       ordinate axis (or for the right one if no left axis
       was specified, resp.).

       \param axisZeroLineColor the zero-line colour.
       \sa axisZeroLineColor
    */
    void setAxisZeroLineColor( QColor axisZeroLineColor )
    {
        _axisZeroLineColor = axisZeroLineColor;
        emit changed();
    }

    /**
       Returns the colour used for the zero-value line
       that is drawn if zero is not at the lower
       edge of the chart.

       \return the zero-line colour.
       \sa setAxisZeroLineColor
    */
    QColor axisZeroLineColor() const
    {
        return _axisZeroLineColor;
    }

    /**
       Specifies whether the axis labels should be drawn.

       \param axisLabelsVisible if true the labels of this axis will be
                                drawn.
       \sa axisLabelsVisible
    */
    void setAxisLabelsVisible( bool axisLabelsVisible )
    {
        _axisLabelsVisible = axisLabelsVisible;
        emit changed();
    }

    /**
       Returns whether the axis labels should be drawn.

       \return whether the axis labels should be drawn.
       \sa setAxisLabelsVisible
    */
    bool axisLabelsVisible() const
    {
        return _axisLabelsVisible;
    }


    /**
       Specifies whether the axis labels start and end at the
       edges of the charts instead being positioned in the
       middle of the first data point (or the last one, resp.)

       \param axisLabelsTouchEdges if the axis labels start and end at the
       edges of the charts instead being positioned in the
       middle of the first data point (or the last one, resp.)

       \sa axisLabelsTouchEdges
    */
    void setAxisLabelsTouchEdges( bool axisLabelsTouchEdges )
    {
        _axisLabelsTouchEdges = axisLabelsTouchEdges;
        emit changed();
    }

    /**
       Returns whether the axis labels start and end at the
       edges of the charts instead being positioned in the
       middle of the first data point (or the last one, resp.)

       \return whether the axis labels start and end at the
       edges of the charts instead being positioned in the
       middle of the first data point (or the last one, resp.)
       \sa setAxisLabelsTouchEdges
    */
    bool axisLabelsTouchEdges() const
    {
        return _axisLabelsTouchEdges;
    }


    /**
       Specifies the axis labels font.

       \note The font size will be ignored if \c useFontSize is false,
       in this case the font size will be calculated dynamically using
       the value stored by you calling setAxisLabelsFontRelSize().

       \param axisLabelsFont the font to be used for the axis' labels.

       \sa axisLabelsFont, axisLabelsFontRelSize, setAxisLabelsFontRelSize
       \sa setAxisLabelsFontUseRelSize
    */
    void setAxisLabelsFont( QFont axisLabelsFont, bool useFontSize )
    {
        _axisLabelsFont = axisLabelsFont;
        _axisLabelsFontUseRelSize = ! useFontSize;
        emit changed();
    }

    /**
       Returns the axis labels font.

       \return the axis labels font.
       \sa setAxisLabelsFont, setAxisLabelsFontRelSize
       \sa axisLabelsFontRelSize
    */
    QFont axisLabelsFont() const
    {
        return _axisLabelsFont;
    }

    /**
       Specifies whether axis labels shall be drawn
       using relative font size.

       \param axisLabelsFontUseRelSize whether axis labels shall be drawn
        using relative font size.
        If true the absolute value of the value set by \c
        setAxisLabelsFontRelSize is per thousand
        of of the average value of the printable area height and width
        to be used. This will make the axis look the same even if scaled
        to very different size.

       \sa setAxisLabelsFontRelSize, setAxisLabelsFont
    */
    void setAxisLabelsFontUseRelSize( bool axisLabelsFontUseRelSize )
    {
        _axisLabelsFontUseRelSize = axisLabelsFontUseRelSize;
        emit changed();
    }

    /**
       Returns whether the fix axis font size is used.

       \return whether the fix axis labels font size is used.
       \sa setAxisLabelsFontRelSize, setAxisLabelsFont
    */
    bool axisLabelsFontUseRelSize() const
    {
        return _axisLabelsFontUseRelSize;
    }

    /**
       Specifies the axis labels relative font size.

       \param axisLabelsFontRelSize the relative axis font size.
        If this value unequals zero the absolute value is per thousand
        of of the average value of the printable area height and width
        to be used. This will make the axis look the same even if scaled
        to very different size.

       \sa setAxisLabelsFontUseRelSize, setAxisLabelsFont
    */
    void setAxisLabelsFontRelSize( int axisLabelsFontRelSize )
    {
        _axisLabelsFontRelSize = axisLabelsFontRelSize;
        emit changed();
    }

    /**
       Returns the axis labels relative font size.

       \return the axis labels relative font size.
       \sa setAxisLabelsFontRelSize, setAxisLabelsFontUseRelSize
    */
    int axisLabelsFontRelSize() const
    {
        return _axisLabelsFontRelSize;
    }

    /**
       Specifies the axis labels colour.

       \param axisLabelsColor the axis labels colour.
       \sa axisLabelsColor
    */
    void setAxisLabelsColor( QColor axisLabelsColor )
    {
        _axisLabelsColor = axisLabelsColor;
        emit changed();
    }

    /**
       Returns the axis labels colour.

       \return the axis labels colour.
       \sa setAxisLabelsColor
    */
    QColor axisLabelsColor() const
    {
        return _axisLabelsColor;
    }


    /**
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
    enum LabelsFromDataRow { LabelsFromDataRowYes,
                             LabelsFromDataRowNo,
                             LabelsFromDataRowGuess };

    /**
    Converts the specified labels from data row enum to a string
    representation.

    \param mode the enum to convert
    \return the string representation of the mode enum
    */
    static QString labelsFromDataRowToString( LabelsFromDataRow mode ) {
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
    static LabelsFromDataRow stringToLabelsFromDataRow( const QString& type ) {
        if( type == "Yes" )
            return LabelsFromDataRowYes;
        else if( type == "No" )
            return LabelsFromDataRowNo;
        else if( type == "Guess" )
            return LabelsFromDataRowGuess;
        else // should not happen
            return LabelsFromDataRowGuess;
    }



    /*

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
            AXIS_LABELS_AUTO_LIMIT() as start value (see below).
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
       \li \b or pass <b>AXIS_LABELS_AUTO_LIMIT()</b> as start value
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
            <b>In either case</b> (AXIS_LABELS_AUTO_LIMIT() or numerical value)
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
                   KDChartData( 1964.0 ), AXIS_LABELS_AUTO_LIMIT(), 1, 0 );
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
                      AXIS_LABELS_AUTO_LIMIT(),
                      AXIS_LABELS_AUTO_DELTA,
                      AXIS_LABELS_AUTO_DIGITS,
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
       \param axisMaxEmptyInnerSpan specifies the percentage of the y-axis range that may to contain NO data entries, if - and only if - axisValueStart (or axisValueEnd, resp.) is set to AXIS_LABELS_AUTO_LIMIT(). To prevent \c setAxisValues from changing the current setting you may specify DONT_CHANGE_EMPTY_INNER_SPAN_NOW here, to deactivate taking into account the inner span entirely just use AXIS_IGNORE_EMPTY_INNER_SPAN.
       \param takeLabelsFromDataRow specifies whether the labels texts shall be taken from a special row (reserved for this in each dataset) or not.
       \param axisLabelStringList points to a \c QStringList containing the label texts to be used.
       \param axisShortLabelsStringList points to a \c QStringList containing the label texts to be used in case their full-size counterparts cannot be shown due to limited axis area size.
       \sa setAxisValueStart, setAxisValueEnd, setAxisValueDelta
       \sa axisValueStart axisValueEnd, axisValueDelta
       \sa LabelsFromDataRow, axisLabelTextsFormDataRow, axisLabelTexts
       \sa axisSteadyValueCalc
    */
    void setAxisValues( bool axisSteadyValueCalc = true,
                        KDChartData axisValueStart = AXIS_LABELS_AUTO_LIMIT,
                        KDChartData axisValueEnd = AXIS_LABELS_AUTO_LIMIT,
                        double axisValueDelta = AXIS_LABELS_AUTO_DELTA,
                        int axisDigitsBehindComma = AXIS_LABELS_AUTO_DIGITS,
                        int axisMaxEmptyInnerSpan = DONT_CHANGE_EMPTY_INNER_SPAN_NOW,
                        LabelsFromDataRow takeLabelsFromDataRow = LabelsFromDataRowNo,
                        int labelTextsDataRow = 0,
                        QStringList* axisLabelStringList = 0,
                        QStringList* axisShortLabelsStringList = 0 )
    {
        _axisSteadyValueCalc = axisSteadyValueCalc;
        _axisValueStart = axisValueStart;
        _axisValueEnd = axisValueEnd;
        _axisValueDelta = axisValueDelta;
        _axisDigitsBehindComma = axisDigitsBehindComma;
        if ( DONT_CHANGE_EMPTY_INNER_SPAN_NOW != axisMaxEmptyInnerSpan ) {
            if ( 100 < axisMaxEmptyInnerSpan
                    || 1 > axisMaxEmptyInnerSpan )
                _axisMaxEmptyInnerSpan = AXIS_IGNORE_EMPTY_INNER_SPAN;
            else
                _axisMaxEmptyInnerSpan = axisMaxEmptyInnerSpan;
        }
        _takeLabelsFromDataRow = takeLabelsFromDataRow;
        _labelTextsDataRow = labelTextsDataRow;
        _axisLabelStringList = axisLabelStringList;
        _axisShortLabelsStringList = axisShortLabelsStringList;
        // label texts must be recalculated?
        setAxisLabelTexts( 0 );
        setTrueAxisLowHighDelta( 0.0, 0.0, 0.0 );
        emit changed();
    }

    /**
        Specifies the lower limit for the axis labels: the start value.

        \param axisValueStart the lower limit for the axis labels: the start
        value.
        \sa setAxisValues, setAxisValueEnd, setAxisValueDelta
        \sa axisValueStart, axisValueEnd, axisValueDelta
        \sa axisLabelsFromDataRow, axisLabelTexts
        \sa axisLabelStringList, axisShortLabelsStringList
    */
    void setAxisValueStart( const KDChartData axisValueStart )
    {
        _axisValueStart = axisValueStart;
    }

    /**
        Returns the lower limit for the axis labels: the start value.

        \return the lower limit for the axis labels: the start value.
        \sa setAxisValues, setAxisValueStart, setAxisValueEnd, setAxisValueDelta
        \sa axisValueEnd, axisValueDelta
        \sa axisLabelsFromDataRow, axisLabelTexts
        \sa axisLabelStringList, axisShortLabelsStringList
    */
    KDChartData axisValueStart() const
    {
        return _axisValueStart;
    }

    /**
        Specifies the upper limit for the axis labels: the end value.

        \param axisValueStart the upper limit for the axis labels: the end
        value.
        \sa setAxisValues, setAxisValueStart, setAxisValueDelta
        \sa axisValueStart, axisValueEnd, axisValueDelta
        \sa axisLabelsFromDataRow, axisLabelTexts
        \sa axisLabelStringList, axisShortLabelsStringList
    */
    void setAxisValueEnd( const KDChartData axisValueEnd )
    {
        _axisValueEnd = axisValueEnd;
    }

    /**
        Returns the upper limit for the axis labels: the end value.

        \return the upper limit for the axis labels: the end value.
        \sa setAxisValues, setAxisValueStart, setAxisValueEnd, setAxisValueDelta
        \sa axisValueStart, axisValueDelta
        \sa axisLabelsFromDataRow, axisLabelTexts
        \sa axisLabelStringList, axisShortLabelsStringList
    */
    KDChartData axisValueEnd() const
    {
        return _axisValueEnd;
    }

    /**
        Specifies the DELTA value for the axis labels: the distance
        between two labels.

        \param axisValueDelta the DELTA value for the axis labels: the distance
        between two labels.
        \sa setAxisValues
        \sa setAxisValueStart, setAxisValueEnd, setAxisValueDelta
        \sa axisValueStart, axisValueEnd, axisValueDelta
        \sa axisLabelsFromDataRow, axisLabelTexts
        \sa axisLabelStringList, axisShortLabelsStringList
    */
    void setAxisValueDelta( const double axisValueDelta )
    {
        _axisValueDelta = axisValueDelta;
    }

    /**
        Returns the DELTA value for the axis labels: the distance
        between two labels.

        \return the DELTA value for the axis labels: the distance
        between two labels.
        \sa setAxisValues
        \sa setAxisValueStart, setAxisValueEnd, setAxisValueDelta
        \sa axisValueStart, axisValueEnd
        \sa axisLabelsFromDataRow, axisLabelTexts
        \sa axisLabelStringList, axisShortLabelsStringList
    */
    double axisValueDelta() const
    {
        return _axisValueDelta;
    }


    /**
       Specifies the true axis lower and upper limit values of the axis
       and the exact Delta value between the axis delimiters.

       \param nLow/nHigh/nDelta the true values as they were calculated.

        \note Do <b>not call</b> this function unless you are knowing
        exactly what you are doing. \c setAxisTrueAreaSize is normally
        reserved for internal usage by methods calculating the axis
        label texts. Thus the signal \c changed() is not sended here.

       \sa trueAxisLow, trueAxisHigh, trueAxisDelta
       \sa setAxisArea
    */
    void setTrueAxisLowHighDelta( double nLow, double nHigh, double nDelta )
    {
        _trueLow = nLow;
        _trueHigh = nHigh;
        _trueDelta = nDelta;
    }
    /**
        Returns the <b>true</b> start value of the ordinate axis labels
        as is was calculated and set by \c setTrueAxisLowHighDelta.

        \return the true lower limit of the axis labels
        \sa setAxisValues
        \sa trueAxisHigh, trueAxisDelta
    */
    double trueAxisLow() const
    {
        return _trueLow;
    }
    /**
        Returns the <b>true</b> end value of the ordinate axis labels
        as is was calculated and set by \c setTrueAxisLowHighDelta.

        \return the true upper limit of the axis labels
        \sa setAxisValues
        \sa trueAxisLow, trueAxisDelta
    */
    double trueAxisHigh() const
    {
        return _trueHigh;
    }
    /**
        Returns the <b>true</b> delta value of the ordinate axis labels
        as is was calculated and set by \c setTrueAxisLowHighDelta.

        \return the true delta value of the axis labels
        \sa setAxisValues
        \sa trueAxisLow, trueAxisHigh
    */
    double trueAxisDelta() const
    {
        return _trueDelta;
    }

    /**
        Specifies the not-rounded screen positions where drawing of
        this axis zero line started.

        \Note You may not call this internal function - it is reserved for
        internal usage of methodes needing to know the zero-line offsets

        \param Pt the not-rounded screen positions where drawing of
        this axis zero line started.

        \sa axisZeroLineStartX, axisZeroLineStartY
    */
    void setAxisZeroLineStart( double x, double y )
    {
        _axisZeroLineStartX = x;
        _axisZeroLineStartY = y;
    }
    /**
        Returns the not-rounded x-position where drawing of
        this axis zero line started. This function needed
        when painting the data of isometric axes (bars, lines, dots...).

        \return the unrounded x-position where drawing of
        this axis zero line started.

        \sa setAxisZeroLineStart, axisZeroLineStartY
    */
    double axisZeroLineStartX() const
    {
        return _axisZeroLineStartX;
    }
    /**
        Returns the not-rounded y-position where drawing of
        this axis zero line started. This function needed
        when painting the data of isometric axes (bars, lines, dots...).

        \return the unrounded y-position where drawing of
        this axis zero line started.

        \sa setAxisZeroLineStart, axisZeroLineStartX
    */
    double axisZeroLineStartY() const
    {
        return _axisZeroLineStartY;
    }

    /**
        Specifies the number of digits to be printed behind the comma
        on the axis labels.

        \param digits the number of digits to be printed behind the comma
        on the axis labels.

        \sa axisDigitsBehindComma
    */
    void setAxisDigitsBehindComma( const int digits )
    {
        _axisDigitsBehindComma = digits;
    }
    /**
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
    int axisDigitsBehindComma() const
    {
        return _axisDigitsBehindComma;
    }

    /**
        Specifies the percentage of the y-axis range that may to contain NO
        data entries, if - and only if - axisValueStart (or axisValueEnd,
        resp.) is set to AXIS_LABELS_AUTO_LIMIT().

        \param maxEmpty the percentage of the y-axis range that may to contain NO
        data entries, if - and only if - axisValueStart (or axisValueEnd,
        resp.) is set to AXIS_LABELS_AUTO_LIMIT().

        \sa axisMaxEmptyInnerSpan
    */
    void setAxisMaxEmptyInnerSpan( const int maxEmpty )
    {
        _axisMaxEmptyInnerSpan = maxEmpty;
    }
    /**
        Returns the percentage of the y-axis range that may to contain NO
        data entries, if - and only if - axisValueStart (or axisValueEnd,
        resp.) is set to AXIS_LABELS_AUTO_LIMIT().

        \note If more space is empty the zero-line will not be included info the chart but the lowest (or highest, resp.) entry of the axis will be shifted accordingly.
        \sa setAxisValues
    */
    int axisMaxEmptyInnerSpan() const
    {
        return _axisMaxEmptyInnerSpan;
    }

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
    void setLabelTextsFormDataRow( int row, LabelsFromDataRow mode )
    {
        _labelTextsDataRow = row;
        _takeLabelsFromDataRow = mode;
    }

    /**
       Returns whether the axis labels will be taken from the associated dataset.

       \return whether the axis limits will be taken from the associated dataset.
       \sa setAxisValues
       \sa axisValueStart, axisValueEnd
       \sa axisLabelsFromDataRow, axisLabelTexts
    */
    LabelsFromDataRow axisLabelTextsFormDataRow() const
    {
        return _takeLabelsFromDataRow;
    }

    /**
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
    int labelTextsDataRow() const
    {
        return _labelTextsDataRow;
    }

    /**
        Specifies the \c QStringList where the axis label texts are to
        be taken from - regarding to Start, End and/or Delta value, see
        \c setAxisValues.
        The second parameter specifies a list of strings that are to
        be displayed in case the axis area size is too small to show
        the full-size names.

        \note The complementary functions to this one are \c
        axisLabelStringList() and \c axisShortLabelsStringList().
        To retrieve the texts that are <b>actually</b>
        drawn at the axis labels you should rather call
        \c axisLabelTexts().

        \sa axisLabelStringList, axisShortLabelsStringList
        \sa setAxisValues, setLabelTextsFormDataRow
        \sa axisLabelTexts
    */
    void setAxisLabelStringLists( QStringList* axisLabelStringList,
                                  QStringList* axisShortLabelsStringList )
    {
        _axisLabelStringList = axisLabelStringList;
        _axisShortLabelsStringList = axisShortLabelsStringList;
    }

    /**
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
    QStringList* axisLabelStringList() const
    {
        return _axisLabelStringList;
    }

    /**
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
    QStringList* axisShortLabelsStringList() const
    {
        return _axisShortLabelsStringList;
    }

    /**
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
    const QStringList* axisLabelTexts() const
    {
        return _axisLabelTextsDirty ? 0 : &_axisLabelTexts;
    }

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
    */
    void setAxisLabelTexts( const QStringList* axisLabelTexts )
    {
        _axisLabelTexts.clear();
        _axisLabelTextsDirty = ( 0 == axisLabelTexts );
        if ( !_axisLabelTextsDirty )
            _axisLabelTexts = *axisLabelTexts;
    }
    /**
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
    void setAxisLabelTextsDirty( bool axisLabelTextsDirty )
    {
        _axisLabelTextsDirty = axisLabelTextsDirty;
    }
    /**
        Returns whether the label texts that are <b>actually</b> drawn
        at the axis labels are unknown. If \c false, they could
        successfully be retrieved by \c axisLabelTexts().

        \return whether the texts that are <b>actually</b> drawn at
        the axis labels are unknown.

        \sa setAxisLabelStringLists, setLabelTextsFormDataRow, setAxisValues
    */
    bool axisLabelTextsDirty() const
    {
        return _axisLabelTextsDirty;
    }


    /**
        Specifies whether label values shall be calculataed based upon the associated dataset values (normally this is true for ordinate axes) or based upon some string list (as you might expect it for abscissa labels).

        \sa setAxisValues
    */
    void setAxisSteadyValueCalc( bool axisSteadyValueCalc )
    {
        _axisSteadyValueCalc = axisSteadyValueCalc;
    }
    /**
        Returns whether label values shall be calculataed based upon the associated dataset values (normally this is true for ordinate axes) or based upon some string list (as you might expect it for abscissa labels).

        \sa setAxisValues
    */
    bool axisSteadyValueCalc() const
    {
        return _axisSteadyValueCalc;
    }


    /**
        Constructor. Define default values:

                   - do not show this axis at all,
                   - use circa 100 per thousand of the printable area
                     width (or height, resp.) for drawing this axis,
                   - calc mode linear,
                   - line is visible,
                   - line width: 3 per mille of the average value of
                                 the printable area height and width
                   - line color: black,
                   - labels are visible,
                   - labels font:      helvetica
                   - labels font size: 20 / 1000 of the average value of
                                          the printable area height and width
                   - labels color:     black,
                   - this axis will be used for an ordinate
                   - label value limits will be calculated automatically,
    */
    KDChartAxisParams()
    {
        _axisType = AxisTypeUnknown;
        _axisVisible = false;
        _axisAreaMode = AxisAreaModeFixedSize;
        _axisAreaMin = -100;
        _axisAreaMax = 0;
        _axisTrueAreaSize = 0;
        _axisZeroLineStartX = 0.0;
        _axisZeroLineStartY = 0.0;

        _axisShowSubDelimiters = true;

        _axisLineWidth = -3; // == 3/1000
        _axisTrueLineWidth = 1;
        _axisLineVisible = true;
        _axisLineColor = QColor( 0, 0, 0 );

        _axisShowGrid = false;
        _axisGridColor = DEFAULT_AXIS_GRID_COLOR;
        _axisGridLineWidth = AXIS_GRID_AUTO_LINEWIDTH;
        _axisGridStyle = Qt::SolidLine;
        _axisGridSubStyle = Qt::DotLine;
        _axisZeroLineColor = QColor( 0x00, 0x00, 0x80 );

        _axisLabelsVisible = true;
        _axisLabelsFont = QFont( "helvetica", 12,
                                 QFont::Normal, false );
        _axisLabelsFontUseRelSize = true;
        _axisLabelsFontRelSize = 20;
        _axisLabelsColor = QColor( 0, 0, 0 );

        _axisSteadyValueCalc = true;
        _axisValueStart = AXIS_LABELS_AUTO_LIMIT;
        _axisValueEnd = AXIS_LABELS_AUTO_LIMIT;
        _axisValueDelta = AXIS_LABELS_AUTO_DELTA;
        _axisDigitsBehindComma = AXIS_LABELS_AUTO_DIGITS;
        _axisMaxEmptyInnerSpan = 90;
        _takeLabelsFromDataRow = LabelsFromDataRowNo;
        _labelTextsDataRow = 0;
        _axisLabelStringList = 0;
        _axisShortLabelsStringList = 0;

        setAxisLabelTexts( 0 );
        setTrueAxisLowHighDelta( 0.0, 0.0, 0.0 );
    }

    /**
       Destructor. Only defined to have it virtual.
    */
    virtual ~KDChartAxisParams();

    /*
        Copy-c'tor: durch Aufruf der Kopiermethode,
        siehe auch Zuweisungsoperators.
    */
    KDChartAxisParams( const KDChartAxisParams& R ) : QObject()
    {
        deepCopy( *this, R );
    }

    /*
        Zuweisungsoperator: durch Aufruf der Kopiermethode,
        siehe auch Copy-c'tor.
    */
    KDChartAxisParams& operator=( const KDChartAxisParams& R )
    {
        if ( this != &R )
            deepCopy( *this, R );
        return *this;
    }

    static void deepCopy( KDChartAxisParams& D, const KDChartAxisParams& R )
    {
        D._axisType = R._axisType;
        D._axisVisible = R._axisVisible;
        D._axisAreaMode = R._axisAreaMode;
        D._axisAreaMin = R._axisAreaMin;
        D._axisAreaMax = R._axisAreaMax;
        D._axisCalcMode = R._axisCalcMode;
        D._axisTrueAreaSize = R._axisTrueAreaSize;
        D._axisTrueAreaRect = R._axisTrueAreaRect;
        D._axisZeroLineStartX = R._axisZeroLineStartX;
        D._axisZeroLineStartY = R._axisZeroLineStartY;
        D._axisShowSubDelimiters = R._axisShowSubDelimiters;
        D._axisLineVisible = R._axisLineVisible;
        D._axisLineWidth = R._axisLineWidth;
        D._axisTrueLineWidth = R._axisTrueLineWidth;
        D._axisLineColor = R._axisLineColor;
        D._axisShowGrid = R._axisShowGrid;
        D._axisGridColor = R._axisGridColor;
        D._axisGridLineWidth = R._axisGridLineWidth;
        D._axisGridStyle = R._axisGridStyle;
        D._axisGridSubStyle = R._axisGridSubStyle;
        D._axisZeroLineColor = R._axisZeroLineColor;
        D._axisLabelsVisible = R._axisLabelsVisible;
        D._axisLabelsFont = R._axisLabelsFont;
        D._axisLabelsFontUseRelSize = R._axisLabelsFontUseRelSize;
        D._axisLabelsFontRelSize = R._axisLabelsFontRelSize;
        D._axisLabelsColor = R._axisLabelsColor;

        D._axisSteadyValueCalc = R._axisSteadyValueCalc;
        D._axisValueStart = R._axisValueStart;
        D._axisValueEnd = R._axisValueEnd;
        D._axisValueDelta = R._axisValueDelta;
        D._axisDigitsBehindComma = R._axisDigitsBehindComma;
        D._axisMaxEmptyInnerSpan = R._axisMaxEmptyInnerSpan;
        D._takeLabelsFromDataRow = R._takeLabelsFromDataRow;
        D._labelTextsDataRow = R._labelTextsDataRow;
        D._axisLabelStringList = R._axisLabelStringList;
        D._axisShortLabelsStringList = R._axisShortLabelsStringList;
        D._axisLabelTextsDirty = R._axisLabelTextsDirty;


        D._axisLabelTexts = R._axisLabelTexts;
        D._trueHigh = R._trueHigh;
        D._trueLow = R._trueLow;
        D._trueDelta = R._trueDelta;
    }

    friend QTextStream& operator<<( QTextStream& s, const KDChartParams& p );
    friend QTextStream& operator>>( QTextStream& s, KDChartParams& p );

signals:
        /**
           This signal is emitted when any of the chart axis
           parameters have changed.
        */
        void changed();


private:
    /**
       Specifies the axis type.

        \sa setAxisType
    */
    AxisType _axisType;

    /**
        Specifies whether this axis is to be drawn. False by default.

        \sa setAxisVisible
    */
    bool _axisVisible;

    /**
       Specifies whether the axis labels start and end at the
       edges of the charts instead being positioned in the
       middle of the first data point (or the last one, resp.)

       \sa axisLabelsTouchEdges
    */
    bool _axisLabelsTouchEdges;

    /**
       Specifies how to find out the size of the area to be
       used by this axis.

       \sa setAxisAreaMode, setAxisAreaMin, setAxisAreaMax, setAxisArea
    */
    AxisAreaMode _axisAreaMode;
    /**
        Specifies the minimum axis area width (or height, resp.).

        \sa setAxisAreaMin, setAxisAreaMode, setAxisAreaMax, setAxisArea
    */
    int _axisAreaMin;
    /**
        Specifies the maximum axis area width (or height, resp.).

       \sa setAxisAreaMax, setAxisAreaMode, setAxisAreaMin, setAxisArea
    */
    int _axisAreaMax;
    /**
       Specifies the axis calculation mode.

        \sa setAxisCalcMode
    */
    AxisCalcMode _axisCalcMode;
    /**
        Specifies the axis area width (or height, resp.)
        as it was calculated and drawn.

       \sa setAxisAreaMax, setAxisAreaMode, setAxisAreaMin, setAxisArea
    */
    int _axisTrueAreaSize;
    /**
       Specifies the true axis area rectangle
       as it was calculated and drawn.

       \sa setAxisAreaMax, setAxisAreaMin, setAxisArea
    */
    QRect _axisTrueAreaRect;

    /**
       Specifies whether the axis sub-delimiters will be drawn.

       \sa setAxisShowSubDelimiters
    */
    bool _axisShowSubDelimiters;

    /**
        Specifies whether the axis line is visible or not.

        \sa setAxisLineVisible
    */
    bool _axisLineVisible;
    /**
        Specifies the axis line width.

        \sa setAxisLineWidth
    */
    int _axisLineWidth;
    /**
        Specifies the actual axis line width, as calculated and drawn.

        \sa setAxisTrueLineWidth
    */
    int _axisTrueLineWidth;
    /**
        Specifies the axis line colour.

        \sa setAxisLineColor
    */
    QColor _axisLineColor;

    /**
       Specifies whether a grid will be drawn at the chart data area.

       \sa setAxisShowGrid
    */
    bool _axisShowGrid;

    /**
        Specifies the axis grid colour.

        \sa setAxisGridColor, setAxisShowGrid
    */
    QColor _axisGridColor;

    /**
        Specifies the width of the axis grid lines.

        \sa setAxisGridLineWidth
    */
    int _axisGridLineWidth;

    /**
        Specifies the axis grid line pattern for main grid lines.

        \sa setAxisGridStyle, setAxisShowGrid
    */
    PenStyle _axisGridStyle;
    /**
        Specifies the axis grid line pattern for sub-delimiter grid lines.

        \sa setAxisGridSubStyle, setAxisGridStyle, setAxisShowGrid
    */
    PenStyle _axisGridSubStyle;

    /**
        Specifies the zero-line colour.

        \sa setAxisZeroLineColor
    */
    QColor _axisZeroLineColor;

    /**
        Specifies whether the axis' labels are visible or not.

        \sa setAxisLabelsVisible
    */
    bool _axisLabelsVisible;

    /**
        Specifies the axis labels font.

        \sa setAxisLabelsFont
        \sa setAxisLabelsFontUseRelSize, setAxisLabelsFontRelSize
    */
    QFont _axisLabelsFont;

    /**
        Specifies whether the size of the label font is to be calculated
        on a relative basis.

        \sa setAxisLabelsFontUseRelSize, setAxisLabelsFontRelSize
        \sa setAxisLabelsFont
    */
    bool _axisLabelsFontUseRelSize;

    /**
        Specifies the per mille basis for calculating the relative
        axis labels font size.

        \sa setAxisLabelsFontRelSize, setAxisLabelsFontUseRelSize
        \sa setAxisLabelsFont
    */
    int _axisLabelsFontRelSize;

    /**
        Specifies the axis labels color.

        \sa setAxisLabelsColor
    */
    QColor _axisLabelsColor;


    /**
        Specifies whether label values shall be calculataed based upon the associated dataset values (normally this is true for ordinate axes) or based upon some string list (as you might expect it for abscissa labels).

        \sa setAxisValues
    */
    bool _axisSteadyValueCalc;
    /**
        Specifies the lower limit for the axis labels: the start value.

        \sa setAxisValues
    */
    KDChartData _axisValueStart;
    /**
        Specifies the higher limit for the axis labels: the end value.

        \sa setAxisValues
    */
    KDChartData _axisValueEnd;
    /**
        Specifies the DELTA value for the axis labels: the distance
        between two labels.

        \sa setAxisValues
    */
    double _axisValueDelta;

    /**
        Specifies the <b>true</b> start value of the axis labels
        as is was calculated and set by \c setTrueAxisLowHighDelta.

        \sa setAxisValues
        \sa trueAxisLow, trueAxisHigh, trueAxisDelta
    */
    double _trueLow;
    /**
        Specifies the <b>true</b> end value of the axis labels
        as is was calculated and set by \c setTrueAxisLowHighDelta.

        \sa setAxisValues
        \sa trueAxisLow, trueAxisHigh, trueAxisDelta
    */
    double _trueHigh;
    /**
        Specifies the <b>true</b> delta value of the axis labels
        as is was calculated and set by \c setTrueAxisLowHighDelta.

        \sa setAxisValues
        \sa trueAxisLow, trueAxisHigh, trueAxisDelta
    */
    double _trueDelta;

    /**
        Specifies the not-rounded screen x-position where drawing of
        this axis zero line started.
    */
    double _axisZeroLineStartX;
    /**
        Specifies the not-rounded screen y-position where drawing of
        this axis zero line started.
    */
    double _axisZeroLineStartY;

    /**
        Specifies the number of digits to be printed behind the comma
        on the axis labels.

        \sa setAxisValues
    */
    int _axisDigitsBehindComma;

    /**
        Specifies the percentage of the y-axis range that may to contain NO
        data entries, if - and only if - axisValueStart (or axisValueEnd,
        resp.) is set to AXIS_LABELS_AUTO_LIMIT.

        \sa setAxisValues
    */
    int _axisMaxEmptyInnerSpan;

    /**
        Specifies whether the axis labels shall be taken directly
        from the entries of a data row.

        \sa setAxisValues
    */
    LabelsFromDataRow _takeLabelsFromDataRow;
    /**
        Specifies a data row which the axis labels shall be taken from.

        \sa setAxisValues
    */
    int _labelTextsDataRow;
    /**
        Specifies a QStringList containing the label texts to be used.

        \sa _axisShortLabelsStringList
        \sa setAxisValues, _axisLabelTexts, _axisLabelTextsDirty
    */
    QStringList* _axisLabelStringList;
    /**
        Specifies a QStringList containing the label texts to be used
        in case the axis area is not wide enough to show their full-size
        counterparts.

        \sa _axisLabelStringList
        \sa setAxisValues, _axisLabelTexts, _axisLabelTextsDirty
    */
    QStringList* _axisShortLabelsStringList;
    /**
        Contains the label texts <b>actually</b> being used.

        \sa setAxisValues, _axisLabelStringList, _axisLabelTextsDirty
    */
    QStringList _axisLabelTexts;
    /**
        Specifies whether the QStringList _axisLabelTexts content is invalid.

        \sa setAxisValues, _axisLabelTexts
    */
    bool _axisLabelTextsDirty;
};

#endif
