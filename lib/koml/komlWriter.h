#ifndef __koml_writer_h__
#define __koml_writer_h__

#include <iostream.h>

ostream& otag( ostream& outs );
ostream& etag( ostream& outs );
ostream& indent( ostream& outs );

void intToHexStr( char *_buffer, int _val );
int hexStrToInt( const char *_buffer );

void shortToHexStr( char *_buffer, short _val );
short hexStrToShort( const char *_buffer );

#endif
