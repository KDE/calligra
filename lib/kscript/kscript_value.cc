#include "kscript_value.h"
#include "kscript_func.h"
#include "kscript_class.h"
#include "kscript_object.h"
#include "kscript_context.h"
#include "kscript_struct.h"
#include "kscript_proxy.h"
#include "kscript_interface.h"
#include "kscript_typecode.h"

KSValue* KSValue::s_null = 0;

KSValue::KSValue()
{
  typ = Empty;
  m_mode = Temp;
}

KSValue::KSValue( Type _type )
{
  typ = _type;
  m_mode = Temp;

  switch( typ )
    {
    case StringType:
      val.ptr = new QString;
      break;
    case ListType:
      val.ptr = new QValueList<Ptr>;
      break;
    case MapType:
      val.ptr = new QMap<QString,Ptr>;
      break;
    case CharRefType:
      val.ptr = new KScript::CharRef( 0, 0 );
      break;
    case CharType:
      val.c = 0;
      break;
    case FunctionType:
    case ClassType:
    case ObjectType:
    case MethodType:
    case PropertyType:
    case ModuleType:
    case StructType:
    case StructClassType:
    case TypeCodeType:
    case ProxyType:
    case InterfaceType:
    case AttributeType:
      val.ptr = 0;
      break;
    case BuiltinMethodType:
      val.m = 0;
      break;
    case ProxyBuiltinMethodType:
      val.pm = 0;
      break;
    case StructBuiltinMethodType:
      val.sm = 0;
      break;
    case IntType:
    case BoolType:
    case DoubleType:
    case Empty:
      // Do nothing
      break;
    case NTypes:
      ASSERT( 0 );
    }
}

KSValue::KSValue( const KSValue& p ) : QShared()
{
  typ = Empty;
  *this = p;
}

KSValue::~KSValue()
{
    clear();
}

KSValue& KSValue::operator= ( const KSValue& p )
{
  clear();

  switch( p.type() )
    {
    case Empty:
      break;
    case StringType:
      val.ptr = new QString( p.stringValue() );
      break;
    case ListType:
      val.ptr = new QValueList<Ptr>( p.listValue() );
      break;
    case MapType:
      val.ptr = new QMap<QString,Ptr>( p.mapValue() );
      break;
    case IntType:
      val.i = p.intValue();
      break;
    case BoolType:
      val.b = p.boolValue();
      break;
    case DoubleType:
      val.d = p.doubleValue();
      break;
    case CharType:
      val.c = p.charValue().unicode();
      break;
    case CharRefType:
      val.ptr = new KScript::CharRef( p.charRefValue() );
      break;
    case BuiltinMethodType:
      val.m = p.val.m;
      break;
    case StructBuiltinMethodType:
      val.sm = p.val.sm;
      break;
    case ProxyBuiltinMethodType:
      val.pm = p.val.pm;
      break;
    case FunctionType:
    case ClassType:
    case ObjectType:
    case MethodType:
    case PropertyType:
    case ModuleType:
    case StructClassType:
    case TypeCodeType:
    case ProxyType:
    case InterfaceType:
    case AttributeType:
      val.ptr = p.val.ptr;
      ((QShared*)val.ptr)->ref();
      break;
    case StructType:
      val.ptr = ((KSStruct*)p.val.ptr)->clone();
      break;
    case NTypes:
      ASSERT( 0 );
    }

  typ = p.type();
  m_mode = p.mode();

  return *this;
}

QString KSValue::typeName() const
{
  return typeToName( typ );
}

void KSValue::setValue( const QString& _value )
{
  clear();
  typ = StringType;
  val.ptr = new QString( _value );
}

void KSValue::setValue( const QValueList<Ptr>& _value )
{
  clear();
  typ = ListType;
  val.ptr = new QValueList<Ptr>( _value );
}

void KSValue::setValue( const QMap<QString,Ptr>& _value )
{
  clear();
  typ = MapType;
  val.ptr = new QMap<QString,Ptr>( _value );
}

void KSValue::setValue( KScript::Long _value )
{
  clear();
  typ = IntType;
  val.i = _value;
}

