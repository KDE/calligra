#include "kpresenter_utils.h"

#include <stdio.h>
#include <stdlib.h>

QPixmap string_to_pixmap(const char *_pixmap)
{
    if ( _pixmap == 0L || _pixmap[0] == 0 )
	return QPixmap();
    
    // Count lines in XPM
    char pixmap[ strlen( _pixmap ) + 1 ];
    strcpy( pixmap, _pixmap );
    int lines = 1;
    char* pos = &pixmap[0];
    char* start = 0L;
    char* pline = pixmap;
    char first = *pixmap;
    
    while (*pos)
      {
	if (*pos == 1)
	  *pos = '\"';

	pos++;
      }

    pos = pixmap;
    while ( *pos )
      {	
	if ( *pos++ == '\n' )
	  {
	    if ( strncmp( pos - 3, "\",\n", 3 ) == 0 )
	      { *(pos-1) = 0; *(pos-2) = 0; *(pos-3) = 0; }
	    else if ( strncmp( pos - 4, "\"};\n", 4 ) == 0 )
	      { *(pos-1) = 0; *(pos-2) = 0; *(pos-3) = 0; *(pos-4) = 0; }	    
	    else if ( strncmp( pos - 3, "\"};", 3 ) == 0 )
	      { *(pos-1) = 0; *(pos-2) = 0; *(pos-3) = 0; }	    
	    else
	      { *(pos-1) = 0; }	    
	    if ( first == '\"' && start == 0L )
	      start = pline;
	    if ( first == '\"' )
	      lines++;
	    pline = pos;
	    first = *pline;
	  }
      }

    if ( start == 0L )
      return QPixmap();

    const char* list[ lines ];
    int i;
    const char* p = start;
    for ( i = 0; i < lines; i++ )
    {
      list[i] = p + 1;
      p += strlen( p ) + 1;
      while ( *p == 0 ) p++;
    }
    list[i] = 0L;

    return QPixmap( list );
}

QString load_pixmap(const char *_file)
{
    FILE *f = fopen( _file, "r" );
    if ( f == 0L )
    {
	warning(" Could not open pixmap file '%s\n", _file );
	return QString();
    }
    
    char buffer[ 2048 ];
    
    QString str( "" );
    while( !feof( f ) )
    {
	int i = fread( buffer, 1, 2047, f );
	if ( i > 0 )
	{
	    buffer[i] = 0;
	    str += buffer;
	}
    }
    
    fclose( f );
    
    return str;
}

QString load_pixmap_native_format(const char *_file)
{
    FILE *f = fopen( _file, "r" );
    if ( f == 0L )
    {
	warning(" Could not open pixmap file '%s\n", _file );
	return QString();
    }
    
    char buffer[ 2048 ];
    
    QString str( "" );
    while( !feof( f ) )
    {
	int i = fread( buffer, 1, 2047, f );
	if ( i > 0 )
	{
	    buffer[i] = 0;
	    char* c = &buffer[0];
	    while(*c)
	      {
		if (*c == '\"')
		  *c = 1;

		c++;
	      }

	    str += buffer;
	}
    }
    
    fclose( f );
    
    return str;
}
