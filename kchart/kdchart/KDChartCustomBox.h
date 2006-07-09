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
#ifndef __KDCHARTCUSTOMBOX_H__
#define __KDCHARTCUSTOMBOX_H__

#include <qobject.h>
#include <qtextstream.h>
#include <qfont.h>
#include <qdom.h>
#include "KDChartGlobal.h"
#include "KDChartEnums.h"
#include "KDChartTextPiece.h"


class KDFrame;

// needed because there was no AlignAuto before Qt 3.0
#define KDCHART_AlignAuto 0


/** \file KDChartCustomBox.h
  \brief Definition of a class for specifying and drawing custom boxes.
  */

/**
  Class for specifying and drawing custom boxes.
  */
class KDCHART_EXPORT KDChartCustomBox :public QObject
{
    Q_OBJECT
    friend class KDChartPainter;

public:

    /**
      Constructor.
      Set default values.
      */
    KDChartCustomBox() :
        _rotation( 0 ),
        _fontSize( -10 ),
        _fontScaleGlobal( true ),
        _deltaX( 0 ),
        _deltaY( 0 ),
        _width(  0 ),
        _height( 0 ),
        _color( Qt::black ),
        _paper( Qt::NoBrush ),
        _anchorArea(  KDChartEnums::AreaInnermost ),
        _anchorPos(   KDChartEnums::PosTopLeft ),
        _anchorAlign( Qt::AlignTop + Qt::AlignLeft ),
        _dataRow( 0 ),
        _dataCol( 0 ),
        _data3rd( 0 ),
        _deltaAlign( KDCHART_AlignAuto ),
        _deltaScaleGlobal( true ),
        _anchorBeingCalculated( false )
    {
        const KDChartTextPiece piece( 0, "", QFont( "helvetica", 8, QFont::Normal, false ) );
        _content.deepCopy( &piece );
    }

    /**
      Constructor.
      Set content and font size but no position/size parameters.

      \note If \c fontScaleGlobal is set to true it is not necessary to
      specify the size of the box: if both \c width and \c height are zero
      and \c fontScaleGlobal is true the size of the box will be calculated
      automatically based upon the content size.  In this case the content
      size is interpreted as rich text string - even if the text is NOT
      framed by <tt><qt></tt> and <tt></qt></tt>.

      \param content The string or rich text string to be written into the box.
      \param fontSize The size of the font to be used, see explanation above.
      \param fontScaleGlobal If true the font size will be calculated based
      upon the the size of the drawing area, otherwise it will be calculated
      based upon the height of the box.
      */
    KDChartCustomBox( const KDChartTextPiece & content,
                      int fontSize,
                      bool fontScaleGlobal = true ) :
        _rotation( 0 ),
        _fontSize( fontSize ),
        _fontScaleGlobal( fontScaleGlobal ),
        _deltaX( 0 ),
        _deltaY( 0 ),
        _width(  0 ),
        _height( 0 ),
        _color( Qt::black ),
        _paper( Qt::NoBrush ),
        _anchorArea(  KDChartEnums::AreaInnermost ),
        _anchorPos(   KDChartEnums::PosTopLeft ),
        _anchorAlign( Qt::AlignTop + Qt::AlignLeft ),
        _dataRow( 0 ),
        _dataCol( 0 ),
        _data3rd( 0 ),
        _deltaAlign( KDCHART_AlignAuto ),
        _deltaScaleGlobal( true ),
        _anchorBeingCalculated( false )
    {
        _content.deepCopy( &content );
    }