void KSValue::setValue( KScript::Boolean _value )
{
  clear();
  typ = BoolType;
  val.b = _value;
}

void KSValue::setValue( KScript::Double _value )
{
  clear();
  typ = DoubleType;
  val.d = _value;
}

void KSValue::setValue( const KScript::Char& _value )
{
  clear();
  typ = CharType;
  val.c = _value.unicode();
}

void KSValue::setValue( const KScript::CharRef& _value )
{
  clear();
  typ = CharRefType;
  val.ptr = new KScript::CharRef( _value );
}

void KSValue::setValue( KSFunction* _value )
{
  clear();
  typ = FunctionType;
  // Do not call ref() since we take over ownership
  val.ptr = _value;
}

void KSValue::setValue( KSClass* _value )
{
  clear();
  typ = ClassType;
  // Do not call ref() since we take over ownership
  val.ptr = _value;
}

void KSValue::setValue( KSObject* _value )
{
  clear();
  typ = ObjectType;
  // Do not call ref() since we take over ownership
  val.ptr = _value;
}

void KSValue::setValue( KSMethod* _value )
{
  clear();
  typ = MethodType;
  // Do not call ref() since we take over ownership
  val.ptr = _value;
}

void KSValue::setValue( KSBuiltinMethod _value )
{
  clear();
  typ = BuiltinMethodType;
  val.m = _value;
}

void KSValue::setValue( KSProperty* _value )
{
  clear();
  typ = PropertyType;
  // Do not call ref() since we take over ownership
  val.ptr = _value;
}

void KSValue::setValue( KSModule* _value )
{
  clear();
  typ = ModuleType;
  // Do not call ref() since we take over ownership
  val.ptr = _value;
}

void KSValue::setValue( KSStruct* _value )
{
  clear();
  typ = StructType;
  // Do not call ref() since we take over ownership
  val.ptr = _value;
}

void KSValue::setValue( KSStructClass* _value )
{
  clear();
  typ = StructClassType;
  // Do not call ref() since we take over ownership
  val.ptr = _value;
}

void KSValue::setValue( KSStructBuiltinMethod _value )
{
  clear();
  typ = StructBuiltinMethodType;
  val.sm = _value;
}

void KSValue::setValue( KSProxy* _value )
{
  clear();
  typ = ProxyType;
  // Do not call ref() since we take over ownership
  val.ptr = _value;
}

void KSValue::setValue( KSInterface* _value )
{
  clear();
  typ = InterfaceType;
  // Do not call ref() since we take over ownership
  val.ptr = _value;
}

void KSValue::setValue( KSTypeCode* _value )
{
  clear();
  typ = TypeCodeType;
  // Do not call ref() since we take over ownership
  val.ptr = _value;
}

void KSValue::setValue( KSAttribute* _value )
{
  clear();
  typ = AttributeType;
  // Do not call ref() since we take over ownership
  val.ptr = _value;
}

void KSValue::setValue( KSProxyBuiltinMethod _value )
{
  clear();
  typ = ProxyBuiltinMethodType;
  val.pm = _value;
}

