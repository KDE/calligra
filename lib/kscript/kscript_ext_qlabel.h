
// NOT YET FUNCTIONAL, ONLY FOR TESTING

#ifndef __KSCRIPT_EXT_QLABEL_H__
#define __KSCRIPT_EXT_QLABEL_H__

#include "kscript_class.h"
// #include "kscript_ext_qt.h"
#include "kscript_ext_qwidget.h"

class KSContext;

class KSClass_QLabel : public KSScriptClass
{
public:
  KSClass_QLabel( KSModule* );

  virtual bool isBuiltin() { return true; }

protected:
  virtual KSScriptObject* createObject( KSClass* c );
};

class KSObject_QLabel : public KSObject_QWidget
{
public:
  KSObject_QLabel( KSClass* );

  bool ksQLabel( KSContext& );
  bool ksQLabel_show( KSContext& );
  bool ksQLabel_delete( KSContext& );

  KSValue::Ptr member( KSContext& context, const QString& name );
  bool setMember( KSContext& context, const QString& name, KSValue* v );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QLabel" ) == 0 || KSObject_QWidget::inherits( name ) ); }
};

#endif
