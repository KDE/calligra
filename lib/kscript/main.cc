#include "kscript.h"

#include <kapp.h>
#include <dcopclient.h>
#include <qstring.h>
#include <stdio.h> /* for printf */
#include <qdir.h>

int main( int argc, char** argv )
{
    qDebug("..... Process started");
      
  if ( argc < 2 )
  {
    printf("Syntax: kscript <script> [command line arguments]\n");
    return 0;
  }

  KApplication app( argc, argv, argv[1] );

  kapp->dcopClient()->attach();
  kapp->dcopClient()->registerAs( argv[1] );

  qDebug("..... KScript started");
  {
      QStringList args;
      for( int i = 2; i < argc; ++i )
	  args.append( argv[i] );

      KSInterpreter script;
      script.addSearchPath( QDir::currentDirPath() );
      QString ex = script.runScript( argv[1], args );
      if ( !ex.isEmpty() )
	  printf("%s\n",ex.ascii());
  }

  return 0;
}