void KSValue::clear()
{
  switch( typ )
    {
    case Empty:
    case IntType:
    case BoolType:
    case DoubleType:
    case CharType:
    case BuiltinMethodType:
    case StructBuiltinMethodType:
    case ProxyBuiltinMethodType:
      break;
    case TypeCodeType:
#if 0
	if ( val.ptr )
	if ( typeCodeValue()->deref() )
	  delete ((KSTypeCode*)val.ptr);
#endif
      break;
    case ProxyType:
#if 0
	if ( val.ptr )
	if ( proxyValue()->deref() )
	  delete ((KSProxy*)val.ptr);
#endif
      break;
    case InterfaceType:
#if 0
      if ( val.ptr )
	if ( interfaceValue()->deref() )
	  delete ((KSInterface*)val.ptr);
#endif
      break;
    case AttributeType:
#if 0
      if ( val.ptr )
	if ( attributeValue()->deref() )
	  delete ((KSAttribute*)val.ptr);
#endif
      break;
    case FunctionType:
      if ( val.ptr )
	if ( functionValue()->deref() )
	  delete ((KSFunction*)val.ptr);
      break;
    case PropertyType:
      if ( val.ptr )
	if ( propertyValue()->deref() )
	  delete ((KSProperty*)val.ptr);
      break;
    case ClassType:
      if ( val.ptr )
	if ( classValue()->deref() )
	  delete ((KSClass*)val.ptr);
      break;
    case ObjectType:
      if ( val.ptr )
	if ( objectValue()->deref() )
	  delete ((KSObject*)val.ptr);
      break;
    case MethodType:
      if ( val.ptr )
	if ( methodValue()->deref() )
	  delete ((KSMethod*)val.ptr);
      break;
    case ModuleType:
      if ( val.ptr )
	if ( moduleValue()->deref() )
	  delete ((KSModule*)val.ptr);
      break;
    case StructType:
      if ( val.ptr )
	if ( structValue()->deref() )
	  delete ((KSStruct*)val.ptr);
      break;
    case StructClassType:
      if ( val.ptr )
	if ( structClassValue()->deref() )
	  delete ((KSStructClass*)val.ptr);
      break;
    case StringType:
      delete (QString*)val.ptr;
      break;
    case ListType:
      delete (QValueList<Ptr>*)val.ptr;
      break;
    case MapType:
      delete (QMap<QString,Ptr>*)val.ptr;
      break;
    case CharRefType:
      delete (KScript::CharRef*)val.ptr;
      break;
    case NTypes:
      ASSERT(0);
      break;
    }

  typ = Empty;
}

static QString *typ_to_name = 0;

void KSValue::initTypeNameMap()
{
    if ( typ_to_name ) return;

    typ_to_name = new QString[(int)NTypes];

    typ_to_name[(int)Empty] = QString::fromLatin1("<none>");
    typ_to_name[(int)StringType] = QString::fromLatin1("String");
    typ_to_name[(int)IntType] = QString::fromLatin1("Integer");
    typ_to_name[(int)BoolType] = QString::fromLatin1("Boolean");
    typ_to_name[(int)DoubleType] = QString::fromLatin1("Double");
    typ_to_name[(int)ListType] = QString::fromLatin1("List");
    typ_to_name[(int)MapType] = QString::fromLatin1("Map");
    typ_to_name[(int)CharType] = QString::fromLatin1("Char");
    typ_to_name[(int)CharRefType] = QString::fromLatin1("Char");
    typ_to_name[(int)FunctionType] = QString::fromLatin1("Function");
    typ_to_name[(int)ClassType] = QString::fromLatin1("Class");
    typ_to_name[(int)ObjectType] = QString::fromLatin1("Object");
    typ_to_name[(int)MethodType] = QString::fromLatin1("Method");
    typ_to_name[(int)BuiltinMethodType] = QString::fromLatin1("BuiltinMethod");
    typ_to_name[(int)PropertyType] = QString::fromLatin1("Property");
    typ_to_name[(int)ModuleType] = QString::fromLatin1("Module");
    typ_to_name[(int)StructType] = QString::fromLatin1("Struct");
    typ_to_name[(int)StructClassType] = QString::fromLatin1("StructClass");
    typ_to_name[(int)StructBuiltinMethodType] = QString::fromLatin1("StructBuiltinMethod");
    typ_to_name[(int)ProxyType] = QString::fromLatin1("Proxy");
    typ_to_name[(int)InterfaceType] = QString::fromLatin1("Interface");
    typ_to_name[(int)ProxyBuiltinMethodType] = QString::fromLatin1("ProxyBuiltinMethod");
    typ_to_name[(int)TypeCodeType] = QString::fromLatin1("TypeCode");
    typ_to_name[(int)AttributeType] = QString::fromLatin1("Attribute");
}

QString KSValue::typeToName( KSValue::Type _typ )
{
    initTypeNameMap();
    return typ_to_name[_typ];
}

