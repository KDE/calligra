/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

#ifndef __KDCHARTPAINTER_H__
#define __KDCHARTPAINTER_H__

#include <qmap.h>
#include <qrect.h>
#include <qstring.h>

#include <KDChartUnknownTypeException.h>
#include <KDChartNotEnoughSpaceException.h>
#include <KDChartTable.h>
#include <KDChartDataRegion.h>

class KDChartParams;
class KDChartTextPiece;
class QPainter;

class KDChartPainter
{
public:
    static KDChartPainter* create( KDChartParams* params, bool make2nd = false )
#ifdef USE_EXCEPTIONS
    throw( KDChartUnknownTypeException )
#endif
    ;

    virtual ~KDChartPainter();

    static void registerPainter( const QString& painterName,
                                 KDChartPainter* painter );
    static void unregisterPainter( const QString& painterName )
#ifdef USE_EXCEPTIONS
    throw( KDChartUnknownTypeException )
#endif
    ;

    virtual void paint( QPainter* painter, KDChartTableData* data,
                        bool paint2nd,
                        KDChartDataRegionList* regions = 0 );

protected:
    KDChartPainter( KDChartParams* );

    virtual void paintData( QPainter* painter, KDChartTableData* data,
                            bool paint2nd,
                            KDChartDataRegionList* regions = 0 ) = 0;
    virtual void paintAxes( QPainter* painter, KDChartTableData* data );
    virtual void paintLegend( QPainter* painter, KDChartTableData* data,
                              const QFont& actLegendFont,
                              const QFont& actLegendTitleFont );
    virtual void paintHeader( QPainter* painter, KDChartTableData* data );
    virtual void paintFooter( QPainter* painter, KDChartTableData* data );
    virtual void setupGeometry ( QPainter* painter, KDChartTableData* data,
                                 QFont& actLegendFont,
                                 QFont& actLegendTitleFont )
#ifdef USE_EXCEPTIONS
    throw( KDChartNotEnoughSpaceException )
#endif
    ;

    virtual QString fallbackLegendText( uint dataset ) const;
    virtual uint numLegendFallbackTexts( KDChartTableData* data ) const;

    const KDChartParams* params() const
    {
        return _params;
    }
    QRect _dataRect;

    QRect _axesRect; /* The Axes rect contains the Data rect. Up to 4
    			axes might be at the left and bottom as well
    			as at the right and/or at the to top of the
    			chart
    		     */

    QRect _header1Rect; /* The header 1 rect is the topmost one of all
    			   rectangles. It contains the first header
    			   line and extends over the whole width.
    			*/

    QRect _header2Rect; /* The header 2 rect is below the header one
    			   rect. It contains the second header line
    			   and extends over the whole width.
    			*/

    QRect _footerRect; /* The footer is the bottommost one of all
    			  rectangles. It contains the footer line and
    			  extends over the whole width.
    		       */

    QRect _legendRect; /* The legend position depends on the parameter
    			  settings. If it is not directly to the left or
    			  to the right of the data display, it will be
    			  below the headers and on top of the footers.
    		       */
    uint _legendEMSpace; // an em space in the legend font
    uint _legendSpacing; // the line spacing in the legend font
    uint _legendHeight; // the font height in the legend font
    uint _legendLeading; // the font leading in the legend font
//     uint _legendTitleSpacing; // the line spacing in the legend title font
//    uint _legendTitleHeight; // the font height in the legend title font
//     uint _legendTitleLeading; // the font leading in the legend title font
    KDChartTextPiece* _legendTitle;
    
    uint _numLegendTexts; // the number of legend items to show

    int _logicalWidth;
    int _logicalHeight;

    QMap < uint, QString > _legendTexts; // precomputed legend texts

private:
    // disallow copy-construction and assignment
    KDChartPainter( const KDChartPainter& );
    KDChartPainter& operator=( const KDChartPainter& );


    QMap < QString, KDChartPainter* > _customPainters;
    KDChartParams* _params;

    void findLegendTexts( KDChartTableData* );
};

#endif
