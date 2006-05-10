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
#ifndef __KDCHARTPARAMS_H__
#define __KDCHARTPARAMS_H__

#include <qapplication.h>
#include <QFont>
#include <QColor>
#include <qpen.h>
#include <QMap>
#include <q3dict.h>
#include <q3intdict.h>
#include <qobject.h>
#include <qtextstream.h>
#include <q3simplerichtext.h>
#include <qdom.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3ValueList>
#include <limits.h>

#include <math.h>
#ifdef Q_WS_WIN
#define M_PI 3.14159265358979323846
#endif

#include "KDChartGlobal.h"
#include "KDChartEnums.h"
#include "KDChartCustomBox.h"
#include "KDFrame.h"
#include "KDChartAxisParams.h"
#include "KDChartPropertySet.h"

#if COMPAT_QT_VERSION >= 0x030000
#include <q3valuevector.h>
#else
#include <qarray.h>
#endif

/** \file KDChartParams.h
    \brief Header for all common chart parameters.

    This file is used to access all chart parameters except of the
    axis settings which you will find in \c KDChartAxisParams.h

    \see KDChartAxisParams.h
*/


typedef Q3IntDict<KDChartPropertySet> KDChartPropertySetList;

#define KDCHART_ALL_AXES UINT_MAX-1

// PENDING(blackie) KHZ, please clean up here. I guess this defined can be removed now.
// Note: The following specification matches the UINT_MAX value used
//       in KDChartAxisParams::KDChartAxisParams() to initialize the
//       axis' _axisIsoRefAxis member.
//       OK, this is a dirty hack but is avoids circular dependency
//       between KDChartParams and KDChartAxisParams
#define KDCHART_NO_AXIS UINT_MAX
#define KDCHART_ALL_DATASETS UINT_MAX - 1

#define KDCHART_NO_DATASET UINT_MAX
#define KDCHART_UNKNOWN_CHART UINT_MAX - 2
#define KDCHART_ALL_CHARTS UINT_MAX - 1
#define KDCHART_NO_CHART UINT_MAX
#define KDCHART_GLOBAL_LINE_STYLE UINT_MAX
#define KDCHART_AUTO_SIZE INT_MAX
#define KDCHART_DATA_VALUE_AUTO_DIGITS INT_MAX
#define KDCHART_SAGITTAL_ROTATION INT_MAX
#define KDCHART_TANGENTIAL_ROTATION INT_MAX - 1
#define KDCHART_PROPSET_NORMAL_DATA 0
#define KDCHART_PROPSET_TRANSPARENT_DATA 1
#define KDCHART_PROPSET_HORI_LINE  2
#define KDCHART_PROPSET_VERT_LINE  3

// #define KDCHART_DATA_VALUE_AUTO_COLOR KDChartAutoColor::instance()->color()

// PENDING(blackie) Can we delete this now?
/* we must keep this wrongly spelled value for backward compatibility reasons */
#define KDCHART_SAGGITAL_ROTATION INT_MAX

/**
  Our charts may have up to 4 ordinate axes:
  2 left ones and 2 right ones
  */
#define KDCHART_CNT_ORDINATES 4

#define KDCHART_MAX_POLAR_DELIMS_AND_LABELS_POS 8
#define KDCHART_MAX_AXES 12


#define KDCHART_DATA_VALUE_AUTO_COLOR (KDChartAutoColor::instance()->color())

class KDCHART_EXPORT KDChartAutoColor {
public:
    static const KDChartAutoColor* instance();
    static void freeInstance();
    const QColor* color() const
    {
        return &mColor;
    }
private:
    KDChartAutoColor();
    KDChartAutoColor( KDChartAutoColor const& );
    ~KDChartAutoColor();
    QColor mColor;
    static KDChartAutoColor *mInstance;
};

/*
// #define KDCHART_DATA_VALUE_AUTO_COLOR KDChartParams_KDChartAutoColor
*/


class KDCHART_EXPORT KDChartParams : public QObject
{
    Q_OBJECT
    Q_ENUMS(ChartType)
    Q_ENUMS(SourceMode)
    Q_ENUMS(BarChartSubType)
    Q_ENUMS(LineChartSubType)
    Q_ENUMS(LineMarkerStyle)
    Q_ENUMS(AreaChartSubType)
    Q_ENUMS(AreaLocation)
    Q_ENUMS(PolarChartSubType)
    Q_ENUMS(PolarMarkerStyle)
    Q_ENUMS(HiLoChartSubType)
    Q_ENUMS(BWChartSubType)
    Q_ENUMS(BWStatVal)
    Q_ENUMS(LegendPosition)
    Q_ENUMS(LegendSource)
    Q_ENUMS(HdFtPos)

    // Needed for QSA
    Q_ENUMS( Qt::BrushStyle )
    Q_ENUMS( Qt::PenStyle )
    Q_ENUMS( Qt::Orientation )

public slots: // PENDING(blackie) merge public slots into one section.
    void setOptimizeOutputForScreen( bool screenOutput )
        {
            _optimizeOutputForScreen = screenOutput;
        }


    bool optimizeOutputForScreen() const
        {
            return _optimizeOutputForScreen;
        }

    void setGlobalLeading( int left, int top, int right, int bottom );

    void setGlobalLeadingLeft( int leading )
        {
            _globalLeadingLeft = leading;
            emit changed();
        }
    void setGlobalLeadingTop( int leading )
        {
            _globalLeadingTop = leading;
            emit changed();
        }
    void setGlobalLeadingRight( int leading )
        {
            _globalLeadingRight = leading;
            emit changed();
        }
    void setGlobalLeadingBottom( int leading )
        {
            _globalLeadingBottom = leading;
            emit changed();
        }
    int globalLeadingLeft() const
        {
            return _globalLeadingLeft;
        }
    int globalLeadingTop() const
        {
            return _globalLeadingTop;
        }
    int globalLeadingRight() const
        {
            return _globalLeadingRight;
        }
    int globalLeadingBottom() const
        {
            return _globalLeadingBottom;
        }

    int registerProperties( KDChartPropertySet& rSet );
    void setProperties( int id, KDChartPropertySet& rSet );
    bool removeProperties( int id );
    bool properties( int id, KDChartPropertySet& rSet ) const;
    KDChartPropertySet* properties( int id );
    bool calculateProperties( int startId, KDChartPropertySet& rSet ) const;

public:

#ifndef Q_MOC_RUN
    class KDCHART_EXPORT KDChartFrameSettings
    {
        // Q_OBJECT
    public:
        KDChartFrameSettings();
        KDChartFrameSettings( uint           dataRow,
                              uint           dataCol,
                              uint           data3rd,
                              const KDFrame& frame,
                              int            outerGapX,
                              int            outerGapY,
                              int            innerGapX,
                              int            innerGapY,
                              bool           addFrameWidthToLayout  = true,
                              bool           addFrameHeightToLayout = true );

        // public slots:
        uint dataRow() const
            {
                return _dataRow;
            }
        uint dataCol() const
            {
                return _dataCol;
            }
        uint data3rd() const
            {
                return _data3rd;
            }

        void setDataRow( uint dataRow )
            {
                _dataRow = dataRow;
            }
        void setDataCol( uint dataCol )
            {
                _dataCol = dataCol;
            }
        void setData3rd( uint data3rd )
            {
                _data3rd = data3rd;
            }


        void resetFrame()
        {
            if( _frame )
                delete _frame;
            _frame = new KDFrame();
        }


        const KDFrame* framePtr() const
            {
                return _frame;
            }


        const KDFrame& frame() const
            {
                return *_frame;
            }


        int innerGapX() const
            {
                return _innerGapX;
            }


        int innerGapY() const
            {
                return _innerGapY;
            }


        int outerGapX() const
            {
                return _outerGapX;
            }


        int outerGapY() const
            {
                return _outerGapY;
            }


        bool addFrameWidthToLayout() const
            {
                return _addFrameWidthToLayout;
            }


        bool addFrameHeightToLayout() const
            {
                return _addFrameHeightToLayout;
            }

        // public:
        virtual ~KDChartFrameSettings();


        static void createFrameSettingsNode( QDomDocument& document,
                                             QDomNode& parent,
                                             const QString& elementName,
                                             const KDChartFrameSettings* settings,
                                             uint areaId );

        static bool readFrameSettingsNode( const QDomElement& element,
                                           KDChartFrameSettings& settings,
                                           uint& areaId );


        static void deepCopy( KDChartFrameSettings& D, const KDChartFrameSettings& R ) {
            D._dataRow = R._dataRow;
            D._dataCol = R._dataCol;
            D._data3rd = R._data3rd;

            delete D._frame;
            if( R._frame ){
                D._frame = new KDFrame();
                KDFrame::deepCopy(*D._frame, *R._frame);
            }else{
                D._frame = 0;
            }

            D._outerGapX = R._outerGapX;
            D._outerGapY = R._outerGapY;
            D._innerGapX = R._innerGapX;
            D._innerGapY = R._innerGapY;
            D._addFrameWidthToLayout = R._addFrameWidthToLayout;
            D._addFrameHeightToLayout = R._addFrameHeightToLayout;
        }

