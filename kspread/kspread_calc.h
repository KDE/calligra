#ifndef __calc_h__
#define __calc_h__

bool evalFormular( const char *_formular, KSpreadTable* _table, double& _result );
bool makeDepend( const char* _formular, KSpreadTable* _table, QList<KSpreadDepend>* _list );

#include "kspread_calcerr.h"

#endif
