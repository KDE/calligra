#include "kscript.h"

#include <kapp.h>
#include <dcopclient.h>
#include <qstring.h>
#include <stdio.h> /* for printf */

int main( int argc, char** argv )
{
  if ( argc != 2 )
  {
    printf("Syntax: kscript <script>\n");
    return 0;
  }

  KApplication app( argc, argv, argv[1] );

  kapp->dcopClient()->attach();
  kapp->dcopClient()->registerAs( argv[1] );
  
  KSInterpreter script;
  QString ex = script.runScript( argv[1] );
  if ( !ex.isEmpty() )
    printf("%s\n",ex.ascii());

  return 0;
}

