/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KSCRIPT_H__
#define __KSCRIPT_H__

#include <qstring.h>
#include <qmap.h>
#include <qshared.h>
#include <qstringlist.h>

#include <kregexp.h>

#include "koscript_context.h"
#include "koscript_parsenode.h"
#include "koscript_ptr.h"
#include "koscript_value.h"

class QTextStream;
class QIODevice;

/**
 * This class offers the API to koscript functionality. If you want to
 * execute a koscript on your hard disk or if you want to embed koscript
 * in your application, then you need an instance of this class.
 *
 * @author Torben Weis <weis@kde.org>
 */
class KSInterpreter : public QShared
{
public:
  typedef KSSharedPtr<KSInterpreter> Ptr;

  /**
   * Creates a new interpreter. It is ok to have many of them
   * at the same time.
   */
  KSInterpreter();
  /**
   * Destroys the interpreter and all associated modules etc.
   */
  virtual ~KSInterpreter();

  /**
   * Reads the script @p filename from the hard disk and tries to execute it.
   *
   * @param args is the list of parameters passed to the main function
   *
   * @return the stringified exception or an empty string if everything was ok.
   *         This is a convenience function for @ref #runModule.
   */
  QString runScript( const QString& filename, const QStringList& args );

  /**
   * Runs a module located in the file @p filename. The modules name is given by @p name.
   * Usually you may want to use @ref #runScript instead.
   *
   * @param context contains the initialized module as value and
   *        the exception in case of an error. Once the value ( read: the module )
   *        of "context" is deleted, you may no longer use the exception contained
   *        in "context". That means you may not extract the exception and destroy
   *        the KSContext object and then still use the exception object.
   * @param args is the list of parameters passed to the main function
   */
  bool runModule( KSContext& context, const QString& name, const QString& filename, const QStringList& args );

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
   *
   * This method is internal. DON'T USE.
   */
  KSContext& context() { return m_globalContext; }

  /**
   * Add a search path. KScript will search for modules in this path.
   *
   * @see #runModule
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

  /**
   * Internal function for implementing regexp and $0, $1, $2, ...
   */
  KRegExp* regexp();

  /**
   * Internal function for implementing the <> operator.
   */
  QString readInput();

  /**
   * Internal function used for implementing the $_ operator.
   */
  KSValue::Ptr lastInputLine() const;

  /**
   * Internal function that returns the global namespace
   * of this interpreter.
   */
  KSNamespace* globalNamespace() { return m_global; }

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
  /**
   * Rgeular expression matcher used for constructions like "/(.+) (.+)/".
   */
  KRegExp m_regexp;
  /**
   * The last arguments passed to @ref #runScript. They are saved here, since
   * the <> operator may be used to read the files named by the arguments.
   */
  QStringList m_args;
  /**
   * When reading in files with the <> operator, then this variable tells us
   * which file we just opened. So it is used like this: m_args[ m_currentArg ].
   * The initial value is -1, that means no argument file was opened until now.
   *
   * @see #m_args.
   */
  int m_currentArg;
  /**
   * When reading in files with the <> operator, then this stream
   * handles the current input file. If there is no such input file
   * specified in the command line then this stream points to stdin.
   */
  QTextStream* m_outStream;
  /**
   * Used for @ref #m_outStream
   */
  QIODevice* m_outDevice;
  /**
   * The last line read by the <> operator is stored here.
   *
   * @ref #lastInputLine
   */
  KSValue::Ptr m_lastInputLine;
};

#endif
