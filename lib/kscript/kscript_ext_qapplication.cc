#include "kscript_ext_qapplication.h"
#include "kscript_context.h"
#include "kscript_util.h"

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
  qDebug("QApplication\n");

  if ( object() )
  {
    QString tmp( "The constructor of the class QApplication was called twice." );
    context.setException( new KSException( "ConstructorCalledTwice", tmp, -1 ) );
    return false;
  }

  if ( !KSUtil::checkArgumentsCount( context, 0, "QApplication::QApplication" ) )
      return false;

  ASSERT( qApp );
  // We dont know the application object -> FALSE
  setObject( qApp, FALSE );

  return true;
}

bool KSObject_QApplication::ksQApplication_exec( KSContext& context )
{
  qDebug("QApplication::exec\n");

  if ( !object() )
  {
    QString tmp( "The method QApplication::exec was called before the QApplication constructor or after the QApplication destructor" );
    context.setException( new KSException( "MethodCalledBeforeConstructor", tmp, -1 ) );
    return false;
  }

  if ( !KSUtil::checkArgumentsCount( context, 0, "QApplication::QApplication" ) )
      return false;

  QApplication* a = (QApplication*)object();
  a->exec();

  return true;
}

bool KSObject_QApplication::ksQApplication_delete( KSContext& context )
{
  qDebug("QApplication::delete\n");
  
  if ( !KSUtil::checkArgumentsCount( context, 0, "QApplication::QApplication" ) )
      return false;

  if ( !object() )
    return true;

  if ( hasOwnership() )
    delete object();
  setObject( 0 );

  return true;
}
