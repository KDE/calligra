#ifndef __KSCRIPT_EXT_QLINEEDIT_H__
#define __KSCRIPT_EXT_QLINEEDIT_H__

#include "kscript_class.h"
#include "kscript_ext_qwidget.h"

class KSContext;

class QLineEdit;

class KSClass_QLineEdit : public KSClass_QWidget
{
public:
  KSClass_QLineEdit( KSModule*, const char* name = "QLineEdit" );

protected:
  virtual KSScriptObject* createObject( KSClass* c );
};

class KSObject_QLineEdit : public KSObject_QWidget
{
public:
  KSObject_QLineEdit( KSClass* );

  bool ksQLineEdit( KSContext& );
  bool ksQLineEdit_setText( KSContext& );
  bool ksQLineEdit_textChanged( KSContext& );

  KSValue::Ptr member( KSContext& context, const QString& name );
  bool setMember( KSContext& context, const QString& name, const KSValue::Ptr& v );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QLineEdit" ) == 0 || KSObject_QWidget::inherits( name ) ); }

  void setObject( QObject* obj );

  static QLineEdit* convert( KSValue* v ) { return (QLineEdit*) ((KS_Qt_Object*)v->objectValue())->object(); }
};

#endif
