#ifndef __KSCRIPT_EXT_QLISTBOX_H__
#define __KSCRIPT_EXT_QLISTBOX_H__

#include "kscript_class.h"
#include "kscript_ext_qwidget.h"

class QListBox;
class KSContext;

class KSClass_QListBox : public KSClass_QWidget
{
public:
  KSClass_QListBox( KSModule*, const char* name = "QListBox" );

protected:
  virtual KSScriptObject* createObject( KSClass* c );
};

class KSObject_QListBox : public KSObject_QWidget
{
public:
  KSObject_QListBox( KSClass* );

  bool ksQListBox( KSContext& );
  bool ksQListBox_selected( KSContext& );
  bool ksQListBox_insertItem( KSContext& );
  bool ksQListBox_clear( KSContext & );

  KSValue::Ptr member( KSContext& context, const QString& name );
  bool setMember( KSContext& context, const QString& name, const KSValue::Ptr& v );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QListBox" ) == 0 || KSObject_QWidget::inherits( name ) ); }

  static QListBox* convert( KSValue* v ) { return (QListBox*) ((KS_Qt_Object*)v->objectValue())->object(); }

  void setObject( QObject* obj );
};

#endif
