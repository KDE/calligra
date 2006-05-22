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


#include <KDChartEnums.h>
#include <kdchart_export.h>

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
\sa KDCHART_PROPSET_NORMAL_DATA, KDCHART_PROPSET_TRANSPARENT_DATA
\sa KDChartParams::registerProperties
*/
class KDCHART_EXPORT KDChartPropertySet :public QObject
{
    Q_OBJECT
    Q_ENUMS( SpecialDataPropertyID )

    // Required by QSA
    Q_ENUMS( PenStyle )

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
      and name to empty string
      */
    KDChartPropertySet() :
        mOwnID( UndefinedID )
    { fullReset(""); }


    /**
      Constructor setting all Property Set IDs to the same ID value.

      This constructor may be used to initialize a property set
      and let it have all property IDs set to a specific value,
      e.g. you might pass KDCHART_PROPSET_NORMAL_DATA
      as ID to make the default property set the parent of all
      values.

      \param name (may be empty) a name describing this property set.
      \param idParent the ID of the parent property set. Skip this paramter
      to define a property set without specifying a parent.
      */
    KDChartPropertySet( const QString& name, int idParent=KDChartPropertySet::UndefinedID ) :
        mOwnID( UndefinedID )
    { fullReset( name, idParent ); }


    /**
      Copy the settings stored by property set \c source into this property set.

      \note Use this method instead of using the assignment operator.

      \sa clone, quickReset, fullReset
      */
    void deepCopy( const KDChartPropertySet* source );


    /**
      Create a new property set on the heap, copy the settings stored by
      this property set into the newly created property set and return
      the pointer to the new property set.

      \note Use this method instead of using the copy constructor.

      \sa deepCopy, quickReset, fullReset
      */
    const KDChartPropertySet* clone() const;


    /**
      Set the name,
      set all of the ID settings to idParent,
      but do NOT change the value settings,
      and do NOT modify mOwnID.

      \note Use this to quickly reset the ID settings: in most cases this should
      be sufficient for resetting the property set.

      \sa clone, fullReset
      */
    void quickReset( const QString& name, int idParent=KDChartPropertySet::UndefinedID );


    /**
      Set the name,
      set all of the ID settings to idParent,
      set all of the value settings back to their default value,
      but do NOT modify mOwnID.

      \note Use this to entirely reset both the ID values and the value settings: one of
      the very few reasons why you might want to do that might be your saving this property set
      into a data stream. In most other cases just calling quickReset should be sufficient.

      \sa clone, quickReset
      */
    void fullReset( const QString& name, int idParent=KDChartPropertySet::UndefinedID );


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

public slots:
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
    void setLineStyle( int idLineStyle, const PenStyle& lineStyle )
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
    bool hasOwnLineStyle( int& idLineStyle, PenStyle& lineStyle )
    {
        idLineStyle = mIdLineStyle;
        if( OwnID == mIdLineStyle ){
            lineStyle = mLineStyle;
            return true;
        }
        return false;
    }



    /**
      Specify the ID of the property set specifying the area brush
      to be used for this cell
      <b>or</b> specify the area brush directly.

      \note This function should be used for Area Charts in <b>Normal</b> mode
      only, otherwise the settings specified here will be ignored.

      \param idAreaBrush ID of the property set specifying the area brush.
      Use special value KDChartPropertySet::UndefinedID
      to specify neither another property set's ID
      nor an own value for the area brush.
      Use special value KDChartPropertySet::OwnID
      if you do NOT want to inherit another property set's
      settings but want to specify the area brush by using
      the following parameter.
      \param areaBrush   The area brush to be used.
      This parameter is stored but ignored if the previous parameter
      is not set to KDChartPropertySet::OwnID.

      \sa hasOwnAreaBrush
      */
    void setAreaBrush( int idAreaBrush, const QBrush& areaBrush )
    {
        mIdAreaBrush = idAreaBrush;
        mAreaBrush = areaBrush;
    }