    /**
      Constructor.
      Set content and position/size parameters and the color and brush to be used.

      \note If \c fontScaleGlobal is set to true it is not necessary to
      specify the size of the box: if both \c width and \c height are zero
      and \c fontScaleGlobal is true the size of the box will be calculated
      automatically based upon the content size.  In this case the content
      size is interpreted as rich text string - even if the text is NOT
      framed by <tt><qt></tt> and <tt></qt></tt>.

      \param content The text piece to be displayed.
      \param fontSize If \c fontSize value is greater 0, the value is taken as exact size,
      if \c fontSize is less than 0 it is interpreted as being a per-mille value
      of the size of the drawing area (or of the height of the box in case
      \c fontScaleGlobal is set to false, resp.).
      Normally the actual font size is calculated dynamically in methode paint.
      <b>However</b> if fontSize is zero no calculating will take place but the
      size of the content font is used.
      \param fontScaleGlobal If true the font size will be calculated based
      upon the the size of the drawing area, otherwise it will be calculated
      based upon the height of the box.
      \param deltaX The X distance between the box and its anchor.
      <b>Note: </b> If greater 0, the value is taken as exact offset,
      if less than 0, it is interpreted as being a per-mille value of the
      size of the drawing area (or as percent value of the actual font size
      (as returned by QFontMetrics::lineSpacing() ) if deltaScaleGlobal is FALSE, resp.).
      Actual drawing position/size is calculated dynamically in methode trueRect.
      \param deltaY The Y distance between the box and its anchor.
      \param width The width of the box.
      \param height The height of the box.
      \param color The text color.
      \param paper The brush to be used for the background.
      \param area The area to which the box is to be aligned.
      \param position The anchor position. This is the edge (or
      the corner, resp.) of the area to which
      the box is to be aligned.
      \param align The way how the box is to be aligned to its anchor.
      \param dataRow The row number of the KDChart data region that is to be used
      as anchor area. This parameter is ignored if \c area is not \c AreaChartDataRegion.
      \param dataCol The column number of the KDChart data region that is to be used
      as anchor area. This parameter is ignored if \c area is not \c AreaChartDataRegion.
      \param data3rd The third dimensions number of the KDChart data region that is to be used
      as anchor area. This parameter is ignored if \c area is not \c AreaChartDataRegion
      or if there is no 3-dimensional data structure.
      \param deltaAlign The way how \c deltaX and \deltaY affect the position of the box.
      Leave this parameter to its default value KDCHART_AlignAuto to have the delta values
      used according to the box's main \c align settings, otherwise specify your own
      alignment settings: e.g. right means there will be a gap between the right side of
      the box and its anchor point - if the main \c align parameter is set to right too
      the anchor point will to be outside of the box / if \c align is set to left
      (but the \c deltaAlign to right) the anchor point will be inside the box.
      Possible values for \c deltaAlign are:
      \li \c KDCHART_AlignAuto
      \li \c Qt::AlignLeft | Qt::AlignTop
      \li \c Qt::AlignLeft | Qt::AlignBottom
      \li \c Qt::AlignRight | Qt::AlignTop
      \li \c Qt::AlignRight | Qt::AlignBottom
      Using AlignVCenter or AlignHCenter or AlignCenter does not make sense here:
      center delta alignment will cause KDChart to ignore the respective delta
      settings: deltaX or deltaY or both will become ineffective.
      \param deltaScaleGlobal If true the actual delta X and delta Y values will
      be calculated by \c deltaX and \c deltaY based upon the size of the
      drawing area, otherwise they will be calculated based upon the actual
      font size.
      */
    KDChartCustomBox( const KDChartTextPiece & content,
                      int fontSize,
                      bool fontScaleGlobal,
                      int deltaX,
                      int deltaY,
                      int width,
                      int height,
                      const QColor & color = QColor( Qt::black   ),
                      const QBrush & paper = QBrush( Qt::NoBrush ),
                      uint area                           = KDChartEnums::AreaInnermost,
                      KDChartEnums::PositionFlag position = KDChartEnums::PosTopLeft,
                      uint align                          = Qt::AlignTop + Qt::AlignLeft,
                      uint dataRow = 0,
                      uint dataCol = 0,
                      uint data3rd = 0,
                      uint deltaAlign = KDCHART_AlignAuto,
                      bool deltaScaleGlobal = true )
        : _rotation( 0 ),
          _fontSize( fontSize ),
          _fontScaleGlobal( fontScaleGlobal ),
          _deltaX( deltaX ),
          _deltaY( deltaY ),
          _width( width ),
          _height( height ),
          _color( color ),
          _paper( paper ),
          _anchorArea( area ),
          _anchorPos( position ),
          _anchorAlign( align ),
          _dataRow( dataRow ),
          _dataCol( dataCol ),
          _data3rd( data3rd ),
          _deltaAlign( deltaAlign ),
          _deltaScaleGlobal( deltaScaleGlobal ),
          _anchorBeingCalculated( false )
    {
        _content.deepCopy( &content );
    }


