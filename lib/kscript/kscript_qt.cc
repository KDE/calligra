#include "kscript_qt.h"
#include "kscript_qstructs.h"
#include "kscript_value.h"
#include "kscript_util.h"
#include "kscript.h"
#include "kscript_object.h"

KSModule::Ptr ksCreateModule_Qt( KSInterpreter* interp )
{
  KSModule::Ptr module = new KSModule( interp, "qt" );

  // Add all Qt classes to the module
  module->addObject( "QRect", new KSValue( new KSQt::Rect( module, "QRect" ) ) );
  module->addObject( "QPoint", new KSValue( new KSQt::Point( module, "QPoint" ) ) );
  module->addObject( "QSize", new KSValue( new KSQt::Size( module, "QSize" ) ) );

  return module;
}

