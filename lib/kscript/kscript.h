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
  virtual ~KSInterpreter() { }

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
   * Add a search path. KScript will search for modules in this path.
   */
  void addSearchPath( const QString& p ) { m_searchPaths.prepend( p ); }

  /**
   * KScript can be extended with special syntax. For example "Table1!A1"
   * for use in some spread sheet. If such a node has to be evaluated,
   * then this functions is told to do so.
   *
   * @return TRUE if the evaluation was successful
   */
  virtual bool processExtension( KSContext& context, KSParseNode* node );

protected:
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
   * List of all classes where we can search for modules.
   */
  QStringList m_searchPaths;
};

#endif