    /**
      Constructor.
      Use this special constructor to specify a <b>rotated</b> box, reference
      point of the rotation is the anchor specified by the \c area and the
      \c position parameters.

      \note If \c fontScaleGlobal is set to true it is not necessary to
      specify the size of the box: if both \c width and \c height are zero
      and \c fontScaleGlobal is true the size of the box will be calculated
      automatically based upon the content size.  In this case the content
      size is interpreted as rich text string - even if the text is NOT
      framed by <tt><qt></tt> and <tt></qt></tt>.

      \param rotation The box's rotation angle in degrees (0 .. 360).
      \param content The text piece to be displayed.
      \param fontSize If \c fontSize value is greater 0, the value is taken as exact size,
      if \c fontSize is less than 0 it is interpreted as being a per-mille value
      of the size of the drawing area (or of the height of the box in case
      \c fontScaleGlobal is set to false, resp.).
      Normally the actual font size is calculated dynamically in methode paint.
      <b>However</b> if fontSize is zero no calculating will take place but the
      size of the content font is used.
      \param fontScaleGlobal If true the font size will be calculated based
      upon the the size of the drawing area, otherwise it will be calculated
      based upon the height of the box.
      \param deltaX The X distance between the box and its anchor.
      <b>Note: </b> If greater 0, the value is taken as exact offset,
      if less than 0, it is interpreted as being a per-mille value of the
      width of the drawing area (or as percent value of the actual font size
      (as returned by QFontMetrics::lineSpacing() ) if deltaScaleGlobal is FALSE, resp.).
      Actual drawing position/size is calculated dynamically in method trueRect.
      \param deltaY The Y distance between the box and its anchor.
      <b>Note: </b> If greater 0, the value is taken as exact offset,
      if less than 0, it is interpreted as being a per-mille value of the
      height of the drawing area (or as percent value of the actual font size
      (as returned by QFontMetrics::lineSpacing() ) if deltaScaleGlobal is FALSE, resp.).
      Actual drawing position/size is calculated dynamically in method trueRect.
      \param width The width of the box.
      \param height The height of the box.
      \param color The text color.
      \param paper The brush to be used for the background.
      \param area The area to which the box is to be aligned.
      \param position The anchor position. This is the edge (or
      the corner, resp.) of the area to which the box is to be aligned
      and it is also used as the reference point of the rotation.
      \param align The way how the box is to be aligned to its anchor.
      \param dataRow The row number of the KDChart data region that is to be used
      as anchor area. This parameter is ignored if \c area is not \c AreaChartDataRegion.
      \param dataCol The column number of the KDChart data region that is to be used
      as anchor area. This parameter is ignored if \c area is not \c AreaChartDataRegion.
      \param data3rd The third dimensions number of the KDChart data region that is to be used
      as anchor area. This parameter is ignored if \c area is not \c AreaChartDataRegion
      or if there is no 3-dimensional data structure.
      \param deltaAlign The way how \c deltaX and \deltaY affect the position of the box.
      Leave this parameter to its default value KDCHART_AlignAuto to have the delta values
      used according to the box's main \c align settings, otherwise specify your own
      alignment settings: e.g. Qt::AlignRight means the box will be moved to the left
      (by the amount calculated using the \c deltaX value), so there will be a gap
      between the right side of the box and its anchor point IF the main \c align flag
      is set to Qt::AlignRight too, so the anchor point will to be outside of the
      box then. However if the main \c align flag is set to Qt::AlignLeft the anchor
      point will be inside the box.
      Possible values for \c deltaAlign are:
      \li \c KDCHART_AlignAuto
      \li \c Qt::AlignLeft | Qt::AlignTop
      \li \c Qt::AlignLeft | Qt::AlignBottom
      \li \c Qt::AlignRight | Qt::AlignTop
      \li \c Qt::AlignRight | Qt::AlignBottom
      Using AlignVCenter or AlignHCenter or AlignCenter does not make sense here:
      center delta alignment will cause KDChart to ignore the respective delta
      settings: deltaX or deltaY or both will become ineffective.
      \note Moving of the box due to \c deltaAlign settings is applied after
      the box is rotated: e.g. this means a gap specified by \c deltaAlign = Qt::AlignTop
      gap will actually result in a left gap if the box is rotated by 90 degrees.
      \param deltaScaleGlobal If true the actual delta X and delta Y values will
      be calculated by \c deltaX and \c deltaY based upon the size of the
      drawing area, otherwise they will be calculated based upon the actual
      font size.
      */
    KDChartCustomBox( int rotation,
                      const KDChartTextPiece & content,
                      int fontSize,
                      bool fontScaleGlobal,
                      int deltaX,
                      int deltaY,
                      int width,
                      int height,
                      const QColor & color = QColor( Qt::black   ),
                      const QBrush & paper = QBrush( Qt::NoBrush ),
                      uint area                           = KDChartEnums::AreaInnermost,
                      KDChartEnums::PositionFlag position = KDChartEnums::PosTopLeft,
                      uint align                          = Qt::AlignTop + Qt::AlignLeft,
                      uint dataRow = 0,
                      uint dataCol = 0,
                      uint data3rd = 0,
                      uint deltaAlign = KDCHART_AlignAuto,
                      bool deltaScaleGlobal = true )
        : _rotation( rotation ),
          _fontSize( fontSize ),
          _fontScaleGlobal( fontScaleGlobal ),
          _deltaX( deltaX ),
          _deltaY( deltaY ),
          _width( width ),
          _height( height ),
          _color( color ),
          _paper( paper ),
          _anchorArea( area ),
          _anchorPos( position ),
          _anchorAlign( align ),
          _dataRow( dataRow ),
          _dataCol( dataCol ),
          _data3rd( data3rd ),
          _deltaAlign( deltaAlign ),
          _deltaScaleGlobal( deltaScaleGlobal ),
          _anchorBeingCalculated( false )
    {
        _content.deepCopy( &content );
    }