    private:
        // KDChartFrameSettings( const KDChartFrameSettings& other ) :QObject( 0 ) {
        KDChartFrameSettings( const KDChartFrameSettings& ) {}
        KDChartFrameSettings& operator=( const KDChartFrameSettings& ){return *this;}


    private:
        uint    _dataRow;
        uint    _dataCol;
        uint    _data3rd;
        KDFrame* _frame;
        int     _outerGapX;
        int     _outerGapY;
        int     _innerGapX;
        int     _innerGapY;
        bool    _addFrameWidthToLayout;
        bool    _addFrameHeightToLayout;
    };

#endif

public slots:

    void setFrame( uint           area,
                   const KDFrame& frame,
                   int            outerGapX,
                   int            outerGapY,
                   int            innerGapX,
                   int            innerGapY,
                   bool           addFrameWidthToLayout = true,
                   bool           addFrameHeightToLayout = true )
        {
            _areaDict.setAutoDelete( TRUE );
            _areaDict.insert( QString( "%1/-----/-----/-----" ).arg( area, 5 ),
                              new KDChartFrameSettings(0,0,0,
                                                       frame,
                                                       outerGapX,
                                                       outerGapY,
                                                       innerGapX,
                                                       innerGapY,
                                                       addFrameWidthToLayout,
                                                       addFrameHeightToLayout ) );
            emit changed();
        }


    // Note if you change the parameters here, then you must also change them in wrappers/KDChartParametersWrapper.h
    void setSimpleFrame( uint        area,
                         int         outerGapX = 0,
                         int         outerGapY = 0,
                         int         innerGapX = 0,
                         int         innerGapY = 0,
                         bool        addFrameWidthToLayout      = true,
                         bool        addFrameHeightToLayout     = true,
                         KDFrame::SimpleFrame    simpleFrame    = KDFrame::FrameFlat,
                         int                     lineWidth      = 1,
                         int                     midLineWidth   = 0,
                         QPen                    pen            = QPen(),
                         QBrush                  background     = QBrush( Qt::NoBrush ),
                         const QPixmap*          backPixmap     = 0, // no pixmap
                         KDFrame::BackPixmapMode backPixmapMode = KDFrame::PixStretched,
                         int                     shadowWidth    = 0,
                         KDFrame::CornerName     sunPos         = KDFrame::CornerTopLeft )
        {
            _areaDict.setAutoDelete( TRUE );
            KDFrame frame( QRect(0,0,0,0),
                     simpleFrame,
                     lineWidth,
                     midLineWidth,
                     pen,
                     background,
                     backPixmap,
                     backPixmapMode,
                     shadowWidth,
                           sunPos );

            _areaDict.insert( QString( "%1/-----/-----/-----" ).arg( area, 5 ),
                              new KDChartFrameSettings( 0,0,0, frame,
                                                        outerGapX,
                                                        outerGapY,
                                                        innerGapX,
                                                        innerGapY,
                                                        addFrameWidthToLayout,
                                                        addFrameHeightToLayout ) );
            emit changed();
        }


    // Note if you change the parameters here, then you must also change them in wrappers/KDChartParametersWrapper.h
    void setDataRegionFrame( uint dataRow,
                             uint dataCol,
                             uint, // important: we ignore the data3rd parameter for now!
                             int  innerGapX = 0,
                             int  innerGapY = 0,
                             bool addFrameWidthToLayout      = true,
                             bool addFrameHeightToLayout     = true,
                             KDFrame::SimpleFrame    simpleFrame    = KDFrame::FrameFlat,
                             int                     lineWidth      = 1,
                             int                     midLineWidth   = 0,
                             QPen                    pen            = QPen(),
                             int                     shadowWidth    = 0,
                             KDFrame::CornerName     sunPos         = KDFrame::CornerTopLeft )
        {
            _areaDict.setAutoDelete( TRUE );
            KDFrame frame( QRect(0,0,0,0),
                           simpleFrame,
                           lineWidth,
                           midLineWidth,
                           pen,
                           QBrush( Qt::NoBrush ),
                           0,
                           KDFrame::PixStretched,
                           shadowWidth,
                           sunPos );

            _areaDict.insert(
                dataRegionFrameAreaName( dataRow, dataCol, 0 ), //data3rd 5 ),
                new KDChartFrameSettings( dataRow,
                                          dataCol,
                                          0, // important: we ignore the data3rd parameter for now!
                                          frame,
                                          0,
                                          0,
                                          innerGapX,
                                          innerGapY,
                                          addFrameWidthToLayout,
                                          addFrameHeightToLayout ) );
            emit changed();
        }


    // Note if you change the parameters here, then you must also change them in wrappers/KDChartParametersWrapper.h
    bool moveDataRegionFrame( uint oldDataRow,
                             uint oldDataCol,
                             uint, // important: we ignore the data3rd parameter for now!
                             uint newDataRow,
                             uint newDataCol,
                             uint // important: we ignore the data3rd parameter for now!
                             );


    const KDChartFrameSettings* frameSettings( uint area,
                                               bool& bFound,
                                               int* pIterIdx=0 ) const;

    const KDChartFrameSettings* nextFrameSettings( bool& bFound,
                                                   int* pIterIdx ) const;


public:
    typedef Q3IntDict <KDChartCustomBox > CustomBoxDict;

public slots:
    uint insertCustomBox( const KDChartCustomBox & box );
    bool removeCustomBox( const uint & idx );

    void removeAllCustomBoxes()
    {
        emit changed();
        _customBoxDict.setAutoDelete( true );
        _customBoxDict.clear();
    }

    KDChartCustomBox* customBoxRef( uint box );
    const KDChartCustomBox* customBox( uint box ) const;
    uint maxCustomBoxIdx() const;


public:
    enum ChartType { NoType, Bar, Line, Area, Pie, HiLo, Ring, Polar, BoxWhisker };

public slots:
    void setChartType( ChartType chartType );
    ChartType chartType() const
        {
            return _chartType;
        }


    static QString chartTypeToString( ChartType type );
    static ChartType stringToChartType( const QString& string );

    void setAdditionalChartType( ChartType chartType );
    ChartType additionalChartType() const
        {
            return _additionalChartType;
        }
public:

    enum SourceMode {
        UnknownMode      = 0,
        DontUse          = 1,
        DataEntry        = 2,
        AxisLabel        = 3,
        LegendText       = 4,
        ExtraLinesAnchor = 5,
        Last_SourceMode  = ExtraLinesAnchor };

public slots:
    void setChartSourceMode( SourceMode mode,
                             uint dataset,
                             uint dataset2 = KDCHART_NO_DATASET,
                             uint chart = 0 );

    static QString chartSourceModeToString( const SourceMode& mode );
    static SourceMode stringToChartSourceMode( const QString& string );


    bool neverUsedSetChartSourceMode() const
        {
            return !_setChartSourceModeWasUsed;
        }

    SourceMode chartSourceMode( uint dataset,
                                uint dataset2 = KDCHART_NO_DATASET,
                                uint* chart = 0 ) const;

    bool findDataset( SourceMode mode,
                      uint& dataset,
                      uint& dataset2,
                      uint chart = 0 ) const;

    bool findDatasets( SourceMode mode1,
                       SourceMode mode2,
                       uint& dataset,
                       uint& dataset2,
                       uint chart = 0 ) const;

    uint maxDatasetSourceMode() const
        {
            return _maxDatasetSourceMode;
        }


    void setDefaultAxesTypes();
    void activateDefaultAxes();


    void setNumValues( uint numValues )
        {
            _numValues = numValues;
            emit changed();
        }


    int numValues() const
        {
            return _numValues;
        }


    void calculateShadowColors( QColor color,
                                QColor& shadow1,
                                QColor& shadow2 ) const;

    void setDataColor( uint dataset, QColor color );
    QColor dataColor( uint dataset ) const;

    void setDataDefaultColors();
    void setDataRainbowColors();
    void setDataSubduedColors( bool ordered = false );


    void setShadowBrightnessFactor( double factor )
        {
            _shadowBrightnessFactor = factor;
            recomputeShadowColors();
            emit changed();
        }

    double shadowBrightnessFactor() const
        {
            return _shadowBrightnessFactor;
        }


    void setShadowPattern( Qt::BrushStyle style ) {
        _shadowPattern = style;
        emit changed();
    }

    Qt::BrushStyle shadowPattern() const {
        return _shadowPattern;
    }


    void setOutlineDataColor( QColor color )
        {
            _outlineDataColor = color;
            emit changed();
        }


    QColor outlineDataColor() const
        {
            return _outlineDataColor;
        }


    void setOutlineDataLineWidth( uint width )
        {
            _outlineDataLineWidth = width;
            emit changed();
        }

    uint outlineDataLineWidth() const
        {
            return _outlineDataLineWidth;
        }


    void setOutlineDataLineStyle( Qt::PenStyle style )
        {
            _outlineDataLineStyle = style;
            emit changed();
        }

    Qt::PenStyle outlineDataLineStyle() const
        {
            return _outlineDataLineStyle;
        }


    uint maxDataColor() const
        {
            return _maxDatasetColor;
        }


    void setThreeDShadowColors( bool shadow )
        {
            _threeDShadowColors = shadow;
            emit changed();
        }


    bool threeDShadowColors() const
        {
            return _threeDShadowColors;
        }


