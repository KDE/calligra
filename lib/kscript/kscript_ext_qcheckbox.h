
// NOT YET FUNCTIONAL, ONLY FOR TESTING

#ifndef __KSCRIPT_EXT_QCHECKBOX_H__
#define __KSCRIPT_EXT_QCHECKBOX_H__

#include "kscript_class.h"
// #include "kscript_ext_qt.h"
#include "kscript_ext_qwidget.h"

class KSContext;

class KSClass_QCheckBox : public KSScriptClass
{
public:
  KSClass_QCheckBox( KSModule* );

  virtual bool isBuiltin() { return true; }

protected:
  virtual KSScriptObject* createObject( KSClass* c );
};

class KSObject_QCheckBox : public KSObject_QWidget
{
public:
  KSObject_QCheckBox( KSClass* );

  bool ksQCheckBox( KSContext& );
  bool ksQCheckBox_show( KSContext& );
  bool ksQCheckBox_delete( KSContext& );

  KSValue::Ptr member( KSContext& context, const QString& name );
  bool setMember( KSContext& context, const QString& name, KSValue* v );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QCheckBox" ) == 0 || KSObject_QWidget::inherits( name ) ); }
};

#endif
