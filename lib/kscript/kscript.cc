#include "kscript_parser.h"
#include "kscript_context.h"
#include "kscript_func.h"
#include "kscript_class.h"
#include "kscript.h"
#include "kscript_ext_qt.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

KSInterpreter::KSInterpreter()
{
  KSModule::Ptr m = ksCreateModule_KScript( this );
  m_modules.insert( m->name(), m );

  // This module will serve as our global namespace
  // since adressing all kscript builtin stuff via its module
  // is too much typing for our users.
  m_global = m->nameSpace();

  m = ksCreateModule_Qt( this );
  m_modules.insert( m->name(), m );

  // Integrate the Qt module in the global namespace for convenience
  KSNamespace::Iterator it = m->nameSpace()->begin();
  KSNamespace::Iterator end = m->nameSpace()->end();
  for(; it != end; ++it )
    m_global->insert( it.key(), it.data() );

  m_globalContext.setScope( new KSScope( m_global, 0 ) );
}

KSModule::Ptr KSInterpreter::module( const QString& name )
{
  QMap<QString,KSModule::Ptr>::Iterator it = m_modules.find( name );
  if ( it == m_modules.end() )
    return 0;

  return it.data();
}

QString KSInterpreter::runScript( const QString& filename, const QStringList& args )
{
  KSContext context( m_globalContext );
  // The "" indicates that this is not a module but
  // a script in its original meaning.
  if ( !runModule( context, "", filename, args ) )
    return context.exception()->toString( context );

  return QString::null;
}

bool KSInterpreter::runModule( KSContext& context, const QString& name )
{
  // Did we load this module already ? Dont load it twice
  if ( m_modules.contains( name ) )
  {
    KSModule* m = m_modules[name];
    m->ref();
    context.setValue( new KSValue( m ) );

    return true;
  }

  QStringList::Iterator it = m_searchPaths.begin();
  for( ; it != m_searchPaths.end(); ++it )
  {
    DIR *dp = 0L;
    struct dirent *ep;

    dp = opendir( *it );
    if ( dp == 0L )
      return false;

    while ( ( ep = readdir( dp ) ) != 0L )
    {
      if ( name == ep->d_name )
      {
	QString f = *it;
	f += "/";
	f += ep->d_name;
	struct stat buff;
	if ( ( stat( f, &buff ) == 0 ) && S_ISREG( buff.st_mode ) )
	{
	  QStringList lst;
	  return runModule( context, name, f, lst );
	}
      }
    }

    closedir( dp );
  }

  QString tmp( "Could not find module %1" );
  context.setException( new KSException( "IOError", tmp.arg( name ) ) );
  return false;
}

bool KSInterpreter::runModule( KSContext& result, const QString& name, const QString& filename, const QStringList& args )
{
  // Did we load this module already ? Dont load it twice
  if ( m_modules.contains( name ) )
  {
    KSModule* m = m_modules[name];
    m->ref();
    result.setValue( new KSValue( m ) );

    return true;
  }

  m_globalContext.setException( 0 );

  // s_current = this;

  FILE* f = fopen( filename, "r" );
  if ( !f )
  {
    QString tmp( "Could not open file %1" );
    result.setException( new KSException( "IOError", tmp.arg( filename ) ) );
    return false;
  }

  // Create the parse tree.
  KSParser parser;
  if ( !parser.parse( f, filename.ascii() ) )
  {
    fclose( f );
    result.setException( new KSException( "SyntaxError", parser.errorMessage() ) );
    return false;
  }
  fclose( f );
  // parser.print( true );

  // Create a module
  KSModule::Ptr module = new KSModule( this, name, parser.donateParseTree() );
  // Put the module in the return value
  module->ref();
  result.setValue( new KSValue( &*module ) );

  // Put all global functions etc. in the scope
  KSContext context;
  context.setScope( new KSScope( m_global, module ) );

  // Travers the parse tree to find functions and classes etc.
  if ( !module->eval( context ) )
  {
    if ( context.exception() )
    {
      result.setException( context );
      return false;
    }
    // TODO: create exception
    printf("No exception available\n");
    return false;
  }

  // Is there a main function to execute ?
  KSValue* code = module->object( "main" );
  if ( code )
  {
    // create a context that holds the argument list
    // for the main function ( the list is empty currently ).
    KSContext context;
    context.setValue( new KSValue( KSValue::ListType ) );
    context.setScope( new KSScope( m_global, module ) );

    // Insert parameters
    QStringList::ConstIterator sit = args.begin();
    QStringList::ConstIterator send = args.end();
    for( ; sit != send; ++sit )
    {
	context.value()->listValue().append( new KSValue( *sit ) );
    }
    
    if ( !code->functionValue()->call( context ) )
    {
      if ( context.exception() )
      {
	result.setException( context );
	return false;
      }

      // TODO: create exception
      printf("No exception available\n");
      return false;
    }
  }

  // Dump the namespace
  /* printf("\nNamespace\n---------\n");
  KSNamespace::Iterator it = m_space.begin();
  for( ; it != m_space.end(); ++it )
  {
    printf("%s = %s\n",it.key().ascii(),it.data()->toString().ascii() );
    if ( it.data()->type() == KSValue::ClassType )
    {
      KSNamespace::Iterator it2 = it.data()->classValue()->nameSpace()->begin();
      KSNamespace::Iterator end = it.data()->classValue()->nameSpace()->end();
      for( ; it2 != end; ++it2 )
	printf("  %s = %s\n",it2.key().ascii(),it2.data()->toString().ascii() );
    }
    } */

  // Clear the namespace
  // m_space.clear();
  // m_globalContext.setValue( 0 );

  /* Not needed any more, or ?
  if ( m_globalContext.exception() )
    m_globalContext.exception()->print();
  m_globalContext.setException( 0 );
  */

  KSException* ex = m_globalContext.shareException();
  m_globalContext.setException( 0 );
  if ( ex )
  {
    result.setException( ex );
    return false;
  }

  // Did we just execute a file ? -> Done
  if ( name.isEmpty() )
    return true;

  m_modules.insert( name, module );

  return true;
}

bool KSInterpreter::processExtension( KSContext& context, KSParseNode* node )
{
  QString tmp( "The interpreter does not support an extended syntax you are using.");
  context.setException( new KSException( "UnsupportedSyntaxExtension", tmp, node->getLineNo() ) );

  return false;
}

KRegExp* KSInterpreter::regexp()
{
    return &m_regexp;
}
