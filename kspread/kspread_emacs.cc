#include "kspread_emacs.h"
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

KEmacs::KEmacs()
{
    emacsPID = 1;
    
    int i = system( "gnuclient -q" );
    if ( i != 0 )
    {
	int j = system( "emacs &" );
	if ( j != 0 )
	    emacsPID = 0;
	sleep( 5 );
    }
    
}

KEmacs::~KEmacs()
{
}

QString KEmacs::runCmd( const char *_cmd )
{
    QString ret = "";
    
    FILE *f = popen( _cmd, "r" );
    if ( f == 0L )
	return QString();
    char buffer[ 1024 ];
    int n = 1;
    while ( !feof( f ) )
    {
	n = fread( buffer, 1, 1023, f );
	
	if ( n > 0 )
	{
	    buffer[n] = 0;
	    ret += buffer;
	}
    }
    
    pclose( f );
    if ( ret.length() > 0 )
	ret.truncate( ret.length() - 1 );
    return QString( ret.data() );
}
	
QString KEmacs::openFile( const char *_name )
{
    QString cmd;
    cmd.sprintf( "gnudoit '(find-file \"%s\")'", _name );
    QString erg = runCmd( cmd );

    if ( erg.isNull() )
	return QString();
    return QString( erg.data() );
}


bool KEmacs::switchToBuffer( const char *_buffer )
{
    QString cmd;
    cmd.sprintf( "gnudoit '(switch-to-buffer \"%s\")'", _buffer );
    QString erg = runCmd( cmd );

    return !( erg.isNull() );
}

bool KEmacs::killBuffer( const char *_buffer )
{
    QString cmd;
    cmd.sprintf( "gnudoit '(kill-to-buffer \"%s\")'", _buffer );
    QString erg = runCmd( cmd );

    return !( erg.isNull() );
}

bool KEmacs::saveBuffer( const char *_buffer )
{
    QString cmd;
    cmd.sprintf( "gnudoit '(save-buffer \"%s\")'", _buffer );
    QString erg = runCmd( cmd );

    return !( erg.isNull() );
}

bool KEmacs::hide()
{
    QString cmd;
    cmd.sprintf( "gnudoit '(lower-frame)'" );
    QString erg = runCmd( cmd );

    return !( erg.isNull() );
}

bool KEmacs::show()
{
    QString cmd;
    cmd.sprintf( "gnudoit '(raise-frame)'" );
    QString erg = runCmd( cmd );

    return !( erg.isNull() );
}

bool KEmacs::updateFromFile( const char *_file )
{
    QString cmd;
    cmd.sprintf( "gnudoit '(find-alternate-file \"%s\")'", _file );
    QString erg = runCmd( cmd );

    return !( erg.isNull() );
}












