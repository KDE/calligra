#ifndef __KSCRIPT_VALUE_H
#define __KSCRIPT_VALUE_H

#include <qstring.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <qdatetime.h>

#include <ksharedptr.h>

#include "koscript_types.h"

class KSFunction;
class KSMethod;
class KSContext;
class KSProperty;
class KSModule;
class KSStruct;
class KSStructClass;

typedef bool (KSStruct::*KSStructBuiltinMethod)( KSContext&, const QString& );

/**
 * This class acts like a union. It can hold one value at the
 * time and it can hold the most common types.
 * For CORBA people: It is a poor mans CORBA::Any.
 */
class KSValue : public KShared
{
public:
    typedef KSharedPtr<KSValue> Ptr;

    enum Type {
      Empty,
      StringType,
      IntType,
      BoolType,
      DoubleType,
      ListType,
      MapType,
      CharType,
      CharRefType,
      FunctionType,
      MethodType,
      PropertyType,
      ModuleType,
      StructType,
      StructClassType,
      StructBuiltinMethodType,
      DateType,
      TimeType,
      NTypes
    };

    enum Mode {
      LeftExpr,
      Constant,
      Temp
    };

    KSValue();
    KSValue( Type );
    KSValue( const KSValue& );
    virtual ~KSValue();

    KSValue( const QString& _v ) : KShared() { m_mode = Temp; typ = Empty; setValue( _v ); }
    KSValue( const QValueList<Ptr>& _v ) : KShared() { m_mode = Temp; typ = Empty; setValue( _v ); }
    KSValue( const QMap<QString,Ptr>& _v ) : KShared() { m_mode = Temp; typ = Empty; setValue( _v ); }
    KSValue( KScript::Long _v ) : KShared() { m_mode = Temp; typ = Empty; setValue( _v ); }
    KSValue( int _v ) : KShared() { m_mode = Temp; typ = Empty; setValue( (KScript::Long)_v ); }
    KSValue( KScript::Boolean _v ) : KShared() { m_mode = Temp; typ = Empty; setValue( _v ); }
    KSValue( KScript::Double _v ) : KShared() { m_mode = Temp; typ = Empty; setValue( _v ); }
    KSValue( const KScript::Char& _v ) : KShared() { m_mode = Temp; typ = Empty; setValue( _v ); }
    KSValue( const KScript::CharRef& _v ) : KShared() { m_mode = Temp; typ = Empty; setValue( _v ); }
    KSValue( KSFunction* _v ) : KShared() { m_mode = Temp; typ = Empty; setValue( _v ); }
    KSValue( KSMethod* _v ) : KShared() { m_mode = Temp; typ = Empty; setValue( _v ); }
    KSValue( KSProperty* _v ) : KShared() { m_mode = Temp; typ = Empty; setValue( _v ); }
    KSValue( KSModule* _v ) : KShared() { m_mode = Temp; typ = Empty; setValue( _v ); }
    KSValue( KSStruct* _v ) : KShared() { m_mode = Temp; typ = Empty; setValue( _v ); }
    KSValue( KSStructClass* _v ) : KShared() { m_mode = Temp; typ = Empty; setValue( _v ); }
    KSValue( KSStructBuiltinMethod _v ) : KShared() { m_mode = Temp; typ = Empty; setValue( _v ); }
    KSValue( const QTime& t ) : KShared() { m_mode = Temp; typ = Empty; setValue( t ); }
    KSValue( const QDate& d ) : KShared() { m_mode = Temp; typ = Empty; setValue( d ); }

    KSValue& operator= ( const KSValue& );

    void setValue( const QString& );
    void setValue( const QValueList<Ptr>& );
    void setValue( const QMap<QString,Ptr>& );
    void setValue( int _v ) { setValue( (KScript::Long)_v ); }
    void setValue( KScript::Long );
    void setValue( KScript::Boolean );
    void setValue( KScript::Double );
    void setValue( const KScript::Char& );
    void setValue( const KScript::CharRef& );
    void setValue( KSFunction* );
    void setValue( KSMethod* );
    void setValue( KSProperty* );
    void setValue( KSModule* );
    void setValue( KSStruct* );
    void setValue( KSStructClass* );
    void setValue( KSStructBuiltinMethod );
    void setValue( const QDate& );
    void setValue( const QTime& );

    void suck( KSValue* );

    Mode mode() const { return m_mode; }
    void setMode( Mode m ) { m_mode = m; }

    Type type() const { return typ; }
    virtual QString typeName() const;

    bool isEmpty() const { return ( typ == Empty ); }

    const QDate& dateValue() const { ASSERT( typ == DateType ); return *((QDate*)val.ptr); }
    QDate& dateValue() { ASSERT( typ == DateType ); return *((QDate*)val.ptr); }

