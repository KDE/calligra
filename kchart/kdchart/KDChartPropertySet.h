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
#ifndef __KDCHARTPROPERTYSET__
#define __KDCHARTPROPERTYSET__


#include <KDXMLTools.h>
#include <KDChartEnums.h>


/**
   \file KDChartPropertySet.h

   \brief Property settings of a single KDChart data cell.
*/

/**
   \class KDChartPropertySet KDChartPropertySet.h
   \brief Property settings of a single KDChart data cell.

   \note The property settings feature currently is supported by
   normal (== not 3-dimensional!) Line Charts only. Future releases
   of KDChart will offer a rich set of options to use cell-specific
   properties for other chart types as well...

   The KDChartPropertySet class stores several settings to be
   used by the painter classes to decide how to visualize the
   respective data cell.
   Each of the settings may be specified either by giving a
   value for it or by using the ID of another KDChartPropertySet
   instance: If the later the value of that instance is used
   instead of the value that is stored locally.

   \sa KDChartData::setPropertySet
   \sa KDChartParams::KDCHART_PROPSET_NORMAL_DATA, KDChartParams::KDCHART_PROPSET_TRANSPARENT_DATA
   \sa KDChartParams::registerProperties
*/
class KDChartPropertySet
{
    friend class KDChartParams;

public:
    /**
       Special property IDs.

       \li \c UndefinedID (the default value)
       used to indicate that <b>no</b> ID was set for
       members \c mIdLineWidth, \c mIdLineColor,
       \c mIdLineStyle, \c mIdShowMarker and the like...

       \li \c OwnID used for referencing a property set's *own* ID.

       \sa KDChartPropertySet
    */
    enum SpecialDataPropertyID {
        UndefinedID = -2,
        OwnID = -1
    };

    /**
       default constructor setting all values to undefined
       and name to empty string (default for QString)
    */
    KDChartPropertySet() :
        mOwnID(               UndefinedID ),
        mIdLineWidth(         UndefinedID ),
        mIdLineColor(         UndefinedID ),
        mIdLineStyle(         UndefinedID ),
        mIdShowMarker(        UndefinedID ),
        mIdExtraLinesAlign(   UndefinedID ),
        mIdExtraLinesInFront( UndefinedID ),
        mIdExtraLinesLength(  UndefinedID ),
        mIdExtraLinesWidth(   UndefinedID ),
        mIdExtraLinesColor(   UndefinedID ),
        mIdExtraLinesStyle(   UndefinedID ),
        mIdExtraMarkersAlign( UndefinedID ),
        mIdExtraMarkersSize(  UndefinedID ),
        mIdExtraMarkersColor( UndefinedID ),
        mIdExtraMarkersStyle( UndefinedID ),
        mIdShowBar(           UndefinedID ),
        mIdBarColor(          UndefinedID )
        { fillValueMembersWithDummyValues(); }

    /**
       Constructor setting all values to undefined.

       This constructor may be used to initialize a property set
       specifying neither property IDs nor special property values

       \param name (may be empty) a name describing this property set.
    */
    KDChartPropertySet( const QString& name ) :
        mOwnID( UndefinedID ),
        mName( name ),
        mIdLineWidth(         UndefinedID ),
        mIdLineColor(         UndefinedID ),
        mIdLineStyle(         UndefinedID ),
        mIdShowMarker(        UndefinedID ),
        mIdExtraLinesAlign(   UndefinedID ),
        mIdExtraLinesInFront( UndefinedID ),
        mIdExtraLinesLength(  UndefinedID ),
        mIdExtraLinesWidth(   UndefinedID ),
        mIdExtraLinesColor(   UndefinedID ),
        mIdExtraLinesStyle(   UndefinedID ),
        mIdExtraMarkersAlign( UndefinedID ),
        mIdExtraMarkersSize(  UndefinedID ),
        mIdExtraMarkersColor( UndefinedID ),
        mIdExtraMarkersStyle( UndefinedID ),
        mIdShowBar(           UndefinedID ),
        mIdBarColor(          UndefinedID )
        { fillValueMembersWithDummyValues(); }

    /**
       Constructor setting all Property Set IDs to the same ID value.

       This constructor may be used to initialize a property set
       specifying and have all property IDs set to a specific value,
       e.g. you might pass KDChartParams::KDCHART_PROPSET_NORMAL_DATA
       as ID to make the default property set the parent of all
       values.

       \param name (may be empty) a name describing this property set.
       \param idParent the ID of the parent property set.
    */
    KDChartPropertySet( const QString& name, int idParent ) :
        mOwnID( UndefinedID ),
        mName( name ),
        mIdLineWidth(         idParent ),
        mIdLineColor(         idParent ),
        mIdLineStyle(         idParent ),
        mIdShowMarker(        idParent ),
        mIdExtraLinesAlign(   idParent ),
        mIdExtraLinesInFront( idParent ),
        mIdExtraLinesLength(  idParent ),
        mIdExtraLinesWidth(   idParent ),
        mIdExtraLinesColor(   idParent ),
        mIdExtraLinesStyle(   idParent ),
        mIdExtraMarkersAlign( idParent ),
        mIdExtraMarkersSize(  idParent ),
        mIdExtraMarkersColor( idParent ),
        mIdExtraMarkersStyle( idParent ),
        mIdShowBar(           idParent ),
        mIdBarColor(          idParent )
        { fillValueMembersWithDummyValues(); }

