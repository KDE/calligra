#ifndef __KSCRIPT_EXT_QBUTTONGROUP_H__
#define __KSCRIPT_EXT_QBUTTONGROUP_H__

#include "kscript_class.h"
#include "kscript_ext_qwidget.h"

class KSContext;

class KSClass_QButtonGroup : public KSClass_QWidget
{
public:
  KSClass_QButtonGroup( KSModule*, const char* name = "QButtonGroup" );

protected:
  virtual KSScriptObject* createObject( KSClass* c );
};

class KSObject_QButtonGroup : public KSObject_QWidget
{
public:
  KSObject_QButtonGroup( KSClass* );

  bool ksQButtonGroup( KSContext& );
  bool ksQButtonGroup_insert( KSContext& );
  bool ksQButtonGroup_remove( KSContext& );
  bool ksQButtonGroup_clicked( KSContext& );
    
  KSValue::Ptr member( KSContext& context, const QString& name );
  bool setMember( KSContext& context, const QString& name, const KSValue::Ptr& v );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QButtonGroup" ) == 0 || KSObject_QWidget::inherits( name ) ); }
  
  void setObject( QObject* obj );
};

#endif