KSValue::Type KSValue::nameToType( const QString& _name )
{
    initTypeNameMap();

    int t = (int)NTypes;
    while ( t > (int)Empty && typ_to_name[(int)--t] != _name )
	;
    return Type(t);
}

bool KSValue::cast( Type _typ )
{
  if ( typ == _typ )
    return true;

  switch( typ )
    {
    case Empty:
      return false;
    case IntType:
      if ( _typ == DoubleType )
      {
	KScript::Double d = (KScript::Double)val.i;
	val.d = d;
	typ = _typ;
	return true;
      }
      return false;
    case BoolType:
      if ( _typ == StringType )
      {
	KScript::Boolean b = val.b;
	if ( b )
	  setValue( "TRUE" );
	else
	  setValue( "FALSE" );
	typ = _typ;
	return true;
      }
      break;
    case DoubleType:
      if ( _typ == IntType )
      {
	KScript::Long i = (KScript::Long)val.d;
	val.i = i;
	typ = _typ;
	return true;
      }
      return false;
    case StringType:
      return false;
    case CharRefType:
      if ( _typ != CharType )
	return false;
      typ = _typ;
      return true;
    case PropertyType:
      /* {
	KSValue* v = propertyValue()->object()->member( propertyValue()->name(), FALSE );
	if ( !v )
	  return false;
	if ( !v->cast( _typ ) )
	  return false;
	*this = *v;
      }
      break; */
    case ListType:
    case MapType:
    case CharType:
    case FunctionType:
    case ClassType:
    case ObjectType:
    case MethodType:
    case BuiltinMethodType:
    case StructBuiltinMethodType:
    case StructType:
    case StructClassType:
    case ModuleType:
    case TypeCodeType:
    case ProxyType:
    case InterfaceType:
    case AttributeType:
    case ProxyBuiltinMethodType:
      // They can be casted to nothing
      return false;
    case NTypes:
      ASSERT(0);
      break;
    }

  typ = _typ;

  return true;
}

QString KSValue::toString() const
{
  switch( typ )
    {
    case Empty:
      return QString( "<none>" );
      break;
    case FunctionType:
      return QString( "<function>" );
      break;
    case PropertyType:
      return QString( "<property>" );
      break;
    case ClassType:
      return ( QString( "<class " ) + classValue()->name() + ">" );
      break;
    case StructClassType:
      return ( QString( "<struct class " ) + structClassValue()->name() + ">" );
      break;
    case ModuleType:
      return ( QString( "<module " ) + moduleValue()->name() + ">" );
      break;
    case TypeCodeType:
      return QString( "<typecode>" );
      break;
    case ProxyType:
      return QString( "<proxy>" );
      break;
    case InterfaceType:
      return QString( "<interface>" );
      break;
    case AttributeType:
      return QString( "<attribute>" );
      break;
    case ProxyBuiltinMethodType:
      return QString( "<proxy builtin method>" );
      break;
    case ObjectType:
      {
	QString tmp( "{ Object %1 { " );
	tmp = tmp.arg( objectValue()->getClass()->name() );
	KSNamespace::ConstIterator it2 = objectValue()->instanceNameSpace()->begin();
	KSNamespace::ConstIterator end = objectValue()->instanceNameSpace()->end();
	for( ; it2 != end; ++it2 )
	{
	  QString s("( %1, %2 ), ");
	  s = s.arg( it2.key() ).arg( it2.data()->toString() );
	  tmp += s;
	}
	tmp += "} }";
	return tmp;
      }
      break;
    case StructType:
      {
	QString tmp( "{ Struct %1 { " );
	tmp = tmp.arg( structValue()->getClass()->name() );
	KSNamespace::ConstIterator it2 = structValue()->instanceNameSpace()->begin();
	KSNamespace::ConstIterator end = structValue()->instanceNameSpace()->end();
	for( ; it2 != end; ++it2 )
	{
	  QString s("( %1, %2 ), ");
	  s = s.arg( it2.key() ).arg( it2.data()->toString() );
	  tmp += s;
	}
	tmp += "} }";
	return tmp;
      }
      break;
    case MethodType:
      return QString( "<method>" );
      break;
    case BuiltinMethodType:
      return QString( "<builtin method>" );
      break;
    case StructBuiltinMethodType:
      return QString( "<struct builtin method>" );
      break;
    case IntType:
      {
	QString tmp;
	tmp.setNum( val.i );
	return tmp;
      }
      break;
    case BoolType:
      {
	if ( val.b )
	  return QString( "TRUE" );
	else
	  return QString( "FALSE" );
      }
      break;
    case DoubleType:
      {
	QString tmp;
	tmp.setNum( val.d );
	return tmp;
      }
      break;
    case StringType:
      return *((QString*)val.ptr);
      break;
    case ListType:
      {
	QString tmp( "[ " );
	const QValueList<Ptr>* lst = (QValueList<Ptr>*)val.ptr;
	QValueList<Ptr>::ConstIterator it = lst->begin();
	QValueList<Ptr>::ConstIterator end = lst->end();
	for( ; it != end; ++it )
	{
	  tmp += (*it)->toString();
	  tmp += ", ";
	}
	tmp += "]";
	return tmp;
      }
      break;
    case MapType:
      {
	QString tmp( "{ " );
	const QMap<QString,Ptr>* lst = (QMap<QString,Ptr>*)val.ptr;
	QMap<QString,Ptr>::ConstIterator it = lst->begin();
	QMap<QString,Ptr>::ConstIterator end = lst->end();
	for( ; it != end; ++it )
	{
	  tmp += "( ";
	  tmp += it.key();
	  tmp += ", ";
	  tmp += it.data()->toString();
	  tmp += " ), ";
	}
	tmp += "}";
	return tmp;
      }
      break;
    case CharRefType:
    case CharType:
      {
	QString tmp( "'%1'" );
	return tmp.arg( charValue() );
      }
      break;
    case NTypes:
      ASSERT(0);
      break;
    }

  // Never reached
  return QString::null;
}

