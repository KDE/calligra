#ifndef __kspread_chart_impl_h__
#define __kspread_chart_impl_h__

class KSpreadTable;
class ChartCallback_impl;

#include "kspread_chart.h"

class ChartCallback_impl : public CHART::Callback
{
public:
  ChartCallback_impl( KSpreadTable *table );

  // Hack
  CHART::Matrix* request( const CHART::Range& range )
  {
    return 0L;
  }
  
protected:
  KSpreadTable *pTable;
};

#endif
