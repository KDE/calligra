#ifndef __trader_scan_h__
#define __trader_scan_h__

#include <CORBA.h>

class Trader;

void koScanParts( Trader *_trader, CORBA::ORB_ptr _orb );
void koScanParts( Trader *_trader, CORBA::ImplRepository_ptr _imr );
void koScanParts( Trader *_trader, const char* _path, CORBA::ImplRepository_ptr _imr );
void koScanPartFile( Trader *_trader, const char* _file, CORBA::ImplRepository_ptr _imr );

void koScanFilters( Trader *_trader, CORBA::ORB_ptr _orb );
void koScanFilters( Trader *_trader, CORBA::ImplRepository_ptr _imr );
void koScanFilters( Trader *_trader, const char* _path, CORBA::ImplRepository_ptr _imr );
void koScanFilterFile( Trader *_trader, const char* _file, CORBA::ImplRepository_ptr _imr );

#endif
