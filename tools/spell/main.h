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

#ifndef __main_h__
#define __main_h__

#include <komApplication.h>
#include <komComponent.h>

#include "datatools.h"

#include <kspell.h>
#include <qobject.h>

class SpellChecker : public QObject
{
  Q_OBJECT
public:
  SpellChecker( const char *_buffer, KOM::Base_ptr _sender, const CORBA::Any& _id );

public slots:
  void slotReady();
  void slotDone( char* );
  
protected:
  QString m_strBuffer;
  KSpell* m_pSpell;
  KOM::Base_var m_vSender;
  CORBA::Any m_id;
};

class MyTool : virtual public DataTools::Tool_skel
{
public:
  MyTool( const CORBA::ORB::ObjectTag &_tag );
  MyTool( CORBA::Object_ptr _obj );
  
  virtual void run( const char* _command, KOM::Base_ptr _sender, const CORBA::Any& _value, const CORBA::Any& _id );
  virtual void cancel( CORBA::Long _id );
};

class MyApplication : public KOMApplication
{
public:
  MyApplication( int &argc, char **argv );
  
  void start();
};

#endif
