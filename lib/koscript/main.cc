#include "koscript.h"

#include <kapp.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <dcopclient.h>
#include <qstring.h>
#include <stdio.h> /* for printf */
#include <stdlib.h>
#include <qdir.h>

static KCmdLineOptions options[] =
{
  { "+script", I18N_NOOP("Script to run."), 0 },
  { "+-- [options]", I18N_NOOP("Options to pass to the script."), 0 },
  { 0, 0, 0 }
};

int main( int argc, char** argv )
{
  const char *appName = (argc > 1) ? argv[1] : "koscript";
  KCmdLineArgs::init(argc, argv, appName,
        I18N_NOOP("KOffice Script interpreter."),
        "2.0.0");

  KCmdLineArgs::addCmdLineOptions(options);

  KApplication app;

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  if (args->count() < 1)
  {
     fprintf(stderr, i18n("You must specify a script.\n").local8Bit());
     ::exit(1);
  }

  qDebug("..... KScript started");
  {
      QStringList argList;
      for( int i = 1; i < args->count(); ++i )
          argList.append( args->arg(i) );

      KSInterpreter script;
      script.addSearchPath( QDir::currentDirPath() );
      QString ex = script.runScript( args->arg(0), argList );
      if ( !ex.isEmpty() )
          printf("%s\n",ex.local8Bit().data());
  }

  return 0;
}
