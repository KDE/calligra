#ifndef __trader_scan_h__
#define __trader_scan_h__

#include <CORBA.h>

class Trader;

void koScanFilters( Trader *_trader, CORBA::ORB_ptr _orb );
void koScanOfficeParts( Trader *_trader, CORBA::ORB_ptr _orb );
void koScanParts( Trader *_trader, CORBA::ORB_ptr _orb );

#endif
