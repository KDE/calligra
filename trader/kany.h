#ifndef __kany_h__
#define __kany_h__

#include <CORBA.h>

#include <iostream>

ostream& operator<<( ostream& out, CORBA::Any& any );

bool printAny( ostream& out, CORBA::Any& _any, CORBA::TypeCode_ptr _tc );

#endif
