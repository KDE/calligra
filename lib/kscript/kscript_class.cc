#include "kscript_class.h"
#include "kscript_object.h"
#include "kscript_func.h"
#include "kscript_parsenode.h"

#include <klocale.h>

KSScriptObject* KSScriptClass::createObject( KSClass* c )
{
  return new KSScriptObject( c );
}

bool KSScriptClass::hasSignal( const QString& name )
{
    KSNamespace::Iterator it = m_space.begin();
    for( ; it != m_space.end(); ++it )
	if ( it.data()->type() == KSValue::FunctionType )
	    if ( it.data()->functionValue()->name() == name && it.data()->functionValue()->isSignal() )
		return true;

    QValueList<KSValue::Ptr>::Iterator it2 = m_superClasses.begin();
    for( ; it2 != m_superClasses.end(); ++it2 )
    {
	if ( (*it2)->classValue()->hasSignal( name ) )
	    return TRUE;
    }

    return false;
}

bool KSScriptClass::constructor( KSParseNode* node, KSContext& context )
{
  ASSERT( context.value() && context.value()->type() == KSValue::ListType );

  QValueList<KSValue::Ptr> super;
  allSuperClasses( super);
  QValueList<KSValue::Ptr>::Iterator sit = super.begin();
  KSClass* cl = 0;
  for( ; sit != super.end() && cl == 0; ++sit )
  {
    if ( (*sit)->classValue()->isBuiltin() )
    {
      if ( cl )
      {
	QString tmp( i18n("Multiple inheritance of builtin classes is not allowed.\nThe classes are %1 and %2.\n"));
	context.setException( new KSException( "CastingError", tmp.arg( cl->name() ).arg( (*sit)->classValue()->name() ), node->getLineNo() ) );
	return false;
      }
      cl = (*sit)->classValue();
    }
  }
  if ( !cl )
    cl = this;

  // Search constructor
  KSNamespace::Iterator it = m_space.find( m_name );
  if ( it == m_space.end() )
  {
    if ( !context.value()->listValue().isEmpty() )
    {
      QString tmp( i18n("Default constructor of class %1 does not take any arguments") );
      context.setException( new KSException( "CastingError", tmp.arg( m_name ), node->getLineNo() ) );
      return false;
    }

    context.setValue( new KSValue( cl->createObject( this ) ) );
    return true;
  }

  KSValue::Ptr v( new KSValue( cl->createObject( this ) ) );

  if ( it.data()->type() == KSValue::FunctionType )
  {
    context.value()->listValue().prepend( v );
    if ( !it.data()->functionValue()->call( context ) )
      return false;
  }
  else if ( it.data()->type() == KSValue::BuiltinMethodType )
  {
    if ( !( (v->objectValue()->*(it.data()->builtinMethodValue()))( context ) ) )
      return false;
  }
  else
    ASSERT( 0 );

  context.setValue( v );

  return true;
}

void KSClass::allSuperClasses( QValueList<KSValue::Ptr>& lst )
{
  QValueList<KSValue::Ptr>::Iterator it = m_superClasses.begin();
  for( ; it != m_superClasses.end(); ++it )
  {
    lst.append( *it );
    (*it)->classValue()->allSuperClasses( lst );
  }
}

KSValue::Ptr KSClass::member( KSContext& context, const QString& name )
{
  KSNamespace::Iterator it = m_space.find( name );
  if ( it == m_space.end() )
  {
    QString tmp( i18n("Unknown symbol '%1' in object of module '%2'") );
    context.setException( new KSException( "UnknownName", tmp.arg( name ).arg( m_name ) ) );
    return 0;
  }

  return it.data();
}

QString KSClass::fullName() const
{
    return ( m_module->name() + ":" + m_name );
}

bool KSClass::inherits( const QCString& name ) const
{
    qDebug("Comparing %s with %s", name.data(), fullName().latin1() );

    if ( fullName() == name.data() )
	return TRUE;

    QValueList<KSValue::Ptr>::ConstIterator it = m_superClasses.begin();
    for( ; it != m_superClasses.end(); ++it )
    {
	if ( (*it)->classValue()->inherits( name ) )
	    return TRUE;
    }

    return FALSE;
}
