/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

#ifndef __KDCHARTAXESPAINTER_H__
#define __KDCHARTAXESPAINTER_H__

#include <qpen.h>
#include <qpainter.h>

#include <KDChartPainter.h>
#include <KDChartTable.h>
#include <KDChartAxisParams.h>

class KDChartParams;

class KDChartAxesPainter : public KDChartPainter
{
protected:
    KDChartAxesPainter( KDChartParams* params );
    virtual ~KDChartAxesPainter();

    virtual void paintAxes( QPainter* painter, KDChartTableData* data );
private:
public:
    static void saveDrawLine( QPainter& painter,
                              QPoint pA,
                              QPoint pZ,
                              QPen pen );
    static void calculateLabelTexts( QPainter& painter,
                                      const KDChartTableData& data,
                                      const KDChartParams& params,
                                      uint axisNumber,
                                      double averageValueP1000,
                                      KDChartAxisParams::AxisPos basicPos,
                                      const QPoint& orig,
                                      double delimLen,
                                      // start of return parameters
                                      double& nSubDelimFactor,
                                      double& pDelimDelta,
                                      double& nTxtHeight,
                                      double& pTextsX,
                                      double& pTextsY,
                                      double& pTextsW,
                                      double& pTextsH,
                                      int& textAlign );
    static void calculateBasicTextFactors( double nTxtHeight,
                                            const KDChartAxisParams& para,
                                            double averageValueP1000,
                                            KDChartAxisParams::AxisPos basicPos,
                                            const QPoint& orig,
                                            double delimLen,
                                            uint nLabels,
                                            // start of return parameters
                                            double& pDelimDelta,
                                            double& pTextsX,
                                            double& pTextsY,
                                            double& pTextsW,
                                            double& pTextsH,
                                            int& textAlign );
    static QString trunctateBehindComma( const double nVal,
                                  const int    behindComma,
                                  const double nDelta,
                                  int& trueBehindComma );
    static void calculateOrdinateFactors( const KDChartAxisParams& para,
                                   double& nDist,
                                   double& nDivisor,
                                   double& nRound,
                                   double& nDelta,
                                   double& nSubDelimFactor,
                                   double& nLow,
                                   double& nHigh,
                                   bool findNextRound = false );
    static void findInfos( double averageValueP1000,
                    const KDChartAxisParams& para,
                    uint axisPos,
                    KDChartAxisParams::AxisPos& basicPos,
                    QPoint& orig,
                    QPoint& dest );
};

#endif
