#ifndef KDCHART_STOCK_DIAGRAM_P_H
#define KDCHART_STOCK_DIAGRAM_P_H

#include "KDChartStockDiagram.h"
#include "KDChartAbstractCartesianDiagram_p.h"
#include "KDChartCartesianDiagramDataCompressor_p.h"
#include "KDChartPaintContext.h"

namespace KDChart {

class StockDiagram::Private : public AbstractCartesianDiagram::Private
{
    friend class StockDiagram;

public:
    Private();
    Private( const Private& r );
    ~Private();

    Type type;
    StockDiagram *diagram;

    QBrush upTrendCandlestickBrush;
    QBrush downTrendCandlestickBrush;
    QPen upTrendCandlestickPen;
    QPen downTrendCandlestickPen;

    QMap<int, QBrush> upTrendCandlestickBrushes;
    QMap<int, QBrush> downTrendCandlestickBrushes;
    QMap<int, QPen> upTrendCandlestickPens;
    QMap<int, QPen> downTrendCandlestickPens;

    QPen lowHighLinePen;
    QMap<int, QPen> lowHighLinePens;


    void drawOHLCBar( const CartesianDiagramDataCompressor::DataPoint &open,
                      const CartesianDiagramDataCompressor::DataPoint &high,
                      const CartesianDiagramDataCompressor::DataPoint &low,
                      const CartesianDiagramDataCompressor::DataPoint &close,
                      PaintContext *context );
    void drawHLCBar( const CartesianDiagramDataCompressor::DataPoint &high,
                     const CartesianDiagramDataCompressor::DataPoint &low,
                     const CartesianDiagramDataCompressor::DataPoint &close,
                     PaintContext *context );
    void drawCandlestick( const CartesianDiagramDataCompressor::DataPoint &open,
                          const CartesianDiagramDataCompressor::DataPoint &high,
                          const CartesianDiagramDataCompressor::DataPoint &low,
                          const CartesianDiagramDataCompressor::DataPoint &close,
                          PaintContext *context );

private:
    void drawLine( int col, const QPointF &point1, const QPointF &p2, PaintContext *context );
    QPointF projectPoint( PaintContext *context, const QPointF &point ) const;
    QRectF projectCandlestick( PaintContext *context, const QPointF &open, const QPointF &close, qreal width ) const;
    int openValueColumn() const;
    int highValueColumn() const;
    int lowValueColumn() const;
    int closeValueColumn() const;

    class ThreeDPainter;
};

KDCHART_IMPL_DERIVED_DIAGRAM( StockDiagram, AbstractCartesianDiagram, CartesianCoordinatePlane )

}

#endif // KDCHART_STOCK_DIAGRAM_P_H