    /**
       Constructor.

       \param name (may be empty) a name describing this property set.

       \param idLineWidth (for Line Charts only) ID of the property set
       specifying the line width to be used for drawing
       a line from this data point to the next one.
       Use special value KDChartPropertySet::UndefinedID
       to specify neither another property set's ID
       nor an own value for the line width.
       Use special value KDChartPropertySet::OwnID
       if you want to specify the line width by using
       the following parameter.

       \param lineWidth (for Line Charts only) the line width to be used
       for drawing a line from this data point to the
       next one.  If \c lineWidth is negative it is
       interpreted as per-mille value of the chart's
       drawing  area, the true line width will then be
       calculated dynamically at drawing time.
       This parameter is stored but ignored if the
       previous parameter is not set to KDChartPropertySet::OwnID.

       \param idLineColor (for Line Charts only) ID of the property set
       specifying the QColor to be used for drawing
       a line from this data point to the next one.
       Use special value KDChartPropertySet::UndefinedID
       to specify neither another property set's ID
       nor an own value for the line color.
       Use special value KDChartPropertySet::OwnID
       if you want to specify the line color by using
       the following parameter.

       \param lineColor (for Line Charts only) the QColor to be used
       for drawing a line from this data point to the
       next one.  This parameter is stored but ignored if the
       previous parameter is not set to KDChartPropertySet::OwnID.

       \param idLineStyle (for Line Charts only) ID of the property set
       specifying the Qt::PenStyle to be used for drawing
       a line from this data point to the next one.
       Use special value KDChartPropertySet::UndefinedID
       to specify neither another property set's ID
       nor an own value for the line style.
       Use special value KDChartPropertySet::OwnID
       if you want to specify the line style by using
       the following parameter.

       \param lineStyle (for Line Charts only) the Qt::PenStyle to be used
       for drawing a line from this data point to the
       next one.  This parameter is stored but ignored if the
       previous parameter is not set to KDChartPropertySet::OwnID.

       \param idShowMarker (for Line Charts only) ID of the property set
       specifying the showMarker flag saying whether
       a marker is to be shown for this cell's data value.
       Use special value KDChartPropertySet::UndefinedID
       to specify neither another property set's ID
       nor an own value for this flag.
       Use special value KDChartPropertySet::OwnID
       if you want to specify the flag by using
       the following parameter.

       \param showMarker (for Line Charts only) flag indicating whether
       a marker is to be shown for this cell's data value.
       This parameter is stored but ignored if the
       previous parameter is not set to KDChartPropertySet::OwnID.

       \param idExtraLinesAlign (for Line or Bar Charts only) ID of the property set
       specifying the alignment of extra horizontal or vertical
       lines to be drawn through this point, see param \c
       extraLinesAlign for details.

       \param extraLinesAlign (for Line or Bar Charts only) the alignment of extra
       horizontal or vertical lines to be drawn through this point.
       These extra lines connect the point with the chart's axes,
       or they reach from the point to the left and right
       and/or towards the top and bottom without touching the axes.
       Use either \c 0 (to surpress drawing of extra lines) or any
       <b>OR</b> combination of the following bit flags to specify
       which of the 4 possible lines are to be drawn.
       Do <b>not</b> use any of the other flags defined with
       Qt::AlignmentFlags: all values not listed here are reserved
       for KDChart's further extensions and will be ignored.
       \li \c Qt::AlignLeft horizontal line from this point to the left axis
       \li \c Qt::AlignRight horizontal line to the right axis
       \li \c Qt::AlignHCenter horizontal line of \c extraLinesLength
       \li \c Qt::AlignTop vertical line to the top axis
       \li \c Qt::AlignBottom vertical line to the bottom axis
       \li \c Qt::AlignVCenter vertical line of \c extraLinesLength
       \li \c Qt::AlignCenter horizontal and vertical lines of \c extraLinesLength

       \param idExtraLinesInFront (for Line or Bar Charts only) ID of the property set
       specifying whether the extra horizontal or vertical
       lines (and their respective extra markers) to be drawn in front of
       the normal lines (and/or in front of the normal markers, resp.).
       This parameter will be ignored if extraLinesAlign is zero
       (or idExtraLinesAlign points to a property set specifying
       zero extraLinesAlign, resp.).

       \param extraLinesInFront (for Line or Bar Charts only) specifying whether
       the extra horizontal or vertical lines (and their respective extra
       markers) to be drawn in front of the normal lines (and/or in front
       of the normal markers, resp.): by default they are drawn behind.
       This parameter will be ignored if extraLinesAlign is zero
       (or idExtraLinesAlign points to a property set specifying
       zero extraLinesAlign, resp.).

       \param idExtraLinesLength (for Line or Bar Charts only) ID of the property set
       specifying the length of the extra horizontal or vertical
       lines to be drawn through this point.  This parameter will
       be ignored if extraLinesAlign is <b>not</b> Qt::AlignHCenter nor
       Qt::AlignVCenter (or idExtraLinesAlign points to a property
       set not specifying such alignment, resp.).

       \param extraLinesLength (for Line or Bar Charts only) the length
       of the extra horizontal or vertical lines to be drawn through
       this point.  If \c extraLinesLength is negative it is interpreted
       as per-mille value of the data area's size, the true line length
       will then be calculated dynamically at drawing time.
       This parameter will be ignored if extraLinesAlign is <b>not</b> Qt::AlignHCenter
       nor Qt::AlignVCenter (or idExtraLinesAlign points to a property set
       not specifying such alignment, resp.)

       \param idExtraLinesWidth (for Line or Bar Charts only) ID of the property set
       specifying the width of the extra horizontal or vertical
       lines to be drawn through this point.  This parameter will be
       ignored if extraLinesAlign is zero (or idExtraLinesAlign
       points to a property set specifying zero extraLinesAlign, resp.).

       \param extraLinesWidth (for Line or Bar Charts only) the width of the extra horizontal
       or vertical lines to be drawn through this point.
       If \c extraLinesWidth is negative it is interpreted as per-mille
       value of the chart's drawing area, the true line width will then
       be calculated dynamically at drawing time.
       This parameter will be ignored if extraLinesAlign is zero
       (or idExtraLinesAlign points to a property set specifying
       zero extraLinesAlign, resp.).

       \param idExtraLinesColor (for Line or Bar Charts only) ID of the property set
       specifying the QColor of the extra horizontal or vertical
       lines to be drawn through this point.  This parameter will
       be ignored if extraLinesAlign is zero (or idExtraLinesAlign
       points to a property set specifying zero extraLinesAlign, resp.).

       \param extraLinesColor (for Line or Bar Charts only) the QColor of the extra horizontal
       or vertical lines to be drawn through this point.  This parameter
       will be ignored if extraLinesAlign is zero (or
       idExtraLinesAlign points to a property set specifying zero
       extraLinesAlign, resp.).

       \param idExtraLinesStyle (for Line or Bar Charts only) ID of the property set
       specifying the Qt::PenStyle of the extra horizontal or vertical
       lines to be drawn through this point.  This parameter will
       be ignored if extraLinesAlign is zero (or idExtraLinesAlign
       points to a property set specifying zero extraLinesAlign, resp.).

       \param extraLinesStyle (for Line or Bar Charts only) the Qt::PenStyle of the extra horizontal
       or vertical lines to be drawn through this point.  This parameter
       will be ignored if extraLinesAlign is zero (or
       idExtraLinesAlign points to a property set specifying zero
       extraLinesAlign, resp.).

       \param idExtraMarkersAlign (for Line or Bar Charts only) ID of the property set
       specifying the vertical and/or horizontal position of markers
       to be drawn at the end(s) of extra horizontal or vertical
       lines which are drawn through this point, see parameter \c
       extraMarkersAlign for details.

       \param extraMarkersAlign (for Line or Bar Charts only) the vertical and/or horizontal
       position of markers to be drawn at the end(s) of extra
       horizontal or vertical lines which are drawn through this point.
       These markers can be drawn at the end(s) of one or both
       extra lines, use either \c 0 (to surpress drawing of the
       markers) or any <b>OR</b> combination of the following bit
       flags to specify which of the markers are to be drawn.
       Do <b>not</b> use any of the other flags defined with
       Qt::AlignmentFlags: all values not listed here are reserved
       for KDChart's further extensions and will be ignored.
       \li \c Qt::AlignLeft marker will be drawn at the left end of
       the horizontal line.
       \li \c Qt::AlignRight marker will be drawn at the right end of
       the horizontal line, this flag and the \c Qt::AlignLeft flag will
       be ignored if no horizontal line is drawn but they will
       <b>not</b> be ignored if the line is drawn using a \c Qt::NoPen
       line style.
       \li \c Qt::AlignTop marker will be drawn at the top end of
       the vertical line.
       \li \c Qt::AlignBottom marker will be drawn at the bottom end of
       the vertical line, this flag and the \c Qt::AlignTop flag will
       be ignored if no vertical line is drawn but they will
       <b>not</b> be ignored if the line is drawn using a \c Qt::NoPen
       line style.

       \param idExtraMarkersSize (for Line or Bar Charts only) ID of the property set
       specifying the size of the markers to be drawn at the end(s)
       of extra horizontal or vertical lines which are drawn through
       this point.
       This parameter will be ignored if no markers are drawn, see
       parameter \c extraMarkersAlign for details.

       \param extraMarkersSize (for Line or Bar Charts only) the size of the markers
       to be drawn at the end(s) of extra horizontal or vertical
       lines which are drawn through this point.
       Negative values for \c extraMarkersSize.width() or
       \c extraMarkersSize.height() are interpreted as per-mille
       values of the chart's drawing area, the true marker size will
       then be calculated dynamically at drawing time.
       This parameter will be ignored if no markers are drawn, see
       parameter \c extraMarkersAlign for details.

       \param idExtraMarkersColor (for Line or Bar Charts only) ID of the property set
       specifying a QColor for the markers to be drawn at the end(s)
       of extra horizontal or vertical lines which are drawn through
       this point.
       This parameter will be ignored if no markers are drawn, see
       parameter \c extraMarkersAlign for details.

       \param extraMarkersSize (for Line or Bar Charts only) a QColor for the markers
       to be drawn at the end(s) of extra horizontal or vertical
       lines which are drawn through this point.
       This parameter will be ignored if no markers are drawn, see
       parameter \c extraMarkersAlign for details.

       \param idExtraMarkersColor (for Line or Bar Charts only) ID of the property set
       specifying a KDChartParams::LineMarkerStyle for the markers
       to be drawn at the end(s) of extra horizontal or vertical
       lines which are drawn through this point.
       This parameter will be ignored if no markers are drawn, see
       parameter \c extraMarkersAlign for details.

       \param extraMarkersSize (for Line or Bar Charts only) a KDChartParams::LineMarkerStyle
       for the markers to be drawn at the end(s) of extra horizontal
       or vertical lines which are drawn through this point.
       This parameter will be ignored if no markers are drawn, see
       parameter \c extraMarkersAlign for details.

       \param idShowBar (for Bar Charts only) ID of the property set
       specifying the showBar flag saying whether
       this cell's bar is to be painted.
       Use special value KDChartPropertySet::UndefinedID
       to specify neither another property set's ID
       nor an own value for this flag.
       Use special value KDChartPropertySet::OwnID
       if you want to specify the flag by using
       the following parameter.

       \param showBar (for Bar Charts only) flag indicating whether
       this cell's bar is to be painted.
       This parameter is stored but ignored if the
       previous parameter is not set to KDChartPropertySet::OwnID.

       \param idBarColor (for Bar Charts only) ID of the property set
       specifying the QColor to be used for drawing
       this cell's bar.
       Use special value KDChartPropertySet::UndefinedID
       to specify neither another property set's ID
       nor an own value for the bar color.
       Use special value KDChartPropertySet::OwnID
       if you want to specify the bar color by using
       the following parameter.

       \param barColor (for Bar Charts only) the QColor to be used
       for drawing this cell's bar.  This parameter is stored
       but ignored if the previous parameter is not set to
       KDChartPropertySet::OwnID.

       \sa name
       \sa setLineWidth, setLineColor, setLineStyle, setShowMarker
       \sa hasOwnLineWidth, hasOwnLineColor, hasOwnLineStyle, hasOwnShowMarker
    */
    KDChartPropertySet( const QString& name,
                        int idLineWidth,        int lineWidth,
                        int idLineColor,        const QColor& lineColor,
                        int idLineStyle,        const Qt::PenStyle& lineStyle,
                        int idShowMarker,       bool showMarker,
                        int idExtraLinesAlign,  uint                extraLinesAlign,
                        int idExtraLinesInFront,bool                extraLinesInFront,
                        int idExtraLinesLength, int                 extraLinesLength,
                        int idExtraLinesWidth,  int                 extraLinesWidth,
                        int idExtraLinesColor,  const QColor&       extraLinesColor,
                        int idExtraLinesStyle,  const Qt::PenStyle& extraLinesStyle,
                        int idExtraMarkersAlign,uint                extraMarkersAlign,
                        int idExtraMarkersSize, const QSize&        extraMarkersSize,
                        int idExtraMarkersColor,const QColor&       extraMarkersColor,
                        int idExtraMarkersStyle,int                 extraMarkersStyle,
                        int idShowBar,          bool showBar,
                        int idBarColor,         const QColor& barColor
        ) :
        mOwnID( UndefinedID ),
        mName( name ),
        mIdLineWidth(         idLineWidth ),         mLineWidth(         lineWidth  ),
        mIdLineColor(         idLineColor ),         mLineColor(         lineColor  ),
        mIdLineStyle(         idLineStyle ),         mLineStyle(         lineStyle  ),
        mIdShowMarker(        idShowMarker ),        mShowMarker(        showMarker ),
        mIdExtraLinesAlign(   idExtraLinesAlign ),   mExtraLinesAlign(   extraLinesAlign ),
        mIdExtraLinesInFront( idExtraLinesInFront ), mExtraLinesInFront( extraLinesInFront ),
        mIdExtraLinesLength(  idExtraLinesLength ),  mExtraLinesLength(  extraLinesLength ),
        mIdExtraLinesWidth(   idExtraLinesWidth ),   mExtraLinesWidth(   extraLinesWidth ),
        mIdExtraLinesColor(   idExtraLinesColor ),   mExtraLinesColor(   extraLinesColor ),
        mIdExtraLinesStyle(   idExtraLinesStyle ),   mExtraLinesStyle(   extraLinesStyle ),
        mIdExtraMarkersAlign( idExtraMarkersAlign ), mExtraMarkersAlign( extraMarkersAlign ),
        mIdExtraMarkersSize(  idExtraMarkersSize  ), mExtraMarkersSize(  extraMarkersSize ),
        mIdExtraMarkersColor( idExtraMarkersColor ), mExtraMarkersColor( extraMarkersColor ),
        mIdExtraMarkersStyle( idExtraMarkersStyle ), mExtraMarkersStyle( extraMarkersStyle ),
        mIdShowBar(           idShowBar ),           mShowBar(           showBar ),
        mIdBarColor(          idBarColor ),          mBarColor(          barColor )
        {}

