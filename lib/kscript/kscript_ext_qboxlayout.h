#ifndef __KSCRIPT_EXT_QBOXLAYOUT_H__
#define __KSCRIPT_EXT_QBOXLAYOUT_H__

#include "kscript_class.h"
#include "kscript_ext_qwidget.h"

class KSContext;

class QBoxLayout;

class KSClass_QBoxLayout : public KSClass_QObject
{
public:
  KSClass_QBoxLayout( KSModule*, const char* name = "QBoxLayout" );

protected:
  virtual KSScriptObject* createObject( KSClass* c );
};

class KSObject_QBoxLayout : public KS_Qt_Object
{
public:
  KSObject_QBoxLayout( KSClass* );

  bool ksQBoxLayout( KSContext& );
  bool ksQBoxLayout_addWidget( KSContext& );
  bool ksQBoxLayout_addLayout( KSContext& );

  KSValue::Ptr member( KSContext& context, const QString& name );
  bool setMember( KSContext& context, const QString& name, const KSValue::Ptr& v );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QBoxLayout" ) == 0 || KS_Qt_Object::inherits( name ) ); }

  static QBoxLayout* convert( KSValue* v ) { return (QBoxLayout*) ((KS_Qt_Object*)v->objectValue())->object(); }
};

#endif
