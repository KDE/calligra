/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 
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

#include "main.h"

#include <iostream>

#include <komAutoLoader.h>

#include <qmsgbox.h>

typedef KOMAutoLoader<MyTool> MyAutoLoader;

MyApplication::MyApplication( int &argc, char **argv ) : KOMApplication( argc, argv , "spell_tool")
{
}

void MyApplication::start()
{
}

MyTool::MyTool( const CORBA::ORB::ObjectTag &_tag ) : DataTools::Tool_skel( _tag )
{
}

MyTool::MyTool( CORBA::Object_ptr _obj ) : DataTools::Tool_skel( _obj )
{
}

void MyTool::run( const char* _command, KOM::Base_ptr _sender, const CORBA::Any& _value, const CORBA::Any& _id )
{
  if ( strcmp( _command, "spellcheck" ) != 0L )
  {
    DataTools::UnknownCommand exc;
    exc.command = CORBA::string_dup( _command );
    mico_throw( exc );
  }
 
  CORBA::String_var str;
  _value >>= CORBA::Any::to_string( str, 0 );
  
  (void)new SpellChecker( str.in(), _sender, _id );
  
  cerr << "I am called" << endl;
}

void MyTool::cancel( CORBA::Long _id )
{
}

SpellChecker::SpellChecker( const char *_buffer, KOM::Base_ptr _sender, const CORBA::Any& _id )
{
  m_id = _id;
  m_vSender = KOM::Base::_duplicate( _sender );
  m_strBuffer = _buffer;
  m_strBuffer = m_strBuffer.stripWhiteSpace();
  m_pSpell = new KSpell( 0L, i18n("Spell Checker"), this, SLOT( slotReady() ) );
}

void SpellChecker::slotReady()
{
  connect( m_pSpell, SIGNAL( done( char* ) ), this, SLOT( slotDone( char* ) ) );
  m_pSpell->check( (char*)m_strBuffer.data() );
}

void SpellChecker::slotDone( char *_buffer )
{
  QString str1 = (const char*)_buffer;
  QString str2 = str1.stripWhiteSpace();
 
  cerr << "The result is ´" << str2 << "´" << endl;
 
  if ( str2 == m_strBuffer )
  {
    QMessageBox::information( 0L, i18n("Spell Checker"), i18n("The text is spelled correctly") );
  }
  else
  {
    CORBA::Any any;
    DataTools::Answer a;
    a.id = m_id;
    a.value <<= CORBA::Any::from_string( (char*)str2.data(), 0 );
    any <<= a;
    m_vSender->receive( DataTools::eventDone, any );
  }
  
  m_pSpell->cleanUp();
  delete m_pSpell;
  
  delete this;
}

int main( int argc, char **argv )
{
  MyApplication app( argc, argv );

  MyAutoLoader loader( "IDL:DataTools/Tool:1.0", "SpellTool" );
  
  app.exec();

  return 0;
}

#include "main.moc"
