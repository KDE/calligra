#ifndef __KSCRIPT_EXT_QWIDGET_H__
#define __KSCRIPT_EXT_QWIDGET_H__

#include "kscript_class.h"
#include "kscript_ext_qt.h"

class KSContext;

class KSClass_QWidget : public KSScriptClass
{
public:
  KSClass_QWidget( KSModule* );

  virtual bool isBuiltin() { return true; }

protected:
  virtual KSScriptObject* createObject( KSClass* c );
};

class KSObject_QWidget : public KS_Qt_Object
{
public:
  KSObject_QWidget( KSClass* );

  bool ksQWidget( KSContext& );
  bool ksQWidget_show( KSContext& );
  bool ksQWidget_delete( KSContext& );

  KSValue::Ptr member( KSContext& context, const QString& name );
  bool setMember( KSContext& context, const QString& name, KSValue* v );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QWidget" ) == 0 || KS_Qt_Object::inherits( name ) ); }
};

#endif
