#ifndef __KSCRIPT_EXT_QDIALOG_H__
#define __KSCRIPT_EXT_QDIALOG_H__

#include "kscript_class.h"
#include "kscript_ext_qwidget.h"

class KSContext;

class KSClass_QDialog : public KSClass_QWidget
{
public:
  KSClass_QDialog( KSModule*, const char* name = "QDialog" );

protected:
  virtual KSScriptObject* createObject( KSClass* c );
};

class KSObject_QDialog : public KSObject_QWidget
{
public:
  KSObject_QDialog( KSClass* );

  bool ksQDialog( KSContext& );
  bool ksQDialog_show( KSContext& );
  bool ksQDialog_hide( KSContext& );

  KSValue::Ptr member( KSContext& context, const QString& name );
  bool setMember( KSContext& context, const QString& name, const KSValue::Ptr& v );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QDialog" ) == 0 || KSObject_QWidget::inherits( name ) ); }

  static QDialog* convert( KSValue* v ) { return (QDialog*) ((KS_Qt_Object*)v->objectValue())->object(); }

  void setObject( QObject* obj );
};

#endif