    const QTime& timeValue() const { ASSERT( typ == TimeType ); return *((QTime*)val.ptr); }
    QTime& timeValue() { ASSERT( typ == TimeType ); return *((QTime*)val.ptr); }

    const QString& stringValue() const { ASSERT( typ == StringType ); return *((QString*)val.ptr); }
    QString& stringValue() { ASSERT( typ == StringType ); return *((QString*)val.ptr); }
    const QValueList<Ptr>& listValue() const { ASSERT( typ == ListType );  return *((QValueList<Ptr>*)val.ptr); }
    QValueList<Ptr>& listValue() { ASSERT( typ == ListType );  return *((QValueList<Ptr>*)val.ptr); }
    const QMap<QString,Ptr>& mapValue() const { ASSERT( typ == MapType ); return *((QMap<QString,Ptr>*)val.ptr); }
    QMap<QString,Ptr>& mapValue() { ASSERT( typ == MapType ); return *((QMap<QString,Ptr>*)val.ptr); }
    KScript::Long intValue() const { ASSERT( typ == IntType || typ == DoubleType ); if ( typ == IntType ) return val.i; return (int)val.d; }
    KScript::Boolean boolValue() const { ASSERT( typ == BoolType || typ == StringType ); if ( typ == BoolType ) return val.b;
                                         return !stringValue().isEmpty(); }
    KScript::Double doubleValue() const { ASSERT( typ == DoubleType || typ == IntType ); if ( typ == DoubleType ) return val.d;
                                          return (double)val.i; }
    KScript::Char charValue() const { if ( typ == CharRefType ) return *((KScript::CharRef*)val.ptr);
                                            ASSERT( typ == CharType ); return QChar( val.c ); }
    KScript::CharRef& charRefValue() { ASSERT( typ == CharRefType ); return *((KScript::CharRef*)val.ptr); }
    const KScript::CharRef& charRefValue() const { ASSERT( typ == CharRefType ); return *((KScript::CharRef*)val.ptr); }
    KSFunction* functionValue() { ASSERT( typ == FunctionType ); return ((KSFunction*)val.ptr); }
    const KSFunction* functionValue() const { ASSERT( typ == FunctionType ); return ((KSFunction*)val.ptr); }
    KSMethod* methodValue() { ASSERT( typ == MethodType ); return ((KSMethod*)val.ptr); }
    const KSMethod* methodValue() const { ASSERT( typ == MethodType ); return ((KSMethod*)val.ptr); }
    KSProperty* propertyValue() { ASSERT( typ == PropertyType ); return ((KSProperty*)val.ptr); }
    const KSProperty* propertyValue() const { ASSERT( typ == PropertyType ); return ((KSProperty*)val.ptr); }
    KSModule* moduleValue() { ASSERT( typ == ModuleType ); return ((KSModule*)val.ptr); }
    const KSModule* moduleValue() const { ASSERT( typ == ModuleType ); return ((KSModule*)val.ptr); }
    KSStructClass* structClassValue() { ASSERT( typ == StructClassType ); return ((KSStructClass*)val.ptr); }
    const KSStructClass* structClassValue() const { ASSERT( typ == StructClassType ); return ((KSStructClass*)val.ptr); }
    KSStruct* structValue() { ASSERT( typ == StructType ); return ((KSStruct*)val.ptr); }
    const KSStruct* structValue() const { ASSERT( typ == StructType ); return ((KSStruct*)val.ptr); }
    KSStructBuiltinMethod structBuiltinMethodValue() { ASSERT( typ == StructBuiltinMethodType ); return val.sm; }

    /**
     * DO NOT USE ANY MORE.
     */
    bool cast( Type );

    /**
     * @return TRUE if the stored value is of type @p typ or can be implicit
     *         casted to that type.
     */
    bool implicitCast( Type typ ) const;

    QString toString( KSContext& context );

    bool operator==( const KSValue& v ) const;

    bool cmp( const KSValue& v ) const;

    /**
     * Frees all data allocated by this KSValue.
     */
    void clear();

    static QString typeToName( Type _typ );
    /**
     * @return KSValue::Empty if the given name is empty or unknown.
     */
    static Type nameToType( const QString& _name );

    /**
     * @return an empty value. Its reference count is increased so that you can assign
     *         it directly to some @ref KSContext.
     */
    static KSValue* null() { if ( !s_null ) s_null = new KSValue; s_null->_KShared_ref(); return s_null; }

protected:

    Mode m_mode;
    Type typ;
    union
    {
	KScript::Long i;
	KScript::Boolean b;
	KScript::Double d;
	ushort c;
	void *ptr;
	KSStructBuiltinMethod sm;
	QDate* date;
	QTime* time;
    } val;

private:
    static void initTypeNameMap();
    static KSValue* s_null;
};

#endif

