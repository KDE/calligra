#ifndef __KSCRIPT_EXT_QBUTTON_H__
#define __KSCRIPT_EXT_QBUTTON_H__

#include "kscript_class.h"
#include "kscript_ext_qwidget.h"

class QButton;
class KSContext;

class KSClass_QButton : public KSClass_QWidget
{
public:
  KSClass_QButton( KSModule*, const char* name = "QButton" );

protected:
  virtual KSScriptObject* createObject( KSClass* c );
};

class KSObject_QButton : public KSObject_QWidget
{
public:
  KSObject_QButton( KSClass* );

  bool ksQButton( KSContext& );
  bool ksQButton_setText( KSContext& );
  bool ksQButton_clicked( KSContext& );
    
  KSValue::Ptr member( KSContext& context, const QString& name );
  bool setMember( KSContext& context, const QString& name, const KSValue::Ptr& v );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QButton" ) == 0 || KSObject_QWidget::inherits( name ) ); }

  static QButton* convert( KSValue* v ) { return (QButton*) ((KS_Qt_Object*)v->objectValue())->object(); }

  void setObject( QObject* obj );
};

#endif
