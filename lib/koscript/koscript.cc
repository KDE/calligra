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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "koscript_parser.h"
#include "koscript_context.h"
#include "koscript_func.h"
#include "koscript.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#include <qfile.h>
#include <qtextstream.h>

#include <kglobal.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <klocale.h>

KSInterpreter::KSInterpreter()
{
    m_outStream = 0;
    m_currentArg = -1;
    m_outDevice = 0;
    m_lastInputLine = new KSValue( QString() );
    m_lastInputLine->setMode( KSValue::LeftExpr );

    KSModule::Ptr m = ksCreateModule_KScript( this );
    m_modules.insert( m->name(), m );

    // This module will serve as our global namespace
    // since adressing all kscript builtin stuff via its module
    // is too much typing for our users.
    m_global = m->nameSpace();

    m_globalContext.setScope( new KSScope( m_global, 0 ) );
}

KSInterpreter::~KSInterpreter()
{
    if ( m_outStream )
        delete m_outStream;
    if ( m_outDevice )
    {
        m_outDevice->close();
        delete m_outDevice;
    }
}

KSModule::Ptr KSInterpreter::module( const QString& name )
{
  QMap<QString,KSModule::Ptr>::Iterator it = m_modules.find( name );
  if ( it == m_modules.end() )
    return 0;

  return it.data();
}

QString KSInterpreter::runScript( const QString& filename, const QStringList& args )
{
    // Save for usage by the <> operator
    m_args = args;

    KSContext context( m_globalContext );
    // The "" indicates that this is not a module but
    // a script in its original meaning.
    if ( !runModule( context, "", filename, args ) )
        return context.exception()->toString( context );

    return QString::null;
}

bool KSInterpreter::runModule( KSContext& context, const QString& name )
{
  // Did we load this module already ? Dont load it twice
  if ( m_modules.contains( name ) )
  {
    KSModule* m = m_modules[name];
    m->ref();
    context.setValue( new KSValue( m ) );

    return true;
  }

  QString ksname = name + ".ks";

  QStringList::Iterator it = m_searchPaths.begin();
  for( ; it != m_searchPaths.end(); ++it )
  {
    DIR *dp = 0L;
    struct dirent *ep;

    dp = opendir( QFile::encodeName(*it) );
    if ( dp == 0L )
      return false;

    while ( ( ep = readdir( dp ) ) != 0L )
    {
      if ( ksname == ep->d_name )
      {
        QString f = *it;
        f += "/";
        f += ep->d_name;
        struct stat buff;
        if ( ( stat( QFile::encodeName(f), &buff ) == 0 ) && S_ISREG( buff.st_mode ) )
        {
          QStringList lst;
          kdDebug() << "runModule " << name << " " << f << endl;
          return runModule( context, name, f, lst );
        }
      }
    }

    closedir( dp );
  }

  QString tmp( i18n("Could not find module %1") );
  context.setException( new KSException( "IOError", tmp.arg( name ) ) );
  return false;
}

bool KSInterpreter::runModule( KSContext& result, const QString& name, const QString& filename, const QStringList& args )
{
  // Did we load this module already ? Dont load it twice
  if ( m_modules.contains( name ) )
  {
    KSModule* m = m_modules[name];
    m->ref();
    result.setValue( new KSValue( m ) );

    return true;
  }

  m_globalContext.setException( 0 );

  // s_current = this;

  FILE* f = fopen( QFile::encodeName(filename), "r" );
  if ( !f )
  {
    QString tmp( i18n("Could not open file %1") );
    result.setException( new KSException( "IOError", tmp.arg( filename ) ) );
    return false;
  }

  KSModule::Ptr module;
  // Create the parse tree.
  KSParser parser;
  if ( !parser.parse( f, QFile::encodeName( filename ) ) )
  {
      fclose( f );
      result.setException( new KSException( "SyntaxError", parser.errorMessage() ) );
      return false;
  }
  // Create a module
  module = new KSModule( this, name, parser.donateParseTree() );

  fclose( f );
  // parser.print( true );

  // Put the module in the return value
  module->ref();
  result.setValue( new KSValue( &*module ) );

  // Put all global functions etc. in the scope
  KSContext context;
  // ### TODO: Do we create a memory leak here ?
  context.setScope( new KSScope( m_global, module ) );

  // Travers the parse tree to find functions and classes etc.
  if ( !module->eval( context ) )
  {
    if ( context.exception() )
    {
      result.setException( context );
      return false;
    }
    // TODO: create exception
    printf("No exception available\n");
    return false;
  }

  // Is there a main function to execute ?
  KSValue* code = module->object( "main" );
  if ( code )
  {
    // create a context that holds the argument list
    // for the main function ( the list is empty currently ).
    KSContext context;
    context.setValue( new KSValue( KSValue::ListType ) );
    // ### TODO: Do we create a memory leak here ?
    context.setScope( new KSScope( m_global, module ) );

    // Insert parameters
    QStringList::ConstIterator sit = args.begin();
    QStringList::ConstIterator send = args.end();
    for( ; sit != send; ++sit )
    {
        context.value()->listValue().append( new KSValue( *sit ) );
    }

    if ( !code->functionValue()->call( context ) )
    {
      if ( context.exception() )
      {
        result.setException( context );
        return false;
      }

      // TODO: create exception
      printf("No exception available\n");
      return false;
    }
  }

  KSException* ex = m_globalContext.shareException();
  m_globalContext.setException( 0 );
  if ( ex )
  {
    result.setException( ex );
    return false;
  }

  // Did we just execute a file ? -> Done
  if ( name.isEmpty() )
    return true;

  m_modules.insert( name, module );

  return true;
}

bool KSInterpreter::processExtension( KSContext& context, KSParseNode* node )
{
  QString tmp( i18n("The interpreter does not support an extended syntax you are using."));
  context.setException( new KSException( "UnsupportedSyntaxExtension", tmp, node->getLineNo() ) );

  return false;
}

KRegExp* KSInterpreter::regexp()
{
    return &m_regexp;
}

QString KSInterpreter::readInput()
{
    if ( !m_outStream )
    {
        if ( m_args.count() > 0 )
        {
            m_currentArg = 0;
            m_outDevice = new QFile( m_args[ m_currentArg ] );
            m_outDevice->open( IO_ReadOnly );
            m_outStream = new QTextStream( m_outDevice );
        }
        else
            m_outStream = new QTextStream( stdin, IO_ReadOnly );
    }

    QString tmp = m_outStream->readLine();

    if ( !tmp.isNull() )
    {
        tmp += "\n";
        m_lastInputLine->setValue( tmp );
        return tmp;
    }

    m_lastInputLine->setValue( tmp );

    // Ended reading a file ...

    // Did we scan the last file ?
    if ( m_currentArg == (int)m_args.count() - 1 )
        return QString();
    else
    {
        m_currentArg++;
        if ( m_outStream )
            delete m_outStream;
        if ( m_outDevice )
            delete m_outDevice;
        m_outDevice = new QFile( m_args[ m_currentArg ] );
        m_outDevice->open( IO_ReadOnly );
        m_outStream = new QTextStream( m_outDevice );
    }

    return readInput();
}

KSValue::Ptr KSInterpreter::lastInputLine() const
{
    return m_lastInputLine;
}