    KDChartPropertySet& operator=( const KDChartPropertySet& R );

    /**
       Save this property set's settings in a stream,
       this stores the own property set ID as well.

       \sa loadXML
    */
    QDomElement saveXML(QDomDocument& doc) const;


    /**
       Retrieves a property set and stores it in parameter set.

       \note Since the property set's internal <b>ID</b> is also read
       make sure to call KDChartParams::setProperties( set.id(), set )
       after calling \c load() if you want to use the loaded
       property set in the context of KDChartParam settings.

       \returns TRUE if the property set could be read.

       \sa id, saveXLM
    */
    static bool loadXML( const QDomElement& element, KDChartPropertySet& set );


    /**
       Returns the name of this property set.
    */
    int id() const { return mOwnID; }

    /**
       Change the descriptive name of this property set.
    */
    void setName( const QString& name )
        {
            mName = name;
        }

    /**
       Returns the name of this property set.
    */
    QString name() const { return mName; }



    /**
       Specify the ID of the property set specifying a boolean flag indicating
       whether this cell's bar is to be painted
       <b>or</b> specifying this flag directly.

       \note This function should be used for Bar Charts only, otherwise
       the settings specified here will be ignored.

       \param idShowBar ID of the property set specifying the flag
       indicating whether this cell's bar is to be painted.
       Use special value KDChartPropertySet::UndefinedID
       to specify neither another property set's ID
       nor an own value for this flag.
       Use special value KDChartPropertySet::OwnID
       if you do NOT want to inherit another property set's
       settings but want to specify the flag by using
       the following parameter.
       \param showBar  Flag indicating whether this cell's bar is to be painted.
       This parameter is stored but ignored if the previous parameter
       is not set to KDChartPropertySet::OwnID.

       \sa hasOwnShowBar
       \sa setBarColor
       \sa hasOwnBarColor
    */
    void setShowBar( int idShowBar, bool showBar )
        {
            mIdShowBar = idShowBar;
            mShowBar =   showBar;
        }

