
// NOT YET FUNCTIONAL, ONLY FOR TESTING

#ifndef __KSCRIPT_EXT_QRADIOBUTTON_H__
#define __KSCRIPT_EXT_QRADIOBUTTON_H__

#include "kscript_class.h"
// #include "kscript_ext_qt.h"
#include "kscript_ext_qwidget.h"

// does it make sense to have a QButton class in KScript, so that
// things like QPushButton and QRadioButton aren't derived from
// QWidget directly? Hm.

class KSContext;

class KSClass_QRadioButton : public KSScriptClass
{
public:
  KSClass_QRadioButton( KSModule* );

  virtual bool isBuiltin() { return true; }

protected:
  virtual KSScriptObject* createObject( KSClass* c );
};

class KSObject_QRadioButton : public KSObject_QWidget
{
public:
  KSObject_QRadioButton( KSClass* );

  bool ksQRadioButton( KSContext& );
  bool ksQRadioButton_show( KSContext& );
  bool ksQRadioButton_delete( KSContext& );

  KSValue::Ptr member( KSContext& context, const QString& name );
  bool setMember( KSContext& context, const QString& name, KSValue* v );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QRadioButton" ) == 0 || KSObject_QWidget::inherits( name )); } 
};

#endif
