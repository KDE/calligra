#ifndef __KSCRIPT_EXT_QPUSHBUTTON_H__
#define __KSCRIPT_EXT_QPUSHBUTTON_H__

#include "kscript_class.h"
#include "kscript_ext_qbutton.h"

class QPushButton;
class KSContext;

class KSClass_QPushButton : public KSClass_QButton
{
public:
  KSClass_QPushButton( KSModule*, const char* name = "QPushButton" );

protected:
  virtual KSScriptObject* createObject( KSClass* c );
};

class KSObject_QPushButton : public KSObject_QButton
{
public:
  KSObject_QPushButton( KSClass* );

  bool ksQPushButton( KSContext& );

  KSValue::Ptr member( KSContext& context, const QString& name );
  bool setMember( KSContext& context, const QString& name, const KSValue::Ptr& v );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QPushButton" ) == 0 || KSObject_QButton::inherits( name ) ); }

  static QPushButton* convert( KSValue* v ) { return (QPushButton*) ((KS_Qt_Object*)v->objectValue())->object(); }

  void setObject( QObject* obj );
};

#endif
