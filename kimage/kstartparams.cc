/* This file is part of the KDE project
   Copyright (C) 1999 Michael Koch <mkoch@bigfoot.de>

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

#include <kdebug.h>

#include "kstartparams.h"

KStartParams::KStartParams( int& argc, char** argv )
{
  int i;

  for( i = 1; i < argc; i++ )
  {
    m_paramList.append( QString( argv[ i ] ) );
  }
}

KStartParams::~KStartParams()
{
  // Liste leeren
  // => wird automatisch gemacht
}

uint KStartParams::countParams() const
{
  return m_paramList.count();
}

QString KStartParams::getParam( uint _index ) const
{
  QString result = "";
  if( _index < countParams() )
  {
    result = *m_paramList.at( _index );
  }
  return result;
}

void KStartParams::deleteParam( const uint _index )
{
  if( _index < countParams() )
  {
    m_paramList.remove( m_paramList.at( _index ) );
  }
}

void KStartParams::deleteParam( const QString& _param, bool _check )
{
  QStringList::Iterator it;
  if( getIndex( _param, _check, it ) )
    deleteParam( it );
}

void KStartParams::deleteParam( const QString& _shortparam, const QString& _longparam, bool _check )
{
  QStringList::Iterator it;
  if( getIndex( _shortparam, _longparam, _check, it ) )
    deleteParam( it );
}

void KStartParams::deleteParam( const QStringList::Iterator _it )
{
  m_paramList.remove( _it );
}

bool KStartParams::paramIsPresent( const QString& _param, bool _check, QStringList::Iterator& _it )
{
  QStringList::Iterator it;

  for( it = m_paramList.begin(); it != m_paramList.end(); ++it )
  {
    if( compareParam( *it, _param, _check ) )
    {
      _it = it;
      return true;
    }
  }
  return false;
}

bool KStartParams::paramIsPresent( const QString& _longparam, const QString& _shortparam, bool _check, QStringList::Iterator& _it )
{
  QStringList::Iterator it;

  for( it = m_paramList.begin(); it != m_paramList.end(); ++it )
  {
    if( compareParam( *it, _longparam, _check ) ||
        compareParam( *it, _shortparam, _check ) )
    {
      _it = it;
      return true;
    }
  }
  return false;
}

bool KStartParams::getIndex( const QString& _param, bool _check, QStringList::Iterator& _it )
{
  QStringList::Iterator it;

  for( it = m_paramList.begin(); it != m_paramList.end(); ++it )
  {
    if( compareParam( *it, _param, _check ) )
    {
      _it = it; 
      return true;
    }
  }
  return false;
}

bool KStartParams::getIndex( const QString& _longparam, const QString& _shortparam, bool _check, QStringList::Iterator& _it )
{
  QStringList::Iterator it;

  for( it = m_paramList.begin(); it != m_paramList.end(); ++it )
  {
    if( compareParam( *it, _longparam, _check ) ||
        compareParam( *it, _shortparam, _check ) )
    {
      _it = it;
      return true;
    }
  }
  return false;
}

bool KStartParams::compareParam( const QString& _arg, const QString& _param, bool _check ) const
{
  if( _check )
  {
    if( _arg != _param)
      return false;
  }
  else
  {
    if( _arg.length() < _param.length() )
      return false;
    if( _arg.left( _param.length() ) != _param )
      return false;
  }  
  return true;
};
