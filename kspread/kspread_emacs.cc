/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
 
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
 
   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/     

#include "kspread_emacs.h"
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

KEmacs::KEmacs()
{
    emacsPID = 1;

    // Is emacs running ?

    // First try the fuser trick
    QString command;
    command.sprintf("/usr/sbin/fuser -s -n tcp %d",21490 + getuid());
    int ret = system( command.data() );
    if ( ret != 0 )
    {
        // If it didn't work (fuser not present or xemacs not launched),
        // try gnuclient -q (might create a new window !)
        ret = system( "gnuclient -q" );
        if ( ret != 0 )
        {
            ret = system( "emacs &" ); // shouldn't that be 'xemacs' ?
            if ( ret != 0 )
                emacsPID = 0;
            sleep( 5 );
        }
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
    cmd.sprintf( "gnudoit '(kill-buffer \"%s\")'", _buffer );
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