    /**
      Creates a DOM element node that represents a custom box for use
      in a DOM document.

      \param document the DOM document to which the node will belong
      \param parent the parent node to which the new node will be appended
      \param elementName the name of the new node
      \param custombox the custom box to be represented
      */
    static void createCustomBoxNode( QDomDocument& document,
                                     QDomNode& parent,
                                     const QString& elementName,
                                     const KDChartCustomBox* custombox );

    /**
      Reads data from a DOM element node that represents a custom box
      object and fills a KDChartCustomBox object with the data.

      \param element the DOM element to read from
      \param settings the custom box object to read the data into
      */
    static bool readCustomBoxNode( const QDomElement& element,
                                   KDChartCustomBox& custombox );

public slots: // PENDING(blackie) merge slots sections.

    float trueFontSize( double areaWidthP1000,
                        double areaHeightP1000,
                        int rectHeight ) const;
    int trueFontLineSpacing( double areaWidthP1000,
                             double areaHeightP1000,
                             int rectHeight ) const;
    int trueRectAlignX(const QRect& rect) const;
    int trueRectAlignY(const QRect& rect) const;
    void getTrueShift( double areaWidthP1000,
                       double areaHeightP1000,
                       int rectHeight,
                       int& dX,
                       int& dY )const;


    /**
      Return the actual rectangle which to draw box into.

      \param anchor The anchor point which the box is to be aligned to.
      This can be any point within the painter drawing area but you
      will probably compute a point using anchorArea(), anchorPosition(), anchorAlign()
      (and dataRow(), dataCol(), data3rd() when dealing with KDChart data regions, resp.)
      \param areaWidthP1000 The thousands part of the logical width
      of the area to be used for drawing.
      \param areaHeightP1000 The thousands part of the logical height
      of the area to be used for drawing.
      */

    virtual QRect trueRect( QPoint anchor,
                            double areaWidthP1000,
                            double areaHeightP1000 ) const ;

    /**
      Paints the box.

      \param painter The QPainter to be used for drawing.
      \param anchor The anchor point which the box is to be aligned to.
      This can be any point within the painter drawing area but you
      will probably compute a point using anchorArea(), anchorPosition(), anchorAlign()
      (and dataRow(), dataCol(), data3rd() when dealing with KDChart data regions, resp.)
      \param areaWidthP1000 The thousands part of the drawing area width.
      \param areaHeightP1000 The thousands part of the drawing area height.
      \param color The text color to be used.
      \param paper The brush to be used for the background.
      \param rect The rectangle to be drawn into. If empty the rectangle will be
      calculated dynamically based upon the \c x, \c y, \c width, \c height values
      (that were set via Constructor or via setPosAndSize) and based upon the
      logical height (or width, resp.) of the painters drawing area.
      */
    virtual void paint( QPainter* painter,
                        QPoint anchor,
                        double areaWidthP1000,
                        double areaHeightP1000,
                        const KDFrame* frame     = 0,
                        const QRect&   frameRect = QRect(),
                        const QColor * color = 0,
                        const QBrush * paper = 0 ) const ;