    /**
      Returns whether this property set is specifying it's own area brush settings.

      \note This function should be used for Area Charts in <b>Normal</b> mode
      only, otherwise the settings specified here will be ignored.

      \returns TRUE if this property set is specifying it's own area brush settings,
      FALSE if the settings of another property set are to be used instead.

      \note The return value will also be FALSE if the 'default' properties
      are to be used: in this case idAreaBrush will be KDChartParams::NormalData

      \param idAreaBrush to be ignored if return value is TRUE.
      If idAreaBrush is KDChartPropertySet::UndefinedID
      then neither a property set ID
      nor an own area brush were specified (so no special
      area brush is associated to the respective data cell),
      else idAreaBrush contains the ID of another property set
      that is specifying the area brush to be used.
      \param areaBrush   this parameter's value is not modified if return value is FALSE.
      Parameter areaBrush contains the area brush value
      associated with the respective data cell.
      If return value is FALSE the areaBrush
      value is not set (so the parameter keeps its previous value)
      but this is to be overridden by the respective value
      of another property set that is indicated
      by the idAreaBrush parameter - unless this has the special
      value KDChartPropertySet::UndefinedID as decribed above.

      \sa setAreaBrush
      */
    bool hasOwnAreaBrush( int& idAreaBrush, QBrush& areaBrush )
    {
        idAreaBrush = mIdAreaBrush;
        if( OwnID == mIdAreaBrush ){
            areaBrush = mAreaBrush;
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
      \sa setMarkerAlign, setMarkerSize, setMarkerColor, setMarkerStyle
      \sa setLineWidth, setLineColor, setLineStyle
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
      \sa hasOwnMarkerAlign, hasOwnMarkerSize, hasOwnMarkerColor, hasOwnMarkerStyle
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
    
    /**
      Specify the ID of the property set specifying the alignment of the
      Marker to be displayed for this data value
      <b>or</b> specifying this flag directly.

      \note This function should be used for Line Charts only, otherwise
      the settings specified here will be ignored.

      \param idMarkerAlign ID of the property set specifying the alignment
      of the Marker to be shown.
      Use special value KDChartPropertySet::UndefinedID
      to specify neither another property set's ID
      nor an own value for this flag.
      Use special value KDChartPropertySet::OwnID
      if you do NOT want to inherit another property set's
      settings but want to specify the flag by using
      the following parameter.
      \param markerAlign  The alignment of the marker to be shown.
      This parameter is stored but ignored if the previous parameter
      is not set to KDChartPropertySet::OwnID.

      \sa hasOwnShowMarker
      \sa setMarkerAlign, setMarkerSize, setMarkerColor, setMarkerStyle
      \sa setLineWidth, setLineColor, setLineStyle
      */
    void setMarkerAlign( int idMarkerAlign, uint markerAlign )
    {
        mIdMarkerAlign = idMarkerAlign;
        mMarkerAlign =   markerAlign;
    }
    bool hasOwnMarkerAlign( int& idMarkerAlign, uint& markerAlign )
    {
        idMarkerAlign = mIdMarkerAlign;
        if( OwnID == idMarkerAlign ){
            markerAlign = mMarkerAlign;
            return true;
        }
        return false;
    }
    void setMarkerSize( int idMarkerSize, const QSize& markerSize )
    {
        mIdMarkerSize = idMarkerSize;
        mMarkerSize =   markerSize;
    }
    bool hasOwnMarkerSize( int& idMarkerSize, QSize& markerSize )
    {
        idMarkerSize = mIdMarkerSize;
        if( OwnID == idMarkerSize ){
            markerSize = mMarkerSize;
            return true;
        }
        return false;
    }
    void setMarkerColor( int idMarkerColor, const QColor& markerColor )
    {
        mIdMarkerColor = idMarkerColor;
        mMarkerColor =   markerColor;
    }
    bool hasOwnMarkerColor( int& idMarkerColor, QColor& markerColor )
    {
        idMarkerColor = mIdMarkerColor;
        if( OwnID == idMarkerColor ){
            markerColor = mMarkerColor;
            return true;
        }
        return false;
    }
    void setMarkerStyle( int idMarkerStyle, int markerStyle )
    {
        mIdMarkerStyle = idMarkerStyle;
        mMarkerStyle =   markerStyle;
    }
    bool hasOwnMarkerStyle( int& idMarkerStyle, int& markerStyle )
    {
        idMarkerStyle = mIdMarkerStyle;
        if( OwnID == idMarkerStyle ){
            markerStyle = mMarkerStyle;
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
    void setExtraLinesStyle( int idExtraLinesStyle, const PenStyle extraLinesStyle )
    {
        mIdExtraLinesStyle = idExtraLinesStyle;
        mExtraLinesStyle =   extraLinesStyle;
    }
    bool hasOwnExtraLinesStyle( int& idExtraLinesStyle, PenStyle& extraLinesStyle )
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
    KDChartPropertySet( const KDChartPropertySet& ) : QObject(0) {}

    QString mName;
    // IDs:                     values used if ID == OwnID:
    int mIdLineWidth;         int          mLineWidth;
    int mIdLineColor;         QColor       mLineColor;
    int mIdLineStyle;         Qt::PenStyle mLineStyle;
    int mIdShowMarker;        bool         mShowMarker;
    int mIdMarkerSize;        QSize        mMarkerSize;
    int mIdMarkerColor;       QColor       mMarkerColor;
    int mIdMarkerStyle;       int          mMarkerStyle;
    int mIdMarkerAlign;       uint         mMarkerAlign;
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
    int mIdAreaBrush;         QBrush       mAreaBrush;
    void fillValueMembersWithDummyValues();
};

#endif
