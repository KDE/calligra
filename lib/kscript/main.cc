#include "kscript.h"

#include <qapplication.h>
#include <qstring.h>

#include <CORBA.h>
// The fuck does not compile without this ....
// dont ask me why!
#include <mico/template_impl.h>

CORBA::ORB* pOrb;

CORBA::ORB* orb()
{
  return pOrb;
}

int main( int argc, char** argv )
{
  if ( argc != 2 )
  {
    printf("Syntax: kscript <script>\n");
    return 0;
  }

  QApplication app( argc, argv );

  CORBA::ORB_var orb = CORBA::ORB_init( argc, argv, "mico-local-orb");
  pOrb = orb;
  CORBA::BOA_var boa = orb->BOA_init( argc, argv, "mico-local-boa");

  KSInterpreter script;
  QString ex = script.runScript( argv[1] );
  if ( !ex.isEmpty() )
    printf("%s\n",ex.ascii());

  return 0;
}