    /**
      Specifies the rotation angle of the box in degrees (0..360).
      */
    void setRotation( int rotation )
    {
        _rotation = rotation;
    }

    /**
      Specifies the text piece content to be drawn.
      */
    void setContent( const KDChartTextPiece & content )
    {
        _content.deepCopy( &content );
    }

    /**
      Specifies the font size to be used.

      \note If \c fontScaleGlobal is set to true it is not necessary to
      specify the size of the box: if both \c width and \c height are zero
      and \c fontScaleGlobal is true the size of the box will be calculated
      automatically based upon the content size.  In this case the content
      size is interpreted as rich text string - even if the text is NOT
      framed by <tt><qt></tt> and <tt></qt></tt>.

      \param fontSize If \c fontSize value is greater 0, the value is taken as exact size,
      if \c fontSize is less than 0 it is interpreted as being a per-mille value
      of the size of the drawing area (or of the height of the box in case
      \c fontScaleGlobal is set to false, resp.).
      Normally the actual font size is calculated dynamically in methode paint.
      <b>However</b> if fontSize is zero no calculating will take place but the
      size of the content font is used.
      \param fontScaleGlobal If true the font size will be calculated based
      upon the the size of the drawing area, otherwise it will be calculated
      based upon the height of the box.
      */
    void setFontSize( int fontSize, bool fontScaleGlobal )
    {
        _fontSize        = fontSize;
        _fontScaleGlobal = fontScaleGlobal;
    }

    /**
      Specifies the area to which the box is to be aligned.
      */
    void setAnchorArea( uint area )
    {
        _anchorArea = area;
    }

    /**
      Specifies the anchor position.
      This is the edge (or the corner, resp.) of the area
      to which the box is to be aligned.
      */
    void setAnchorPosition( KDChartEnums::PositionFlag position )
    {
        // Note if you change the parameters here, then you must also change them in wrappers/KDChartCustomBoxWrapper.h
        _anchorPos = position;
    }

    /**
      Specifies the way how the box is to be aligned to its anchor.
      */
    void setAnchorAlign( uint align )
    {
        _anchorAlign = align;
    }

    /**
      Specifies the row number of the KDChart data region that is to be used
      as anchor area. This value is ignored if anchorArea is not \c AreaChartDataRegion.
      */
    void setDataRow( uint dataRow )
    {
        _dataRow = dataRow;
    }

    /**
      Specifies the column number of the KDChart data region that is to be used
      as anchor area. This value is ignored if anchorArea is not \c AreaChartDataRegion.
      */
    void setDataCol( uint dataCol )
    {
        _dataCol = dataCol;
    }

    /**
      Specifies the third dimensions number of the KDChart data region that is to be used
      as anchor area. This value is ignored if anchorArea is not \c AreaChartDataRegion
      or if there is no 3-dimensional data structure.
      */
    void setData3rd( uint data3rd )
    {
        _data3rd = data3rd;
    }

    /**
      Specifies the distance between the box and the anchor point and
      specifies the size of the box.

      \param deltaX The X distance between the box and its anchor.
      <b>Note: </b> If greater 0, the value is taken as exact offset,
      if less than 0, it is interpreted as being a per-mille value of the
      size of the drawing area (or as percent value of the actual font size
      (as returned by QFontMetrics::lineSpacing() ) if deltaScaleGlobal is FALSE, resp.).
      Actual drawing position/size is calculated dynamically in methode trueRect.
      \param deltaY The Y distance between the box and its anchor.
      \param width The width of the drawing region.
      \param height The height of the drawing region.
      \param deltaAlign the way how the values specified for deltaX and/or deltaY
      affect the position of the box.
      \param deltaScaleGlobal If true the actual delta X and delta Y values will
      be calculated by \c deltaX and \c deltaY based upon the size of the
      drawing area, otherwise they will be calculated based upon the actual
      font size.

      \sa setDistance, setSize, setDeltaAlign, setDeltaScale
      */
    void setDistanceAndSize( int deltaX,
                             int deltaY,
                             int width,
                             int height,
                             uint deltaAlign = KDCHART_AlignAuto,
                             bool deltaScaleGlobal = true )
    {
        _deltaX = deltaX;
        _deltaY = deltaY;
        _width = width;
        _height = height;
        _deltaAlign = deltaAlign;
        _deltaScaleGlobal = deltaScaleGlobal;
    }