void KSValue::suck( KSValue* v )
{ 
  if ( v->mode() != Temp )
  {
    *this = *v;
    return;
  }

  clear();
  
  typ = v->type();
  val = v->val;

  v->typ = Empty;
}

bool KSValue::operator==( const KSValue& v ) const
{
  return ( val.ptr == v.val.ptr && typ == v.typ );
}

bool KSValue::cmp( const KSValue& v ) const
{
  if ( typ != v.typ )
    return false;

  switch( typ )
    {
    case Empty:
      return true;
    case StringType:
      return ( stringValue() == v.stringValue() );
    case KSValue::IntType:
      return ( val.i == v.val.i );
    case BoolType:
      return ( val.b == v.val.b );
    case DoubleType:
      return ( val.d == v.val.d );
    case ListType:
      return ( listValue() == v.listValue() );
    case MapType:
      {
	QMap<QString,KSValue::Ptr>::ConstIterator it, it2, end, end2;
	it = mapValue().begin();
	it2 = v.mapValue().begin();
	end = mapValue().end();
	end2 = v.mapValue().end();
	while( it != end && it2 != end2 )
	{
	  if ( it.key() != it2.key() || !it2.data()->cmp( *it.data() ) )
	    return false;
	  ++it;
	  ++it2;
	}
	return ( it == end && it2 == end2 );
      }
    case CharType:
      return ( val.c == v.val.c );
    case CharRefType:
      return ( ((KScript::Char)charRefValue()) == ((KScript::Char)v.charRefValue()) );
    case FunctionType:
    case ClassType:
    case ObjectType:
    case MethodType:
    case PropertyType:
    case ModuleType:
    case StructType:
    case StructClassType:
    case ProxyType:
    case InterfaceType:
    case AttributeType:
    case TypeCodeType:
      return ( val.ptr == v.val.ptr );
    case BuiltinMethodType:
      return ( val.m == v.val.m );
    case StructBuiltinMethodType:
      return ( val.sm == v.val.sm );
    case ProxyBuiltinMethodType:
      return ( val.pm == v.val.pm );
    case NTypes:
      ASSERT( 0 );
    }

  // Never reached
  return false;
}