    /**
       Returns whether this property set is specifying it's own flag indicating
       whether this cell's bar is to be painted.

       \note This function should be used for Bar Charts only, otherwise
       the settings specified here will be ignored.

       \returns TRUE if this property set is specifying it's own showBar flag,
       FALSE if the settings of another property set are to be used instead.

       \note The return value will also be FALSE if the 'default' properties
       are to be used: in this case idShowBar will be KDChartParams::NormalData

       \param idShowBar to be ignored if return value is TRUE.
       If idShowBar is KDChartPropertySet::UndefinedID
       then neither a property set ID
       nor an own flag were specified (so no special
       enabeling/disabeling of bars is associated to the
       respective data cell), else idShowBar contains the
       ID of another property set that is specifying the flag
       to be used.
       \param showBar  this parameter's value is not modified if return value is FALSE.
       Parameter showBar contains the showBar flag
       associated with the respective data cell.
       If return value is FALSE the showBar
       flag is not set (so the parameter keeps its previous value)
       but this is to be overridden by the respective value
       of another property set that is indicated
       by the idShowBar parameter - unless this has the special
       value KDChartPropertySet::UndefinedID as decribed above.

       \sa setShowBar
       \sa setBarColor
       \sa hasOwnBarColor
    */
    bool hasOwnShowBar( int& idShowBar, bool& showBar )
        {
            idShowBar = mIdShowBar;
            if( OwnID == idShowBar ){
                showBar = mShowBar;
                return true;
            }
            return false;
        }