    static int roundVal( double d )
        {
            double fr;
            double i=0.0; // initialization necessary for Borland C++
            fr = modf( d, &i );
            int ret = static_cast < int > ( i );
            if( 0.49999 <= fabs( fr ) )
                ret += ( 0.0 < d ) ? 1 : -1;
            return ret;
        }

    // Note if you change the parameters here, then you must also change them in wrappers/KDChartParametersWrapper.h
    void setPrintDataValues( bool active,
                             uint chart = KDCHART_ALL_CHARTS,
                             int divPow10 = 0,
                             int digitsBehindComma = KDCHART_DATA_VALUE_AUTO_DIGITS,
                             QFont* font   = 0,
                             uint size     = UINT_MAX, //  <-- makes us use the *default* font params
                             //                                by IGNORING settings of
                             //                                the following parameters!
                             const QColor* color = KDCHART_DATA_VALUE_AUTO_COLOR,
                             KDChartEnums::PositionFlag negativePosition = KDChartEnums::PosCenter,
                             uint negativeAlign    = Qt::AlignCenter,
                             int  negativeDeltaX   =    0,
                             int  negativeDeltaY   =    0,
                             int  negativeRotation =    0,
                             KDChartEnums::PositionFlag positivePosition = KDChartEnums::PosCenter,
                             uint positiveAlign    = Qt::AlignCenter,
                             int  positiveDeltaX   =    0,
                             int  positiveDeltaY   =    0,
                             int  positiveRotation =    0,
                             KDChartEnums::TextLayoutPolicy policy = KDChartEnums::LayoutPolicyRotate );

    void setDataValuesCalc( int divPow10 = 0,
                            int digitsBehindComma = KDCHART_DATA_VALUE_AUTO_DIGITS,
                            uint chart = KDCHART_ALL_CHARTS );
    void setDataValuesFont( QFont* font,
                            uint size  = UINT_MAX,
                            uint chart = KDCHART_ALL_CHARTS );

    // Note if you change the parameters here, then you must also change them in wrappers/KDChartParametersWrapper.h
    void setDataValuesPlacing( KDChartEnums::PositionFlag position,
                               uint align,
                               int  deltaX,
                               int  deltaY,
                               int  rotation,
                               bool specifyingPositiveValues = true,
                               uint chart = KDCHART_ALL_CHARTS );

    void setDataValuesColors( const QColor* color = KDCHART_DATA_VALUE_AUTO_COLOR,
                              const QBrush& background = Qt::NoBrush,
                              uint chart = KDCHART_ALL_CHARTS );

    // Note if you change the parameters here, then you must also change them in wrappers/KDChartParametersWrapper.h
    void setDataValuesPolicy( KDChartEnums::TextLayoutPolicy policy = KDChartEnums::LayoutPolicyRotate,
                              uint chart = KDCHART_ALL_CHARTS );

    void setDataValuesShowInfinite( bool dataValuesShowInfinite = true,
                                    uint chart = KDCHART_ALL_CHARTS );

    void setPrintDataValuesWithDefaultFontParams( uint chart = KDCHART_ALL_CHARTS,
                                                  bool callSetPrintDataValues = true );

    void setAllowOverlappingDataValueTexts( bool allow )
        {
            _allowOverlappingDataValueTexts = allow;
        }


    bool allowOverlappingDataValueTexts() const
        {
            return _allowOverlappingDataValueTexts;
        }

    bool printDataValuesWithDefaultFontParams( uint chart ) const
        {
            return chart ? _printDataValuesSettings2._useDefaultFontParams
                : _printDataValuesSettings._useDefaultFontParams;
        }

    bool printDataValues( uint chart ) const
        {
            return chart ? _printDataValuesSettings2._printDataValues
                : _printDataValuesSettings._printDataValues;
        }


    int dataValuesDivPow10( uint chart ) const
        {
            return chart ? _printDataValuesSettings2._divPow10
                : _printDataValuesSettings._divPow10;
        }


    int dataValuesDigitsBehindComma( uint chart ) const
        {
            return chart ? _printDataValuesSettings2._digitsBehindComma
                : _printDataValuesSettings._digitsBehindComma;
        }


    QFont dataValuesFont( uint chart ) const
        {
            return chart ? _printDataValuesSettings2._dataValuesFont
                : _printDataValuesSettings._dataValuesFont;
        }


    bool dataValuesUseFontRelSize( uint chart ) const
        {	  
            return chart ? _printDataValuesSettings2._dataValuesUseFontRelSize
                : _printDataValuesSettings._dataValuesUseFontRelSize;
        }


    int dataValuesFontRelSize( uint chart ) const
        {
            return chart ? _printDataValuesSettings2._dataValuesFontRelSize
                : _printDataValuesSettings._dataValuesFontRelSize;
        }


    QColor dataValuesColor( uint chart ) const
        {
            return chart ? _printDataValuesSettings2._dataValuesColor
                : _printDataValuesSettings._dataValuesColor;
        }


    QBrush dataValuesBackground( uint chart ) const
        {
            return chart ? _printDataValuesSettings2._dataValuesBrush
                : _printDataValuesSettings._dataValuesBrush;
        }


    bool dataValuesAutoColor( uint chart ) const
        {
            return chart ? _printDataValuesSettings2._dataValuesAutoColor
                : _printDataValuesSettings._dataValuesAutoColor;
        }


    KDChartEnums::PositionFlag dataValuesAnchorPosition( uint chart,
                                                         bool negative ) const;
    uint dataValuesAnchorAlign( uint chart, bool negative ) const;
    int dataValuesAnchorDeltaX( uint chart, bool negative ) const;
    int dataValuesAnchorDeltaY( uint chart, bool negative ) const;
    int dataValuesRotation( uint chart, bool negative ) const;

    KDChartEnums::TextLayoutPolicy dataValuesLayoutPolicy( uint chart ) const
        {
            return chart ? _printDataValuesSettings2._dataValuesLayoutPolicy
                : _printDataValuesSettings._dataValuesLayoutPolicy;
        }


    bool dataValuesShowInfinite( uint chart ) const
        {
            return chart ? _printDataValuesSettings2._dataValuesShowInfinite
                : _printDataValuesSettings._dataValuesShowInfinite;
        }


    // for backward compatibility we may NOT remove these functions:
    void setPrintDataValuesColor( uint chart = KDCHART_ALL_CHARTS,
                                  const QColor* color = KDCHART_DATA_VALUE_AUTO_COLOR );
    void setPrintDataValuesFontRelSize( uint chart, uint size );


    static QString markerStyleToString(   int style );
    static QString markerStyleToStringTr( int style );
    static int stringToMarkerStyle(   const QString& string );
    static int stringToMarkerStyleTr( const QString& string );

    // END GENERAL


public:

    // BAR CHART-SPECIFIC
    enum BarChartSubType { BarNormal, BarStacked, BarPercent, BarMultiRows };

public slots:
    void setBarChartSubType( BarChartSubType barChartSubType )
        {
            _barChartSubType = barChartSubType;
            emit changed();
        }

    BarChartSubType barChartSubType() const
        {
            return _barChartSubType;
        }





    static QString barChartSubTypeToString( BarChartSubType type );
    static BarChartSubType stringToBarChartSubType( const QString& string );


    void setThreeDBars( bool threeDBars )
        {
            _threeDBars = threeDBars;
            emit changed();
        }

    bool threeDBars() const
        {
            return _threeDBars;
        }


    void setThreeDBarsShadowColors( bool shadow )
        {
            _threeDShadowColors = shadow;
            emit changed();
        }


    bool threeDBarsShadowColors() const
        {
            return _threeDShadowColors;
        }


    QColor dataShadow1Color( uint dataset ) const;
    QColor dataShadow2Color( uint dataset ) const;


    void setThreeDBarAngle( uint angle );
    uint threeDBarAngle() const
        {
            return _threeDBarAngle;
        }


    double cosThreeDBarAngle() const
        {
            return _cosThreeDBarAngle;
        }


    void setThreeDBarDepth( double depth )
        {
            _threeDBarDepth = depth;
            emit changed();
        }


    double threeDBarDepth() const
        {
            return _threeDBarDepth;
        }


    void setDatasetGap( int gap )
        {
            _datasetGap = gap;
            emit changed();
        }

    int datasetGap() const
        {
            return _datasetGap;
        }


    void setDatasetGapIsRelative( bool gapIsRelative )
        {
            _datasetGapIsRelative = gapIsRelative;
            emit changed();
        }

    bool datasetGapIsRelative() const
        {
            return _datasetGapIsRelative;
        }


    void setValueBlockGap( int gap )
        {
            _valueBlockGap = gap;
            emit changed();
        }

    int valueBlockGap() const
        {
            return _valueBlockGap;
        }


    void setValueBlockGapIsRelative( bool gapIsRelative )
        {
            _valueBlockGapIsRelative = gapIsRelative;
            emit changed();
        }

    bool valueBlockGapIsRelative() const
        {
            return _valueBlockGapIsRelative;
        }


    void setBarWidth( int width = KDCHART_AUTO_SIZE )
        { 
          if ( width == KDCHART_AUTO_SIZE ) {
	    _barWidth = width;
            _userWidth = 0;
	  }
          else 
	  _userWidth = width;

          emit changed();
        }
 

