#ifndef __KSCRIPT_EXT_QAPPLICATION_H__
#define __KSCRIPT_EXT_QAPPLICATION_H__

#include "kscript_class.h"
#include "kscript_ext_qt.h"

class KSContext;
class KSModule;

class KSClass_QApplication : public KSScriptClass
{
public:
  KSClass_QApplication( KSModule* );

protected:
  virtual KSScriptObject* createObject( KSClass* c );
};

class KSObject_QApplication : public KS_Qt_Object
{
public:
  KSObject_QApplication( KSClass* );

  bool ksQApplication( KSContext& );
  bool ksQApplication_exec( KSContext& );
  bool ksQApplication_delete( KSContext& );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QApplication" ) == 0 || KS_Qt_Object::inherits( name ) ); }
};

#endif
