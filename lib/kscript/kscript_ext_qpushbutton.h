#ifndef __KSCRIPT_EXT_QPUSHBUTTON_H__
#define __KSCRIPT_EXT_QPUSHBUTTON_H__

#include "kscript_class.h"
#include "kscript_ext_qwidget.h"

class KSContext;

class KSClass_QPushButton : public KSClass_QWidget
{
public:
  KSClass_QPushButton( KSModule*, const char* name = "QPushButton" );

protected:
  virtual KSScriptObject* createObject( KSClass* c );
};

class KSObject_QPushButton : public KSObject_QWidget
{
public:
  KSObject_QPushButton( KSClass* );

  bool ksQPushButton( KSContext& );
  bool ksQPushButton_setText( KSContext& );
  bool ksQPushButton_clicked( KSContext& );
    
  KSValue::Ptr member( KSContext& context, const QString& name );
  bool setMember( KSContext& context, const QString& name, const KSValue::Ptr& v );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QPushButton" ) == 0 || KSObject_QWidget::inherits( name ) ); }
    
  void setObject( QObject* obj );
};

#endif
