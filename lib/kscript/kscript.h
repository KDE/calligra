#ifndef __KSCRIPT_H__
#define __KSCRIPT_H__

#include <qstring.h>
#include <qmap.h>
#include <qshared.h>
#include <qstringlist.h>

#include "kscript_context.h"
#include "kscript_parsenode.h"
#include "kscript_ptr.h"

class KSInterpreter : public QShared
{
public:
  typedef KSSharedPtr<KSInterpreter> Ptr;

  KSInterpreter();

  /**
   * @return the stringified exception or an empty string if everything was ok.
   *         This is a convenience function for @ref #runModule.
   */
  QString runScript( const QString& filename );
  /**
   * @param context contains the initialized module as value and
   *        the exception in case of an error. Once the value ( read: the module )
   *        of "context" is deleted, you may no longer use the exception contained
   *        in "context". That means you may not extract the exception and destroy
   *        the KSContext object.
   */
  bool runModule( KSContext& context, const QString& name, const QString& filename );

  /**
   * This function is for convenience. It searches the module in the
   * search paths by appending a ".ks" suffix.
   */
  bool runModule( KSContext& context, const QString& name );
  /**
   * @return the pointer to an already loaded module or 0 if
   *         the module was not loaded successfully until now.
   */
  KSModule::Ptr module( const QString& name );

  /**
   * The global context is needed for destructors, callbacks etc.
   */
  KSContext& context() { return m_globalContext; }

  /**
   * The CORBA wrapper uses this function to see which interface/struct
   * implements a certain repoid.
   */
  KSValue* repoidImplementation( const QString& repoid );
  /**
   * @ref KSStructClass and @ref KSInterface use this function to register themself
   * for some repoid.
   */
  void addRepoidImplementation( const QString& repoid, const KSValue::Ptr& v );

  /**
   * Add a search path. KScript will search for modules in this path.
   */
  void addSearchPath( const QString& p ) { m_searchPaths.prepend( p ); }

private:
  /**
   * A pointer to the namespace of the KScript module.
   */
  KSNamespace* m_global;
  /**
   * Used by destructors. Since they are invoked by reference counters,
   * they dont get a context. Instead they use this context.
   */
  KSContext m_globalContext;
  /**
   * Maps the names of successfully loaded modules to their @ref KSModule
   * object. The name is the logical name of the module, that means NOT
   * the filename.
   */
  QMap<QString,KSModule::Ptr> m_modules;
  /**
   * Maps Repoids to their @ref KSStructClass or @ref KSInterface
   */
  QMap<QString,KSValue::Ptr> m_repoidImpls;
  /**
   * List of all classes where we can search for modules.
   */
  QStringList m_searchPaths;
};

#endif