    int barWidth() const
        {
            return _barWidth;
        }

  int userWidth() const
        {
	  return _userWidth;
        }

    int numBarsDisplayed() const
        {
            return _barsDisplayed;
        }

    int numBarsLeft() const
        {
            return _barsLeft;
        }

    void setDrawSolidExcessArrows( bool solidArrows ) {
        _solidExcessArrows = solidArrows;
        emit changed();
    }

    bool drawSolidExcessArrows() const
        {
            return _solidExcessArrows;
        }

    // END BAR CHART-SPECIFIC

public:
    // LINE/AREA CHART-SPECIFIC
    enum LineChartSubType { LineNormal, LineStacked, LinePercent };

public slots:
    void setLineChartSubType( LineChartSubType lineChartSubType )
        {
            _lineChartSubType = lineChartSubType;
            emit changed();
        }

    LineChartSubType lineChartSubType() const
        {
            return _lineChartSubType;
        }


    static LineChartSubType stringToLineChartSubType( const QString& string );
    static QString lineChartSubTypeToString( LineChartSubType type );


    void setLineMarker( bool marker )
        {
            _lineMarker = marker;
            emit changed();
        }

    bool lineMarker() const
        {
            return _lineMarker;
        }

public:
    enum LineMarkerStyle { LineMarkerCircle  = 0,
            LineMarkerSquare  = 1,
            LineMarkerDiamond = 2,
            LineMarker1Pixel  = 3,
            LineMarker4Pixels = 4,
            LineMarkerRing    = 5,
            LineMarkerCross   = 6,
            LineMarkerFastCross = 7 };

    // PENDING(blackie) Add a wrapper for this
    typedef QMap<uint,LineMarkerStyle> LineMarkerStyleMap;
public slots:

        void setLineMarkerStyle( uint dataset, LineMarkerStyle style );
    void setLineMarkerStyles( LineMarkerStyleMap map );
    LineMarkerStyle lineMarkerStyle( uint dataset ) const;

    static QString lineMarkerStyleToString( LineMarkerStyle style );
    static QString lineMarkerStyleToStringTr( LineMarkerStyle style );
    static LineMarkerStyle stringToLineMarkerStyle( const QString& string );
    static LineMarkerStyle stringToLineMarkerStyleTr( const QString& string );

    LineMarkerStyleMap lineMarkerStyles() const {
        return _lineMarkerStyles;
    }


    uint maxDatasetLineMarkerStyle() const
        {
            return _maxDatasetLineMarkerStyle;
        }


    void setLineMarkerSize( QSize size )
        {
            _lineMarkerSize = size;
            emit changed();
        }

    QSize lineMarkerSize() const
        {
            return _lineMarkerSize;
        }

    void setLineWidth( int width )
        {
            _lineWidth = width;
            emit changed();
        }

    int lineWidth() const
        {
            return _lineWidth;
        }


    void setLineColor( QColor color = QColor() )
        {
            _lineColor = color;
            emit changed();
        }

    QColor lineColor() const
        {
            return _lineColor;
        }

    void setLineStyle( Qt::PenStyle style, uint dataset=KDCHART_GLOBAL_LINE_STYLE );

	Qt::PenStyle lineStyle( uint dataset=KDCHART_GLOBAL_LINE_STYLE ) const;


    void setThreeDLines( bool threeD ) {
        _threeDLines = threeD;
        emit changed();
    }


    bool threeDLines() const {
        return _threeDLines;
    }


    void setThreeDLineDepth( int depth ) {
        _threeDLineDepth = depth;
        emit changed();
    }


    int threeDLineDepth() const {
        return _threeDLineDepth;
    }


    // NOTE: documentation of this function is temporary disabled.
    // Feature is currently not supported, will be implemented
    // by future versions of KDChart
    void setThreeDLineXRotation( int degrees ) {
        _threeDLineXRotation = degrees;
        emit changed();
    }


    // NOTE: documentation of this function is temporary disabled.
    // Feature is currently not supported, will be implemented
    // by future versions of KDChart
    int threeDLineXRotation() const {
        return _threeDLineXRotation;
    }


    // NOTE: documentation of this function is temporary disabled.
    // Feature is currently not supported, will be implemented
    // by future versions of KDChart
    void setThreeDLineYRotation( int degrees ) {
        _threeDLineYRotation = degrees;
        emit changed();
    }


    // NOTE: documentation of this function is temporary disabled.
    // Feature is currently not supported, will be implemented
    // by future versions of KDChart
    int threeDLineYRotation() const {
        return _threeDLineYRotation;
    }

public:
    enum AreaChartSubType { AreaNormal, AreaStacked, AreaPercent };

public slots:
    void setAreaChartSubType( AreaChartSubType areaChartSubType )
        {
            _areaChartSubType = areaChartSubType;
            // activate default data value text settings for this chart type
            if(    printDataValues( 0 )
                   && printDataValuesWithDefaultFontParams( 0 ) )
                setPrintDataValues( true, 0 );
            emit changed();
        }


    AreaChartSubType areaChartSubType() const
        {
            return _areaChartSubType;
        }


    static QString areaChartSubTypeToString( AreaChartSubType type );
    static AreaChartSubType stringToAreaChartSubType( const QString& string );

public:
    enum AreaLocation { AreaAbove, AreaBelow };
public slots:

    void setAreaLocation( AreaLocation location )
        {
            _areaLocation = location;
            emit changed();
        }

    AreaLocation areaLocation() const
        {
            return _areaLocation;
        }


    static QString areaLocationToString( AreaLocation type );
    static AreaLocation stringToAreaLocation( const QString& string );

    // END LINE/AREA CHART-SPECIFIC

public:
    // POLAR CHART-SPECIFIC
    enum PolarChartSubType { PolarNormal, PolarStacked, PolarPercent };

public slots:
    void setPolarChartSubType( PolarChartSubType polarChartSubType )
        {
            _polarChartSubType = polarChartSubType;
            emit changed();
        }

    PolarChartSubType polarChartSubType() const
        {
            return _polarChartSubType;
        }


    static PolarChartSubType stringToPolarChartSubType( const QString& string );
    static QString polarChartSubTypeToString( PolarChartSubType type );

    void setPolarMarker( bool marker )
        {
            _polarMarker = marker;
            emit changed();
        }

    bool polarMarker() const
        {
            return _polarMarker;
        }

public:
    enum PolarMarkerStyle { PolarMarkerCircle  = 0,
                            PolarMarkerSquare  = 1,
                            PolarMarkerDiamond = 2,
                            PolarMarker1Pixel  = 3,
                            PolarMarker4Pixels = 4,
                            PolarMarkerRing    = 5,
                            PolarMarkerCross   = 6 };

public slots:
    void setPolarMarkerStyle( uint dataset, PolarMarkerStyle style );
    PolarMarkerStyle polarMarkerStyle( uint dataset ) const;
    static QString polarMarkerStyleToString( PolarMarkerStyle style );
    static QString polarMarkerStyleToStringTr( PolarMarkerStyle style );
    static PolarMarkerStyle stringToPolarMarkerStyle( const QString& string );
    static PolarMarkerStyle stringToPolarMarkerStyleTr( const QString& string );

public:
    typedef QMap<uint,PolarMarkerStyle> PolarMarkerStyleMap;

public slots:
    void setPolarMarkerStyles( PolarMarkerStyleMap map );

    PolarMarkerStyleMap polarMarkerStyles() const {
        return _polarMarkerStyles;
    }

    uint maxDatasetPolarMarkerStyle() const
        {
            return _maxDatasetPolarMarkerStyle;
        }

    void setPolarMarkerSize( QSize size = QSize( -40, -40 ) )
        {
            _polarMarkerSize = size;
            emit changed();
        }

    QSize polarMarkerSize() const
        {
            return _polarMarkerSize;
        }

    void setPolarLineWidth( int width = -3 )
        {
            _polarLineWidth = width;
            emit changed();
        }

    int polarLineWidth() const
        {
            return _polarLineWidth;
        }


    void setPolarZeroDegreePos( int degrees )
        {
            _polarZeroDegreePos = degrees;
        }

    int polarZeroDegreePos() const
        {
            return _polarZeroDegreePos;
        }


    void setPolarRotateCircularLabels( bool rotateCircularLabels )
        {
            _polarRotateCircularLabels = rotateCircularLabels;
        }

    bool polarRotateCircularLabels() const
        {
            return _polarRotateCircularLabels;
        }


    // Note if you change the parameters here, then you must also change them in wrappers/KDChartParametersWrapper.h
    void setPolarDelimsAndLabelsAtPos( KDChartEnums::PositionFlag pos,
                                       bool showDelimiters,
                                       bool showLabels );

    // Note if you change the parameters here, then you must also change them in wrappers/KDChartParametersWrapper.h
    bool polarDelimAtPos( KDChartEnums::PositionFlag pos ) const;

    // Note if you change the parameters here, then you must also change them in wrappers/KDChartParametersWrapper.h
    bool polarLabelsAtPos( KDChartEnums::PositionFlag pos ) const;

    // END POLAR CHART-SPECIFIC


    // PIE/RING CHART-SPECIFIC

