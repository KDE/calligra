#include "kscript_ext_qapplication.h"
#include "kscript_context.h"
#include <stdio.h>
#include <qapplication.h>

KSClass_QApplication::KSClass_QApplication( KSModule* m ) : KSScriptClass( m, "QApplication", 0 )
{
  nameSpace()->insert( "QApplication", new KSValue( (KSBuiltinMethod)&KSObject_QApplication::ksQApplication ) );
  nameSpace()->insert( "exec", new KSValue( (KSBuiltinMethod)&KSObject_QApplication::ksQApplication_exec ) );
  nameSpace()->insert( "delete", new KSValue( (KSBuiltinMethod)&KSObject_QApplication::ksQApplication_delete ) );
}

KSScriptObject* KSClass_QApplication::createObject( KSClass* c )
{
  return new KSObject_QApplication( c );
}

KSObject_QApplication::KSObject_QApplication( KSClass* c ) : KS_Qt_Object( c )
{
}

bool KSObject_QApplication::ksQApplication( KSContext& context )
{
  printf("QApplication\n");

  if ( object() )
  {
    QString tmp( "The constructor of the class QApplication was called twice." );
    context.setException( new KSException( "ConstructorCalledTwice", tmp, -1 ) );
    return false;
  }

  // TODO: check parameters

  ASSERT( qApp );
  // We dont know the application object -> FALSE
  setObject( qApp, FALSE );

  return true;
}

bool KSObject_QApplication::ksQApplication_exec( KSContext& context )
{
  printf("QApplication::exec\n");

  if ( !object() )
  {
    QString tmp( "The method QApplication::exec was called before the QApplication constructor or after the QApplication destructor" );
    context.setException( new KSException( "MethodCalledBeforeConstructor", tmp, -1 ) );
    return false;
  }

  // TODO: check parameters

  QApplication* a = (QApplication*)object();
  a->exec();

  return true;
}

bool KSObject_QApplication::ksQApplication_delete( KSContext& )
{
  printf("QApplication::delete\n");
  // TODO: check parameters

  if ( !object() )
    return true;

  if ( hasOwnership() )
    delete object();
  setObject( 0 );

  return true;
}
