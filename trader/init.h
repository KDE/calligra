#ifndef __init_h__
#define __init_h__

class Trader;

bool imrCreate( const char* _name, const char* _mode, const char *_exec, const char* _iface, CORBA::ImplRepository_ptr _imr );

void initServiceTypes( Trader* _trader );

#endif