    /**
       Specify the ID of the property set specifying the bar color
       to be used for this cell
       <b>or</b> specify the bar color directly.

       \note This function should be used for Bar Charts only, otherwise
       the settings specified here will be ignored.

       \param idBarColor ID of the property set specifying the bar color.
       Use special value KDChartPropertySet::UndefinedID
       to specify neither another property set's ID
       nor an own value for the bar color.
       Use special value KDChartPropertySet::OwnID
       if you do NOT want to inherit another property set's
       settings but want to specify the bar color by using
       the following parameter.
       \param barColor   The bar color to be used.
       This parameter is stored but ignored if the previous parameter
       is not set to KDChartPropertySet::OwnID.

       \sa hasOwnBarColor
    */
    void setBarColor( int idBarColor, const QColor& barColor )
        {
            mIdBarColor = idBarColor;
            mBarColor = barColor;
        }

    /**
       Returns whether this property set is specifying it's own bar color settings.

       \note This function should be used for Bar Charts only, otherwise
       the settings specified here will be ignored.

       \returns TRUE if this property set is specifying it's own bar color settings,
       FALSE if the settings of another property set are to be used instead.

       \note The return value will also be FALSE if the 'default' properties
       are to be used: in this case idBarColor will be KDChartParams::NormalData

       \param idBarColor to be ignored if return value is TRUE.
       If idBarColor is KDChartPropertySet::UndefinedID
       then neither a property set ID
       nor an own bar color were specified (so no special
       Bar color is associated to the respective data cell),
       else idBarColor contains the ID of another property set
       that is specifying the Bar color to be used.
       \param barColor   this parameter's value is not modified if return value is FALSE.
       Parameter barColor contains the bar color value
       associated with the respective data cell.
       If return value is FALSE the barColor
       value is not set (so the parameter keeps its previous value)
       but this is to be overridden by the respective value
       of another property set that is indicated
       by the idBarColor parameter - unless this has the special
       value KDChartPropertySet::UndefinedID as decribed above.

       \sa setBarColor
    */
    bool hasOwnBarColor( int& idBarColor, QColor& barColor )
        {
            idBarColor = mIdBarColor;
            if( OwnID == mIdBarColor ){
                barColor = mBarColor;
                return true;
            }
            return false;
        }

    /**
       Specify the ID of the property set specifying the line width
       to be used for drawing a line from this data point to the next one
       <b>or</b> specify the line width directly.

       \note This function should be used for Line Charts only, otherwise
       the settings specified here will be ignored.

       \param idLineWidth ID of the property set specifying the line width.
       Use special value KDChartPropertySet::UndefinedID
       to specify neither another property set's ID
       nor an own value for the line width.
       Use special value KDChartPropertySet::OwnID
       if you do NOT want to inherit another property set's
       settings but want to specify the line width by using
       the following parameter.
       \param lineWidth   The line width to be used.
       This parameter is stored but ignored if the previous parameter
       is not set to KDChartPropertySet::OwnID.

       \sa setLineWidth, setLineColor, setLineStyle, setShowMarker
       \sa hasOwnLineWidth, hasOwnLineColor, hasOwnLineStyle, hasOwnShowMarker
    */
    void setLineWidth( int idLineWidth, int lineWidth )
        {
            mIdLineWidth = idLineWidth;
            mLineWidth =   lineWidth;
        }