    void setExplode( bool explode )
        {
            _explode = explode;
            emit changed();
        }

    bool explode() const
        {
            return _explode;
        }


    void setExplodeValues( Q3ValueList<int> explodeList ) {
        _explodeList = explodeList;
        emit changed();
    }


    // Unfortunately this is not avaialble from QSA-
    Q3ValueList<int> explodeValues() const {
        return _explodeList;
    }

public:
    typedef QMap<int,double> ExplodeFactorsMap;

public slots:
    void setExplodeFactors( ExplodeFactorsMap factors ) {
        _explodeFactors = factors;
        emit changed();
    }


    ExplodeFactorsMap explodeFactors() const {
        return _explodeFactors;
    }


    void setExplodeFactor( double factor )
        {
            _explodeFactor = factor;
            emit changed();
        }

    double explodeFactor() const
        {
            return _explodeFactor;
        }


    void setThreeDPies( bool threeDPies )
        {
            _threeDPies = threeDPies;
            emit changed();
        }

    bool threeDPies() const
        {
            return _threeDPies;
        }


    void setThreeDPieHeight( int pixels )
        {
            _threeDPieHeight = pixels;
            emit changed();
        }

    int threeDPieHeight() const
        {
            return _threeDPieHeight;
        }

    void setPieStart( int degrees )
        {
            while ( degrees < 0 )
                degrees += 360;
            while ( degrees >= 360 )
                degrees -= 360;
            _pieStart = degrees;

            emit changed();
        }


    int pieStart() const
        {
            return _pieStart;
        }

    void setRingStart( int degrees )
        {
            while ( degrees < 0 )
                degrees += 360;
            while ( degrees >= 360 )
                degrees -= 360;
            _ringStart = degrees;

            emit changed();
        }


    int ringStart() const
        {
            return _ringStart;
        }

    void setRelativeRingThickness( bool relativeThickness ) {
        _relativeRingThickness = relativeThickness;

        emit changed();
    }


    bool relativeRingThickness() const {
        return _relativeRingThickness;
    }

    // END PIE/RING CHART-SPECIFIC

public:
    // HI/LO CHART-SPECIFIC
    enum HiLoChartSubType { HiLoNormal, HiLoSimple = HiLoNormal,
                            HiLoClose, HiLoOpenClose };

public slots:
    void setHiLoChartSubType( HiLoChartSubType hiLoChartSubType )
        {
            _hiLoChartSubType = hiLoChartSubType;
            emit changed();
        }

    HiLoChartSubType hiLoChartSubType() const
        {
            return _hiLoChartSubType;
        }

    static QString hiLoChartSubTypeToString( HiLoChartSubType type );
    static HiLoChartSubType stringToHiLoChartSubType( const QString& string );
    void setHiLoChartPrintLowValues( bool active,
                                     QFont* font = 0,
                                     int size = 14,
                                     QColor* color = 0 );

    bool hiLoChartPrintLowValues() const
        {
            return _hiLoChartPrintLowValues;
        }


    QFont hiLoChartLowValuesFont() const
        {
            return _hiLoChartLowValuesFont;
        }


    bool hiLoChartLowValuesUseFontRelSize() const
        {
            return _hiLoChartLowValuesUseFontRelSize;
        }


    int hiLoChartLowValuesFontRelSize() const
        {
            return _hiLoChartLowValuesFontRelSize;
        }


    QColor hiLoChartLowValuesColor() const
        {
            return _hiLoChartLowValuesColor;
        }

    void setHiLoChartPrintHighValues( bool active,
                                      QFont* font = 0,
                                      int size = 14,
                                      QColor* color = 0 );


    bool hiLoChartPrintHighValues() const
        {
            return _hiLoChartPrintHighValues;
        }


    QFont hiLoChartHighValuesFont() const
        {
            return _hiLoChartHighValuesFont;
        }


    bool hiLoChartHighValuesUseFontRelSize() const
        {
            return _hiLoChartHighValuesUseFontRelSize;
        }


    int hiLoChartHighValuesFontRelSize() const
        {
            return _hiLoChartHighValuesFontRelSize;
        }


    QColor hiLoChartHighValuesColor() const
        {
            return _hiLoChartHighValuesColor;
        }

    void setHiLoChartPrintOpenValues( bool active,
                                      QFont* font = 0,
                                      uint size = 14,
                                      QColor* color = 0 );

    bool hiLoChartPrintOpenValues() const
        {
            return _hiLoChartPrintOpenValues;
        }


    QFont hiLoChartOpenValuesFont() const
        {
            return _hiLoChartOpenValuesFont;
        }


    bool hiLoChartOpenValuesUseFontRelSize() const
        {
            return _hiLoChartOpenValuesUseFontRelSize;
        }


    int hiLoChartOpenValuesFontRelSize() const
        {
            return _hiLoChartOpenValuesFontRelSize;
        }


    QColor hiLoChartOpenValuesColor() const
        {
            return _hiLoChartOpenValuesColor;
        }

    void setHiLoChartPrintCloseValues( bool active,
                                       QFont* font = 0,
                                       int size = 14,
                                       QColor* color = 0 );

    bool hiLoChartPrintCloseValues() const
        {
            return _hiLoChartPrintCloseValues;
        }


    QFont hiLoChartCloseValuesFont() const
        {
            return _hiLoChartCloseValuesFont;
        }


    bool hiLoChartCloseValuesUseFontRelSize() const
        {
            return _hiLoChartCloseValuesUseFontRelSize;
        }


    int hiLoChartCloseValuesFontRelSize() const
        {
            return _hiLoChartCloseValuesFontRelSize;
        }


    QColor hiLoChartCloseValuesColor() const
        {
            return _hiLoChartCloseValuesColor;
        }


    void setHiLoChartPrintFirstValues( bool active,
                                       QFont* font = 0,
                                       uint size = 14,
                                       QColor* color = 0 )
        {
            setHiLoChartPrintOpenValues( active, font, size, color );
        }

    bool hiLoChartPrintFirstValues() const
        {
            return hiLoChartPrintOpenValues();
        }


    QFont hiLoChartFirstValuesFont() const
        {
            return hiLoChartOpenValuesFont();
        }


    bool hiLoChartFirstValuesUseFontRelSize() const
        {
            return hiLoChartOpenValuesUseFontRelSize();
        }

    int hiLoChartFirstValuesFontRelSize() const
        {
            return hiLoChartOpenValuesFontRelSize();
        }

    QColor hiLoChartFirstValuesColor() const
        {
            return hiLoChartOpenValuesColor();
        }

    void setHiLoChartPrintLastValues( bool active,
                                      QFont* font = 0,
                                      int size = 14,
                                      QColor* color = 0 )
        {
            setHiLoChartPrintCloseValues( active, font, size, color );
        }

    bool hiLoChartPrintLastValues() const
        {
            return hiLoChartPrintCloseValues();
        }

    QFont hiLoChartLastValuesFont() const
        {
            return hiLoChartCloseValuesFont();
        }

    bool hiLoChartLastValuesUseFontRelSize() const
        {
            return hiLoChartCloseValuesUseFontRelSize();
        }

    int hiLoChartLastValuesFontRelSize() const
        {
            return hiLoChartCloseValuesFontRelSize();
        }

    QColor hiLoChartLastValuesColor() const
        {
            return hiLoChartCloseValuesColor();
        }


    // END HI/LO CHART-SPECIFIC


public:
    // BOX WHISKER CHART-SPECIFIC
    enum BWChartSubType { BWNormal, BWSimple = BWNormal };

    enum BWStatVal {                  BWStatValSTART,
                                      UpperOuterFence = BWStatValSTART,  // first value stored in array
                                      UpperInnerFence,
                                      Quartile3,
                                      Median,
                                      Quartile1,
                                      LowerInnerFence,
                                      LowerOuterFence,
                                      MaxValue,
                                      MeanValue,
                                      MinValue,
                                      BWStatValEND = MinValue,          // last value stored in array
                                      //  extra values - not stored in the array:
                                      BWStatValOffEndValue,  // the number of array values
                                      BWStatValALL,          // special flag reprsenting *all* array values
                                      BWStatValUNKNOWN };    // to be returned when value is undefined
public slots:

    void setBWChartSubType( BWChartSubType bWChartSubType )
        {
            _BWChartSubType = bWChartSubType;
            emit changed();
        }

    BWChartSubType bWChartSubType() const
        {
            return _BWChartSubType;
        }

    static QString bWChartSubTypeToString( BWChartSubType type );
    static BWChartSubType stringToBWChartSubType( const QString& string );
    static QString bWChartStatValToString( BWStatVal type );
    static BWStatVal stringToBWChartStatVal( const QString& string );

    void setBWChartFences( double upperInner, double lowerInner,
                           double upperOuter, double lowerOuter );
    void bWChartFences( double& upperInner, double& lowerInner,
                        double& upperOuter, double& lowerOuter ) const;

    void setBWChartBrush( const QBrush& bWChartBrush )
        {
            _BWChartBrush = bWChartBrush;
            emit changed();
        }

    QBrush bWChartBrush() const
        {
            return _BWChartBrush;
        }

    void setBWChartOutValMarkerSize( int size )
        {
            _BWChartOutValMarkerSize = size;
        }