    /**
      Specifies the distance between the box and the anchor point.

      \note If \c fontScaleGlobal is set to true it is not necessary to
      specify the size of the box: if both \c width and \c height are zero
      and \c fontScaleGlobal is true the size of the box will be calculated
      automatically based upon the content size.  In this case the content
      size is interpreted as rich text string - even if the text is NOT
      framed by <tt><qt></tt> and <tt></qt></tt>.

      \param deltaX The X distance between the box and its anchor.
      <b>Note: </b> If greater 0, the value is taken as exact offset,
      if less than 0, it is interpreted as being a per-mille value of the
      size of the drawing area (or as percent value of the actual font size
      (as returned by QFontMetrics::lineSpacing() ) if deltaScaleGlobal is FALSE, resp.).
      Actual drawing position/size is calculated dynamically in methode trueRect.
      \param deltaY The Y distance between the box and its anchor.
      \param align the way how the values specified for deltaX and/or deltaY
      affect the position of the box.
      \param deltaScaleGlobal If true the actual delta X and delta Y values will
      be calculated by \c deltaX and \c deltaY based upon the size of the
      drawing area, otherwise they will be calculated based upon the actual
      font size.

      \sa setDistanceAndSize, setSize, setDeltaAlign, setDeltaScale
      */
    void setDistance( int deltaX,
                      int deltaY,
                      uint align = KDCHART_AlignAuto,
                      bool deltaScaleGlobal = true )
    {
        _deltaX = deltaX;
        _deltaY = deltaY;
        _deltaAlign = align;
        _deltaScaleGlobal = deltaScaleGlobal;
    }

    /**
      Specifies the way how the values specified for deltaX and/or deltaY
      affect the position of the box.

      Set this to KDHART_KDCHART_AlignAuto to have the delta values
      used according to the box's main \c align settings, otherwise specify your own
      alignment settings: e.g. right means there will be a gap between the right side of
      the box and its anchor point - if the main \c align parameter is set to right too
      the anchor point will to be outside of the box / if \c align is set to left
      (but the \c deltaAlign to right) the anchor point will be inside the box.
      Possible values for \c deltaAlign are:
      \li \c KDCHART_AlignAuto
      \li \c Qt::AlignLeft | Qt::AlignTop
      \li \c Qt::AlignLeft | Qt::AlignBottom
      \li \c Qt::AlignRight | Qt::AlignTop
      \li \c Qt::AlignRight | Qt::AlignBottom
      Using AlignVCenter or AlignHCenter or AlignCenter does not make sense here:
      center delta alignment will cause KDChart to ignore the respective delta
      settings: deltaX or deltaY or both will become ineffective.
      \note Moving of the box due to \c deltaAlign settings is applied after
      the box is rotated: e.g. this means a gap specified by \c deltaAlign = Qt::AlignTop
      gap will actually result in a left gap if the box is rotated by 90 degrees.
      \param deltaScaleGlobal If true the actual delta X and delta Y values will
      be calculated by \c deltaX and \c deltaY based upon the size of the
      drawing area, otherwise they will be calculated based upon the actual
      font size.

      \sa setDeltaScale, setDistance, setDistanceAndSize, deltaAlign
      */
    void setDeltaAlign( uint align,
                        bool deltaScaleGlobal = true )
    {
        _deltaAlign = align;
        _deltaScaleGlobal = deltaScaleGlobal;
    }

    /**
      Specifies the way how the distance between the box and its anchor
      will be calculated.

      \param deltaScaleGlobal If true the actual delta X and delta Y values will
      be calculated by \c deltaX and \c deltaY based upon the size of the
      drawing area, otherwise they will be calculated based upon the actual
      font size.

      \sa setDeltaAlign, setDistance, setDistanceAndSize, deltaAlign
      */
    void setDeltaScale( bool deltaScaleGlobal )
    {
        _deltaScaleGlobal = deltaScaleGlobal;
    }