    /**
       Returns whether this property set is specifying it's own line width settings.

       \note This function should be used for Line Charts only.

       \returns TRUE if this property set is specifying it's own line width settings,
       FALSE if the settings of another property set are to be used instead.

       \param idLineWidth to be ignored if return value is TRUE.
       If idLineWidth is KDChartPropertySet::UndefinedID
       then neither a property set ID
       nor an own line width were specified (so no special
       line width is associated to the respective data cell),
       else idLineWidth contains the ID of another property set
       that is specifying the line width to be used.
       \param lineWidth   this parameter's value is not modified if return value is FALSE.
       Parameter lineWidth contains the line width value
       associated with the respective data cell.
       If return value is FALSE the lineWidth
       value is not set (so the parameter keeps its previous value)
       but this is to be overridden by the respective value
       of another property set that is indicated
       by the idLineWidth parameter - unless this has the special
       value KDChartPropertySet::UndefinedID as decribed above.

       \sa setLineWidth, setLineColor, setLineStyle, setShowMarker
       \sa hasOwnLineColor, hasOwnLineStyle, hasOwnShowMarker
    */
    bool hasOwnLineWidth( int& idLineWidth, int& lineWidth )
        {
            idLineWidth = mIdLineWidth;
            if( OwnID == mIdLineWidth ){
                lineWidth = mLineWidth;
                return true;
            }
            return false;
        }

    /**
       Specify the ID of the property set specifying the line color
       to be used for drawing a line from this data point to the next one
       <b>or</b> specify the line color directly.

       \note This function should be used for Line Charts only, otherwise
       the settings specified here will be ignored.

       \param idLineColor ID of the property set specifying the line color.
       Use special value KDChartPropertySet::UndefinedID
       to specify neither another property set's ID
       nor an own value for the line color.
       Use special value KDChartPropertySet::OwnID
       if you do NOT want to inherit another property set's
       settings but want to specify the line color by using
       the following parameter.
       \param lineColor   The line color to be used.
       This parameter is stored but ignored if the previous parameter
       is not set to KDChartPropertySet::OwnID.

       \sa setLineWidth, setLineStyle, setShowMarker
       \sa hasOwnLineWidth, hasOwnLineColor, hasOwnLineStyle, hasOwnShowMarker
    */
    void setLineColor( int idLineColor, const QColor& lineColor )
        {
            mIdLineColor = idLineColor;
            mLineColor = lineColor;
        }

    /**
       Returns whether this property set is specifying it's own line color settings.

       \note This function should be used for Line Charts only, otherwise
       the settings specified here will be ignored.

       \returns TRUE if this property set is specifying it's own line color settings,
       FALSE if the settings of another property set are to be used instead.

       \note The return value will also be FALSE if the 'default' properties
       are to be used: in this case idLineColor will be KDChartParams::NormalData

       \param idLineColor to be ignored if return value is TRUE.
       If idLineColor is KDChartPropertySet::UndefinedID
       then neither a property set ID
       nor an own line color were specified (so no special
       line color is associated to the respective data cell),
       else idLineColor contains the ID of another property set
       that is specifying the line color to be used.
       \param lineColor   this parameter's value is not modified if return value is FALSE.
       Parameter lineColor contains the line color value
       associated with the respective data cell.
       If return value is FALSE the lineColor
       value is not set (so the parameter keeps its previous value)
       but this is to be overridden by the respective value
       of another property set that is indicated
       by the idLineColor parameter - unless this has the special
       value KDChartPropertySet::UndefinedID as decribed above.

       \sa setLineWidth, setLineColor, setLineStyle, setShowMarker
       \sa hasOwnLineWidth, hasOwnLineStyle, hasOwnShowMarker
    */
    bool hasOwnLineColor( int& idLineColor, QColor& lineColor )
        {
            idLineColor = mIdLineColor;
            if( OwnID == mIdLineColor ){
                lineColor = mLineColor;
                return true;
            }
            return false;
        }

    /**
       Specify the ID of the property set specifying the line style
       to be used for drawing a line from this data point to the next one
       <b>or</b> specify the line width directly.

       \note This function should be used for Line Charts only, otherwise
       the settings specified here will be ignored.

       \param idLineStyle ID of the property set specifying the line style.
       Use special value KDChartPropertySet::UndefinedID
       to specify neither another property set's ID
       nor an own value for the line style.
       Use special value KDChartPropertySet::OwnID
       if you do NOT want to inherit another property set's
       settings but want to specify the line style by using
       the following parameter.
       \param lineStyle   The line style to be used.
       This parameter is stored but ignored if the previous parameter
       is not set to KDChartPropertySet::OwnID.

       \sa setLineWidth, setLineColor, setShowMarker
       \sa hasOwnLineWidth, hasOwnLineColor, hasOwnLineStyle, hasOwnShowMarker
    */
    void setLineStyle( int idLineStyle, const Qt::PenStyle& lineStyle )
        {
            mIdLineStyle = idLineStyle;
            mLineStyle =   lineStyle;
        }