    int bWChartOutValMarkerSize() const
        {
            return _BWChartOutValMarkerSize;
        }

    void setBWChartPrintStatistics( BWStatVal statValue,
                                    bool active,
                                    QFont* font = 0,
                                    int size = 24,
                                    QColor* color = 0,
                                    QBrush* brush = 0 );

    bool bWChartPrintStatistics( BWStatVal statValue ) const
        {
            return _BWChartStatistics[ statValue ].active;
        }


    QFont bWChartStatisticsFont( BWStatVal statValue ) const
        {
            return _BWChartStatistics[ statValue ].font;
        }


    bool bWChartStatisticsUseRelSize( BWStatVal statValue ) const
        {
            return _BWChartStatistics[ statValue ].useRelSize;
        }


    int bWChartStatisticsFontRelSize( BWStatVal statValue ) const
        {
            return _BWChartStatistics[ statValue ].relSize;
        }


    QColor bWChartStatisticsColor( BWStatVal statValue ) const
        {
            return _BWChartStatistics[ statValue ].color;
        }

    QBrush bWChartStatisticsBrush( BWStatVal statValue ) const
        {
            return _BWChartStatistics[ statValue ].brush;
        }


public:
    // LEGENDS

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

public slots:
    void setLegendPosition( LegendPosition position )
        {
            _legendPosition = position;
            emit changed();
        }

    LegendPosition legendPosition() const
        {
            return _legendPosition;
        }

    void setLegendOrientation( Qt::Orientation orientation )
        {
            _legendOrientation = orientation;
            emit changed();
        }

    Qt::Orientation legendOrientation() const
        {
            return _legendOrientation;
        }


    void setLegendShowLines( bool legendShowLines )
        {
            _legendShowLines = legendShowLines;
            emit changed();
        }

    bool legendShowLines() const
        {
            return _legendShowLines;
        }


    static QString legendPositionToString( LegendPosition pos );
    static LegendPosition stringToLegendPosition( const QString& string );

public:
    enum LegendSource { LegendManual, LegendFirstColumn, LegendAutomatic };

public slots:
    void setLegendSource( LegendSource source )
        {
            _legendSource = source;
            emit changed();
        }

    LegendSource legendSource() const
        {
            return _legendSource;
        }

    static QString legendSourceToString( LegendSource source );
    static LegendSource stringToLegendSource( const QString& string );

    void setLegendText( uint dataset, const QString& text )
        {
            _legendText[ dataset ] = text;
            emit changed();
        }

    QString legendText( uint dataset ) const
        {
            if( _legendText.find( dataset ) != _legendText.end() )
                return _legendText[ dataset ];
            else
                return QString::null;
        }


    void setLegendTextColor( const QColor& color )
        {
            _legendTextColor = color;
            emit changed();
        }


    QColor legendTextColor() const
        {
            return _legendTextColor;
        }


    void setLegendFont( const QFont& font, bool useFontSize )
        {
            _legendFont = font;
            _legendFontUseRelSize = ! useFontSize;
            emit changed();
        }

    QFont legendFont() const
        {
            return _legendFont;
        }


    void setLegendFontUseRelSize( bool legendFontUseRelSize )
        {
            _legendFontUseRelSize = legendFontUseRelSize;
            emit changed();
        }

    bool legendFontUseRelSize() const
        {
            return _legendFontUseRelSize;
        }

    void setLegendFontRelSize( int legendFontRelSize )
        {
            _legendFontRelSize = legendFontRelSize;
            emit changed();
        }

    int legendFontRelSize() const
        {
            return _legendFontRelSize;
        }


    void setLegendTitleText( const QString& text )
        {
            _legendTitleText = text;
            emit changed();
        }


    QString legendTitleText() const
        {
            return _legendTitleText;
        }


    void setLegendTitleTextColor( const QColor& color )
        {
            _legendTitleTextColor = color;
            emit changed();
        }


    QColor legendTitleTextColor() const
        {
            return _legendTitleTextColor;
        }



    void setLegendTitleFont( const QFont& font, bool useFontSize )
        {
            _legendTitleFont = font;
            _legendTitleFontUseRelSize = ! useFontSize;
            emit changed();
        }

    QFont legendTitleFont() const
        {
            return _legendTitleFont;
        }


    void setLegendTitleFontUseRelSize( bool legendTitleFontUseRelSize )
        {
            _legendTitleFontUseRelSize = legendTitleFontUseRelSize;
            emit changed();
        }

    bool legendTitleFontUseRelSize() const
        {
            return _legendTitleFontUseRelSize;
        }

    void setLegendTitleFontRelSize( int legendTitleFontRelSize )
        {
            _legendTitleFontRelSize = legendTitleFontRelSize;
            emit changed();
        }

    int legendTitleFontRelSize() const
        {
            return _legendTitleFontRelSize;
        }


    void setLegendSpacing( uint space )
        {
            _legendSpacing = space;
        }


    uint legendSpacing() const
        {
            return _legendSpacing;
        }

    // END LEGENDS



    // AXES

    // Note if you change the parameters here, then you must also change them in wrappers/KDChartParametersWrapper.h
    void setAxisType( uint n, const KDChartAxisParams::AxisType axisType )
        {
            if ( n < KDCHART_MAX_AXES ) {
                _axisSettings[ n ].params.setAxisType( axisType );
                emit changed();
            }
        }

    void setAxisVisible( uint n,
                         const bool axisVisible )
        {
            if ( n < KDCHART_MAX_AXES ) {
                _axisSettings[ n ].params.setAxisVisible( axisVisible );
                emit changed();
            }
        }

    bool axisVisible( uint n ) const
        {
            return n < KDCHART_MAX_AXES ? _axisSettings[ n ].params.axisVisible()
                : false;
        }

    void setAxisShowGrid( uint n,
                          bool axisShowGrid );
    bool showGrid() const;
    void setAxisDatasets( uint n,
                          uint dataset,
                          uint dataset2 = KDCHART_NO_DATASET,
                          uint chart = 0 );
    bool axisDatasets( uint n,
                       uint& dataset,
                       uint& dataset2,
                       uint& chart ) const;

public:
#if COMPAT_QT_VERSION >= 0x030000
    typedef Q3ValueVector<uint> AxesArray;
#else
    typedef QArray<uint> AxesArray;
#endif

public slots:
    bool chartAxes( uint chart, uint& cnt, AxesArray& axes ) const;

    void setAxisArea( const uint n, const QRect& areaRect )
        {
            if ( n < KDCHART_MAX_AXES ) {
                _axisSettings[ n ].params.setAxisTrueAreaRect( areaRect );
                // Do not do emit changed() here!
            }
        }

    void setAxisLabelsTouchEdges( uint n, bool axisLabelsTouchEdges )
        {
            _axisSettings[ n ].params.setAxisLabelsTouchEdges(
                axisLabelsTouchEdges );
            emit changed();
        }

    void setAxisLabelsVisible( uint n,
                               bool axisLabelsVisible )
        {
            if ( n < KDCHART_MAX_AXES )
                _axisSettings[ n ].params.setAxisLabelsVisible( axisLabelsVisible );
        }

    void setAxisLabelsFont( uint n,
                            QFont axisLabelsFont,
                            int axisLabelsFontSize = 0,
                            QColor axisLabelsColor = Qt::black );

    // Note if you change the parameters here, then you must also change them in wrappers/KDChartParametersWrapper.h
    void setAxisLabelTextParams( uint n,
                                 bool axisSteadyValueCalc   = true,
                                 QVariant axisValueStart = KDCHART_AXIS_LABELS_AUTO_LIMIT,
                                 QVariant axisValueEnd   = KDCHART_AXIS_LABELS_AUTO_LIMIT,
                                 double axisValueDelta      = KDCHART_AXIS_LABELS_AUTO_DELTA,
                                 int axisDigitsBehindComma  = KDCHART_AXIS_LABELS_AUTO_DIGITS,
                                 int axisMaxEmptyInnerSpan  = 67,
                                 KDChartAxisParams::LabelsFromDataRow takeLabelsFromDataRow
                                 = KDChartAxisParams::LabelsFromDataRowNo,
                                 int labelTextsDataRow      = 0,
                                 QStringList* axisLabelStringList = 0,
                                 QStringList* axisShortLabelsStringList = 0,
                                 int axisValueLeaveOut      = KDCHART_AXIS_LABELS_AUTO_LEAVEOUT,
                                 KDChartAxisParams::ValueScale axisValueDeltaScale = KDChartAxisParams::ValueScaleNumerical )
        {
            _axisSettings[ n ].params.setAxisValues(
                axisSteadyValueCalc,
                axisValueStart,
                axisValueEnd,
                axisValueDelta,
                axisDigitsBehindComma,
                axisMaxEmptyInnerSpan,
                takeLabelsFromDataRow,
                labelTextsDataRow,
                axisLabelStringList,
                axisShortLabelsStringList,
                axisValueLeaveOut,
                axisValueDeltaScale );
            emit changed();
        }


    void setAxisLabelStringParams( uint n,
                                   QStringList* axisLabelStringList,
                                   QStringList* axisShortLabelStringList,
                                   const QString& valueStart = QString::null,
                                   const QString& valueEnd   = QString::null );


