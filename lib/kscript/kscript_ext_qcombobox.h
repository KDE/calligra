#ifndef __KSCRIPT_EXT_QCOMBOBOX_H__
#define __KSCRIPT_EXT_QCOMBOBOX_H__

#include "kscript_class.h"
#include "kscript_ext_qwidget.h"

class QComboBox;
class KSContext;

class KSClass_QComboBox : public KSClass_QWidget
{
public:
  KSClass_QComboBox( KSModule*, const char* name = "QComboBox" );

protected:
  virtual KSScriptObject* createObject( KSClass* c );
};

class KSObject_QComboBox : public KSObject_QWidget
{
public:
  KSObject_QComboBox( KSClass* );

  bool ksQComboBox( KSContext& );
  bool ksQComboBox_activated( KSContext& );
  bool ksQComboBox_insertItem( KSContext& );
    
  KSValue::Ptr member( KSContext& context, const QString& name );
  bool setMember( KSContext& context, const QString& name, const KSValue::Ptr& v );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QComboBox" ) == 0 || KSObject_QWidget::inherits( name ) ); }

  static QComboBox* convert( KSValue* v ) { return (QComboBox*) ((KS_Qt_Object*)v->objectValue())->object(); }

  void setObject( QObject* obj );
};

#endif