    /**
       Returns whether this property set is specifying it's own line style settings.

       \note This function should be used for Line Charts only, otherwise
       the settings specified here will be ignored.

       \returns TRUE if this property set is specifying it's own line style settings,
       FALSE if the settings of another property set are to be used instead.

       \note The return value will also be FALSE if the 'default' properties
       are to be used: in this case idLineStyle will be KDChartParams::NormalData

       \param idLineStyle to be ignored if return value is TRUE.
       If idLineStyle is KDChartPropertySet::UndefinedID
       then neither a property set ID
       nor an own line style were specified (so no special
       line style is associated to the respective data cell),
       else idLineStyle contains the ID of another property set
       that is specifying the line style to be used.
       \param lineStyle   this parameter's value is not modified if return value is FALSE.
       Parameter lineStyle contains the line style value
       associated with the respective data cell.
       If return value is FALSE the lineStyle
       value is not set (so the parameter keeps its previous value)
       but this is to be overridden by the respective value
       of another property set that is indicated
       by the idLineStyle parameter - unless this has the special
       value KDChartPropertySet::UndefinedID as decribed above.

       \sa setLineWidth, setLineColor, setLineStyle, setShowMarker
       \sa hasOwnLineWidth, hasOwnLineColor, hasOwnShowMarker
    */
    bool hasOwnLineStyle( int& idLineStyle, Qt::PenStyle& lineStyle )
        {
            idLineStyle = mIdLineStyle;
            if( OwnID == mIdLineStyle ){
                lineStyle = mLineStyle;
                return true;
            }
            return false;
        }

    /**
       Specify the ID of the property set specifying a boolean flag indicating
       whether a Marker is to be displayed for this data value
       <b>or</b> specifying this flag directly.

       \note This function should be used for Line Charts only, otherwise
       the settings specified here will be ignored.

       \param idShowMarker ID of the property set specifying the flag
       indicating whether a marker is to be shown.
       Use special value KDChartPropertySet::UndefinedID
       to specify neither another property set's ID
       nor an own value for this flag.
       Use special value KDChartPropertySet::OwnID
       if you do NOT want to inherit another property set's
       settings but want to specify the flag by using
       the following parameter.
       \param showMarker  Flag indicating whether a marker is to be shown.
       This parameter is stored but ignored if the previous parameter
       is not set to KDChartPropertySet::OwnID.

       \sa hasOwnShowMarker
       \sa setLineWidth, setLineColor, setLineStyle
       \sa hasOwnLineWidth, hasOwnLineColor, hasOwnLineStyle
    */
    void setShowMarker( int idShowMarker, bool showMarker )
        {
            mIdShowMarker = idShowMarker;
            mShowMarker =   showMarker;
        }

    /**
       Returns whether this property set is specifying it's own flag indicating
       whether a Marker is to be displayed.

       \note This function should be used for Line Charts only, otherwise
       the settings specified here will be ignored.

       \returns TRUE if this property set is specifying it's own showMarker flag,
       FALSE if the settings of another property set are to be used instead.

       \note The return value will also be FALSE if the 'default' properties
       are to be used: in this case idShowMarker will be KDChartParams::NormalData

       \param idShowMarker to be ignored if return value is TRUE.
       If idShowMarker is KDChartPropertySet::UndefinedID
       then neither a property set ID
       nor an own flag were specified (so no special
       enabeling/disabeling of markers is associated to the
       respective data cell), else idShowMarker contains the
       ID of another property set that is specifying the flag
       to be used.
       \param showMarker  this parameter's value is not modified if return value is FALSE.
       Parameter showMarker contains the showMarker flag
       associated with the respective data cell.
       If return value is FALSE the showMarker
       flag is not set (so the parameter keeps its previous value)
       but this is to be overridden by the respective value
       of another property set that is indicated
       by the idShowMarker parameter - unless this has the special
       value KDChartPropertySet::UndefinedID as decribed above.

       \sa setShowMarker
       \sa setLineWidth, setLineColor, setLineStyle
       \sa hasOwnLineWidth, hasOwnLineColor, hasOwnLineStyle
    */
    bool hasOwnShowMarker( int& idShowMarker, bool& showMarker )
        {
            idShowMarker = mIdShowMarker;
            if( OwnID == idShowMarker ){
                showMarker = mShowMarker;
                return true;
            }
            return false;
        }