    void setAxisParams( uint n,
                        const KDChartAxisParams& axisParams );

    const KDChartAxisParams& axisParams( uint n ) const
        {
            return n < KDCHART_MAX_AXES ? _axisSettings[ n ].params
                : _axisSettings[ KDCHART_MAX_AXES ].params;
        }

    // some convenience functions refering to the first KDChartCustomBox
    // that is anchored to the axis n.
    bool findFirstAxisCustomBoxID(   uint n, uint& boxID ) const;
    void setAxisTitle(               uint n, const QString& axisTitle );
    
    QString axisTitle(               uint n ) const;
    void setAxisTitleColor(          uint n, QColor axisTitleColor );
    QColor axisTitleColor(           uint n ) const;
    void setAxisTitleFont(           uint n, QFont axisTitleFont );
    void setAxisTitleFont(           uint n, QFont axisTitleFont, bool useFixedFontSize );
    QFont axisTitleFont(             uint n ) const;
    void setAxisTitleFontUseRelSize( uint n, bool useRelSize );
    bool axisTitleFontUseRelSize(    uint n ) const;
    void setAxisTitleFontRelSize(    uint n, int axisTitleFontRelSize );
    int axisTitleFontRelSize(        uint n ) const;

    QRect axisArea( const uint n ) const
        {
            if ( n < KDCHART_MAX_AXES ) {
                return _axisSettings[ n ].params.axisTrueAreaRect();
                // Do not do emit changed() here!
            }else{
                return QRect(QPoint(0,0),QSize(0,0));
            }
        }
    // END AXES



    QRect dataArea()   const { return _dataAreaRect; }
    QRect legendArea() const { return _legendAreaRect; }



    // HEADERS/FOOTERS

public:

    /*
      READ before changing/enhancing the following enum
      =================================================

      Please note: The following enum is made in a way that
      sections with smaller Y values come first.
      Thus one can construct a loop itterating over theHdFtPos values
      and so going from the top of the screen toiwards the bottom...

      Also please note that extending this enum shound NOT break the
      tripple groups (numbers 0/1/2, 3/4/5, and 6/7/8) since the
      algorithms in
      KDChartPainter::setupGeometry() and
      KDChartPainter::paintHeaderFooter() rely on this schema.
      Also don't forget to always update the auxiliary values
      HdFtPosSTART, HdFtPosHeadersSTART,
      HdFtPosHeadersEND, HdFtPosFootersSTART,
      HdFtPosFootersEND, and HdFtPosEND muessen - these are used
      fpr other itarators...
    */
    enum HdFtPos{ // start of all sections
        HdFtPosSTART = 0,

        // headers:
        HdFtPosHeadersSTART  = 0,
        // top-most headers
        HdFtPosHeaders0START = 0,
        HdFtPosHeader0       = 0,
        HdFtPosHeader0L = 1,
        HdFtPosHeader0R      = 2,
        HdFtPosHeaders0END   = 2,
        // normal ( == middle ) headers
        HdFtPosHeaders1START = 3,
        HdFtPosHeader        = 3,        // center
        HdFtPosHeaderL = 4,              // left
        HdFtPosHeaderR       = 5,        // right
        HdFtPosHeaders1END   = 5,
        // bottom headers
        HdFtPosHeaders2START = 6,
        HdFtPosHeader2       = 6,
        HdFtPosHeader2L = 7,
        HdFtPosHeader2R      = 8,
        HdFtPosHeaders2END   = 8,

        HdFtPosHeadersEND = 8,

        // footers:
        HdFtPosFootersSTART = 9,
        // normal ( == middle ) footers
        HdFtPosFooters1START =  9,
        HdFtPosFooter        =  9,
        HdFtPosFooterL = 10,
        HdFtPosFooterR       = 11,
        HdFtPosFooters1END   = 11,
        // bottom footers
        HdFtPosFooters2START = 12,
        HdFtPosFooter2       = 12,
        HdFtPosFooter2L = 13,
        HdFtPosFooter2R      = 14,
        HdFtPosFooters2END   = 14,
        // top-most footers
        HdFtPosFooters0START = 15,
        HdFtPosFooter0       = 15,
        HdFtPosFooter0L = 16,
        HdFtPosFooter0R      = 17,
        HdFtPosFooters0END   = 17,

        HdFtPosFootersEND = 17,

        HdFtPosEND = 17 };

public slots:

    void setHeaderFooterText( uint pos, const QString& text );
    QString headerFooterText( uint pos ) const;
    const QRect& headerFooterRect( uint pos ) const;
    void setHeaderFooterColor( uint pos, const QColor color );
    QColor headerFooterColor( uint pos ) const;
    void setHeaderFooterFont( uint pos, const QFont& font,
                              bool fontUseRelSize,
                              int fontRelSize );
    QFont headerFooterFont( uint pos ) const;
    bool headerFooterFontUseRelSize( uint pos ) const;
    int headerFooterFontRelSize( uint pos ) const;


    // quick&dirty functions: access Header, Header2 and Footer
    //                        without having to specify the HdFtPos
    //
    // This may be convenient for specifying simple charts with
    // up to two centered header(s) and up to one centered footer.


    void setHeader1Text( const QString& text )
        {
            _hdFtParams[ HdFtPosHeader ]._text = text;
            emit changed();
        }


    QString header1Text() const
        {
            return _hdFtParams[ HdFtPosHeader ]._text;
        }

    void setHeader1Font( const QFont& font )
        {
            _hdFtParams[ HdFtPosHeader ]._font = font;
            emit changed();
        }


    QFont header1Font() const
        {
            return _hdFtParams[ HdFtPosHeader ]._font;
        }

    void setHeader2Text( const QString& text )
        {
            _hdFtParams[ HdFtPosHeader2 ]._text = text;
            emit changed();
        }


    QString header2Text() const
        {
            return _hdFtParams[ HdFtPosHeader2 ]._text;
        }

    void setHeader2Font( const QFont& font )
        {
            _hdFtParams[ HdFtPosHeader2 ]._font = font;
            emit changed();
        }


    QFont header2Font() const
        {
            return _hdFtParams[ HdFtPosHeader2 ]._font;
        }



    void setFooterText( const QString& text )
        {
            _hdFtParams[ HdFtPosFooter ]._text = text;
            emit changed();
        }


    QString footerText() const
        {
            return _hdFtParams[ HdFtPosFooter ]._text;
        }

    void setFooterFont( const QFont& font )
        {
            _hdFtParams[ HdFtPosFooter ]._font = font;
            emit changed();
        }


    QFont footerFont() const
        {
            return _hdFtParams[ HdFtPosFooter ]._font;
        }


    /**
       \internal
    */
    void __internalStoreHdFtRect( int pos, QRect rect )
        {
            if ( 0 <= pos && HdFtPosEND >= pos )
                _hdFtParams[ pos ].setRect( rect );
        }


    // END HEADERS/FOOTERS



public:
    KDChartParams();
    virtual ~KDChartParams();

public slots:
    void loadAxesFormXML(int& curAxisSettings, QDomElement& element);
    bool loadXML( const QDomDocument& doc );

    void saveAxesToXML(QDomDocument& doc, QDomElement& docRoot) const;
    QDomDocument saveXML( bool withPI = true ) const;

public:
    friend QTextStream& operator<<( QTextStream& s, const KDChartParams& p );
    friend QTextStream& operator>>( QTextStream& s, KDChartParams& p );

public slots:
    static void createChartValueNode( QDomDocument& doc, QDomNode& parent,
                                      const QString& elementName,
                                      const QVariant& valY,
                                      const QVariant& valX,
                                      const int& propID  );
    static void createColorMapNode( QDomDocument& doc, QDomNode& parent,
                                    const QString& elementName,
                                    const QMap< uint, QColor >& map );
    static void createDoubleMapNode( QDomDocument& doc, QDomNode& parent,
                                     const QString& elementName,
                                     const QMap< int, double >& map );
    static void createChartFontNode( QDomDocument& doc, QDomNode& parent,
                                     const QString& elementName,
                                     const QFont& font, bool useRelFont,
                                     int relFont,
                                     int minFont=-1 );
    static bool readColorMapNode( const QDomElement& element,
                                  QMap<uint,QColor>* map );
    static bool readDoubleMapNode( const QDomElement& element,
                                   QMap<int,double>* map );
    static bool readChartFontNode( const QDomElement& element, QFont& font,
                                   bool& useRelFont, int& relFontSize,
                                   int* minFontSize=0 );
    static bool readChartValueNode( const QDomElement& element,
                                    QVariant& valY,
                                    QVariant& valX,
                                    int& propID );


