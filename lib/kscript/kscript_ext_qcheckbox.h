#ifndef __KSCRIPT_EXT_QCHECKBOX_H__
#define __KSCRIPT_EXT_QCHECKBOX_H__

#include "kscript_class.h"
#include "kscript_ext_qbutton.h"

class KSContext;

class KSClass_QCheckBox : public KSClass_QButton
{
public:
  KSClass_QCheckBox( KSModule*, const char* name = "QCheckBox" );

  virtual bool isBuiltin() { return true; }

protected:
  virtual KSScriptObject* createObject( KSClass* c );
};

class KSObject_QCheckBox : public KSObject_QButton
{
public:
  KSObject_QCheckBox( KSClass* );

  bool ksQCheckBox( KSContext& );

  KSValue::Ptr member( KSContext& context, const QString& name );
  bool setMember( KSContext& context, const QString& name, const KSValue::Ptr& v );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QCheckBox" ) == 0 || KSObject_QButton::inherits( name ) ); }
};

#endif