    void setExtraLinesAlign( int idExtraLinesAlign, uint extraLinesAlign )
        {
            mIdExtraLinesAlign = idExtraLinesAlign;
            mExtraLinesAlign =   extraLinesAlign;
        }
    bool hasOwnExtraLinesAlign( int& idExtraLinesAlign, uint& extraLinesAlign )
        {
            idExtraLinesAlign = mIdExtraLinesAlign;
            if( OwnID == idExtraLinesAlign ){
                extraLinesAlign = mExtraLinesAlign;
                return true;
            }
            return false;
        }
    void setExtraLinesInFront( int idExtraLinesInFront, bool extraLinesInFront )
        {
            mIdExtraLinesInFront = idExtraLinesInFront;
            mExtraLinesInFront   = extraLinesInFront;
        }
    bool hasOwnExtraLinesInFront( int& idExtraLinesInFront, bool& extraLinesInFront )
        {
            idExtraLinesInFront = mIdExtraLinesInFront;
            if( OwnID == idExtraLinesInFront ){
                extraLinesInFront = mExtraLinesInFront;
                return true;
            }
            return false;
        }
    void setExtraLinesLength( int idExtraLinesLength, int extraLinesLength )
        {
            mIdExtraLinesLength = idExtraLinesLength;
            mExtraLinesLength =   extraLinesLength;
        }
    bool hasOwnExtraLinesLength( int& idExtraLinesLength, int& extraLinesLength )
        {
            idExtraLinesLength = mIdExtraLinesLength;
            if( OwnID == idExtraLinesLength ){
                extraLinesLength = mExtraLinesLength;
                return true;
            }
            return false;
        }
    void setExtraLinesWidth( int idExtraLinesWidth, int extraLinesWidth )
        {
            mIdExtraLinesWidth = idExtraLinesWidth;
            mExtraLinesWidth =   extraLinesWidth;
        }
    bool hasOwnExtraLinesWidth( int& idExtraLinesWidth, int& extraLinesWidth )
        {
            idExtraLinesWidth = mIdExtraLinesWidth;
            if( OwnID == idExtraLinesWidth ){
                extraLinesWidth = mExtraLinesWidth;
                return true;
            }
            return false;
        }
    void setExtraLinesColor( int idExtraLinesColor, const QColor& extraLinesColor )
        {
            mIdExtraLinesColor = idExtraLinesColor;
            mExtraLinesColor =   extraLinesColor;
        }
    bool hasOwnExtraLinesColor( int& idExtraLinesColor, QColor& extraLinesColor )
        {
            idExtraLinesColor = mIdExtraLinesColor;
            if( OwnID == idExtraLinesColor ){
                extraLinesColor = mExtraLinesColor;
                return true;
            }
            return false;
        }
    void setExtraLinesStyle( int idExtraLinesStyle, const Qt::PenStyle extraLinesStyle )
        {
            mIdExtraLinesStyle = idExtraLinesStyle;
            mExtraLinesStyle =   extraLinesStyle;
        }
    bool hasOwnExtraLinesStyle( int& idExtraLinesStyle, Qt::PenStyle& extraLinesStyle )
        {
            idExtraLinesStyle = mIdExtraLinesStyle;
            if( OwnID == idExtraLinesStyle ){
                extraLinesStyle = mExtraLinesStyle;
                return true;
            }
            return false;
        }

    void setExtraMarkersAlign( int idExtraMarkersAlign, uint extraMarkersAlign )
        {
            mIdExtraMarkersAlign = idExtraMarkersAlign;
            mExtraMarkersAlign =   extraMarkersAlign;
        }
    bool hasOwnExtraMarkersAlign( int& idExtraMarkersAlign, uint& extraMarkersAlign )
        {
            idExtraMarkersAlign = mIdExtraMarkersAlign;
            if( OwnID == idExtraMarkersAlign ){
                extraMarkersAlign = mExtraMarkersAlign;
                return true;
            }
            return false;
        }
    void setExtraMarkersSize( int idExtraMarkersSize, const QSize& extraMarkersSize )
        {
            mIdExtraMarkersSize = idExtraMarkersSize;
            mExtraMarkersSize =   extraMarkersSize;
        }
    bool hasOwnExtraMarkersSize( int& idExtraMarkersSize, QSize& extraMarkersSize )
        {
            idExtraMarkersSize = mIdExtraMarkersSize;
            if( OwnID == idExtraMarkersSize ){
                extraMarkersSize = mExtraMarkersSize;
                return true;
            }
            return false;
        }
    void setExtraMarkersColor( int idExtraMarkersColor, const QColor& extraMarkersColor )
        {
            mIdExtraMarkersColor = idExtraMarkersColor;
            mExtraMarkersColor =   extraMarkersColor;
        }
    bool hasOwnExtraMarkersColor( int& idExtraMarkersColor, QColor& extraMarkersColor )
        {
            idExtraMarkersColor = mIdExtraMarkersColor;
            if( OwnID == idExtraMarkersColor ){
                extraMarkersColor = mExtraMarkersColor;
                return true;
            }
            return false;
        }
    void setExtraMarkersStyle( int idExtraMarkersStyle, int extraMarkersStyle )
        {
            mIdExtraMarkersStyle = idExtraMarkersStyle;
            mExtraMarkersStyle =   extraMarkersStyle;
        }
    bool hasOwnExtraMarkersStyle( int& idExtraMarkersStyle, int& extraMarkersStyle )
        {
            idExtraMarkersStyle = mIdExtraMarkersStyle;
            if( OwnID == idExtraMarkersStyle ){
                extraMarkersStyle = mExtraMarkersStyle;
                return true;
            }
            return false;
        }

protected:
    // the following member only to be set internally by KDChartParams::registerProperties
    // and by KDChartParams::setProperties
    int mOwnID;

private:
    QString mName;
    // IDs:                     values used if ID == OwnID:
    int mIdLineWidth;         int          mLineWidth;
    int mIdLineColor;         QColor       mLineColor;
    int mIdLineStyle;         Qt::PenStyle mLineStyle;
    int mIdShowMarker;        bool         mShowMarker;
    int mIdExtraLinesAlign;   uint         mExtraLinesAlign;
    int mIdExtraLinesInFront; bool         mExtraLinesInFront;
    int mIdExtraLinesLength;  int          mExtraLinesLength;
    int mIdExtraLinesWidth;   int          mExtraLinesWidth;
    int mIdExtraLinesColor;   QColor       mExtraLinesColor;
    int mIdExtraLinesStyle;   Qt::PenStyle mExtraLinesStyle;
    int mIdExtraMarkersAlign; uint         mExtraMarkersAlign;
    int mIdExtraMarkersSize;  QSize        mExtraMarkersSize;
    int mIdExtraMarkersColor; QColor       mExtraMarkersColor;
    int mIdExtraMarkersStyle; int          mExtraMarkersStyle;
    int mIdShowBar;           bool         mShowBar;
    int mIdBarColor;          QColor       mBarColor;
    void fillValueMembersWithDummyValues();
};

#endif