    // do *not* call this function manually: it is used by KDChartPainter::setupGeometry()
    void setDataArea( const QRect& areaRect )
        {
            _dataAreaRect = areaRect;
            // Do not do emit changed() here!
        }
    // do *not* call this function manually: it is used by KDChartPainter::setupGeometry()
    void setLegendArea( const QRect& areaRect )
        {
            _legendAreaRect = areaRect;
            // Do not do emit changed() here!
        }


signals:
    void changed();


protected:
    void setBarsDisplayed( int barsDisplayed )
        {
            _barsDisplayed = barsDisplayed;
        }
    void setBarsLeft( int barsLeft )
        {
            _barsLeft = barsLeft;
        }


private:
    QString dataRegionFrameAreaName( uint dataRow,
                             uint dataCol,
                             uint data3rd );
    void recomputeShadowColors();
    void insertDefaultAxisTitleBox( uint n,
                                    bool setTitle,       const QString& axisTitle,
                                    bool setColor,       const QColor& axisTitleColor,
                                    bool setFont,        const QFont& axisTitleFont,
                                    bool setFontUseRel,  bool useFontSize,
                                    bool setFontRelSize, int axisTitleFontRelSize );

    static QColor _internalPointer_DataValueAutoColor;

    KDChartPropertySet* tempPropSetA;
    KDChartPropertySet* tempPropSetB;

    bool _optimizeOutputForScreen;
    int _globalLeadingLeft;
    int _globalLeadingTop;
    int _globalLeadingRight;
    int _globalLeadingBottom;
    ChartType _chartType;
    ChartType _additionalChartType;
    int _numValues;
    QFont _defaultFont;

    typedef Q3Dict < KDChartFrameSettings > AreaDict;
    AreaDict _areaDict;

    CustomBoxDict _customBoxDict;
    bool _customBoxDictMayContainHoles;
    QRect _noRect;
    KDFrame _noFrame;

    KDChartFrameSettings _noFrameSettings;

    class ModeAndChart
    {
    public:
        ModeAndChart()
            : _mode( UnknownMode ), _chart( KDCHART_NO_CHART )
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

    typedef QMap < uint, ModeAndChart > ModeAndChartMap;
    ModeAndChartMap _dataSourceModeAndChart;
    bool _setChartSourceModeWasUsed;
    QMap < uint, QColor > _dataColors;
    uint _maxDatasetSourceMode;
    KDChartPropertySetList _propertySetList;
    double _shadowBrightnessFactor;
    Qt::BrushStyle _shadowPattern;
    bool _threeDShadowColors;
    uint _maxDatasetColor;
    QMap < uint, QColor > _dataColorsShadow1;
    QMap < uint, QColor > _dataColorsShadow2;
    QColor _outlineDataColor;
    uint _outlineDataLineWidth;
	Qt::PenStyle _outlineDataLineStyle;


    struct PrintDataValuesSettings {
        bool _printDataValues;
        int _divPow10;
        int _digitsBehindComma;
        QFont _dataValuesFont;
        bool _dataValuesUseFontRelSize;
        int _dataValuesFontRelSize;
        QColor _dataValuesColor;
        QBrush _dataValuesBrush;
        bool _dataValuesAutoColor;
        KDChartEnums::PositionFlag _dataValuesAnchorNegativePosition;
        uint _dataValuesAnchorNegativeAlign;
        int _dataValuesAnchorNegativeDeltaX;
        int _dataValuesAnchorNegativeDeltaY;
        int _dataValuesNegativeRotation;
        KDChartEnums::PositionFlag _dataValuesAnchorPositivePosition;
        uint _dataValuesAnchorPositiveAlign;
        int _dataValuesAnchorPositiveDeltaX;
        int _dataValuesAnchorPositiveDeltaY;
        int _dataValuesPositiveRotation;
        KDChartEnums::TextLayoutPolicy _dataValuesLayoutPolicy;
        bool _dataValuesShowInfinite;

        bool _useDefaultFontParams;  // internal flag, do NOT store it!
    };
    PrintDataValuesSettings _printDataValuesSettings;
    PrintDataValuesSettings _printDataValuesSettings2;

    bool _allowOverlappingDataValueTexts;
    BarChartSubType _barChartSubType;
    bool _threeDBars;
    int _threeDBarAngle;
    double _threeDBarDepth;
    double _cosThreeDBarAngle;
    int _datasetGap;
    bool _datasetGapIsRelative;
    int _valueBlockGap;
    bool _valueBlockGapIsRelative;
    int _barWidth;
    int _userWidth;
    bool _solidExcessArrows;
    // volatile bar chart information, not saved in the stream
    QRect _dataAreaRect;
    QRect _legendAreaRect;
    int _barsDisplayed;
    int _barsLeft;

    // LINES/AREAS-specific
    LineChartSubType _lineChartSubType;
    bool _threeDLines;
    int _threeDLineDepth;
    int _threeDLineXRotation;
    int _threeDLineYRotation;
    bool _lineMarker;
    LineMarkerStyleMap _lineMarkerStyles;
    uint _maxDatasetLineMarkerStyle;
    QSize _lineMarkerSize;
    QColor _lineColor;
    int _lineWidth;
	Qt::PenStyle _lineStyle;
    typedef QMap<uint, Qt::PenStyle> LineStyleMap;
    LineStyleMap _datasetLineStyles;
    AreaChartSubType _areaChartSubType;
    AreaLocation _areaLocation;


    // POLAR-specific
    PolarChartSubType _polarChartSubType;
    bool _polarMarker;
    PolarMarkerStyleMap _polarMarkerStyles;
    uint _maxDatasetPolarMarkerStyle;
    QSize _polarMarkerSize;
    int _polarLineWidth;
    int _polarZeroDegreePos;
    bool _polarRotateCircularLabels;
    struct _polarDelimsAndLabelStruct {
        bool showDelimiters;
        bool showLabels;
    };
    _polarDelimsAndLabelStruct _polarDelimsAndLabels[ 1 + KDCHART_MAX_POLAR_DELIMS_AND_LABELS_POS ];



    // PIES/RINGS
    bool _explode;
    double _explodeFactor;
    ExplodeFactorsMap _explodeFactors;
    Q3ValueList<int> _explodeList;
    bool _threeDPies;
    int _threeDPieHeight;
    int _pieStart;
    int _ringStart;
    bool _relativeRingThickness;


    // HI-LO CHARTS

    HiLoChartSubType _hiLoChartSubType;
    bool _hiLoChartPrintLowValues;
    QFont _hiLoChartLowValuesFont;
    bool _hiLoChartLowValuesUseFontRelSize;
    int _hiLoChartLowValuesFontRelSize;
    QColor _hiLoChartLowValuesColor;
    bool _hiLoChartPrintHighValues;
    QFont _hiLoChartHighValuesFont;
    bool _hiLoChartHighValuesUseFontRelSize;
    int _hiLoChartHighValuesFontRelSize;
    QColor _hiLoChartHighValuesColor;
    bool _hiLoChartPrintOpenValues;
    QFont _hiLoChartOpenValuesFont;
    bool _hiLoChartOpenValuesUseFontRelSize;
    int _hiLoChartOpenValuesFontRelSize;
    QColor _hiLoChartOpenValuesColor;
    bool _hiLoChartPrintCloseValues;
    QFont _hiLoChartCloseValuesFont;
    bool _hiLoChartCloseValuesUseFontRelSize;
    int _hiLoChartCloseValuesFontRelSize;
    QColor _hiLoChartCloseValuesColor;



    // BOX AND WHISKER CHARTS

    BWChartSubType _BWChartSubType;

    struct BWChartStatistics {
        double value;
        int    y;
        bool   active;
        QFont  font;
        bool   useRelSize;
        int    relSize;
        QColor color;
        QBrush brush;
    };
    BWChartStatistics _BWChartStatistics[ BWStatValOffEndValue ];
    double _BWChartFenceUpperInner;
    double _BWChartFenceLowerInner;
    double _BWChartFenceUpperOuter;
    double _BWChartFenceLowerOuter;
    int _BWChartOutValMarkerSize;
    QBrush _BWChartBrush;


    // LEGENDS

    LegendPosition _legendPosition;
    Qt::Orientation _legendOrientation;
    bool _legendShowLines;
    LegendSource _legendSource;
    QMap < int, QString > _legendText;
    QColor _legendTextColor;
    QFont _legendFont;
    bool _legendFontUseRelSize;
    int _legendFontRelSize;
    QString _legendTitleText;
    QColor _legendTitleTextColor;
    QFont _legendTitleFont;
    bool _legendTitleFontUseRelSize;
    int _legendTitleFontRelSize;
    uint _legendSpacing;


    // AXES (private)
    // Must be exported because of the inline methods
    struct KDCHART_EXPORT AxisSettings
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
    // END AXES (private)


    // HEADER/FOOTER (private)
    // Must be exported because of the inline methods
    struct KDCHART_EXPORT HdFtParams
    {
        QString _text;
        QColor _color;
        QFont _font;
        bool _fontUseRelSize;
        int _fontRelSize;
        HdFtParams()
            {
                _color = QColor( Qt::black );
                _font = QFont( "helvetica", 10, QFont::Normal, false );
                _fontUseRelSize = true;
                _fontRelSize = 8; // per default quite small
            }
        void setRect( QRect rect )
            {
                _rect = rect;
            }
        const QRect& rect() const
            {
                return _rect;
            }
    private:
        // temporary data that are NOT to be stored within sessions:
        QRect _rect;
    };

    HdFtParams _hdFtParams[ HdFtPosEND + 1 ];
    // END HEADER/FOOTER (private)
};


QTextStream& operator<<( QTextStream& s, const KDChartParams& p );


QTextStream& operator>>( QTextStream& s, KDChartParams& p );


#endif
