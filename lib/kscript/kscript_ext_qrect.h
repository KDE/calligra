#ifndef __KSCRIPT_EXT_QRECT_H__
#define __KSCRIPT_EXT_QRECT_H__

#include "kscript_class.h"
#include "kscript_object.h"

#include <qrect.h>

class KSContext;
class KSModule;

class KSClass_QRect : public KSScriptClass
{
public:
  KSClass_QRect( KSModule* );

  virtual bool isBuiltin() { return true; }

  static KSClass_QRect* self() { return s_pSelf; }

protected:
  virtual KSScriptObject* createObject( KSClass* c );

private:
  static KSClass_QRect* s_pSelf;
};

class KSObject_QRect : public KSScriptObject
{
public:
  KSObject_QRect( KSClass* );
  KSObject_QRect( const QRect& );

  bool ksQRect( KSContext& );
  bool ksQRect_intersects( KSContext& context );
  bool ksQRect_unite( KSContext& context );

  KSValue::Ptr member( KSContext& context, const QString& name );
  bool setMember( KSContext& context, const QString& name, const KSValue::Ptr& v );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QRect" ) == 0 || KSScriptObject::inherits( name ) ); }

  static QRect* convert( const KSValue::Ptr& v ) { return &((KSObject_QRect*)v->objectValue())->m_rect; }

private:
  QRect m_rect;
};

#endif
