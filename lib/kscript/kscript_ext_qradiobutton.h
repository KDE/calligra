#ifndef __KSCRIPT_EXT_QRADIOBUTTON_H__
#define __KSCRIPT_EXT_QRADIOBUTTON_H__

#include "kscript_class.h"
#include "kscript_ext_qbutton.h"

class KSContext;

class KSClass_QRadioButton : public KSClass_QButton
{
public:
  KSClass_QRadioButton( KSModule*, const char* name = "QRadioButton" );

  virtual bool isBuiltin() { return true; }

protected:
  virtual KSScriptObject* createObject( KSClass* c );
};

class KSObject_QRadioButton : public KSObject_QButton
{
public:
  KSObject_QRadioButton( KSClass* );

  bool ksQRadioButton( KSContext& );

  KSValue::Ptr member( KSContext& context, const QString& name );
  bool setMember( KSContext& context, const QString& name, const KSValue::Ptr& v );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QRadioButton" ) == 0 || KSObject_QButton::inherits( name )); }
};

#endif