    /**
      Specifies the size of the box.

      \note If \c fontScaleGlobal is set to true it is not necessary to
      specify the size of the box: if both \c width and \c height are zero
      and \c fontScaleGlobal is true the size of the box will be calculated
      automatically based upon the content size.  In this case the content
      size is interpreted as rich text string - even if the text is NOT
      framed by <tt><qt></tt> and <tt></qt></tt>.

      \param width The width of the box.
      <b>Note:</b> If greater 0, the value is taken as exact offset,
      if less than 0, it is interpreted as being a per-mille value
      of the logical height (or width, resp.) of the area to be used for drawing.
      Actual drawing position/size is calculated dynamically in methode trueRect.
      \param height The height of the box.

      \sa setDistance, setDistanceAndSize, setDeltaAlign
      */
    void setSize( int width, int height )
    {
        _width = width;
        _height = height;
    }

    /**
      Specifies the text color to be used.

      \param color The text color.
      */
    void setColor( QColor color )
    {
        _color = color;
    }

    /**
      Specifies the brush to be used for the background.

      \param paper The brush to be used for the background.
      */
    void setPaper( const QBrush & paper )
    {
        _paper = paper;
    }

    /**
      Returns the rotation angle of the box in degrees (0..360).
      */
    int rotation() const
    {
        return _rotation;
    }

    /**
      Returns the text piece content that is to be drawn.
      */
    const KDChartTextPiece & content() const
    {
        return _content;
    }

    /**
      Returns the font size to be used.

      \note If fontSize value is greater 0, the value is taken as exact size,
      if less than 0, it is interpreted as being a per-mille value of the logical
      height (or width, resp.) of the area to be used for drawing.
      Normally the actual font size is calculated dynamically in methode paint.
      <b>However</b> if fontSize is zero no calculating will take place but the
      size of the content font is used.
      */
    int fontSize() const
    {
        return _fontSize;
    }

    /**
      Returns the way how the font size is calculated <b>if</b> fontSize() is negative.

      If true the font size will be calculated based upon the the size of the drawing
      area, otherwise it will be calculated based upon the height of the box.
      */
    bool fontScaleGlobal() const
    {
        return _fontScaleGlobal;
    }

    /**
      Returns the area to which the box is to be aligned.
      */
    uint anchorArea() const
    {
        return _anchorArea;
    }

    /**
      Returns the anchor position.
      This is the edge (or the corner, resp.) of the area
      to which the box is to be aligned.
      */
    KDChartEnums::PositionFlag anchorPosition() const
    {
        return _anchorPos;
    }

    /**
      Returns the way how the box is to be aligned to its anchor.
      */
    uint anchorAlign() const
    {
        return _anchorAlign;
    }

    /**
      Returns the row number of the KDChart data region that is to be used
      as anchor area. You should use this if anchorArea is \c AreaChartDataRegion
      to find out the data region which the box is to be aligned to.
      */
    uint dataRow() const
    {
        return _dataRow;
    }

    /**
      Returns the column number of the KDChart data region that is to be used
      as anchor area. You should use this if anchorArea is \c AreaChartDataRegion
      to find out the data region which the box is to be aligned to.
      */
    uint dataCol() const
    {
        return _dataCol;
    }

    /**
      Returns the third dimensions number of the KDChart data region that is to be used
      as anchor area. You should use this if anchorArea is \c AreaChartDataRegion
      and if there is a 3-dimensional data structure
      to find out the data region which the box is to be aligned to.
      */
    uint data3rd() const
    {
        return _data3rd;
    }

    /**
      Returns the X distance between the box and its anchor.
      <b>Note: </b> If greater 0, the value is taken as exact offset,
      if less than 0, it is interpreted as being a per-mille value of the
      size of the drawing area (or as percent value of the actual font size
      (as returned by QFontMetrics::lineSpacing() ) if deltaScaleGlobal is FALSE, resp.).
      Actual drawing position/size is calculated dynamically in methode trueRect.

      \sa deltaY, deltaAlign, deltaScaleGlobal
      */
    int deltaX() const
    {
        return _deltaX;
    }

    /**
      Returns the Y distance between the box and its anchor.
      <b>Note: </b> If greater 0, the value is taken as exact offset,
      if less than 0, it is interpreted as being a per-mille value of the
      size of the drawing area (or as percent value of the actual font size
      (as returned by QFontMetrics::lineSpacing() ) if deltaScaleGlobal is FALSE, resp.).
      Actual drawing position/size is calculated dynamically in methode trueRect.

      \sa deltaX, deltaAlign, deltaScaleGlobal
      */
    int deltaY() const
    {
        return _deltaY;
    }

