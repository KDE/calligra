#ifndef __KSCRIPT_EXT_QHBOXLAYOUT_H__
#define __KSCRIPT_EXT_QHBOXLAYOUT_H__

#include "kscript_class.h"
#include "kscript_ext_qboxlayout.h"

class KSContext;

class QHBoxLayout;

class KSClass_QHBoxLayout : public KSClass_QBoxLayout
{
public:
  KSClass_QHBoxLayout( KSModule*, const char* name = "QHBoxLayout" );

protected:
  virtual KSScriptObject* createObject( KSClass* c );
};

class KSObject_QHBoxLayout : public KSObject_QBoxLayout
{
public:
  KSObject_QHBoxLayout( KSClass* );

  bool ksQHBoxLayout( KSContext& );

  KSValue::Ptr member( KSContext& context, const QString& name );
  bool setMember( KSContext& context, const QString& name, const KSValue::Ptr& v );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QHBoxLayout" ) == 0 || KSObject_QBoxLayout::inherits( name ) ); }

  static QHBoxLayout* convert( KSValue* v ) { return (QHBoxLayout*) ((KS_Qt_Object*)v->objectValue())->object(); }
};

#endif
