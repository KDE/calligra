/*
	To fix the ridiculous INT32 redefinition problem.
	Ok, it's a hack. But it works.
	
	(c)Sirtaj Singh Kang, 1996.

	$Id$

	This now written by tegla@katalin.csoma.elte.hu
*/

#ifdef QGLOBAL_H
#define XMD_H
typedef short INT16;
#endif

extern "C" {
#include<jpeglib.h>
}

#ifdef QGLOBAL_H
#undef XMD_H
#endif