    /**
      Returns the way how the values specified for deltaX and/or deltaY
      affect the position of the box.

      \sa setDeltaAlign, deltaX, deltaY, deltaScaleGlobal
      */
    uint deltaAlign() const
    {
        return _deltaAlign;
    }

    /**
      Returns the way how the distance between the box and its anchor
      is calculated.

      \sa setDeltaScaleGlobal, deltaX, deltaY, deltaAlign
      */
    bool deltaScaleGlobal() const
    {
        return _deltaScaleGlobal;
    }

    /**
      Returns the width of the region where
      to draw the box. <b>Note:</b> If greater 0, the value is the exact offset,
      if less than 0, it is interpreted as being a per-mille value
      of the logical height (or width, resp.) of the area to be used for drawing.
      Actual drawing position/size is calculated dynamically in methode trueRect.
      */
    int width() const
    {
        return _width;
    }

    /**
      Returns the height of the region where
      to draw the box. <b>Note:</b> If greater 0, the value is the exact offset,
      if less than 0, it is interpreted as being a per-mille value
      of the logical height (or width, resp.) of the area to be used for drawing.
      Actual drawing position/size is calculated dynamically in methode trueRect.
      */
    int height() const
    {
        return _height;
    }

    /**
      Returns the text color.
      */
    QColor color() const
    {
        return _color;
    }

    /**
      Returns the background brush.
      */
    const QBrush & paper() const
    {
        return _paper;
    }

public:
    /**
      Destructor. Only defined to have it virtual.
      */
    virtual ~KDChartCustomBox();

    /**
      Copy the settings of box \c source into this box.

      \note Use this method instead of using the assignment operator.

      \sa clone
      */
    void deepCopy( const KDChartCustomBox* source );


    /**
      Create a new box on the heap, copy the settings stored by
      this box into the newly created box and return
      the pointer to the new box.

      \note Use this method instead of using the copy constructor.

      \sa deepCopy
      */
    const KDChartCustomBox* clone() const;

private:
    KDChartCustomBox( const KDChartCustomBox& ) : QObject(0) {}

protected:
    /**
      Internal routine for recursion handling.
      Note: This is a const methode changing NO VITAL information
            of the box but setting an internal, temporary flag.
      */
    void setInternalFlagAnchorBeingCalculated( bool flag ) const
    {
        KDChartCustomBox* that = const_cast<KDChartCustomBox*>(this);
        that->_anchorBeingCalculated = flag;
    }
    /**
      Internal routine for recursion handling.
      */
    bool anchorBeingCalculated() const
    {
        return _anchorBeingCalculated;
    }

private:
    int              _rotation;
    KDChartTextPiece _content;
    int              _fontSize;
    bool             _fontScaleGlobal;
    // Values to be transformed into a real rect at painting time.
    // If greater 0, values are exact, if less than 0, values are in per-mille
    // of the size of the drawing area.
    int _deltaX;
    int _deltaY;
    int _width;
    int _height;

    QColor _color;
    QBrush _paper;

    uint                       _anchorArea;
    KDChartEnums::PositionFlag _anchorPos;
    uint                       _anchorAlign;
    uint _dataRow;
    uint _dataCol;
    uint _data3rd;
    uint                       _deltaAlign;
    bool                       _deltaScaleGlobal;
    //
    // The following flag is NOT to be saved/restored in a file.
    //
    // Being a TEMPORARY flag preventing circular recursion
    // it must be set to   f a l s e
    // after loading a KDChartCustomBox from a file.
    bool _anchorBeingCalculated;
};


/**
  Writes the KDChartCustomBox object p as an XML document to the text stream s.

  \param s the text stream to write to
  \param p the KDChartCustomBox object to write
  \return the text stream after the write operation
  */
//QTextStream& operator<<( QTextStream& s, const KDChartCustomBox& p );


/**
  Reads the an XML document from the text stream s into the
  KDChartCustomBox object p

  \param s the text stream to read from
  \param p the KDChartCustomBox object to read into
  \return the text stream after the read operation
  */
//QTextStream& operator>>( QTextStream& s, KDChartCustomBox& p );


#endif
