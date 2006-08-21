//
// Created: Mon Aug 21 17:46:50 2006
//      by: bynames.py
//    from: byalpha.html
//
// WARNING! All changes made in this file will be lost!


#ifndef ENTITIES_H
#define ENTITIES_H

#include "kformuladefs.h"

KFORMULA_NAMESPACE_BEGIN
	
struct entityMap {
    int operator<( const char* right ) const {
	    return strcmp( name, right ) < 0;
    }
	const char* name;
	const uint unicode;
};
	
extern const entityMap entities[];

KFORMULA_NAMESPACE_END

#endif // ENTITIES_H

