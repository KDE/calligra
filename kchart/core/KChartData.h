/* $Id$ */

#ifndef _KCHARTDATA_H
#define _KCHARTDATA_H

#include <qbitarry.h>
#include <qobject.h>
#include <qvector.h>
#include <vector>

typedef QVector<QString> KChartXDataSet;
typedef vector<double> KChartYDataSet;

class QStrList;

class KChartData : public QObject
{
	Q_OBJECT

public:
  KChartData( int datasets );
  ~KChartData();

  void setXValue( uint pos, const char* value );
  QString xValue( uint pos ) const;
  void setYValue( uint dataset, uint pos, double value );
  double yValue( uint dataset, uint pos ) const;
  bool hasYValue( uint dataset, uint pos ) const;
  
  int numDatasets() const { return _datasets; }
  uint maxPos() const { return _maxpos; }
  double minYValue( uint dataset ) const;
  double maxYValue( uint dataset ) const;
  void minMaxOverallYValue( double& max, double &min ) const;

signals:
  void dataChanged();
  
private:
  KChartXDataSet* xdata;
  KChartYDataSet* ydatasets;
  QBitArray* ydatasetmasks;
  int _datasets;
  uint _maxpos;
};

#endif
